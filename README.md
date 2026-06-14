# synthesizer_practice

DTMにおけるシンセサイザーを体験的に学ぶアプリ。

このリポジトリには **2つの実装** が含まれています。

| 実装 | 場所 | 状態 | 技術スタック |
|---|---|---|---|
| **Web 版** | [`web/`](./web) | 凍結（メンテのみ） | Vite + React + TypeScript + Web Audio API |
| **Desktop 版** | [`desktop/`](./desktop) | **開発中（主軸）** | C++ + JUCE + CMake（スタンドアロン + VST3） |

## Web 版

ブラウザで動作するシンセ。Vercel にデプロイ済み:
https://synthesizer-practice.vercel.app/

詳しくは [`web/README.md`](./web/README.md) を参照。

## Desktop 版

JUCE を使った C++ 製のネイティブシンセ。スタンドアロン `.exe` と VST3 プラグインを同時に生成。

- 計画書: [`docs/desktop-plan.md`](./docs/desktop-plan.md)
- 詳細設計書: [`docs/desktop-design.md`](./docs/desktop-design.md)
- ビルド方法・並行開発手順: [`desktop/README.md`](./desktop/README.md)
- タスク一覧: [`desktop/TASKS.md`](./desktop/TASKS.md)

## ライセンス

- ソースコード: MIT
- JUCE: Personal License（個人利用無料）
