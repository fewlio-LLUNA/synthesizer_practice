// TODO: Session D 担当 — オシレーターセクション
//
// 波形選択（ラジオボタン）と Detune ノブを配置。
// 値変更時に useSynthEngine() 経由で setOscillatorParams を呼ぶ。

import type { SectionProps } from '../../types';

export function OscillatorSection(_props: SectionProps) {
  return (
    <div className="section-panel" style={panelStyle}>
      <h3 style={titleStyle}>Oscillator</h3>
      <div style={{ color: 'var(--color-text-dim)', fontSize: '11px' }}>
        [Session D で実装]
      </div>
    </div>
  );
}

const panelStyle = {
  background: 'var(--color-panel)',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '4px',
  padding: '12px',
  minHeight: '160px',
} as const;

const titleStyle = {
  margin: '0 0 8px 0',
  fontSize: '12px',
  fontWeight: 600,
  letterSpacing: '0.1em',
  color: 'var(--color-accent)',
} as const;
