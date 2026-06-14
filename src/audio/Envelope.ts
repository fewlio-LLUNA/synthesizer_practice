// Session A — GainNode のスケジューリングで ADSR エンベロープを実現

import type { EnvelopeParams } from '../types';

/** ADSR エンベロープ。GainNode の gain AudioParam に値をスケジュールして制御する */
export class Envelope {
  private readonly node: GainNode;
  private params: EnvelopeParams;

  constructor(ctx: AudioContext, params: EnvelopeParams) {
    this.node = ctx.createGain();
    this.node.gain.value = 0;
    this.params = { ...params };
  }

  get inputNode(): AudioNode { return this.node; }
  get outputNode(): AudioNode { return this.node; }
  get gainParam(): AudioParam { return this.node.gain; }

  /** Attack → Decay → Sustain をスケジュール。velocity は 0〜1 */
  trigger(when: number, velocity: number): void {
    const { attack, decay, sustain } = this.params;
    const g = this.node.gain;
    g.cancelScheduledValues(when);
    g.setValueAtTime(0, when);
    g.linearRampToValueAtTime(velocity, when + attack);
    g.linearRampToValueAtTime(sustain * velocity, when + attack + decay);
  }

  /** Release をスケジュールし、ゲインが 0 になる時刻を返す */
  release(when: number): number {
    const { release } = this.params;
    const g = this.node.gain;
    const currentValue = g.value;
    g.cancelScheduledValues(when);
    g.setValueAtTime(currentValue, when);
    g.linearRampToValueAtTime(0, when + release);
    return when + release;
  }

  setParams(params: Partial<EnvelopeParams>): void {
    this.params = { ...this.params, ...params };
  }
}
