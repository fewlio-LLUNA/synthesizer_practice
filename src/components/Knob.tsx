// TODO: Session B 担当 — 汎用つまみコンポーネント
//
// マウス縦ドラッグで値を増減。Shift+ドラッグでファイン操作。
// ハードシンセ風の見た目（黒パネルにオレンジ色のインジケータ）。
// 公開 props は src/types.ts の KnobProps を厳守すること（Session D が依存）。

import type { KnobProps } from '../types';

export function Knob(props: KnobProps) {
  // スタブ実装：見た目だけの最小版。
  // Session B が実際のドラッグハンドリングと回転アニメーションを実装する。
  return (
    <div
      className="knob-stub"
      onMouseEnter={() => props.onHover?.(props.paramId)}
      onMouseLeave={() => props.onHover?.(null)}
      style={{
        display: 'inline-flex',
        flexDirection: 'column',
        alignItems: 'center',
        padding: '4px',
        minWidth: '60px',
      }}
    >
      <div
        style={{
          width: '40px',
          height: '40px',
          borderRadius: '50%',
          background: 'var(--color-knob)',
          border: '2px solid var(--color-panel-border)',
        }}
      />
      <label style={{ fontSize: '10px', marginTop: '4px' }}>{props.label}</label>
      <span style={{ fontSize: '10px', color: 'var(--color-text-dim)' }}>
        {props.value.toFixed(2)}
        {props.unit ?? ''}
      </span>
      <input
        type="range"
        min={props.min}
        max={props.max}
        step={props.step ?? (props.max - props.min) / 100}
        value={props.value}
        onChange={(e) => props.onChange(Number(e.target.value))}
        style={{ width: '60px' }}
      />
    </div>
  );
}
