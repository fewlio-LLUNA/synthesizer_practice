// 全モジュールが共有する型定義。
// このファイルは「セッション間の契約」であり、変更には全セッションへの周知が必要。
// 変更時は TASKS.md の「I/F 変更ルール」に従うこと。

// ---------- オシレーター ----------

export type WaveformType = 'sine' | 'sawtooth' | 'square' | 'triangle';

export interface OscillatorParams {
  /** 波形種別 */
  waveform: WaveformType;
  /** デチューン（cent 単位、-1200〜+1200） */
  detune: number;
}

// ---------- フィルター ----------

export type FilterType = 'lowpass' | 'highpass' | 'bandpass';

export interface FilterParams {
  /** フィルター種別 */
  type: FilterType;
  /** カットオフ周波数（Hz、20〜20000） */
  cutoff: number;
  /** レゾナンス（Q値、0.1〜30） */
  resonance: number;
}

// ---------- エンベロープ（ADSR） ----------

export interface EnvelopeParams {
  /** アタック時間（秒、0.001〜5） */
  attack: number;
  /** ディケイ時間（秒、0.001〜5） */
  decay: number;
  /** サスティンレベル（0〜1） */
  sustain: number;
  /** リリース時間（秒、0.001〜5） */
  release: number;
}

// ---------- LFO ----------

export type LFOTarget = 'pitch' | 'filter' | 'amp';

export interface LFOParams {
  /** 周波数（Hz、0.1〜20） */
  rate: number;
  /** 深さ（0〜1。ターゲットごとの最大変調量に対する比率） */
  depth: number;
  /** 変調先 */
  target: LFOTarget;
  /** LFO波形 */
  waveform: WaveformType;
}

// ---------- プリセット ----------

export interface SynthPreset {
  /** プリセット名 */
  name: string;
  /** 説明（リード/ベース/パッド等のキャラクター） */
  description: string;
  oscillator: OscillatorParams;
  filter: FilterParams;
  envelope: EnvelopeParams;
  lfo: LFOParams;
}

// ---------- パラメータ説明文（解説パネル用） ----------

export interface ParameterDescription {
  /** パラメータ識別子（例: "filter.cutoff"） */
  id: string;
  /** 表示名（例: "カットオフ周波数"） */
  name: string;
  /** 一行サマリ */
  summary: string;
  /** 詳細解説（複数行可） */
  detail: string;
  /** 単位（例: "Hz", "ms", "%"） */
  unit?: string;
}

// ---------- SynthEngine 公開API ----------
// Session A（音声エンジン）が実装。他セッションはこの I/F だけを参照する。

export interface ISynthEngine {
  /**
   * AudioContext を初期化する。ユーザー操作後に1度だけ呼ぶこと。
   * 解決後は noteOn 等が呼び出し可能になる。
   */
  init(): Promise<void>;

  /** ノートオン（MIDIノート番号、velocity は 0〜1） */
  noteOn(midiNote: number, velocity?: number): void;

  /** ノートオフ */
  noteOff(midiNote: number): void;

  /** オシレーターパラメータを更新 */
  setOscillatorParams(params: Partial<OscillatorParams>): void;
  /** フィルターパラメータを更新 */
  setFilterParams(params: Partial<FilterParams>): void;
  /** エンベロープパラメータを更新（次回ノートオンから適用） */
  setEnvelopeParams(params: Partial<EnvelopeParams>): void;
  /** LFOパラメータを更新 */
  setLFOParams(params: Partial<LFOParams>): void;

  /** マスター音量（0〜1） */
  setMasterVolume(volume: number): void;

  /**
   * 波形表示・スペクトラム表示用の AnalyserNode を返す。
   * 同一インスタンスを返してよい（Visualizer 側で connect 済み前提）。
   */
  getAnalyserNode(): AnalyserNode;

  /** 現在の AudioContext を返す（高度な用途用） */
  getAudioContext(): AudioContext;

  /** リソース解放 */
  dispose(): void;
}

// ---------- UIコンポーネント Props ----------

export interface KnobProps {
  /** パラメータ識別子（解説パネルとの紐付け用、例: "filter.cutoff"） */
  paramId: string;
  /** 表示ラベル */
  label: string;
  /** 現在値 */
  value: number;
  /** 最小値 */
  min: number;
  /** 最大値 */
  max: number;
  /** ステップ */
  step?: number;
  /** 単位表示（例: "Hz", "ms"） */
  unit?: string;
  /**
   * 値が変化したときに呼ばれる。
   * 連続的なドラッグ中も発火する。
   */
  onChange: (value: number) => void;
  /** ホバー時に paramId を通知（解説パネル更新用） */
  onHover?: (paramId: string | null) => void;
  /**
   * 値の対数スケール表示（周波数など）
   * デフォルト false（リニア）
   */
  logarithmic?: boolean;
}

export interface SectionProps {
  /**
   * 各セクションは Engine 参照を受け取り、setXxxParams を呼ぶ。
   * Session D が React Context 経由で取得する設計にする。
   */
  onParamHover: (paramId: string | null) => void;
}
