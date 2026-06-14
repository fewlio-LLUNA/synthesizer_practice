// TODO: Session A 担当 — 音声エンジン本体
//
// このファイルは Web Audio API を使ったシンセサイザーエンジンの実装。
// 信号フロー: Oscillator → Filter → Amp(VCA) → Master → Destination
//                 ↑          ↑          ↑
//               LFO         Envelope   Envelope
//
// 公開 I/F は src/types.ts の ISynthEngine を厳守すること（他セッションが依存）。

import type {
  ISynthEngine,
  OscillatorParams,
  FilterParams,
  EnvelopeParams,
  LFOParams,
} from '../types';

export class SynthEngine implements ISynthEngine {
  private ctx: AudioContext | null = null;
  private analyser: AnalyserNode | null = null;

  async init(): Promise<void> {
    // TODO: AudioContext を生成し、各ノードを接続する
    throw new Error('SynthEngine.init: 未実装（Session A）');
  }

  noteOn(_midiNote: number, _velocity = 1): void {
    // TODO: ボイス割り当て、エンベロープ開始
  }

  noteOff(_midiNote: number): void {
    // TODO: エンベロープのリリース段階に入る
  }

  setOscillatorParams(_params: Partial<OscillatorParams>): void {
    // TODO
  }

  setFilterParams(_params: Partial<FilterParams>): void {
    // TODO
  }

  setEnvelopeParams(_params: Partial<EnvelopeParams>): void {
    // TODO
  }

  setLFOParams(_params: Partial<LFOParams>): void {
    // TODO
  }

  setMasterVolume(_volume: number): void {
    // TODO
  }

  getAnalyserNode(): AnalyserNode {
    if (!this.analyser) {
      throw new Error('AnalyserNode 未初期化（init() を先に呼ぶこと）');
    }
    return this.analyser;
  }

  getAudioContext(): AudioContext {
    if (!this.ctx) {
      throw new Error('AudioContext 未初期化（init() を先に呼ぶこと）');
    }
    return this.ctx;
  }

  dispose(): void {
    this.ctx?.close();
    this.ctx = null;
    this.analyser = null;
  }
}
