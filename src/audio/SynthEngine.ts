// Session A — シンセサイザーエンジン本体
//
// 信号フロー（ボイスごと）:
//   Oscillator → Filter(Biquad) → Envelope(GainNode/VCA) → Master → Analyser → Destination
//
// LFO は pitch / filter / amp のいずれかの AudioParam に接続して変調を加える。
// ボイス管理: 最大 MAX_VOICES のポリフォニー（超過時は最古ボイスを解放）。

import type {
  ISynthEngine,
  OscillatorParams,
  NoiseParams,
  FilterParams,
  EnvelopeParams,
  FilterEnvelopeParams,
  LFOParams,
  LFOTarget,
  UnisonParams,
  PerformanceParams,
  DriveParams,
  DelayParams,
  ReverbParams,
} from '../types';
import {
  DEFAULT_OSCILLATOR,
  DEFAULT_NOISE,
  DEFAULT_FILTER,
  DEFAULT_ENVELOPE,
  DEFAULT_FILTER_ENVELOPE,
  DEFAULT_LFO,
  DEFAULT_UNISON,
  DEFAULT_PERFORMANCE,
  DEFAULT_DRIVE,
  DEFAULT_DELAY,
  DEFAULT_REVERB,
  DEFAULT_MASTER_VOLUME,
} from '../data/defaults';
import { Oscillator, midiToHz } from './Oscillator';
import { Filter } from './Filter';
import { Envelope } from './Envelope';
import { LFO } from './LFO';
import { Noise, createNoiseBuffer } from './Noise';
import { Drive } from './Drive';
import { Delay } from './Delay';
import { Reverb } from './Reverb';

const MAX_VOICES = 4;
/** フィルターエンベロープ amount=1 で動かす最大幅（cents）。約5オクターブ */
const FILTER_ENV_RANGE_CENTS = 6000;

interface Voice {
  /** ユニゾン分のオシレーター（count=1なら1個、count=8なら8個） */
  readonly oscs: Oscillator[];
  readonly oscGain: GainNode;
  readonly noise: Noise;
  readonly noiseGain: GainNode;
  readonly filter: Filter;
  readonly env: Envelope;
  readonly filterEnv: Envelope;
  readonly filterEnvAmount: GainNode;
  /** LFOの接続先 AudioParam リスト（pitch ターゲット時は全 osc.detune に接続するため配列） */
  lfoParams: AudioParam[];
}

export class SynthEngine implements ISynthEngine {
  private ctx: AudioContext | null = null;
  private analyser: AnalyserNode | null = null;
  private master: GainNode | null = null;
  private lfo: LFO | null = null;
  private drive: Drive | null = null;
  private delay: Delay | null = null;
  private reverb: Reverb | null = null;

  private oscParams: OscillatorParams = { ...DEFAULT_OSCILLATOR };
  private noiseParams: NoiseParams = { ...DEFAULT_NOISE };
  private filterParams: FilterParams = { ...DEFAULT_FILTER };
  private envParams: EnvelopeParams = { ...DEFAULT_ENVELOPE };
  private filterEnvParams: FilterEnvelopeParams = { ...DEFAULT_FILTER_ENVELOPE };
  private lfoParams: LFOParams = { ...DEFAULT_LFO };
  private unisonParams: UnisonParams = { ...DEFAULT_UNISON };
  private performanceParams: PerformanceParams = { ...DEFAULT_PERFORMANCE };
  private driveParams: DriveParams = { ...DEFAULT_DRIVE };
  private delayParams: DelayParams = { ...DEFAULT_DELAY };
  private reverbParams: ReverbParams = { ...DEFAULT_REVERB };

  /** ノイズバッファ。Voice 間で使い回す（生成コスト削減） */
  private whiteBuffer: AudioBuffer | null = null;
  private pinkBuffer: AudioBuffer | null = null;

  /** グライド計算用: 最後にトリガーしたノートの周波数。null なら未トリガー */
  private lastFreq: number | null = null;

  private readonly voices = new Map<number, Voice>();

