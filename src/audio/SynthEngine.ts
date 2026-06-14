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
  FilterParams,
  EnvelopeParams,
  LFOParams,
  LFOTarget,
} from '../types';
import {
  DEFAULT_OSCILLATOR,
  DEFAULT_FILTER,
  DEFAULT_ENVELOPE,
  DEFAULT_LFO,
  DEFAULT_MASTER_VOLUME,
} from '../data/defaults';
import { Oscillator, midiToHz } from './Oscillator';
import { Filter } from './Filter';
import { Envelope } from './Envelope';
import { LFO } from './LFO';

const MAX_VOICES = 4;

interface Voice {
  readonly osc: Oscillator;
  readonly filter: Filter;
  readonly env: Envelope;
  lfoParam: AudioParam; // ターゲット変更時に reconnectLFO が更新する
}

export class SynthEngine implements ISynthEngine {
  private ctx: AudioContext | null = null;
  private analyser: AnalyserNode | null = null;
  private master: GainNode | null = null;
  private lfo: LFO | null = null;

  private oscParams: OscillatorParams = { ...DEFAULT_OSCILLATOR };
  private filterParams: FilterParams = { ...DEFAULT_FILTER };
  private envParams: EnvelopeParams = { ...DEFAULT_ENVELOPE };
  private lfoParams: LFOParams = { ...DEFAULT_LFO };

  private readonly voices = new Map<number, Voice>();

  async init(): Promise<void> {
    this.ctx = new AudioContext();
    await this.ctx.resume();

    this.master = this.ctx.createGain();
    this.master.gain.value = DEFAULT_MASTER_VOLUME;
    this.analyser = this.ctx.createAnalyser();
    this.analyser.fftSize = 2048;

    this.master.connect(this.analyser);
    this.analyser.connect(this.ctx.destination);

    this.lfo = new LFO(this.ctx, this.lfoParams);
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
    const osc = new Oscillator(ctx, this.oscParams, midiToHz(midiNote));
    const filter = new Filter(ctx, this.filterParams);
    const env = new Envelope(ctx, this.envParams);

    osc.outputNode.connect(filter.inputNode);
    filter.outputNode.connect(env.inputNode);
    env.outputNode.connect(this.master);

    const lfoParam = this.pickLFOParam(this.lfoParams.target, osc, filter, env);
    this.lfo.output.connect(lfoParam);

    osc.start(now);
    env.trigger(now, velocity);

    this.voices.set(midiNote, { osc, filter, env, lfoParam });
  }

  noteOff(midiNote: number): void {
    const voice = this.voices.get(midiNote);
    if (!voice || !this.ctx) return;

    const now = this.ctx.currentTime;
    const stopAt = voice.env.release(now);
    voice.osc.stop(stopAt + 0.01);
    this.voices.delete(midiNote);

    // リリース完了後にノードを切断
    const delayMs = (stopAt - now + 0.1) * 1000;
    setTimeout(() => this.cleanupVoice(voice), delayMs);
  }

  setOscillatorParams(params: Partial<OscillatorParams>): void {
    this.oscParams = { ...this.oscParams, ...params };
    for (const v of this.voices.values()) {
      if (params.waveform !== undefined) v.osc.setWaveform(params.waveform);
      if (params.detune !== undefined) v.osc.setDetune(params.detune);
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
    this.ctx?.close();
    this.ctx = null;
    this.analyser = null;
    this.master = null;
    this.lfo = null;
  }

  /** ボイスを即時停止してマップから除去する */
  private stopVoice(midiNote: number): void {
    const voice = this.voices.get(midiNote);
    if (!voice || !this.ctx) return;
    const now = this.ctx.currentTime;
    voice.env.release(now);
    voice.osc.stop(now + 0.01);
    this.voices.delete(midiNote);
    this.cleanupVoice(voice);
  }

  /** ノードを AudioGraph から切断してリソースを解放する */
  private cleanupVoice(voice: Voice): void {
    try { this.lfo?.output.disconnect(voice.lfoParam); } catch { /* 無視 */ }
    try { voice.osc.outputNode.disconnect(); } catch { /* 無視 */ }
    try { voice.filter.outputNode.disconnect(); } catch { /* 無視 */ }
    try { voice.env.outputNode.disconnect(); } catch { /* 無視 */ }
  }

  /** LFO ターゲット変更時に全アクティブボイスの接続を切り替える */
  private reconnectLFO(): void {
    if (!this.lfo) return;
    for (const v of this.voices.values()) {
      try { this.lfo.output.disconnect(v.lfoParam); } catch { /* 無視 */ }
      v.lfoParam = this.pickLFOParam(this.lfoParams.target, v.osc, v.filter, v.env);
      this.lfo.output.connect(v.lfoParam);
    }
  }

  /** LFO ターゲットに対応する AudioParam を返す */
  private pickLFOParam(
    target: LFOTarget,
    osc: Oscillator,
    filter: Filter,
    env: Envelope,
  ): AudioParam {
    if (target === 'pitch') return osc.detuneParam;
    if (target === 'filter') return filter.frequencyParam;
    return env.gainParam; // 'amp'
  }
}
