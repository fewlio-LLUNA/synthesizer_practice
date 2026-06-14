// WaveShaperNode を使った歪みエフェクト。
//
// 信号フロー:
//   input → dryGain ──────────────────────────────→ output
//         → inputBoost → shaper → outputComp → wetGain → output
//
// drive を上げると inputBoost で信号レベルが上がり、shaper で大きくクリッピングする。
// outputComp で出力レベルを補正し、音量バランスを保つ。

import type { DriveParams, DriveType } from '../types';

const CURVE_SIZE = 4096;

/** 新規 ArrayBuffer 上に Float32Array を作る。
 *  WaveShaperNode.curve は Float32Array<ArrayBuffer> 型を要求するため、
 *  既定の Float32Array<ArrayBufferLike> ではなく明示的に ArrayBuffer を使う。 */
function createCurve(size: number): Float32Array<ArrayBuffer> {
  return new Float32Array(new ArrayBuffer(size * Float32Array.BYTES_PER_ELEMENT));
}

/** tanh ベースの soft clip カーブ。なめらかで温かみのある歪み */
function makeSoftClipCurve(drive: number): Float32Array<ArrayBuffer> {
  const curve = createCurve(CURVE_SIZE);
  // k は急峻さ。drive=0 で 1（ほぼリニア）、drive=1 で 50（強い歪み）
  const k = 1 + drive * 49;
  for (let i = 0; i < CURVE_SIZE; i++) {
    const x = (i * 2) / CURVE_SIZE - 1;
    curve[i] = Math.tanh(k * x);
  }
  return curve;
}

/** 閾値カットの hard clip カーブ。攻撃的なディストーション */
function makeHardClipCurve(drive: number): Float32Array<ArrayBuffer> {
  const curve = createCurve(CURVE_SIZE);
  // drive=0 で閾値 1.0（クリッピングなし）、drive=1 で 0.05（強烈にクリップ）
  const threshold = Math.max(0.05, 1 - drive * 0.95);
  for (let i = 0; i < CURVE_SIZE; i++) {
    const x = (i * 2) / CURVE_SIZE - 1;
    const clipped = Math.max(-threshold, Math.min(threshold, x));
    // クリップ後のレベルを 1 に正規化
    curve[i] = clipped / threshold;
  }
  return curve;
}

export class Drive {
  private readonly input: GainNode;
  private readonly output: GainNode;
  private readonly inputBoost: GainNode;
  private readonly shaper: WaveShaperNode;
  private readonly outputComp: GainNode;
  private readonly dryGain: GainNode;
  private readonly wetGain: GainNode;

  private type: DriveType;
  private drive: number;

  constructor(ctx: AudioContext, params: DriveParams) {
    this.type = params.type;
    this.drive = params.drive;

    this.input = ctx.createGain();
    this.output = ctx.createGain();
    this.inputBoost = ctx.createGain();
    this.shaper = ctx.createWaveShaper();
    this.shaper.oversample = '4x'; // エイリアシングを低減
    this.outputComp = ctx.createGain();
    this.dryGain = ctx.createGain();
    this.wetGain = ctx.createGain();

    // ドライ経路
    this.input.connect(this.dryGain);
    this.dryGain.connect(this.output);
    // ウェット経路
    this.input.connect(this.inputBoost);
    this.inputBoost.connect(this.shaper);
    this.shaper.connect(this.outputComp);
    this.outputComp.connect(this.wetGain);
    this.wetGain.connect(this.output);

    this.applyDrive();
    this.setMix(params.mix);
  }

  get inputNode(): AudioNode { return this.input; }
  get outputNode(): AudioNode { return this.output; }

  setType(type: DriveType): void {
    this.type = type;
    this.applyDrive();
  }

  setDrive(drive: number): void {
    this.drive = drive;
    this.applyDrive();
  }

  setMix(mix: number): void {
    const clamped = Math.max(0, Math.min(1, mix));
    this.dryGain.gain.value = 1 - clamped;
    this.wetGain.gain.value = clamped;
  }

  /** 現在の type と drive に応じて curve とゲイン補正を更新する */
  private applyDrive(): void {
    this.shaper.curve =
      this.type === 'hard'
        ? makeHardClipCurve(this.drive)
        : makeSoftClipCurve(this.drive);
    // 入力ブースト（drive を上げるほど信号を大きくして歪ませる）
    this.inputBoost.gain.value = 1 + this.drive * 10;
    // 出力補正（音量が暴れないように下げる）
    this.outputComp.gain.value = 1 / (1 + this.drive * 3);
  }

  dispose(): void {
    try { this.input.disconnect(); } catch { /* 無視 */ }
    try { this.inputBoost.disconnect(); } catch { /* 無視 */ }
    try { this.shaper.disconnect(); } catch { /* 無視 */ }
    try { this.outputComp.disconnect(); } catch { /* 無視 */ }
    try { this.dryGain.disconnect(); } catch { /* 無視 */ }
    try { this.wetGain.disconnect(); } catch { /* 無視 */ }
    try { this.output.disconnect(); } catch { /* 無視 */ }
  }
}
