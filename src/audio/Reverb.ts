// ConvolverNode を使ったシンプルなリバーブ。
//
// 信号フロー:
//   input → dryGain ────────────────→ output
//         → convolver → wetGain ────→ output
//
// インパルス応答（IR）は「指数減衰するノイズ」をアルゴリズム的に生成する。
// 実機のホールでマイク録音した IR には及ばないが、学習用途には十分。

import type { ReverbParams } from '../types';

/**
 * 指数減衰ノイズのインパルス応答を生成する。
 * - 短い decay でルーム、長い decay でホール風になる。
 */
function generateImpulseResponse(ctx: AudioContext, decay: number): AudioBuffer {
  const sampleRate = ctx.sampleRate;
  const length = Math.max(1, Math.floor(sampleRate * decay));
  // ステレオ感を出すため2チャンネル独立に生成
  const buffer = ctx.createBuffer(2, length, sampleRate);
  for (let ch = 0; ch < 2; ch++) {
    const data = buffer.getChannelData(ch);
    for (let i = 0; i < length; i++) {
      const t = i / length;
      // (1 - t)^2 で指数的減衰を近似
      data[i] = (Math.random() * 2 - 1) * Math.pow(1 - t, 2);
    }
  }
  return buffer;
}

export class Reverb {
  private readonly ctx: AudioContext;
  private readonly input: GainNode;
  private readonly output: GainNode;
  private readonly convolver: ConvolverNode;
  private readonly dryGain: GainNode;
  private readonly wetGain: GainNode;

  constructor(ctx: AudioContext, params: ReverbParams) {
    this.ctx = ctx;
    this.input = ctx.createGain();
    this.output = ctx.createGain();
    this.convolver = ctx.createConvolver();
    this.dryGain = ctx.createGain();
    this.wetGain = ctx.createGain();

    this.input.connect(this.dryGain);
    this.dryGain.connect(this.output);
    this.input.connect(this.convolver);
    this.convolver.connect(this.wetGain);
    this.wetGain.connect(this.output);

    this.setDecay(params.decay);
    this.setMix(params.mix);
  }

  get inputNode(): AudioNode { return this.input; }
  get outputNode(): AudioNode { return this.output; }

  /** IR を再生成して差し替える。decay 変更時はやや重い（数十ms） */
  setDecay(decay: number): void {
    this.convolver.buffer = generateImpulseResponse(this.ctx, decay);
  }

  setMix(mix: number): void {
    const clamped = Math.max(0, Math.min(1, mix));
    this.dryGain.gain.value = 1 - clamped;
    this.wetGain.gain.value = clamped;
  }

  dispose(): void {
    try { this.input.disconnect(); } catch { /* 無視 */ }
    try { this.convolver.disconnect(); } catch { /* 無視 */ }
    try { this.dryGain.disconnect(); } catch { /* 無視 */ }
    try { this.wetGain.disconnect(); } catch { /* 無視 */ }
    try { this.output.disconnect(); } catch { /* 無視 */ }
  }
}
