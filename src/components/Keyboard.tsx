// TODO: Session E 担当 — 仮想鍵盤
//
// 画面下部に1〜2オクターブの鍵盤を表示。
// マウスクリック・タップに加え、PCキーボード（A,W,S,E,D...）でも演奏可能にする。
// SynthEngine の noteOn/noteOff を呼ぶ。useSynthEngine() で取得する。

export function Keyboard() {
  // スタブ：プレースホルダ表示のみ。
  return (
    <div
      style={{
        background: 'var(--color-panel)',
        border: '1px solid var(--color-panel-border)',
        borderRadius: '4px',
        padding: '16px',
        textAlign: 'center',
        color: 'var(--color-text-dim)',
        fontSize: '12px',
      }}
    >
      [Keyboard プレースホルダ — Session E で実装]
    </div>
  );
}
