# デスクトップ版 詳細設計書（v1.0）

最終更新: 2026-06-15
対象: Sonnet セッションが各タスクを実装するときの「契約」

このドキュメントは、Web 版での経験を踏まえた **C++ + JUCE 版の具体的な設計** を定義する。
おおきな方針は [`desktop-plan.md`](./desktop-plan.md) を参照。

---

## 1. アーキテクチャ概要

```
┌──────────────────────────────────────────────────────────────┐
│ SynthAudioProcessor (PluginProcessor.{h,cpp})               │
│  ├─ AudioProcessorValueTreeState (apvts)                    │
│  ├─ Synthesiser (synth)                                     │
│  │   └─ SynthVoice ×N (dsp/SynthVoice.{h,cpp})              │
│  │       ├─ Oscillator (×Unison)                            │
│  │       ├─ Noise                                           │
│  │       ├─ Filter                                          │
│  │       ├─ FilterEnvelope                                  │
│  │       ├─ AmpEnvelope                                     │
│  │       └─ LFO                                             │
│  ├─ Drive   (dsp/Drive.{h,cpp})                             │
│  ├─ Delay   (dsp/Delay.{h,cpp})                             │
│  ├─ Reverb  (dsp/Reverb.{h,cpp})                            │
│  ├─ MidiKeyboardState                                       │
│  └─ AudioVisualiserComponent                                │
└──────────────────────────────────────────────────────────────┘
                          ↑
┌──────────────────────────────────────────────────────────────┐
│ SynthAudioProcessorEditor (PluginEditor.{h,cpp})            │
│  ├─ OscillatorPanel                                         │
│  ├─ NoisePanel                                              │
│  ├─ FilterPanel                                             │
│  ├─ FilterEnvelopePanel                                     │
│  ├─ AmpEnvelopePanel                                        │
│  ├─ LfoPanel                                                │
│  ├─ DrivePanel                                              │
│  ├─ DelayPanel                                              │
│  ├─ ReverbPanel                                             │
│  ├─ VoicePanel                                              │
│  ├─ MasterPanel                                             │
│  ├─ WaveformDisplay                                         │
│  ├─ SpectrumDisplay                                         │
│  ├─ MidiKeyboardComponent                                   │
│  └─ InfoPanel                                               │
└──────────────────────────────────────────────────────────────┘
```

### パラメータの流れ
```
GUI (Slider) ←─[SliderAttachment]─→ APVTS ─[atomic load]→ DSP (processBlock)
```

GUI と DSP は **APVTS 経由でのみやり取り**する。DSP の各クラスは Processor から
`std::atomic<float>*` を受け取り、`processBlock` 内で読み出す。

---

## 2. ファイル一覧と責務

### 共有資産（変更注意）
| ファイル | 役割 |
|---|---|
| `Source/Parameters.h` | 全パラメータ ID の定義、列挙型、`createParameterLayout()` 宣言 |
| `Source/Parameters.cpp` | `createParameterLayout()` 実装 |
| `Source/PluginProcessor.{h,cpp}` | AudioProcessor 本体、apvts 保持、DSP 結線 |
| `Source/PluginEditor.{h,cpp}` | エディタ本体、各パネルの配置 |
| `Source/dsp/SynthSound.h` | ノート受け付け判定（全ノート対応） |
| `Source/dsp/SynthVoice.{h,cpp}` | 1ボイス分の処理（Osc + Filter + Env + LFO） |
| `desktop/CMakeLists.txt` | ビルド構成 |

### DSP モジュール（Session A 担当）
| ファイル | 役割 |
|---|---|
| `Source/dsp/Oscillator.{h,cpp}` | 波形生成。`juce::dsp::Oscillator` ベース、ユニゾン対応 |
| `Source/dsp/Filter.{h,cpp}` | `juce::dsp::StateVariableTPTFilter` ベース |
| `Source/dsp/Envelope.{h,cpp}` | `juce::ADSR` のラッパ |
| `Source/dsp/LFO.{h,cpp}` | 低周波変調器 |
| `Source/dsp/Noise.{h,cpp}` | ホワイト/ピンクノイズ |
| `Source/dsp/Drive.{h,cpp}` | `juce::dsp::WaveShaper` + 自前カーブ |
| `Source/dsp/Delay.{h,cpp}` | `juce::dsp::DelayLine` + フィードバック |
| `Source/dsp/Reverb.{h,cpp}` | `juce::dsp::Reverb` ラッパ |

