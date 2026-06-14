// Session A — 低周波オシレーター（LFO）
// OscillatorNode + GainNode で変調信号を生成し、output を AudioParam に connect して使う

import type { LFOParams, LFOTarget, WaveformType } from '../types';

/** depth=1 のときのターゲットごとの最大変調量 */
const LFO_SCALE: Record<LFOTarget, number> = {
  pitch:  1200,   // セント（±1オクターブ）
  filter: 10000,  // Hz（±10kHz）
  amp:    1,      // ゲイン（±1）
};

/** LFO。output.connect(audioParam) で変調先に接続する */
export class LFO {
  private readonly osc: OscillatorNode;
  private readonly gainNode: GainNode;
  private params: LFOParams;

  constructor(ctx: AudioContext, params: LFOParams) {
    this.params = { ...params };
    this.osc = ctx.createOscillator();
    this.gainNode = ctx.createGain();

    this.osc.type = params.waveform;
    this.osc.frequency.value = params.rate;
    this.gainNode.gain.value = this.calcScale();

    this.osc.connect(this.gainNode);
    this.osc.start();
  }

  /** 変調信号の出力ノード。AudioParam に connect して使う */
  get output(): GainNode { return this.gainNode; }

  setRate(hz: number): void {
    this.params.rate = hz;
    this.osc.frequency.value = hz;
  }

  setDepth(depth: number): void {
    this.params.depth = depth;
    this.gainNode.gain.value = this.calcScale();
  }

  setWaveform(waveform: WaveformType): void {
    this.params.waveform = waveform;
    this.osc.type = waveform;
  }

  /** ターゲット変更時は depth スケールを再計算する */
  setTarget(target: LFOTarget): void {
    this.params.target = target;
    this.gainNode.gain.value = this.calcScale();
  }

  dispose(): void {
    try { this.osc.stop(); } catch { /* 停止済みは無視 */ }
    this.osc.disconnect();
    this.gainNode.disconnect();
  }

  private calcScale(): number {
    return this.params.depth * LFO_SCALE[this.params.target];
  }
}
