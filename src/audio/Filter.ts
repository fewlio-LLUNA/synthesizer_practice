// Session A — BiquadFilterNode の薄いラッパ

import type { FilterParams, FilterType } from '../types';

/** BiquadFilterNode をラップし、type / cutoff / resonance を管理する */
export class Filter {
  private readonly node: BiquadFilterNode;

  constructor(ctx: AudioContext, params: FilterParams) {
    this.node = ctx.createBiquadFilter();
    this.node.type = params.type;
    this.node.frequency.value = params.cutoff;
    this.node.Q.value = params.resonance;
  }

  get inputNode(): AudioNode { return this.node; }
  get outputNode(): AudioNode { return this.node; }
  get frequencyParam(): AudioParam { return this.node.frequency; }
  /** カットオフに対する cents 単位のオフセット。フィルターエンベロープが書き込む */
  get detuneParam(): AudioParam { return this.node.detune; }

  setType(type: FilterType): void { this.node.type = type; }
  setCutoff(hz: number): void { this.node.frequency.value = hz; }
  setResonance(q: number): void { this.node.Q.value = q; }
}