### UI プリミティブ（Session B 担当）
| ファイル | 役割 |
|---|---|
| `Source/ui/CustomLookAndFeel.{h,cpp}` | ハードシンセ風スタイル（黒パネル + オレンジ） |
| `Source/ui/RotaryKnob.{h,cpp}` | Slider + Label + ホバー連動 |
| `Source/ui/SectionPanel.{h,cpp}` | 各セクションの共通枠（タイトル + 内容） |

### ビジュアライザ（Session C 担当）
| ファイル | 役割 |
|---|---|
| `Source/ui/WaveformDisplay.{h,cpp}` | 時間波形描画 |
| `Source/ui/SpectrumDisplay.{h,cpp}` | 周波数スペクトラム描画 |

### Editor 統合（Session D 担当）
| ファイル | 役割 |
|---|---|
| `Source/PluginEditor.{h,cpp}` | 各パネルの配置（Editor 側だが Session D 主担当） |
| `Source/ui/OscillatorPanel.{h,cpp}` | 波形選択 + Detune + Octave + Level |
| `Source/ui/NoisePanel.{h,cpp}` | Type + Level |
| `Source/ui/FilterPanel.{h,cpp}` | Type + Cutoff + Resonance |
| `Source/ui/FilterEnvelopePanel.{h,cpp}` | A/D/S/R + Amount |
| `Source/ui/AmpEnvelopePanel.{h,cpp}` | A/D/S/R |
| `Source/ui/LfoPanel.{h,cpp}` | Waveform + Rate + Depth + Target |
| `Source/ui/DrivePanel.{h,cpp}` | Type + Drive + Mix |
| `Source/ui/DelayPanel.{h,cpp}` | Time + Feedback + Mix |
| `Source/ui/ReverbPanel.{h,cpp}` | Decay + Mix |
| `Source/ui/VoicePanel.{h,cpp}` | Unison + Glide |
| `Source/ui/MasterPanel.{h,cpp}` | Volume |
| `Source/ui/InfoPanel.{h,cpp}` | パラメータ解説表示 |

### パラメータ + プリセット（Session E 担当）
| ファイル | 役割 |
|---|---|
| `Source/Parameters.cpp` | 全パラメータの Layout 構築 |
| `Source/PresetManager.{h,cpp}` | プリセットの保存/読み込み、内蔵プリセット定義 |
| `Source/Descriptions.{h,cpp}` | 各パラメータの解説文（InfoPanel が使う） |

---

## 3. パラメータ一覧

`Source/Parameters.h` の `ParamID::*` と1対1対応する。Session E が `Parameters.cpp` で実装する。

| ID | 種別 | 範囲 | デフォルト | 単位 | 備考 |
|---|---|---|---|---|---|
| `osc_waveform` | choice | Sine/Saw/Sqr/Tri | Saw | — | |
| `osc_detune` | float | -1200..+1200 | 0 | cent | |
| `osc_octave` | int | -2..+2 | 0 | oct | |
| `osc_level` | float | 0..1 | 1 | — | |
| `noise_type` | choice | White/Pink | White | — | |
| `noise_level` | float | 0..1 | 0 | — | |
| `filter_type` | choice | LP/HP/BP | LP | — | |
| `filter_cutoff` | float | 20..20000 | 2000 | Hz | log skew |
| `filter_resonance` | float | 0.1..30 | 1 | Q | |
| `fenv_attack` | float | 0.001..5 | 0.01 | s | |
| `fenv_decay` | float | 0.001..5 | 0.3 | s | |
| `fenv_sustain` | float | 0..1 | 0.4 | — | |
| `fenv_release` | float | 0.001..5 | 0.3 | s | |
| `fenv_amount` | float | -1..+1 | 0 | — | |
| `amp_attack` | float | 0.001..5 | 0.01 | s | |
| `amp_decay` | float | 0.001..5 | 0.2 | s | |
| `amp_sustain` | float | 0..1 | 0.7 | — | |
| `amp_release` | float | 0.001..5 | 0.3 | s | |
| `lfo_waveform` | choice | Sine/Saw/Sqr/Tri | Sine | — | |
| `lfo_rate` | float | 0.1..20 | 5 | Hz | |
| `lfo_depth` | float | 0..1 | 0 | — | |
| `lfo_target` | choice | Pitch/Filter/Amp | Pitch | — | |
| `drive_type` | choice | Soft/Hard | Soft | — | |
| `drive_amount` | float | 0..1 | 0 | — | |
| `drive_mix` | float | 0..1 | 0 | — | |
| `delay_time` | float | 0.01..2 | 0.3 | s | |
| `delay_feedback` | float | 0..0.95 | 0.3 | — | |
| `delay_mix` | float | 0..1 | 0 | — | |
| `reverb_decay` | float | 0.1..10 | 1.5 | s | |
| `reverb_mix` | float | 0..1 | 0 | — | |
| `unison_count` | int | 1..8 | 1 | — | |
| `unison_detune` | float | 0..100 | 20 | cent | |
| `glide` | float | 0..2 | 0 | s | |
| `master_volume` | float | 0..1 | 0.5 | — | |