  async init(): Promise<void> {
    this.ctx = new AudioContext();
    await this.ctx.resume();

    this.master = this.ctx.createGain();
    this.master.gain.value = DEFAULT_MASTER_VOLUME;
    this.analyser = this.ctx.createAnalyser();
    this.analyser.fftSize = 2048;

    // エフェクトチェーン: master → drive → delay → reverb → analyser → destination
    this.drive = new Drive(this.ctx, this.driveParams);
    this.delay = new Delay(this.ctx, this.delayParams);
    this.reverb = new Reverb(this.ctx, this.reverbParams);
    this.master.connect(this.drive.inputNode);
    this.drive.outputNode.connect(this.delay.inputNode);
    this.delay.outputNode.connect(this.reverb.inputNode);
    this.reverb.outputNode.connect(this.analyser);
    this.analyser.connect(this.ctx.destination);

    this.lfo = new LFO(this.ctx, this.lfoParams);

    // ノイズバッファを事前生成（CPU 重いので init で1回だけ）
    this.whiteBuffer = createNoiseBuffer(this.ctx, 'white');
    this.pinkBuffer = createNoiseBuffer(this.ctx, 'pink');
  }

  noteOn(midiNote: number, velocity = 1): void {
    if (!this.ctx || !this.master || !this.lfo) return;

    if (this.voices.size >= MAX_VOICES) {
      const oldest = this.voices.keys().next().value;
      if (oldest !== undefined) this.stopVoice(oldest);
    }
    if (this.voices.has(midiNote)) this.stopVoice(midiNote);

    const ctx = this.ctx;
    const now = ctx.currentTime;
    const targetFreq = midiToHz(midiNote);
    const startFreq = this.lastFreq ?? targetFreq;
    const glide = this.performanceParams.glide;

    // ユニゾン: count 分のオシレーターを生成し、均等にデチューンを分散する
    const count = Math.max(1, Math.floor(this.unisonParams.count));
    const spread = this.unisonParams.detune;
    const oscs: Oscillator[] = [];
    for (let i = 0; i < count; i++) {
      // -0.5 〜 +0.5 の範囲で均等配分（count=1 のときは 0）
      const t = count === 1 ? 0 : i / (count - 1) - 0.5;
      const unisonOffset = t * spread;
      const osc = new Oscillator(ctx, this.oscParams, startFreq, unisonOffset);
      // グライド: 前ノート周波数から今ノート周波数へ滑らかに補完
      if (glide > 0 && this.lastFreq !== null && this.lastFreq !== targetFreq) {
        osc.frequencyParam.setValueAtTime(startFreq, now);
        osc.frequencyParam.exponentialRampToValueAtTime(targetFreq, now + glide);
      } else {
        osc.frequencyParam.setValueAtTime(targetFreq, now);
      }
      oscs.push(osc);
    }
    this.lastFreq = targetFreq;

    const oscGain = ctx.createGain();
    // ユニゾン数に応じて音量を補正（重ねるとレベルが膨らむため）
    oscGain.gain.value = this.oscParams.level / Math.sqrt(count);

    // ノイズ
    const buffer = this.getNoiseBuffer(this.noiseParams.type);
    const noise = new Noise(ctx, buffer);
    const noiseGain = ctx.createGain();
    noiseGain.gain.value = this.noiseParams.level;

    const filter = new Filter(ctx, this.filterParams);
    const env = new Envelope(ctx, this.envParams);
    const filterEnv = new Envelope(ctx, this.filterEnvParams);
    const filterEnvAmount = ctx.createGain();
    filterEnvAmount.gain.value = this.filterEnvParams.amount * FILTER_ENV_RANGE_CENTS;

    // 音声フロー:
    //   Oscs (unison) → OscGain ─┐
    //                              ├→ Filter → Amp Env → Master
    //   Noise         → NoiseGain ─┘
    for (const osc of oscs) osc.outputNode.connect(oscGain);
    oscGain.connect(filter.inputNode);
    noise.outputNode.connect(noiseGain);
    noiseGain.connect(filter.inputNode);
    filter.outputNode.connect(env.inputNode);
    env.outputNode.connect(this.master);

    // フィルターエンベロープ: filterEnv → amountGain → filter.detune
    filterEnv.outputNode.connect(filterEnvAmount);
    filterEnvAmount.connect(filter.detuneParam);

    // LFO 接続（pitch ターゲット時は全 osc.detune に接続）
    const lfoParams = this.pickLFOParams(this.lfoParams.target, oscs, filter, env);
    for (const p of lfoParams) this.lfo.output.connect(p);

    for (const osc of oscs) osc.start(now);
    noise.start(now);
    env.trigger(now, velocity);
    filterEnv.trigger(now, 1); // velocity は amp 側のみ反映、フィルター envは固定

    this.voices.set(midiNote, {
      oscs, oscGain, noise, noiseGain, filter,
      env, filterEnv, filterEnvAmount, lfoParams,
    });
  }

