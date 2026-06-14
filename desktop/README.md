# Synthesizer Practice — Desktop (C++ / JUCE)

C++ + JUCE で書かれたシンセサイザー学習アプリ。スタンドアロン `.exe` と VST3 プラグインを同時に生成。

設計書: [`../docs/desktop-design.md`](../docs/desktop-design.md)
計画書: [`../docs/desktop-plan.md`](../docs/desktop-plan.md)

---

## 開発環境セットアップ

### 必須
- Visual Studio 2022 Community（「C++ によるデスクトップ開発」ワークロード）
- CMake 3.22 以上
- Git

### JUCE の取得（共通ローカルクローン推奨）

複数 worktree で並行作業する場合、各セッションで JUCE をダウンロードしないよう、
**共通の場所にクローン**しておくのを強く推奨します。

```powershell
git clone https://github.com/juce-framework/JUCE.git C:\dev\JUCE
cd C:\dev\JUCE
git checkout 8.0.4
```

その後、CMake に `-DJUCE_PATH=C:/dev/JUCE` を渡すか、環境変数 `JUCE_PATH` を設定。

```powershell
# 環境変数として設定（永続化したい場合は setx を使う）
$env:JUCE_PATH = "C:\dev\JUCE"
```

JUCE_PATH を指定しない場合は CMake が FetchContent でダウンロードしますが、初回ビルドが数十分〜遅くなります。

### JUCE Personal License
https://juce.com/get-juce で取得（無料、個人利用）。Projucer を起動して認証するか、CMake ビルドだけなら不要のはず（ただし配布する場合は要確認）。

---

## ビルド

```powershell
cd C:\デスクトップ\programs\synthesizer_practice\desktop

# Configure
cmake -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_PATH=C:/dev/JUCE

# Build（Debug）
cmake --build build --config Debug

# Build（Release）
cmake --build build --config Release
```

成果物:
- スタンドアロン: `build\synthesizer_practice_artefacts\Debug\Standalone\Synthesizer Practice.exe`
- VST3: `build\synthesizer_practice_artefacts\Debug\VST3\Synthesizer Practice.vst3`

VST3 を DAW（Reaper, FL Studio, Ableton, Cubase 等）で使うには、上記の `.vst3` を以下にコピー:
- `C:\Program Files\Common Files\VST3\`

---

## Visual Studio でデバッグ
1. `build\synthesizer_practice.sln` を VS で開く
2. ソリューションエクスプローラーで `synthesizer_practice_Standalone` を右クリック → 「スタートアッププロジェクトに設定」
3. F5 でデバッグ実行

---

## 並行開発（複数 Sonnet セッション）

このリポジトリは複数 Claude セッションが並行作業する前提で構成されています。

### 1. ワークツリーを切る（メインで一度だけ実行）

```powershell
cd C:\デスクトップ\programs\synthesizer_practice
mkdir worktrees -ErrorAction SilentlyContinue

git worktree add worktrees/desktop-A -b desktop/session-A/dsp
git worktree add worktrees/desktop-B -b desktop/session-B/ui-primitives
git worktree add worktrees/desktop-C -b desktop/session-C/visualizer
git worktree add worktrees/desktop-D -b desktop/session-D/editor
git worktree add worktrees/desktop-E -b desktop/session-E/params-presets
```

### 2. 各セッションを起動（別々の PowerShell ウィンドウで）

```powershell
# 例: Session A
cd C:\デスクトップ\programs\synthesizer_practice\worktrees\desktop-A
claude --model sonnet
```

### 3. 各セッションへの初期プロンプト例（Session A）

> あなたは Session A（DSP コア）担当です。
> `desktop/TASKS.md` の「タスク A: DSP コア」を読み、受け入れ条件をすべて満たすように実装してください。
> 触ってよいファイル / 触ってはいけないファイル を厳守してください。
> 共有資産 `desktop/Source/Parameters.h` の `ParamID::*` に従ってください。
> JUCE モジュール `juce::dsp::*` の利用を推奨します。
> 実装が終わったら `cmake --build build --config Debug` を通し、コミットしてください。

### 4. マージ

各セッション完了後、main で順に統合:

```powershell
cd C:\デスクトップ\programs\synthesizer_practice
git merge desktop/session-A/dsp --no-ff
git merge desktop/session-B/ui-primitives --no-ff
git merge desktop/session-C/visualizer --no-ff
git merge desktop/session-D/editor --no-ff
git merge desktop/session-E/params-presets --no-ff
```

### 5. ワークツリー片付け

```powershell
git worktree remove worktrees/desktop-A
git branch -d desktop/session-A/dsp
# ...他も同様
```

---

## ファイル構成

```
desktop/
├── CMakeLists.txt
├── Source/
│   ├── PluginProcessor.{h,cpp}      # AudioProcessor 本体（共有資産）
│   ├── PluginEditor.{h,cpp}         # GUI ルート（Session D 主担当）
│   ├── Parameters.{h,cpp}           # APVTS パラメータ定義（共有資産＋Session E）
│   ├── PresetManager.{h,cpp}        # Session E 担当
│   ├── Descriptions.{h,cpp}         # Session E 担当
│   ├── dsp/
│   │   ├── SynthSound.h             # 共有資産
│   │   ├── SynthVoice.{h,cpp}       # 共有 I/F + Session A の実装
│   │   ├── Oscillator.{h,cpp}       # Session A
│   │   ├── Filter.{h,cpp}           # Session A
│   │   ├── Envelope.{h,cpp}         # Session A
│   │   ├── LFO.{h,cpp}              # Session A
│   │   ├── Noise.{h,cpp}            # Session A
│   │   ├── Drive.{h,cpp}            # Session A
│   │   ├── Delay.{h,cpp}            # Session A
│   │   └── Reverb.{h,cpp}           # Session A
│   └── ui/
│       ├── CustomLookAndFeel.{h,cpp}    # Session B
│       ├── RotaryKnob.{h,cpp}           # Session B
│       ├── SectionPanel.{h,cpp}         # Session B
│       ├── WaveformDisplay.{h,cpp}      # Session C
│       ├── SpectrumDisplay.{h,cpp}      # Session C
│       ├── OscillatorPanel.{h,cpp}      # Session D
│       ├── NoisePanel.{h,cpp}           # Session D
│       ├── FilterPanel.{h,cpp}          # Session D
│       ├── FilterEnvelopePanel.{h,cpp}  # Session D
│       ├── AmpEnvelopePanel.{h,cpp}     # Session D
│       ├── LfoPanel.{h,cpp}             # Session D
│       ├── DrivePanel.{h,cpp}           # Session D
│       ├── DelayPanel.{h,cpp}           # Session D
│       ├── ReverbPanel.{h,cpp}          # Session D
│       ├── VoicePanel.{h,cpp}           # Session D
│       ├── MasterPanel.{h,cpp}          # Session D
│       └── InfoPanel.{h,cpp}            # Session D
├── README.md                  # ← このファイル
└── TASKS.md                   # タスク一覧と並行作業ルール
```

---

## トラブルシューティング

### `cmake -B build` で JUCE が見つからない
→ `-DJUCE_PATH=C:/dev/JUCE` を指定するか、`$env:JUCE_PATH` を設定

### ビルドが極端に遅い（FetchContent でJUCE取得中）
→ 数十分〜1時間程度。一度だけ。次回からはキャッシュされる。

### VST3 が DAW で見つからない
→ `Synthesizer Practice.vst3` を `C:\Program Files\Common Files\VST3\` にコピー（または DAW の VST3 検索パスを追加）
