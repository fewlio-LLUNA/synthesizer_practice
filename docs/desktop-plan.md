# ネイティブデスクトップ版 開発計画書（v1.0）

最終更新: 2026-06-14

## 1. 方針

| 項目 | 決定事項 |
|---|---|
| 技術スタック | **C++ + JUCE** |
| ターゲット | スタンドアロン `.exe` + **VST3 プラグイン**（同時ビルド） |
| 対応 OS | Windows 11（最初の目標）。将来 macOS/Linux 展開可 |
| Web 版の扱い | **現状で凍結**（Vercel デプロイは維持、機能追加なし） |
| ネイティブ版の扱い | 今後の開発はこちらが主軸 |
| Web 版との一致 | 「概念で揃える、数値は微調整」と割り切る |
| リポジトリ | 既存リポジトリを `web/` と `desktop/` に分割（B案） |

### 採用理由（記録）
- **C++ + JUCE**: DTM 業界デファクト、スタンドアロン + VST/AU/AAX まで一気通貫、リアルタイムオーディオ最適化、書籍・チュートリアル豊富
- **Web 版凍結**: 並行維持はコストが高すぎる。学習目的が「シンセを深く理解する」なら主軸を1つに絞る方が良い
- **VST まで含める**: 学習成果が DAW で実用可能なシンセになる、業界本流の経験

---

## 2. リポジトリ構造（最終形）

```
synthesizer_practice/
├── web/                        # 既存 React 一式を移動・凍結
│   ├── src/
│   ├── package.json
│   ├── vite.config.ts
│   ├── tsconfig.json
│   ├── index.html
│   └── ...
├── desktop/                    # JUCE プラグイン（新規・主軸）
│   ├── Source/
│   │   ├── PluginProcessor.h/.cpp     # AudioProcessor（音処理の本体）
│   │   ├── PluginEditor.h/.cpp        # GUI
│   │   ├── SynthVoice.h/.cpp          # 1ボイス分のクラス
│   │   ├── SynthSound.h/.cpp          # 発音判定クラス
│   │   ├── dsp/                       # オシレーター・フィルター・LFO 等
│   │   └── ui/                        # 共通コンポーネント（つまみ等）
│   ├── synthesizer_practice.jucer     # Projucer プロジェクト
│   └── README.md                       # P0 セットアップ手順を保管
├── docs/
│   └── desktop-plan.md         # ★本計画書
├── vercel.json                 # rootDirectory = "web" に変更予定
├── README.md                   # プロジェクト全体説明
└── .gitignore                  # desktop/Builds, JuceLibraryCode を除外
```

> 構造変更は次回以降の作業（今回は計画書ドキュメント化のみ）。

---

## 3. 開発ロードマップ

C++/JUCE 初学者向けに、Audio Plugin テンプレートで最初からスタンドアロン + VST3 両対応。

| Phase | 内容 | 学ぶ JUCE 概念 | 目安 |
|---|---|---|---|
| **P0** | VS2022 + JUCE インストール、Plugin テンプレートでビルド | Projucer、ターゲット設定、VST3 SDK 配置 | 3〜5日 |
| **P1** | サイン波1音だけ鳴る最小プラグイン | `AudioProcessor`, `processBlock`, `Synthesiser`, `SynthesiserVoice` | 1〜2週間 |
| **P2** | ADSR + ノコギリ波 + LPF（Web版の初期状態） | `juce::ADSR`, `dsp::Oscillator`, `dsp::StateVariableTPTFilter` | 1〜2週間 |
| **P3** | UI（つまみ・ラジオ）+ パラメータ連動 | `AudioProcessorValueTreeState`, `Slider`, `AudioProcessorEditor`, `LookAndFeel` | 1〜2週間 |
| **P4** | Filter Envelope + LFO（Web版ステップ2相当） | DSP モジュレーション設計、`SmoothedValue` | 1週間 |
| **P5** | ノイズ + ディレイ + リバーブ + ドライブ | `dsp::DelayLine`, `dsp::Reverb`, `dsp::WaveShaper`, `Random` | 2週間 |
| **P6** | ユニゾン + グライド + プリセット保存/読込 | `XmlElement`/`ValueTree`、複数ボイス管理、状態シリアライズ | 1〜2週間 |
| **P7** | ビジュアライザ + 仕上げ | `Component::paint`, `AudioVisualiserComponent`, リアルタイム描画 | 1週間 |

**合計**: 約 8〜13週間（週末ペース、初学コスト込み）

VST 化は P0 から達成済み（Audio Plugin テンプレートのため）。

---

## 4. 環境セットアップ手順（P0 用メモ）

### 4.1 Visual Studio 2022 Community
- https://visualstudio.microsoft.com/ja/downloads/
- インストール時のワークロード: **「C++ によるデスクトップ開発」** を選択
- ローカライズ言語パック: 日本語と英語両方推奨

### 4.2 JUCE
- GitHub: https://github.com/juce-framework/JUCE
- clone する: `git clone https://github.com/juce-framework/JUCE.git C:\dev\JUCE`
- `JUCE\Projucer\Builds\VisualStudio2022\Projucer.sln` を VS で開いてビルド → Projucer.exe 生成

### 4.3 JUCE Personal License（無料）
- https://juce.com/get-juce
- 「Personal」を選択 → メール登録
- Projucer 起動 → ログインしてライセンスをアクティベート
- 個人・年商 5万ドル以下: 無料、クローズドソース配布も可

