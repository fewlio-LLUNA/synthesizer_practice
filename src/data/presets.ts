// TODO: Session E 担当 — プリセット集
//
// 学習者が「こういう設定にするとこういう音になる」を体感できる例を用意する。
// 例: リード、ベース、パッド、プラック、ストリングス風 など。

import type { SynthPreset } from '../types';

export const presets: SynthPreset[] = [
  // サンプル1件。Session E が複数追加する。
  {
    name: 'Basic Lead',
    description: 'ノコギリ波 + LPFで作る基本的なリード音',
    oscillator: { waveform: 'sawtooth', detune: 0 },
    filter: { type: 'lowpass', cutoff: 2000, resonance: 3 },
    envelope: { attack: 0.01, decay: 0.2, sustain: 0.7, release: 0.3 },
    lfo: { rate: 5, depth: 0, target: 'pitch', waveform: 'sine' },
  },
];
