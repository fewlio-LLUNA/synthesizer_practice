// TODO: Session C 担当 — 時間波形ビジュアライザ
//
// SynthEngine.getAnalyserNode() から getByteTimeDomainData() でデータを取得し、
// Canvas に折れ線で描画する。requestAnimationFrame でループ。

export function WaveformView() {
  // スタブ：プレースホルダ表示のみ。
  return (
    <div
      style={{
        background: 'var(--color-panel)',
        border: '1px solid var(--color-panel-border)',
        borderRadius: '4px',
        padding: '8px',
        height: '160px',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        color: 'var(--color-text-dim)',
        fontSize: '12px',
      }}
    >
      [時間波形 — Session C で実装]
    </div>
  );
}
