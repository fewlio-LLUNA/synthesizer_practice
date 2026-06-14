// Session A — OscillatorNode の 1ボイス分ラッパ

import type { OscillatorParams, WaveformType } from '../types';

/** MIDIノート番号（0〜127）を Hz（A4=440Hz基準）に変換 */
export function midiToHz(note: number): number {
  return 440 * Math.pow(2, (note - 69) / 12);
}

/** OscillatorNode を1ボイス分ラップする。SynthEngine が noteOn ごとに生成する */
export class Oscillator {
  private readonly osc: OscillatorNode;

  constructor(ctx: AudioContext, params: OscillatorParams, frequency: number) {
    this.osc = ctx.createOscillator();
    this.osc.type = params.waveform;
    this.osc.detune.value = params.detune;
    this.osc.frequency.value = frequency;
  }

  get outputNode(): AudioNode { return this.osc; }
  get detuneParam(): AudioParam { return this.osc.detune; }
  get frequencyParam(): AudioParam { return this.osc.frequency; }

  start(when: number): void { this.osc.start(when); }

  stop(when: number): void {
    try { this.osc.stop(when); } catch { /* 停止済みの場合は無視 */ }
  }

  setWaveform(waveform: WaveformType): void { this.osc.type = waveform; }
  setDetune(detune: number): void { this.osc.detune.value = detune; }
}