### 4.4 VST3 SDK
- JUCE のリポジトリ内に同梱されているはず（`modules/juce_audio_plugin_client/VST3/`）
- 追加ダウンロード不要

### 4.5 プロジェクト生成
- Projucer 起動 → New Project → **Audio Plug-In**
- Project name: `synthesizer_practice`
- 場所: `C:\デスクトップ\programs\synthesizer_practice\desktop\`
- フォーマット: Standalone + VST3 を有効化
- Exporter: Visual Studio 2022 を選択
- 保存して "Open IDE" でVSを開く
- Solution Explorer から `Standalone Plugin` ターゲットを実行 → 空のシンセが起動すれば成功

### 4.6 配布時の注意
- 学習目的・個人配布: SmartScreen 警告は出るが無視可
- 一般公開: コード署名（Authenticode 証明書、年 1〜3 万円程度）が必要

---

## 5. 既存 Web 実装 → JUCE 対応マップ

| Web版（React/TypeScript） | JUCE で対応するもの |
|---|---|
| `SynthEngine` クラス | `AudioProcessor` + `juce::Synthesiser` |
| Voice（OscillatorNode + GainNode 等） | `juce::SynthesiserVoice` 派生クラス |
| `Oscillator` | `juce::dsp::Oscillator` または自前 |
| `Filter`（BiquadFilterNode） | `juce::dsp::StateVariableTPTFilter` |
| `Envelope`（GainNode + ramp） | `juce::ADSR` |
| `LFO`（OscillatorNode → gain） | 自前 `LFO` クラス |
| `Noise`（AudioBufferSourceNode） | `juce::Random` + バッファ生成 |
| `Delay`（DelayNode + フィードバック） | `juce::dsp::DelayLine` |
| `Reverb`（ConvolverNode） | `juce::dsp::Reverb`（FDN ベース、IR 不要） |
| `Drive`（WaveShaperNode） | `juce::dsp::WaveShaper` + 自前カーブ関数 |
| Knob（自前 React） | `juce::Slider`（RotaryHorizontalVerticalDrag）|
| ラジオボタン（自前 React） | `juce::TextButton` を radioGroup でグループ化 |
| 鍵盤（自前 React） | `juce::MidiKeyboardComponent`（標準で完成度高い） |
| Canvas波形 | `juce::AudioVisualiserComponent` または自前 `paint` |
| プリセット | `juce::ValueTree` + `XmlElement` でシリアライズ |
| パラメータ管理 | `juce::AudioProcessorValueTreeState`（DAW のオートメーション連動も無料で得られる）|

---

## 6. リスクと対策

| リスク | 対策 |
|---|---|
| C++ / JUCE の学習曲線 | P0〜P1 を急がず1〜2週間かける |
| VS ビルド失敗（最初の山） | Projucer → VS 「再ターゲット」、JUCE は最新リリース版を使う |
| `processBlock` 内で malloc/lock 禁止 | 初学コストだがリアルタイム性能の本質、価値高 |
| Web 版と挙動が微妙にずれる | 「概念で揃える」と割り切る、数値は微調整 |
| SmartScreen 警告 | 個人配布なら無視可、一般公開なら有償コード署名 |
| JUCE ライセンス | Personal License は無料、商用化時に再検討 |
| Vercel rootDirectory 設定変更 | ダッシュボードの Project Settings → General → Root Directory に "web" を入力 |

---

## 7. 推奨学習リソース

### 公式
- JUCE 公式 Tutorial: https://docs.juce.com/master/tutorial_synth_using_midi_input.html
- JUCE API Documentation: https://docs.juce.com/master/

### YouTube
- **The Audio Programmer**: JUCE 入門の英語チャンネル、信頼性高い、シンセ作成シリーズあり
- **Andrew Huang** など DSP/シンセ理論

### コミュニティ
- JUCE フォーラム: https://forum.juce.com/

### 書籍（任意）
- "Designing Audio Effect Plugins in C++" by Will Pirkle: DSP の理論と実装、JUCE の例も豊富
- "The Audio Programming Book" by Boulanger & Lazzarini: 古典、基礎を固めたいなら

---

## 8. 次回作業（このセッション後に進めること）

1. **JUCE / VS2022 のインストール**（ユーザー手作業）
2. **JUCE Personal License 取得**（メール登録 + アクティベート）
3. **リポジトリ構造変更**（Claude セッションで実行可能）
   - 既存ファイルを `web/` 配下に移動
   - `vercel.json` の rootDirectory 設定
   - Vercel ダッシュボードでも rootDirectory 設定変更
4. **P0 開始**: Projucer で Audio Plugin プロジェクト生成、ビルド・起動確認
5. **デバッグ**: 「Hello, Audio!」レベルの音が出たら P0 完了

---

## 9. 過去の決定事項ログ

| 日付 | 決定 |
|---|---|
| 2026-06-14 | 並行開発（5セッション）でReact版を完成 |
| 2026-06-14 | ステップ1〜6 + プリセットブラウザを実装 |
| 2026-06-14 | Vercel デプロイ: https://synthesizer-practice.vercel.app/ |
| 2026-06-14 | デスクトップ版を C++ + JUCE で完全書き直し決定 |
| 2026-06-14 | Web版は凍結、ネイティブ版を主軸 |
| 2026-06-14 | VST プラグイン化もゴールに含める |
| 2026-06-14 | リポジトリ構造: B案（web/ と desktop/ 分割） |
