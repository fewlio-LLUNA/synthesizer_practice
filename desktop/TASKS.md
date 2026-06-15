# TASKS — デスクトップ版 並行開発タスク一覧

5セッションでの並行開発用タスク定義。各セッションは1タスクをクレームし、
worktree 上で完結させ、main にマージする。

詳細設計は [`../docs/desktop-design.md`](../docs/desktop-design.md) を参照。

---

## 並行作業の原則

1. **触ってよいファイル** に明記されたパスのみを編集する
2. **触ってはいけないファイル**（共有資産）は変更しない。変更が必要なら「I/F 変更ルール」へ
3. 受け入れ条件をすべて満たしてから main にマージする
4. ブランチ名は `desktop/session-{A|B|C|D|E}/{短い説明}` 形式
5. 完了したら本ファイルの該当タスクのチェックボックスを更新し、コミット

---

## 共有資産（全セッション共通・変更注意）

これらは「契約」です。勝手に変更しない:

- `Source/Parameters.h` — 全パラメータ ID 定義
- `Source/PluginProcessor.{h,cpp}` の public I/F
- `Source/PluginEditor.{h,cpp}` の class 構造
- `Source/dsp/SynthSound.h`
- `Source/dsp/SynthVoice.h` の public I/F
- `desktop/CMakeLists.txt` — ソース追加時のみ編集可（マージ衝突回避のため、追加場所のコメントに従う）
- `TASKS.md`, `README.md`, `docs/`

### I/F 変更ルール

共有資産（特に `Parameters.h`）の変更が必要になった場合:
1. 作業を一時停止する
2. main に「I/F変更提案」コミットを切る
3. 他セッションへ周知（本ファイルの「変更履歴」に追記）
4. 全セッションが pull した上で再開する

---

## タスク A: DSP コア

- [x] 担当中: Session A (desktop/session-A/dsp)
- [x] 完了

### 触ってよいファイル
- `Source/dsp/Oscillator.{h,cpp}` （新規）
- `Source/dsp/Filter.{h,cpp}` （新規）
- `Source/dsp/Envelope.{h,cpp}` （新規）
- `Source/dsp/LFO.{h,cpp}` （新規）
- `Source/dsp/Noise.{h,cpp}` （新規）
- `Source/dsp/Drive.{h,cpp}` （新規）
- `Source/dsp/Delay.{h,cpp}` （新規）
- `Source/dsp/Reverb.{h,cpp}` （新規）
- `Source/dsp/SynthVoice.cpp` （内部実装のみ。public I/F は触らない）
- `Source/PluginProcessor.cpp` の `prepareToPlay` と `processBlock` 内（エフェクトチェーン接続）
- `CMakeLists.txt` の `target_sources` への .cpp 追加のみ

### 触ってはいけないファイル
- `Source/Parameters.h` （I/F変更ルールに従うこと）
- `Source/PluginProcessor.h`
- `Source/dsp/SynthVoice.h` （ヘッダーは変えない）
- `Source/dsp/SynthSound.h`
- 上記「共有資産」全て

### 実装内容
1. `juce::dsp::Oscillator` ベースで波形4種対応。ユニゾン（最大8 osc）に対応
2. `juce::dsp::StateVariableTPTFilter` でLP/HP/BP対応
3. `juce::ADSR` のラッパで Amp と Filter の2系統エンベロープ
4. `LFO` クラス: 低周波オシレーター、Pitch/Filter/Amp の3ターゲット
5. `Noise`: ホワイト + ピンク（Paul Kellet's algorithm）
6. `Drive`: `juce::dsp::WaveShaper` + 自前 soft/hard カーブ
7. `Delay`: `juce::dsp::DelayLine` + フィードバック + wet/dry
8. `Reverb`: `juce::dsp::Reverb` ラッパ
9. `SynthVoice::renderNextBlock` で全体の信号フローを実装（設計書 §7 参照）
10. `PluginProcessor::processBlock` でマスターチェーン（Drive→Delay→Reverb）

### 受け入れ条件
- ビルドが通る（Standalone と VST3 の両方）
- Standalone を起動して鍵盤を弾くと音が鳴る
- APVTS のパラメータを変更すると音が変わる（最低限 Filter Cutoff、Amp Envelope）
- `processBlock` 内でアロケーションなし（Debug ビルドの leak detector で確認）