→ Web 版の `src/types.ts` + `src/data/defaults.ts` を1対1で写したもの。

---

## 4. DSP 信号フロー

```
For each Voice:
  Oscillator(×Unison) → OscGain ─┐
                                  ├─→ Filter → AmpEnv → Sum
  Noise              → NoiseGain ─┘
                       FilterEnv → Filter.cutoffModulation
                       LFO       → (osc.detune | filter.cutoff | amp.gain)

Sum → Drive → Delay → Reverb → Output → AudioVisualiser → Speakers
```

### ボイスごとの内訳
- Voice は `MAX_VOICES = 8`（Web 版は 4 だったが、JUCE 標準で 8 にする）
- ユニゾン数を増やすときは、同一 Voice 内で複数 `juce::dsp::Oscillator` を持つ
- LFO はボイスごとに1つ（モノラル変調）
- フィルターエンベロープもボイスごとに1つ

### マスター段（全ボイス合算後）
- Drive → Delay → Reverb の順は Web 版と同じ
- 各エフェクトは `Processor` メンバとして1つだけ持つ（モノラル/ステレオ全体に作用）

---

## 5. UI レイアウト

ウィンドウサイズ: **1200 × 720**（リサイズ可能）

```
┌──────────────────────────────────────────────────────────────┐
│ Synthesizer Practice          [Preset ▼]  [Reset]            │ ← Header  60px
├──────────────────────────────────────────────────────────────┤
│ ┌──────WaveformDisplay──────┬──────SpectrumDisplay────────┐  │
│ │                            │                              │  │ ← Visualizer 160px
│ └────────────────────────────┴──────────────────────────────┘  │
├──────────────────────────────────────────────────────────────┤
│ ┌─Osc─┬─Noise─┬─Filter─┬─FEnv─┬─AmpEnv─┬─LFO─┬─Drive─┐       │
│ │     │       │        │      │        │     │       │       │ ← Sections 180px
│ ├─────┴───────┴────────┼──────┴────────┴─────┼───────┤       │   2行
│ │  Delay │  Reverb │       Voice            │ Master │       │   180px
│ └────────┴─────────┴────────────────────────┴────────┘       │
├──────────────────────────────────────────────────────────────┤
│ Info: [選択中パラメータの解説]                                 │ ← InfoPanel 60px
├──────────────────────────────────────────────────────────────┤
│ ░▓░▓░▓░▓░▓░▓░▓░▓ (MidiKeyboardComponent)                    │ ← Keyboard 120px
└──────────────────────────────────────────────────────────────┘
```

### スタイル（CustomLookAndFeel）
- 背景: `#1a1a1a`
- パネル: `#2a2a2a`
- アクセント: `#ff8c00`（オレンジ）
- テキスト: `#e0e0e0` / dim `#888888`
- ツマミ: 円形、上半円が値の表示（0=左端 → 1=右端）

---

## 6. APVTS と GUI の連結方法

各 Slider / ComboBox / Button は対応する Attachment クラスで APVTS と連結する。

