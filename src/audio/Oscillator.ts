// Session A — OscillatorNode の 1ボイス分ラッパ

import type { OscillatorParams, WaveformType } from '../types';

const CENTS_PER_OCTAVE = 1200;

/** MIDIノート番号（0〜127）を Hz（A4=440Hz基準）に変換 */
export function midiToHz(note: number): number {
  return 440 * Math.pow(2, (note - 69) / 12);
}

/** OscillatorNode を1ボイス分ラップする。SynthEngine が noteOn ごとに生成する。
 *  unisonOffset はユニゾン使用時に各 osc を均等に分散させるためのデチューン（セント） */
export class Oscillator {
  private readonly osc: OscillatorNode;
  private userDetune: number;
  private octave: number;
  private readonly unisonOffset: number;

  constructor(
    ctx: AudioContext,
    params: OscillatorParams,
    frequency: number,
    unisonOffset = 0,
  ) {
    this.osc = ctx.createOscillator();
    this.osc.type = params.waveform;
    this.userDetune = params.detune;
    this.octave = params.octave;
    this.unisonOffset = unisonOffset;
    this.osc.frequency.value = frequency;
    this.applyDetune();
  }

  get outputNode(): AudioNode { return this.osc; }
  get detuneParam(): AudioParam { return this.osc.detune; }
  get frequencyParam(): AudioParam { return this.osc.frequency; }

  start(when: number): void { this.osc.start(when); }

  stop(when: number): void {
    try { this.osc.stop(when); } catch { /* 停止済みの場合は無視 */ }
  }

  setWaveform(waveform: WaveformType): void { this.osc.type = waveform; }

  setDetune(detune: number): void {
    this.userDetune = detune;
    this.applyDetune();
  }

  setOctave(octave: number): void {
    this.octave = octave;
    this.applyDetune();
  }

  /** ユーザー指定の detune + オクターブシフト + ユニゾンオフセット を合成して detune.value に反映する */
  private applyDetune(): void {
    this.osc.detune.value =
      this.userDetune + this.octave * CENTS_PER_OCTAVE + this.unisonOffset;
  }
}