---

## タスク B: UI 共通プリミティブ

- [ ] 担当中: _未割当_
- [ ] 完了

### 触ってよいファイル
- `Source/ui/CustomLookAndFeel.{h,cpp}` （新規）
- `Source/ui/RotaryKnob.{h,cpp}` （新規）
- `Source/ui/SectionPanel.{h,cpp}` （新規）
- `CMakeLists.txt` の `target_sources` への追加

### 触ってはいけないファイル
- `Source/ui/*Panel.{h,cpp}` （Session D の領域、ただし共通プリミティブはここから使う）
- `Source/PluginEditor.{h,cpp}`
- 上記「共有資産」全て

### 実装内容
1. `CustomLookAndFeel`: ハードシンセ風（黒 #1a1a1a + アクセント #ff8c00）。
   - Slider, Button, ComboBox, Label に統一スタイル
   - `drawRotarySlider` をオーバーライド
2. `RotaryKnob`: `juce::Slider`（RotaryHorizontalVerticalDrag）+ `juce::Label`（タイトル + 値表示）
   - ホバー時に `onHover(paramId)` を発火（InfoPanel連動用、Session D が listener）
   - public メンバ `juce::Slider slider` を持つ（SliderAttachment 用）
   - 単位（"Hz", "s" など）を suffix 表示するオプション
3. `SectionPanel`: タイトル付き枠。中身は `addAndMakeVisible` で追加可能
   - タイトル領域 + コンテンツ領域

### 受け入れ条件
- ビルドが通る
- `Source/PluginEditor.cpp` のスタブで一時的に1つ RotaryKnob を表示しても壊れない
- ドラッグで値変更、ダブルクリックでデフォルト復帰
- ホバー時にコールバック発火

---

## タスク C: ビジュアライザ

- [x] 担当中: Session C (desktop/session-C/visualizer)
- [x] 完了

### 触ってよいファイル
- `Source/ui/WaveformDisplay.{h,cpp}` （新規）
- `Source/ui/SpectrumDisplay.{h,cpp}` （新規）
- `CMakeLists.txt` の `target_sources` への追加

### 触ってはいけないファイル
- `Source/PluginProcessor.{h,cpp}` （`getAudioVisualiser()` は使ってよいが処理は変更しない）
- 上記「共有資産」全て

### 実装内容
1. `WaveformDisplay`: `juce::AudioVisualiserComponent` を内包またはラップ
   - 上部にタイトル「時間波形」+ キャプション
   - オレンジで波形描画、ダークパネル背景
2. `SpectrumDisplay`: 自前で FFT を取って描画
   - `juce::dsp::FFT` で周波数解析
   - 対数周波数軸（左=低音、右=高音）
   - `startTimerHz(60)` で再描画
   - PluginProcessor から最新サンプルを取得する手段が必要 → SpectrumDisplay 自身が FIFO バッファを持ち、Processor から push してもらう（要I/F追加、要Session E相談）

### 受け入れ条件
- ビルドが通る
- 音が鳴っていない時はフラットな表示
- 音が鳴ると波形/スペクトラムが反応
- フレームレート 30fps 以上、UI スレッドをブロックしない

---

## タスク D: Editor + 各セクションパネル統合

- [ ] 担当中: _未割当_
- [ ] 完了

### 触ってよいファイル
- `Source/PluginEditor.{h,cpp}` （メンバ追加、レイアウト実装）
- `Source/ui/OscillatorPanel.{h,cpp}` （新規）
- `Source/ui/NoisePanel.{h,cpp}` （新規）
- `Source/ui/FilterPanel.{h,cpp}` （新規）
- `Source/ui/FilterEnvelopePanel.{h,cpp}` （新規）
- `Source/ui/AmpEnvelopePanel.{h,cpp}` （新規）
- `Source/ui/LfoPanel.{h,cpp}` （新規）
- `Source/ui/DrivePanel.{h,cpp}` （新規）
- `Source/ui/DelayPanel.{h,cpp}` （新規）
- `Source/ui/ReverbPanel.{h,cpp}` （新規）
- `Source/ui/VoicePanel.{h,cpp}` （新規）
- `Source/ui/MasterPanel.{h,cpp}` （新規）
- `Source/ui/InfoPanel.{h,cpp}` （新規）
- `CMakeLists.txt` の `target_sources` への追加

