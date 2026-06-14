// 各パラメータのデフォルト値。
// このファイルは全セッションが参照する共有資産（初期コミット時に確定）。

import type {
  OscillatorParams,
  NoiseParams,
  FilterParams,
  EnvelopeParams,
  FilterEnvelopeParams,
  LFOParams,
  UnisonParams,
  PerformanceParams,
  DriveParams,
  DelayParams,
  ReverbParams,
} from '../types';

export const DEFAULT_OSCILLATOR: OscillatorParams = {
  waveform: 'sawtooth',
  detune: 0,
  octave: 0,
  level: 1.0,
};

export const DEFAULT_NOISE: NoiseParams = {
  type: 'white',
  level: 0.0, // 0 で無効、つまり既存挙動と同じ
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

export const DEFAULT_FILTER_ENVELOPE: FilterEnvelopeParams = {
  attack: 0.01,
  decay: 0.3,
  sustain: 0.4,
  release: 0.3,
  amount: 0, // 0 で無効。0 にしておけば既存挙動と同じ
};

export const DEFAULT_LFO: LFOParams = {
  rate: 5,
  depth: 0,
  target: 'pitch',
  waveform: 'sine',
};

export const DEFAULT_UNISON: UnisonParams = {
  count: 1, // 1 で無効（標準の単一オシレーター）
  detune: 20,
};

export const DEFAULT_PERFORMANCE: PerformanceParams = {
  glide: 0, // 0 で無効
};

export const DEFAULT_DRIVE: DriveParams = {
  type: 'soft',
  drive: 0,
  mix: 0, // 0 で無効
};

export const DEFAULT_DELAY: DelayParams = {
  time: 0.3,
  feedback: 0.3,
  mix: 0, // 0 で無効
};

export const DEFAULT_REVERB: ReverbParams = {
  decay: 1.5,
  mix: 0, // 0 で無効
};

export const DEFAULT_MASTER_VOLUME = 0.5;
