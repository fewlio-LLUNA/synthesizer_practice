// 各パラメータのデフォルト値。
// このファイルは全セッションが参照する共有資産（初期コミット時に確定）。

import type {
  OscillatorParams,
  FilterParams,
  EnvelopeParams,
  LFOParams,
} from '../types';

export const DEFAULT_OSCILLATOR: OscillatorParams = {
  waveform: 'sawtooth',
  detune: 0,
};

export const DEFAULT_FILTER: FilterParams = {
  type: 'lowpass',
  cutoff: 2000,
  resonance: 1,
};

export const DEFAULT_ENVELOPE: EnvelopeParams = {
  attack: 0.01,
  decay: 0.2,
  sustain: 0.7,
  release: 0.3,
};

export const DEFAULT_LFO: LFOParams = {
  rate: 5,
  depth: 0,
  target: 'pitch',
  waveform: 'sine',
};

export const DEFAULT_MASTER_VOLUME = 0.5;
