# Synthesizer Practice — Web 版（凍結）

ブラウザで動作するシンセサイザー学習アプリ。

## 状態

このディレクトリは **凍結** されています。今後の機能追加はデスクトップ版（`../desktop/`）で行います。
Vercel デプロイは継続して動作します。

公開 URL: https://synthesizer-practice.vercel.app/

## 技術スタック

- Vite + React + TypeScript
- Web Audio API（ネイティブ）
- Canvas（ビジュアライザ）

## ビルド・開発

```powershell
cd web
npm install
npm run dev      # 開発サーバ
npm run build    # 本番ビルド
```

## Vercel 設定

リポジトリのルートが `synthesizer_practice/` に変更されたため、Vercel ダッシュボードで **Root Directory** を `web` に設定する必要があります。

- Vercel Dashboard → Project Settings → General → Root Directory に `web` を入力
- 設定後の最初のデプロイで反映されます

## 機能一覧

- 11セクション: Oscillator / Noise / Filter / Filter Env / Amp Env / LFO / Drive / Delay / Reverb / Voice / Master
- 10プリセット
- 全パラメータに解説、ホバー連動
- 時間波形・スペクトラム表示
