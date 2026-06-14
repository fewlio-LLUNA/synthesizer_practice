import { useRef, useCallback } from 'react';
import type { KnobProps } from '../types';
import '../styles/knob.css';

// ダイアル中心・半径の定数
const CX = 24;
const CY = 24;
const KNOB_R = 17;   // ノブ本体の半径
const TRACK_R = 21;  // 値アークの半径
const IND_R = 13;    // インジケーター線の長さ

// 最小・最大角度（12時を0度として時計回り正）
const MIN_ANGLE = -135;
const MAX_ANGLE = 135;

// フルレンジに対応するドラッグ量（px）
const DRAG_FULL_PX = 180;

// 角度 → SVG座標変換（12時を0とした時計回り角度）
function toXY(angleDeg: number, r: number): { x: number; y: number } {
  const rad = ((angleDeg - 90) * Math.PI) / 180;
  return { x: CX + r * Math.cos(rad), y: CY + r * Math.sin(rad) };
}

// SVGの弧パス文字列を生成（時計回り）
function arcPath(startDeg: number, endDeg: number, r: number): string {
  const s = toXY(startDeg, r);
  const e = toXY(endDeg, r);
  const largeArc = endDeg - startDeg > 180 ? 1 : 0;
  return (
    `M ${s.x.toFixed(2)} ${s.y.toFixed(2)} ` +
    `A ${r} ${r} 0 ${largeArc} 1 ${e.x.toFixed(2)} ${e.y.toFixed(2)}`
  );
}

// 値 → 正規化値 [0, 1]
function valueToNorm(value: number, min: number, max: number, log: boolean): number {
  if (log) {
    const safeMin = Math.max(min, 1e-10);
    const safeVal = Math.max(value, 1e-10);
    return Math.log(safeVal / safeMin) / Math.log(max / safeMin);
  }
  return (value - min) / (max - min);
}

// 正規化値 [0, 1] → 値
function normToValue(norm: number, min: number, max: number, log: boolean): number {
  if (log) {
    const safeMin = Math.max(min, 1e-10);
    return safeMin * Math.pow(max / safeMin, norm);
  }
  return min + norm * (max - min);
}

function clamp(v: number, lo: number, hi: number): number {
  return Math.max(lo, Math.min(hi, v));
}

function applyStep(value: number, step: number | undefined): number {
  if (!step) return value;
  return Math.round(value / step) * step;
}

// 表示用フォーマット（1000以上はkHz表記）
function formatValue(value: number, step: number | undefined): string {
  if (value >= 1000) return `${(value / 1000).toFixed(1)}k`;
  if (!step || step < 0.01) return value.toFixed(2);
  if (step < 0.1) return value.toFixed(2);
  if (step < 1) return value.toFixed(1);
  return Math.round(value).toString();
}

export function Knob({
  paramId,
  label,
  value,
  min,
  max,
  step,
  unit,
  onChange,
  onHover,
  logarithmic = false,
}: KnobProps) {
  const dragRef = useRef<{ startY: number; startNorm: number } | null>(null);

  const norm = clamp(valueToNorm(value, min, max, logarithmic), 0, 1);
  const currentAngle = MIN_ANGLE + norm * (MAX_ANGLE - MIN_ANGLE);
  const indEnd = toXY(currentAngle, IND_R);

  const handleMouseDown = useCallback(
    (e: React.MouseEvent) => {
      e.preventDefault();
      const startNorm = clamp(valueToNorm(value, min, max, logarithmic), 0, 1);
      dragRef.current = { startY: e.clientY, startNorm };

      const onMove = (ev: MouseEvent) => {
        if (!dragRef.current) return;
        // Shiftキーでファイン操作（1/10 の感度）
        const sensitivity = ev.shiftKey ? 0.1 : 1.0;
        const deltaY = ev.clientY - dragRef.current.startY;
        const deltaNorm = (-deltaY / DRAG_FULL_PX) * sensitivity;
        const newNorm = clamp(dragRef.current.startNorm + deltaNorm, 0, 1);
        const raw = normToValue(newNorm, min, max, logarithmic);
        const newValue = clamp(applyStep(raw, step), min, max);
        onChange(newValue);
      };

      const onUp = () => {
        dragRef.current = null;
        document.removeEventListener('mousemove', onMove);
        document.removeEventListener('mouseup', onUp);
      };

      document.addEventListener('mousemove', onMove);
      document.addEventListener('mouseup', onUp);
    },
    [value, min, max, step, logarithmic, onChange],
  );

  // ダブルクリックで中間値にリセット（オプション機能）
  const handleDoubleClick = useCallback(() => {
    const mid = clamp(applyStep(normToValue(0.5, min, max, logarithmic), step), min, max);
    onChange(mid);
  }, [min, max, step, logarithmic, onChange]);

  return (
    <div
      className="knob"
      onMouseEnter={() => onHover?.(paramId)}
      onMouseLeave={() => onHover?.(null)}
    >
      <svg
        className="knob__dial"
        width="48"
        height="48"
        viewBox="0 0 48 48"
        onMouseDown={handleMouseDown}
        onDoubleClick={handleDoubleClick}
      >
        {/* ノブ本体（黒ベース） */}
        <circle
          cx={CX}
          cy={CY}
          r={KNOB_R}
          fill="var(--color-knob)"
          stroke="var(--color-panel-border)"
          strokeWidth="1.5"
        />
        {/* レンジトラック（全体、暗色） */}
        <path
          d={arcPath(MIN_ANGLE, MAX_ANGLE, TRACK_R)}
          fill="none"
          stroke="var(--color-panel-border)"
          strokeWidth="3"
          strokeLinecap="round"
        />
        {/* 現在値のオレンジアーク */}
        {norm > 0.001 && (
          <path
            d={arcPath(MIN_ANGLE, currentAngle, TRACK_R)}
            fill="none"
            stroke="var(--color-accent)"
            strokeWidth="3"
            strokeLinecap="round"
          />
        )}
        {/* インジケーター線 */}
        <line
          x1={CX}
          y1={CY}
          x2={indEnd.x.toFixed(2)}
          y2={indEnd.y.toFixed(2)}
          stroke="var(--color-knob-indicator)"
          strokeWidth="2.5"
          strokeLinecap="round"
        />
      </svg>
      <span className="knob__label">{label}</span>
      <span className="knob__value">
        {formatValue(value, step)}
        {unit ?? ''}
      </span>
    </div>
  );
}