```cpp
// 例: OscillatorPanel
class OscillatorPanel : public juce::Component
{
public:
    OscillatorPanel(juce::AudioProcessorValueTreeState& apvts)
    {
        detuneAttachment = std::make_unique<APVTS::SliderAttachment>(
            apvts, ParamID::OscDetune, detuneKnob.slider);
        // ...
    }
private:
    using APVTS = juce::AudioProcessorValueTreeState;
    RotaryKnob detuneKnob { "Detune" };
    std::unique_ptr<APVTS::SliderAttachment> detuneAttachment;
};
```

`Attachment` を保持する `std::unique_ptr` メンバは、対応する UI コンポーネントより**後**に宣言すること（破棄順がコンポーネント先になるように）。

---

## 7. DSP モジュールの共通インターフェース（推奨）

```cpp
class DspModule
{
public:
    virtual ~DspModule() = default;
    virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
    virtual void reset() = 0;
    virtual void process(juce::dsp::ProcessContextReplacing<float>&) = 0;
};
```

ただし強制ではない。Voice 内で持つ Oscillator / Filter / Envelope は、Voice の `renderNextBlock` から都度メソッドを呼べばよい。

### Voice の `renderNextBlock` 構造（推奨）

```cpp
void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& output,
                                 int startSample, int numSamples)
{
    if (!isVoiceActive()) return;

    // 1. 最新パラメータを APVTS から読む（ポインタ経由、atomic load）
    updateFromParameters();

    // 2. サンプルバッファ確保（一時用）
    juce::AudioBuffer<float> voiceBuffer(1, numSamples);
    voiceBuffer.clear();

    // 3. Oscillator + Noise を加算
    oscillator.process(voiceBuffer, numSamples);
    noise.addTo(voiceBuffer, numSamples);

    // 4. Filter（FilterEnv で cutoff 変調）
    filter.process(voiceBuffer, filterEnv.next(numSamples));

    // 5. Amp Envelope
    ampEnv.applyEnvelopeToBuffer(voiceBuffer, 0, numSamples);

    // 6. ボイスの出力をメインバッファに加算
    for (int ch = 0; ch < output.getNumChannels(); ++ch)
        output.addFrom(ch, startSample, voiceBuffer, 0, 0, numSamples);

    // 7. エンベロープ終了でボイス停止
    if (!ampEnv.isActive())
        clearCurrentNote();
}
```

---

## 8. ビルド・実行手順

### 8.1 必要ツール
- Visual Studio 2022 Community（C++ デスクトップ開発ワークロード）
- CMake 3.22 以上
- Git

### 8.2 JUCE 取得（推奨: 共通ローカルクローン）
```powershell
git clone https://github.com/juce-framework/JUCE.git C:\dev\JUCE
# tag を 8.0.4 にチェックアウト
cd C:\dev\JUCE
git checkout 8.0.4
```

### 8.3 設定 + ビルド
```powershell
cd C:\デスクトップ\programs\synthesizer_practice\desktop

# Configure（初回 + JUCE 場所を明示するとき）
cmake -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_PATH=C:/dev/JUCE

# JUCE_PATH を指定しないと FetchContent でビルド時にダウンロード（遅い）
# cmake -B build -G "Visual Studio 17 2022" -A x64

# Build
cmake --build build --config Debug
```

成果物:
- `build/synthesizer_practice_artefacts/Debug/Standalone/Synthesizer Practice.exe`
- `build/synthesizer_practice_artefacts/Debug/VST3/Synthesizer Practice.vst3`

### 8.4 VS でデバッグ
`build/synthesizer_practice.sln` を VS で開く → `synthesizer_practice_Standalone` をスタートアッププロジェクトに → F5

---

## 9. リアルタイムオーディオの注意

`processBlock` / `renderNextBlock` 内で **絶対にやってはいけない**こと:
- メモリアロケーション（`new`, `std::vector::push_back`, `std::string` 連結など）
- ロック取得（`std::mutex::lock`）
- ファイル I/O、ログ出力（`DBG` も避ける）
- 例外送出

これらはオーディオドロップ（プチプチ音）の原因。GUI スレッドとの通信は `std::atomic` か `juce::AbstractFifo` を使う。

APVTS の `getRawParameterValue()` は `std::atomic<float>*` を返すので、ポインタを `prepareToPlay` で取得して `processBlock` 内で `->load()` するパターンが定石。

---

## 10. 変更履歴

| 日付 | 変更 |
|---|---|
| 2026-06-15 | v1.0 初版 |
