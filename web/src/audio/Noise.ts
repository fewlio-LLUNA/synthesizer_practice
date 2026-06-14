// ホワイト/ピンクのノイズバッファを生成し、AudioBufferSourceNode でループ再生する。
//
// バッファ生成は CPU を食うため、エンジン初期化時に1回だけ作って使い回す。

import type { NoiseType } from '../types';

/** 2秒分のバッファをループさせる。長すぎず短すぎず、繰り返しが耳に付かない長さ */
const BUFFER_DURATION_SEC = 2;

/**
 * 指定タイプのノイズを格納した AudioBuffer を生成する。
 * @param ctx AudioContext
 * @param type 'white' or 'pink'
 */
export function createNoiseBuffer(ctx: AudioContext, type: NoiseType): AudioBuffer {
  const sampleRate = ctx.sampleRate;
  const length = Math.floor(sampleRate * BUFFER_DURATION_SEC);
  const buffer = ctx.createBuffer(1, length, sampleRate);
  const data = buffer.getChannelData(0);

  if (type === 'white') {
    for (let i = 0; i < length; i++) {
      data[i] = Math.random() * 2 - 1;
    }
    return buffer;
  }

  // ピンクノイズ (Paul Kellet's refined algorithm)
  // ホワイトノイズに対し、-3dB/oct の低音重心スペクトルを実現する。
  let b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0;
  for (let i = 0; i < length; i++) {
    const white = Math.random() * 2 - 1;
    b0 = 0.99886 * b0 + white * 0.0555179;
    b1 = 0.99332 * b1 + white * 0.0750759;
    b2 = 0.96900 * b2 + white * 0.1538520;
    b3 = 0.86650 * b3 + white * 0.3104856;
    b4 = 0.55000 * b4 + white * 0.5329522;
    b5 = -0.7616 * b5 - white * 0.0168980;
    data[i] = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362) * 0.11;
    b6 = white * 0.115926;
  }
  return buffer;
}

/** 1ボイス分のノイズソース。バッファは外部から渡してもらい使い回す */
export class Noise {
  private readonly source: AudioBufferSourceNode;

  constructor(ctx: AudioContext, buffer: AudioBuffer) {
    this.source = ctx.createBufferSource();
    this.source.buffer = buffer;
    this.source.loop = true;
  }

  get outputNode(): AudioNode { return this.source; }

  start(when: number): void { this.source.start(when); }

  stop(when: number): void {
    try { this.source.stop(when); } catch { /* 停止済み */ }
  }
}
