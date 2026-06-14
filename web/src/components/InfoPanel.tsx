// TODO: Session D 担当 — 解説パネル
//
// ホバー中のパラメータの解説を src/data/descriptions.ts から検索して表示する。

import { descriptions } from '../data/descriptions';

export function InfoPanel({ paramId }: { paramId: string | null }) {
  const desc = paramId ? descriptions.find((d) => d.id === paramId) : null;

  return (
    <div
      style={{
        background: 'var(--color-panel)',
        border: '1px solid var(--color-panel-border)',
        borderRadius: '4px',
        padding: '12px',
        minHeight: '80px',
        fontSize: '13px',
      }}
    >
      {desc ? (
        <>
          <div style={{ fontWeight: 600, marginBottom: '4px' }}>
            {desc.name}
            {desc.unit ? `（${desc.unit}）` : ''}
          </div>
          <div style={{ color: 'var(--color-text-dim)', marginBottom: '4px' }}>
            {desc.summary}
          </div>
          <div style={{ whiteSpace: 'pre-wrap' }}>{desc.detail}</div>
        </>
      ) : (
        <div style={{ color: 'var(--color-text-dim)' }}>
          つまみにカーソルを合わせると、ここに解説が表示されます。
        </div>
      )}
    </div>
  );
}
