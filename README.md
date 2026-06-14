# シンセサイザー学習アプリ

DTM におけるシンセサイザーを「つまみを回す → 波形が変わる → 音が変わる」の
三位一体で体験的に学べる Web アプリ。

## 技術スタック

- **Vite + React + TypeScript**
- **Web Audio API**（ネイティブ。Tone.js などの抽象化は使わない）
- 描画: Canvas API（AnalyserNode + requestAnimationFrame）

## セットアップ

```powershell
npm install
npm run dev
```

ブラウザで `http://localhost:5173` を開く。
※音を鳴らすには画面上で1度クリック等のユーザー操作が必要（Web Audio の仕様）。

## スクリプト

| コマンド | 内容 |
|---|---|
| `npm run dev` | 開発サーバー起動 |
| `npm run build` | 本番ビルド |
| `npm run preview` | ビルド成果物のプレビュー |
| `npm run typecheck` | TypeScript の型チェック（コミット前推奨） |

## ディレクトリ構成

```
src/
├── main.tsx              # エントリポイント
├── App.tsx               # ルートレイアウト（共有資産）
├── types.ts              # 全モジュールの公開I/F（共有資産）
├── audio/                # 音声エンジン（Session A）
│   ├── SynthEngine.ts
│   ├── Oscillator.ts
│   ├── Filter.ts
│   ├── Envelope.ts
│   ├── LFO.ts
│   └── engineContext.tsx (共有資産: React Context)
├── components/
│   ├── Knob.tsx          # Session B
│   ├── Keyboard.tsx      # Session E
│   ├── WaveformView.tsx  # Session C
│   ├── SpectrumView.tsx  # Session C
│   ├── InfoPanel.tsx     # Session D
│   └── sections/         # Session D
│       ├── OscillatorSection.tsx
│       ├── FilterSection.tsx
│       ├── EnvelopeSection.tsx
│       └── LFOSection.tsx
├── data/                 # Session E
│   ├── defaults.ts (共有資産)
│   ├── presets.ts
│   └── descriptions.ts
└── styles/
    └── global.css        # Session B
```

## 並行開発の進め方（Git worktree）

このプロジェクトは複数の Claude セッションが並行作業する前提で構成されている。

### 1. ワークツリーを切る（メインセッションが各セッションぶん用意）

```powershell
# 一度だけ実行（worktree 置き場を gitignore 済み）
mkdir worktrees -ErrorAction SilentlyContinue

# 各セッション用のブランチ＆作業ディレクトリを作成
git worktree add worktrees/session-A -b session-A/audio-engine
git worktree add worktrees/session-B -b session-B/knob
git worktree add worktrees/session-C -b session-C/visualizer
git worktree add worktrees/session-D -b session-D/sections
git worktree add worktrees/session-E -b session-E/keyboard-data
```

### 2. 各セッションを起動

各セッションは別の PowerShell ウィンドウで:

```powershell
cd C:\デスクトップ\programs\synthesizer_practice\worktrees\session-A
claude
```

セッション内で最初に渡すプロンプト例:

> あなたは Session A（音声エンジン）担当です。
> `TASKS.md` の「タスク A: 音声エンジン」を読み、受け入れ条件をすべて満たすように実装してください。
> 触ってよいファイル / 触ってはいけないファイルを厳守してください。
> 共有資産 `src/types.ts` の `ISynthEngine` インターフェースに準拠してください。
> 実装が終わったら `npm run typecheck` を通し、コミットしてください。

### 3. マージ

各セッションが完了したら、メインセッションが main に取り込む:

```powershell
cd C:\デスクトップ\programs\synthesizer_practice
git merge session-A/audio-engine --no-ff
git merge session-B/knob --no-ff
# ...以下同様
```

### 4. ワークツリー後片付け

```powershell
git worktree remove worktrees/session-A
git branch -d session-A/audio-engine  # マージ済みの場合
```

## 設計思想

- **学習目的**を優先するため、音声エンジンは Web Audio API を直接使う（Tone.js などは使わない）。
- 各パラメータは「数値の意味」と「人間の感覚での意味」を併記する（`src/data/descriptions.ts`）。
- 信号フロー: `Oscillator → Filter → VCA(Amp) → Master → Destination`、
  Envelope は VCA を、LFO は任意のパラメータを変調する。

## 詳細

タスクの粒度・受け入れ条件・触ってよい/いけないファイルは [TASKS.md](./TASKS.md) を参照。