  /** 指定タイプのノイズバッファを返す（init 後に有効） */
  private getNoiseBuffer(type: NoiseParams['type']): AudioBuffer {
    const buffer = type === 'pink' ? this.pinkBuffer : this.whiteBuffer;
    if (!buffer) throw new Error('Noise buffer 未初期化（init() を先に呼ぶこと）');
    return buffer;
  }

  noteOff(midiNote: number): void {
    const voice = this.voices.get(midiNote);
    if (!voice || !this.ctx) return;

    const now = this.ctx.currentTime;
    const stopAt = voice.env.release(now);
    voice.filterEnv.release(now);
    for (const osc of voice.oscs) osc.stop(stopAt + 0.01);
    voice.noise.stop(stopAt + 0.01);
    this.voices.delete(midiNote);

    // リリース完了後にノードを切断
    const delayMs = (stopAt - now + 0.1) * 1000;
    setTimeout(() => this.cleanupVoice(voice), delayMs);
  }

  setOscillatorParams(params: Partial<OscillatorParams>): void {
    this.oscParams = { ...this.oscParams, ...params };
    for (const v of this.voices.values()) {
      for (const osc of v.oscs) {
        if (params.waveform !== undefined) osc.setWaveform(params.waveform);
        if (params.detune !== undefined) osc.setDetune(params.detune);
        if (params.octave !== undefined) osc.setOctave(params.octave);
      }
      if (params.level !== undefined) {
        // ユニゾン補正を維持しながら level を更新
        v.oscGain.gain.value = params.level / Math.sqrt(v.oscs.length);
      }
    }
  }

  setUnisonParams(params: Partial<UnisonParams>): void {
    this.unisonParams = { ...this.unisonParams, ...params };
    // count / detune の変更は次回ノートオン以降に反映される
  }

  setPerformanceParams(params: Partial<PerformanceParams>): void {
    this.performanceParams = { ...this.performanceParams, ...params };
  }

  setNoiseParams(params: Partial<NoiseParams>): void {
    this.noiseParams = { ...this.noiseParams, ...params };
    for (const v of this.voices.values()) {
      // level は即時、type は次回ノートオンから（実行中ボイスのバッファは差し替えない）
      if (params.level !== undefined) v.noiseGain.gain.value = params.level;
    }
  }

  setFilterParams(params: Partial<FilterParams>): void {
    this.filterParams = { ...this.filterParams, ...params };
    for (const v of this.voices.values()) {
      if (params.type !== undefined) v.filter.setType(params.type);
      if (params.cutoff !== undefined) v.filter.setCutoff(params.cutoff);
      if (params.resonance !== undefined) v.filter.setResonance(params.resonance);
    }
  }

  setEnvelopeParams(params: Partial<EnvelopeParams>): void {
    this.envParams = { ...this.envParams, ...params };
    for (const v of this.voices.values()) {
      v.env.setParams(params);
    }
  }

  setFilterEnvelopeParams(params: Partial<FilterEnvelopeParams>): void {
    this.filterEnvParams = { ...this.filterEnvParams, ...params };
    for (const v of this.voices.values()) {
      // ADSR部分は次回ノートオン以降に反映
      v.filterEnv.setParams(params);
      // amount は即時反映
      if (params.amount !== undefined) {
        v.filterEnvAmount.gain.value = params.amount * FILTER_ENV_RANGE_CENTS;
      }
    }
  }

  setLFOParams(params: Partial<LFOParams>): void {
    if (!this.lfo) return;
    const prevTarget = this.lfoParams.target;
    this.lfoParams = { ...this.lfoParams, ...params };

    if (params.rate !== undefined) this.lfo.setRate(params.rate);
    if (params.depth !== undefined) this.lfo.setDepth(params.depth);
    if (params.waveform !== undefined) this.lfo.setWaveform(params.waveform);
    if (params.target !== undefined && params.target !== prevTarget) {
      this.lfo.setTarget(params.target);
      this.reconnectLFO();
    }
  }

  setMasterVolume(volume: number): void {
    if (this.master) this.master.gain.value = volume;
  }

