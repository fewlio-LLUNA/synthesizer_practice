# synthesizer_practice

DTMにおけるシンセサイザーを体験的に学ぶ Web アプリ。

公開 URL: https://synthesizer-practice.vercel.app/

> **デスクトップ版（C++ + JUCE / Standalone + VST3）** は分離されました:
> https://github.com/fewlio-LLUNA/synthesizer_practice_win

## このリポジトリの内容

Vite + React + TypeScript + Web Audio API で書かれたブラウザ版シンセサイザー学習アプリです。

| 機能 | 内容 |
|---|---|
| セクション | 11個（Oscillator, Noise, Filter, Filter Env, Amp Env, LFO, Drive, Delay, Reverb, Voice, Master） |
| プリセット | 10種類（Basic Lead, Super Saw, Wobble Bass, Acid Bass, Dreamy Pad, Pluck, Stab, Strings, Hi-Hat, Wind FX） |
| ビジュアライザ | 時間波形 + 周波数スペクトラム（リアルタイム） |
| 解説 | 全パラメータ・全選択肢に日本語解説（ホバー表示） |
| 鍵盤 | クリック + PCキーボード（A,W,S,E,D...） |

## ファイル構成

```
synthesizer_practice/
├── web/                  # React 実装本体
│   ├── src/
│   ├── package.json
│   ├── vite.config.ts
│   ├── tsconfig.json
│   └── vercel.json
└── README.md
```

## 開発

```powershell
cd web
npm install
npm run dev      # 開発サーバ
npm run build    # 本番ビルド
```

## デプロイ

Vercel に GitHub 連携でデプロイ済み。`web/` を Root Directory に指定。
main ブランチに push すると自動デプロイされます。

## ライセンス

MIT
