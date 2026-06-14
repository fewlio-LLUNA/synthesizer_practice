import { useEffect, useRef } from 'react';
import { useSynthEngine } from '../audio/engineContext';

// 描画対象の周波数範囲
const F_MIN = 20;
const F_MAX = 20000;
const LOG_RATIO = Math.log(F_MAX / F_MIN);

const CONTAINER_STYLE: React.CSSProperties = {
  position: 'relative',
  background: 'var(--color-panel)',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '4px',
  padding: '8px',
  height: '160px',
  display: 'flex',
  flexDirection: 'column',
  // grid item として canvas の自然サイズに引きずられないようにする
  minWidth: 0,
  overflow: 'hidden',
};

const HEADER_STYLE: React.CSSProperties = {
  display: 'flex',
  alignItems: 'baseline',
  gap: '6px',
  marginBottom: '4px',
};

const TITLE_STYLE: React.CSSProperties = {
  fontSize: '11px',
  fontWeight: 600,
  color: 'var(--color-accent)',
  letterSpacing: '0.05em',
};

const CAPTION_STYLE: React.CSSProperties = {
  fontSize: '10px',
  color: 'var(--color-text-dim)',
};

const CANVAS_AREA_STYLE: React.CSSProperties = {
  position: 'relative',
  flex: 1,
  // 子の canvas の自然サイズに引きずられないようにする
  minHeight: 0,
  minWidth: 0,
};

const OVERLAY_STYLE: React.CSSProperties = {
  position: 'absolute',
  inset: 0,
  display: 'flex',
  alignItems: 'center',
  justifyContent: 'center',
  color: 'var(--color-text-dim)',
  fontSize: '12px',
};

/**
 * 画面上の x 位置（0〜1）を周波数（Hz）に変換する（対数スケール）
 */
function xToFrequency(normalizedX: number): number {
  return F_MIN * Math.exp(LOG_RATIO * normalizedX);
}

interface Props {
  onParamHover?: (paramId: string | null) => void;
}

export function SpectrumView({ onParamHover }: Props = {}) {
  const engine = useSynthEngine();
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const canvasAreaRef = useRef<HTMLDivElement>(null);
  const rafRef = useRef<number>(0);

  // canvasの描画バッファを親エリアのサイズに同期する。
  // canvas自体を観測すると、内在サイズとレイアウトが循環して中央がずれる。
  useEffect(() => {
    const canvas = canvasRef.current;
    const area = canvasAreaRef.current;
    if (!canvas || !area) return;

    const observer = new ResizeObserver(entries => {
      for (const entry of entries) {
        const w = Math.max(1, Math.floor(entry.contentRect.width));
        const h = Math.max(1, Math.floor(entry.contentRect.height));
        canvas.width = w;
        canvas.height = h;
      }
    });
    observer.observe(area);

    return () => observer.disconnect();
  }, []);

  // engine が有効になったらアニメーションループを開始する
  useEffect(() => {
    if (!engine) return;

    const canvas = canvasRef.current;
    if (!canvas) return;

    const analyser = engine.getAnalyserNode();
    const bufferLength = analyser.frequencyBinCount;
    const dataArray = new Uint8Array(bufferLength);
    const nyquist = analyser.context.sampleRate / 2;

    const draw = () => {
      rafRef.current = requestAnimationFrame(draw);
      analyser.getByteFrequencyData(dataArray);

      const ctx = canvas.getContext('2d');
      if (!ctx) return;

      const { width, height } = canvas;

      ctx.fillStyle = '#1a1a1a';
      ctx.fillRect(0, 0, width, height);

      // 対数軸で各ピクセル列にバーを描画する
      for (let x = 0; x < width; x++) {
        const freq = xToFrequency(x / width);
        const binIndex = Math.min(
          Math.round((freq / nyquist) * (bufferLength - 1)),
          bufferLength - 1,
        );
        const value = dataArray[binIndex];
        const barHeight = (value / 255) * height;

        // 低音（左）をオレンジ、高音（右）を黄色に近い色でグラデーション
        const hue = 25 + (x / width) * 20;
        const lightness = 40 + (value / 255) * 20;
        ctx.fillStyle = `hsl(${hue}, 100%, ${lightness}%)`;
        ctx.fillRect(x, height - barHeight, 1, barHeight);
      }
    };

    draw();

    return () => cancelAnimationFrame(rafRef.current);
  }, [engine]);

  return (
    <div
      style={CONTAINER_STYLE}
      onMouseEnter={() => onParamHover?.('visualizer.spectrum')}
      onMouseLeave={() => onParamHover?.(null)}
    >
      <div style={HEADER_STYLE}>
        <span style={TITLE_STYLE}>周波数スペクトラム</span>
        <span style={CAPTION_STYLE}>— 含まれる倍音の分布（左=低音、右=高音）</span>
      </div>
      <div ref={canvasAreaRef} style={CANVAS_AREA_STYLE}>
        <canvas
          ref={canvasRef}
          style={{ width: '100%', height: '100%', display: 'block' }}
        />
        {!engine && <div style={OVERLAY_STYLE}>未接続</div>}
      </div>
    </div>
  );
}