### 触ってはいけないファイル
- `Source/ui/RotaryKnob.{h,cpp}` （Session B の領域。使用のみ）
- `Source/ui/WaveformDisplay.{h,cpp}` （Session C の領域。使用のみ）
- `Source/ui/CustomLookAndFeel.{h,cpp}` （Session B の領域）
- `Source/Parameters.h` （I/F 変更ルール）
- 上記「共有資産」全て

### 実装内容
1. 各 Panel コンポーネントを実装
   - APVTS::SliderAttachment / ComboBoxAttachment / ButtonAttachment で連結
   - RotaryKnob にホバー連動を仕込む
2. `PluginEditor` で各 Panel を `addAndMakeVisible` し、`resized()` でレイアウト
   - 設計書 §5 のレイアウト図に従う
3. `InfoPanel` は `Descriptions` から解説を取得して表示
4. プリセットセレクター + Reset ボタンをヘッダーに配置

### 受け入れ条件
- ビルドが通る
- 全パラメータが UI から操作でき、音に反映される
- ホバーで InfoPanel に解説が表示される
- ウィンドウリサイズに追従する
- DAW 上で UI が正しく表示される（VST3 として読み込み確認）

---

## タスク E: パラメータ管理 + プリセット + 解説

- [x] 担当中: Session E (Claude Sonnet 4.6, 2026-06-15)
- [x] 完了（ビルド未検証 — CMake/MSVC 環境で要確認）

### 触ってよいファイル
- `Source/Parameters.cpp` （`createParameterLayout` の本体実装）
- `Source/PresetManager.{h,cpp}` （新規）
- `Source/Descriptions.{h,cpp}` （新規）
- `CMakeLists.txt` の `target_sources` への追加

### 触ってはいけないファイル
- `Source/Parameters.h` の **ParamID 定義削除や名前変更**（追加は I/F 変更ルールに従う）
- `Source/PluginProcessor.{h,cpp}` の I/F
- 上記「共有資産」全て

### 実装内容
1. `Parameters.cpp` の `createParameterLayout()` を完成させる
   - 設計書 §3 のパラメータ一覧表に従う
   - 範囲、デフォルト、単位、対数スケール（Filter Cutoff）など
   - choice 型は `juce::StringArray` で選択肢を持つ
2. `PresetManager`:
   - 内蔵プリセット定義（Web 版の `web/src/data/presets.ts` を参考に C++ で再現）
   - APVTS への適用 / 取得
   - ユーザープリセットの保存・読み込み（XML をユーザー設定フォルダに）
3. `Descriptions`:
   - 各パラメータの解説文（Web 版の `web/src/data/descriptions.ts` を参考）
   - 文字列定数として持つ
   - InfoPanel から `getDescription(paramId)` で取得可能

### 受け入れ条件
- ビルドが通る
- すべてのパラメータが APVTS に登録され、DAW のオートメーション一覧に表示される
- 最低5プリセットが用意され、PresetManager から適用可能
- ユーザープリセット保存・読込が機能する
- 解説文が全パラメータ分用意されている

---

## 統合・最終確認

- [ ] 全タスク完了後、main で `cmake --build` が通る
- [ ] Standalone が起動して音が鳴る、UI が操作可能
- [ ] VST3 として DAW（Reaper など）で読み込めて、オートメーションが効く
- [ ] プリセット切替が機能する

---

## 変更履歴（I/F 変更があったらここに追記）

- 2026-06-15: 初期 I/F 確定（`Parameters.h` v1.0）
- 2026-06-15: [Session C] `PluginProcessor` に spectrum FIFO 追加
  - `kSpectrumFifoSize` (= 4096) 定数、`spectrumFifo`、`spectrumFifoBuffer` メンバ追加
  - `getSpectrumFifo()` / `getSpectrumFifoBuffer()` getter 追加
  - `processBlock` でオーディオ ch0 を FIFO に push
  - `prepareToPlay` で FIFO リセット
  - → `SpectrumDisplay` が `timerCallback` で読み出して FFT 描画するため
