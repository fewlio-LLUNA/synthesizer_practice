// プリセット集。学習者が「この設定にするとこういう音になる」を体感できる例。

import type { SynthPreset } from '../types';

export const presets: SynthPreset[] = [
  {
    name: 'Basic Lead',
    description: 'ノコギリ波 + LPF で作る基本的なリード音',
    oscillator: { waveform: 'sawtooth', detune: 0 },
    filter: { type: 'lowpass', cutoff: 2000, resonance: 3 },
    envelope: { attack: 0.01, decay: 0.2, sustain: 0.7, release: 0.3 },
    lfo: { rate: 5, depth: 0, target: 'pitch', waveform: 'sine' },
  },
  {
    name: 'Wobble Bass',
    description: '矩形波 + フィルターLFO によるウォブルベース',
    oscillator: { waveform: 'square', detune: 0 },
    filter: { type: 'lowpass', cutoff: 400, resonance: 10 },
    envelope: { attack: 0.01, decay: 0.3, sustain: 0.8, release: 0.2 },
    lfo: { rate: 2, depth: 0.7, target: 'filter', waveform: 'sine' },
  },
  {
    name: 'Pad',
    description: 'ゆっくりしたアタックと長いリリースで広がるパッド音',
    oscillator: { waveform: 'sawtooth', detune: 10 },
    filter: { type: 'lowpass', cutoff: 1200, resonance: 1 },
    envelope: { attack: 1.5, decay: 0.5, sustain: 0.8, release: 2.0 },
    lfo: { rate: 0.5, depth: 0.15, target: 'amp', waveform: 'sine' },
  },
  {
    name: 'Pluck',
    description: '素早いアタックと急激なディケイによるプラック音',
    oscillator: { waveform: 'sawtooth', detune: 0 },
    filter: { type: 'lowpass', cutoff: 4000, resonance: 5 },
    envelope: { attack: 0.001, decay: 0.15, sustain: 0.1, release: 0.3 },
    lfo: { rate: 5, depth: 0, target: 'pitch', waveform: 'sine' },
  },
  {
    name: 'Stab',
    description: '歯切れよく鋭いスタブ音（コードカッティング向け）',
    oscillator: { waveform: 'square', detune: 0 },
    filter: { type: 'highpass', cutoff: 800, resonance: 3 },
    envelope: { attack: 0.005, decay: 0.1, sustain: 0.0, release: 0.1 },
    lfo: { rate: 5, depth: 0, target: 'pitch', waveform: 'sine' },
  },
  {
    name: 'Strings',
    description: 'ストリングス風の厚みがあり温かみのある音',
    oscillator: { waveform: 'sawtooth', detune: 7 },
    filter: { type: 'lowpass', cutoff: 1800, resonance: 2 },
    envelope: { attack: 0.8, decay: 0.3, sustain: 0.9, release: 1.5 },
    lfo: { rate: 5, depth: 0.05, target: 'pitch', waveform: 'sine' },
  },
];
