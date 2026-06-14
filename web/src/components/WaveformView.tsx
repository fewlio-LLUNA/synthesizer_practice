import { useEffect, useRef } from 'react';
import { useSynthEngine } from '../audio/engineContext';

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

interface Props {
  onParamHover?: (paramId: string | null) => void;
}

export function WaveformView({ onParamHover }: Props = {}) {
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
    const dataArray = new Uint8Array(analyser.fftSize);

    const draw = () => {
      rafRef.current = requestAnimationFrame(draw);
      analyser.getByteTimeDomainData(dataArray);

      const ctx = canvas.getContext('2d');
      if (!ctx) return;

      const { width, height } = canvas;

      ctx.fillStyle = '#1a1a1a';
      ctx.fillRect(0, 0, width, height);

      // 中央ガイドライン
      ctx.strokeStyle = '#333';
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(0, height / 2);
      ctx.lineTo(width, height / 2);
      ctx.stroke();

      // 時間波形
      ctx.strokeStyle = 'var(--color-accent, #ff8c00)';
      ctx.lineWidth = 2;
      ctx.lineJoin = 'round';
      ctx.beginPath();

      const sliceWidth = width / dataArray.length;
      for (let i = 0; i < dataArray.length; i++) {
        const y = (dataArray[i] / 128.0) * (height / 2);
        if (i === 0) ctx.moveTo(0, y);
        else ctx.lineTo(i * sliceWidth, y);
      }

      ctx.stroke();
    };

    draw();

    return () => cancelAnimationFrame(rafRef.current);
  }, [engine]);

  return (
    <div
      style={CONTAINER_STYLE}
      onMouseEnter={() => onParamHover?.('visualizer.waveform')}
      onMouseLeave={() => onParamHover?.(null)}
    >
      <div style={HEADER_STYLE}>
        <span style={TITLE_STYLE}>時間波形</span>
        <span style={CAPTION_STYLE}>— 波の「形」を時間軸で表示</span>
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
