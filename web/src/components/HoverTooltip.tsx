// ホバー中のパラメータ解説を、カーソル近くに吹き出し（ツールチップ）として表示する。
// 旧 InfoPanel（画面下部の固定枠）に代わり、対象物のそばに追従表示する。

import { CSSProperties, useEffect, useLayoutEffect, useRef, useState } from 'react';
import { descriptions } from '../data/descriptions';

// カーソルと吹き出しの間隔（px）
const GAP = 14;
// 画面端から確保する余白（px）
const EDGE_MARGIN = 8;
// 吹き出しの最大幅（px）
const MAX_WIDTH = 320;
// 吹き出しの三角（矢印）の大きさ（px）
const ARROW_SIZE = 7;

function clamp(v: number, lo: number, hi: number): number {
  return Math.max(lo, Math.min(hi, v));
}

interface Point {
  x: number;
  y: number;
}

interface Size {
  w: number;
  h: number;
}

export function HoverTooltip({ paramId }: { paramId: string | null }) {
  const desc = paramId ? descriptions.find((d) => d.id === paramId) : null;
  const ref = useRef<HTMLDivElement>(null);
  // 最新のカーソル座標を常時記録する（再レンダーを伴わない）
  const posRef = useRef<Point>({ x: 0, y: 0 });
  const [mouse, setMouse] = useState<Point>({ x: 0, y: 0 });
  const [size, setSize] = useState<Size>({ w: 0, h: 0 });

  // カーソル座標は常に ref に記録しておく。
  // これにより、ホバー開始直後（まだ mousemove が来る前）でも正しい位置に出せる。
  useEffect(() => {
    const onMove = (e: MouseEvent) => {
      posRef.current = { x: e.clientX, y: e.clientY };
    };
    window.addEventListener('mousemove', onMove);
    return () => window.removeEventListener('mousemove', onMove);
  }, []);

  // ホバー中だけカーソル座標を state に反映し、吹き出しを追従させる。
  useEffect(() => {
    if (!desc) return;
    setMouse(posRef.current); // 表示開始位置を即座に確定
    const onMove = (e: MouseEvent) => setMouse({ x: e.clientX, y: e.clientY });
    window.addEventListener('mousemove', onMove);
    return () => window.removeEventListener('mousemove', onMove);
  }, [desc]);

  // 内容が変わるたびに実寸を測る（画面端での反転判定に使う）。
  // useLayoutEffect なので描画前にサイズが確定し、位置のちらつきを防ぐ。
  useLayoutEffect(() => {
    if (ref.current) {
      setSize({ w: ref.current.offsetWidth, h: ref.current.offsetHeight });
    }
  }, [desc]);

  if (!desc) return null;

  // 横方向: 右に出すと画面外なら左へ反転
  const placeLeft = mouse.x + GAP + size.w > window.innerWidth - EDGE_MARGIN;
  // 縦方向: 下に出すと画面外なら上へ反転
  const placeAbove = mouse.y + GAP + size.h > window.innerHeight - EDGE_MARGIN;

  let left = placeLeft ? mouse.x - GAP - size.w : mouse.x + GAP;
  let top = placeAbove ? mouse.y - GAP - size.h : mouse.y + GAP;

  // 画面端からはみ出さないよう補正
  left = clamp(left, EDGE_MARGIN, window.innerWidth - size.w - EDGE_MARGIN);
  top = clamp(top, EDGE_MARGIN, window.innerHeight - size.h - EDGE_MARGIN);

  // 矢印はカーソルの方を指す（吹き出しの上端 or 下端のエッジに置く）
  const arrowLeft = clamp(mouse.x - left, ARROW_SIZE + 4, size.w - ARROW_SIZE - 4);

  return (
    <div ref={ref} style={{ ...bubbleStyle, left, top }}>
      <span style={arrowStyle(placeAbove, arrowLeft)} />
      <div style={{ fontWeight: 600, marginBottom: '4px' }}>
        {desc.name}
        {desc.unit ? `（${desc.unit}）` : ''}
      </div>
      <div style={{ color: 'var(--color-text-dim)', marginBottom: '4px' }}>
        {desc.summary}
      </div>
      <div style={{ whiteSpace: 'pre-wrap' }}>{desc.detail}</div>
    </div>
  );
}

const bubbleStyle: CSSProperties = {
  position: 'fixed',
  zIndex: 1000,
  maxWidth: `${MAX_WIDTH}px`,
  background: 'var(--color-panel)',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '6px',
  padding: '10px 12px',
  fontSize: '13px',
  lineHeight: 1.5,
  boxShadow: '0 6px 20px rgba(0, 0, 0, 0.45)',
  // 吹き出し自身がホバー判定を奪うとちらつくため、ポインタ操作を透過させる
  pointerEvents: 'none',
};

// カーソル側のエッジに付く三角形。placeAbove のときは下端、それ以外は上端に出す。
function arrowStyle(placeAbove: boolean, arrowLeft: number): CSSProperties {
  const base: CSSProperties = {
    position: 'absolute',
    left: arrowLeft - ARROW_SIZE,
    width: 0,
    height: 0,
    borderLeft: `${ARROW_SIZE}px solid transparent`,
    borderRight: `${ARROW_SIZE}px solid transparent`,
  };
  if (placeAbove) {
    // 吹き出しが上 → 矢印は下端で下向き
    return {
      ...base,
      bottom: -ARROW_SIZE,
      borderTop: `${ARROW_SIZE}px solid var(--color-panel-border)`,
    };
  }
  // 吹き出しが下 → 矢印は上端で上向き
  return {
    ...base,
    top: -ARROW_SIZE,
    borderBottom: `${ARROW_SIZE}px solid var(--color-panel-border)`,
  };
}
