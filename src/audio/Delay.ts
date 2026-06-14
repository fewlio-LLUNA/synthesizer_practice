// シンプルなフィードバック付きディレイ。
//
// 信号フロー:
//   input → dryGain ───────────────→ output
//         ↓
//         delay ← feedback ─┐
//         ↓                  │
//         (delay の出力) ────┘
//         ↓
//         wetGain ─────────────────→ output

import type { DelayParams } from '../types';

/** 暴走防止のためフィードバックの上限を設定 */
const MAX_FEEDBACK = 0.95;

export class Delay {
  private readonly input: GainNode;
  private readonly output: GainNode;
  private readonly delay: DelayNode;
  private readonly feedback: GainNode;
  private readonly dryGain: GainNode;
  private readonly wetGain: GainNode;

  constructor(ctx: AudioContext, params: DelayParams) {
    this.input = ctx.createGain();
    this.output = ctx.createGain();
    this.delay = ctx.createDelay(2.0);
    this.feedback = ctx.createGain();
    this.dryGain = ctx.createGain();
    this.wetGain = ctx.createGain();

    this.delay.delayTime.value = params.time;
    this.feedback.gain.value = Math.min(params.feedback, MAX_FEEDBACK);

    // ドライ経路
    this.input.connect(this.dryGain);
    this.dryGain.connect(this.output);

    // ウェット経路（フィードバックループ）
    this.input.connect(this.delay);
    this.delay.connect(this.feedback);
    this.feedback.connect(this.delay);
    this.delay.connect(this.wetGain);
    this.wetGain.connect(this.output);

    this.setMix(params.mix);
  }

  get inputNode(): AudioNode { return this.input; }
  get outputNode(): AudioNode { return this.output; }

  setTime(time: number): void {
    this.delay.delayTime.value = time;
  }

  setFeedback(feedback: number): void {
    this.feedback.gain.value = Math.min(feedback, MAX_FEEDBACK);
  }

  /** dry/wet を補完的に設定（合計1.0） */
  setMix(mix: number): void {
    const clamped = Math.max(0, Math.min(1, mix));
    this.dryGain.gain.value = 1 - clamped;
    this.wetGain.gain.value = clamped;
  }

  dispose(): void {
    try { this.input.disconnect(); } catch { /* 無視 */ }
    try { this.delay.disconnect(); } catch { /* 無視 */ }
    try { this.feedback.disconnect(); } catch { /* 無視 */ }
    try { this.dryGain.disconnect(); } catch { /* 無視 */ }
    try { this.wetGain.disconnect(); } catch { /* 無視 */ }
    try { this.output.disconnect(); } catch { /* 無視 */ }
  }
}
