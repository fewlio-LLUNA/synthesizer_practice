import { useEffect, useRef } from 'react';
import { useSynthEngine } from '../audio/engineContext';

const CONTAINER_STYLE: React.CSSProperties = {
  position: 'relative',
  background: 'var(--color-panel)',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '4px',
  padding: '8px',
  height: '160px',
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

export function WaveformView() {
  const engine = useSynthEngine();
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const rafRef = useRef<number>(0);

  // Canvasの描画バッファをCSSサイズに同期する
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const observer = new ResizeObserver(entries => {
      for (const entry of entries) {
        canvas.width = Math.floor(entry.contentRect.width);
        canvas.height = Math.floor(entry.contentRect.height);
      }
    });
    observer.observe(canvas);

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
    <div style={CONTAINER_STYLE}>
      <canvas
        ref={canvasRef}
        style={{ width: '100%', height: '100%', display: 'block' }}
      />
      {!engine && <div style={OVERLAY_STYLE}>未接続</div>}
    </div>
  );
}
