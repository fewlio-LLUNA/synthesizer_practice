// TODO: Session D 担当 — LFOセクション
//
// Rate / Depth ノブ、波形選択、ターゲット選択（Pitch/Filter/Amp）を配置。

import type { SectionProps } from '../../types';

export function LFOSection(_props: SectionProps) {
  return (
    <div
      style={{
        background: 'var(--color-panel)',
        border: '1px solid var(--color-panel-border)',
        borderRadius: '4px',
        padding: '12px',
        minHeight: '160px',
      }}
    >
      <h3
        style={{
          margin: '0 0 8px 0',
          fontSize: '12px',
          fontWeight: 600,
          letterSpacing: '0.1em',
          color: 'var(--color-accent)',
        }}
      >
        LFO
      </h3>
      <div style={{ color: 'var(--color-text-dim)', fontSize: '11px' }}>
        [Session D で実装]
      </div>
    </div>
  );
}