  setDriveParams(params: Partial<DriveParams>): void {
    this.driveParams = { ...this.driveParams, ...params };
    if (!this.drive) return;
    if (params.type !== undefined) this.drive.setType(params.type);
    if (params.drive !== undefined) this.drive.setDrive(params.drive);
    if (params.mix !== undefined) this.drive.setMix(params.mix);
  }

  setDelayParams(params: Partial<DelayParams>): void {
    this.delayParams = { ...this.delayParams, ...params };
    if (!this.delay) return;
    if (params.time !== undefined) this.delay.setTime(params.time);
    if (params.feedback !== undefined) this.delay.setFeedback(params.feedback);
    if (params.mix !== undefined) this.delay.setMix(params.mix);
  }

  setReverbParams(params: Partial<ReverbParams>): void {
    this.reverbParams = { ...this.reverbParams, ...params };
    if (!this.reverb) return;
    if (params.decay !== undefined) this.reverb.setDecay(params.decay);
    if (params.mix !== undefined) this.reverb.setMix(params.mix);
  }

  getAnalyserNode(): AnalyserNode {
    if (!this.analyser) throw new Error('init() を先に呼ぶこと');
    return this.analyser;
  }

  getAudioContext(): AudioContext {
    if (!this.ctx) throw new Error('init() を先に呼ぶこと');
    return this.ctx;
  }

  dispose(): void {
    for (const note of [...this.voices.keys()]) this.stopVoice(note);
    this.lfo?.dispose();
    this.drive?.dispose();
    this.delay?.dispose();
    this.reverb?.dispose();
    this.ctx?.close();
    this.ctx = null;
    this.analyser = null;
    this.master = null;
    this.lfo = null;
    this.drive = null;
    this.delay = null;
    this.reverb = null;
  }

  /** ボイスを即時停止してマップから除去する */
  private stopVoice(midiNote: number): void {
    const voice = this.voices.get(midiNote);
    if (!voice || !this.ctx) return;
    const now = this.ctx.currentTime;
    voice.env.release(now);
    voice.filterEnv.release(now);
    for (const osc of voice.oscs) osc.stop(now + 0.01);
    voice.noise.stop(now + 0.01);
    this.voices.delete(midiNote);
    this.cleanupVoice(voice);
  }

  /** ノードを AudioGraph から切断してリソースを解放する */
  private cleanupVoice(voice: Voice): void {
    if (this.lfo) {
      for (const p of voice.lfoParams) {
        try { this.lfo.output.disconnect(p); } catch { /* 無視 */ }
      }
    }
    for (const osc of voice.oscs) {
      try { osc.outputNode.disconnect(); } catch { /* 無視 */ }
    }
    try { voice.oscGain.disconnect(); } catch { /* 無視 */ }
    try { voice.noise.outputNode.disconnect(); } catch { /* 無視 */ }
    try { voice.noiseGain.disconnect(); } catch { /* 無視 */ }
    try { voice.filter.outputNode.disconnect(); } catch { /* 無視 */ }
    try { voice.env.outputNode.disconnect(); } catch { /* 無視 */ }
    try { voice.filterEnv.outputNode.disconnect(); } catch { /* 無視 */ }
    try { voice.filterEnvAmount.disconnect(); } catch { /* 無視 */ }
  }

  /** LFO ターゲット変更時に全アクティブボイスの接続を切り替える */
  private reconnectLFO(): void {
    if (!this.lfo) return;
    for (const v of this.voices.values()) {
      for (const p of v.lfoParams) {
        try { this.lfo.output.disconnect(p); } catch { /* 無視 */ }
      }
      v.lfoParams = this.pickLFOParams(this.lfoParams.target, v.oscs, v.filter, v.env);
      for (const p of v.lfoParams) this.lfo.output.connect(p);
    }
  }

  /** LFO ターゲットに対応する AudioParam リストを返す。
   *  pitch ターゲット時はユニゾン分すべての osc.detune を返す。 */
  private pickLFOParams(
    target: LFOTarget,
    oscs: Oscillator[],
    filter: Filter,
    env: Envelope,
  ): AudioParam[] {
    if (target === 'pitch') return oscs.map((o) => o.detuneParam);
    if (target === 'filter') return [filter.frequencyParam];
    return [env.gainParam]; // 'amp'
  }
}
