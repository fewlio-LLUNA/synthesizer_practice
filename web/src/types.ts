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
  /** オクターブシフト（-2〜+2）。1段で1オクターブ（1200セント）変化する */
  octave: number;
  /** オシレーターの音量（0〜1）。Noise とのミックス比に使う */
  level: number;
}

// ---------- ノイズジェネレータ ----------

export type NoiseType = 'white' | 'pink';

export interface NoiseParams {
  /** ノイズ種別 */
  type: NoiseType;
  /** ノイズの音量（0〜1）。0 で無効 */
  level: number;
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

/** フィルターエンベロープ。EnvelopeParams に効きの強さ amount を加えたもの */
export interface FilterEnvelopeParams extends EnvelopeParams {
  /**
   * エンベロープがフィルターカットオフに与える影響の強さ（-1〜+1）。
   * 0 で無効。正で「開く（明るく）」、負で「閉じる（暗く）」方向。
   * 1.0 で約5オクターブ分の周波数変化を与える。
   */
  amount: number;
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

// ---------- ユニゾン ----------

export interface UnisonParams {
  /** ユニゾンを構成するオシレーター数（1〜8）。1で無効（単一オシレーター） */
  count: number;
  /** デチューンの広がり（セント、0〜100）。両端の osc がこの幅で広がる */
  detune: number;
}

// ---------- 演奏設定（グライドなど） ----------

export interface PerformanceParams {
  /** グライド（ポルタメント）時間（秒、0〜2）。0で無効。
   *  前ノートの周波数から今ノートの周波数まで滑らかに移行する。 */
  glide: number;
}

// ---------- ドライブ／ディストーション ----------

export type DriveType = 'soft' | 'hard';

export interface DriveParams {
  /** 歪みの種類。soft はオーバードライブ風、hard はディストーション風 */
  type: DriveType;
  /** 歪みの強さ（0〜1）。0で無効、1で激しい歪み */
  drive: number;
  /** Wet/Dry ミックス（0〜1）。0でドライのみ */
  mix: number;
}

// ---------- ディレイ ----------

export interface DelayParams {
  /** ディレイタイム（秒、0.01〜2） */
  time: number;
  /** フィードバック量（0〜0.95）。1以上にすると発振するので上限を制限する */
  feedback: number;
  /** Wet/Dry ミックス（0〜1）。0で完全ドライ（無効）、1で完全ウェット */
  mix: number;
}

// ---------- リバーブ ----------

export interface ReverbParams {
  /** 減衰時間（秒、0.1〜10）。インパルス応答の長さ */
  decay: number;
  /** Wet/Dry ミックス（0〜1）。0で無効 */
  mix: number;
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
  // 以下は後付けのため optional（未指定なら現在値を維持）
  noise?: NoiseParams;
  filterEnvelope?: FilterEnvelopeParams;
  drive?: DriveParams;
  delay?: DelayParams;
  reverb?: ReverbParams;
  unison?: UnisonParams;
  performance?: PerformanceParams;
  /** マスター音量（0〜1）。未指定なら現在値を維持 */
  masterVolume?: number;
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
  /** ユニゾン設定を更新（count 変更は次回ノートオンから反映） */
  setUnisonParams(params: Partial<UnisonParams>): void;
  /** 演奏設定（グライドなど）を更新 */
  setPerformanceParams(params: Partial<PerformanceParams>): void;
  /** ノイズパラメータを更新（level は即時、type は次回ノートオンから） */
  setNoiseParams(params: Partial<NoiseParams>): void;
  /** フィルターパラメータを更新 */
  setFilterParams(params: Partial<FilterParams>): void;
  /** エンベロープパラメータを更新（次回ノートオンから適用） */
  setEnvelopeParams(params: Partial<EnvelopeParams>): void;
  /** フィルターエンベロープパラメータを更新（次回ノートオンから適用、amountは即時） */
  setFilterEnvelopeParams(params: Partial<FilterEnvelopeParams>): void;
  /** LFOパラメータを更新 */
  setLFOParams(params: Partial<LFOParams>): void;

  /** ドライブ（歪み）パラメータを更新 */
  setDriveParams(params: Partial<DriveParams>): void;
  /** ディレイパラメータを更新 */
  setDelayParams(params: Partial<DelayParams>): void;
  /** リバーブパラメータを更新（decay変更時はIRを再生成するためやや重い） */
  setReverbParams(params: Partial<ReverbParams>): void;

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
