// =============================================================================
// PluginEditor.cpp — GUI 実装
//
// レイアウト (1200×720):
//   Header       50px  … タイトル / プリセット / Reset
//   Visualizer  140px  … WaveformDisplay + SpectrumDisplay (Session C)
//   Section 1   175px  … Osc / Noise / Filter / FEnv / AmpEnv / LFO / Drive
//   Section 2   175px  … Delay / Reverb / Voice / Master
//   InfoPanel    50px  … ホバー中パラメータの解説
//   Keyboard    130px  … MidiKeyboardComponent
// =============================================================================
#include "PluginEditor.h"
#include "Descriptions.h"
#include "BinaryData.h"
#include "ui/CustomLookAndFeel.h"

namespace {

// ウィンドウサイズ定数
constexpr int WIN_W  = 1200;
constexpr int WIN_H  = 720;

// 各領域の高さ
constexpr int HDR_H  =  50;
constexpr int VIZ_H  = 140;
constexpr int SEC_H  = 175;
constexpr int INFO_H =  50;
constexpr int KEY_H  = 130;

// カラー
const juce::Colour COL_BG     { 0xff1a1a1a };
const juce::Colour COL_PANEL  { 0xff2a2a2a };
const juce::Colour COL_ACCENT { 0xffff8c00 };
const juce::Colour COL_TEXT   { 0xffe0e0e0 };
const juce::Colour COL_DIM    { 0xff888888 };

} // anonymous namespace

namespace synth {

SynthAudioProcessorEditor::SynthAudioProcessorEditor(SynthAudioProcessor& p)
    : juce::AudioProcessorEditor(&p),
      processorRef(p),
      resetButton("Reset"),
      // セクションパネル — 全て同じ APVTS を受け取る
      oscPanel       (p.getAPVTS()),
      noisePanel     (p.getAPVTS()),
      filterPanel    (p.getAPVTS()),
      filterEnvPanel (p.getAPVTS()),
      ampEnvPanel    (p.getAPVTS()),
      lfoPanel       (p.getAPVTS()),
      drivePanel     (p.getAPVTS()),
      delayPanel     (p.getAPVTS()),
      reverbPanel    (p.getAPVTS()),
      voicePanel     (p.getAPVTS()),
      masterPanel    (p.getAPVTS()),
      // MIDIキーボード
      keyboard(p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard),
      // ビジュアライザ（Session C）
      waveformDisplay(p.getAudioVisualiser()),
      spectrumDisplay(p)
{
    // 日本語対応 LookAndFeel を全配下に適用
    setLookAndFeel(&customLookAndFeel);
    setSize(WIN_W, WIN_H);
    setResizable(true, true);
    setResizeLimits(800, 500, 2400, 1440);

    // ヘッダーウィジェット
    titleLabel.setText("Synthesizer Practice", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(16.0f));
    titleLabel.setColour(juce::Label::textColourId, COL_TEXT);
    addAndMakeVisible(titleLabel);

    setupPresetBox();
    addAndMakeVisible(presetBox);

    // Reset ボタン — 全パラメータをデフォルト値に戻す
    resetButton.onClick = [this]
    {
        for (auto* param : processorRef.getAPVTS().processor.getParameters())
            param->setValueNotifyingHost(param->getDefaultValue());
    };
    addAndMakeVisible(resetButton);

    // セクションパネルを追加
    addAndMakeVisible(oscPanel);
    addAndMakeVisible(noisePanel);
    addAndMakeVisible(filterPanel);
    addAndMakeVisible(filterEnvPanel);
    addAndMakeVisible(ampEnvPanel);
    addAndMakeVisible(lfoPanel);
    addAndMakeVisible(drivePanel);
    addAndMakeVisible(delayPanel);
    addAndMakeVisible(reverbPanel);
    addAndMakeVisible(voicePanel);
    addAndMakeVisible(masterPanel);

    addAndMakeVisible(infoPanel);
    addAndMakeVisible(keyboard);

    // ビジュアライザ
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(spectrumDisplay);

    setupHoverCallbacks();
}

// -----------------------------------------------------------------------------
// プリセットボックスのセットアップ
// PresetManager の内蔵プリセットを ComboBox に登録し、選択変更で適用する
// -----------------------------------------------------------------------------
void SynthAudioProcessorEditor::setupPresetBox()
{
    presetBox.addItem("— プリセット選択 —", 1);

    const auto& presets = presetManager.getBuiltInPresets();
    for (size_t i = 0; i < presets.size(); ++i)
    {
        // ComboBox の id は 1 以上の必要があるため、id = i + 2（1 はラベル用）
        presetBox.addItem(presets[i].name, static_cast<int>(i) + 2);
    }
    presetBox.setSelectedId(1, juce::dontSendNotification);

    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();
        if (id < 2) return; // ラベル選択は無視
        const int index = id - 2;
        presetManager.applyBuiltInPreset(processorRef.getAPVTS(), index);
    };
}

SynthAudioProcessorEditor::~SynthAudioProcessorEditor()
{
    // setLookAndFeel(&lnf) の解除（メンバ破棄前に必須）
    setLookAndFeel(nullptr);
}

// -----------------------------------------------------------------------------
// ホバーコールバック設定
// Session E の Descriptions::getDescription が実装されたら
// infoPanel.setInfo(paramId, Descriptions::getDescription(paramId)) に更新する
// -----------------------------------------------------------------------------
void SynthAudioProcessorEditor::setupHoverCallbacks()
{
    auto hover = [this](const juce::String& paramId)
    {
        // 名前空間を明示（juce::Component::getDescription と衝突するため）
        infoPanel.setInfo(paramId, synth::getDescription(paramId));
    };

    oscPanel.onParamHover       = hover;
    noisePanel.onParamHover     = hover;
    filterPanel.onParamHover    = hover;
    filterEnvPanel.onParamHover = hover;
    ampEnvPanel.onParamHover    = hover;
    lfoPanel.onParamHover       = hover;
    drivePanel.onParamHover     = hover;
    delayPanel.onParamHover     = hover;
    reverbPanel.onParamHover    = hover;
    voicePanel.onParamHover     = hover;
    masterPanel.onParamHover    = hover;
}

// -----------------------------------------------------------------------------
// paint
// -----------------------------------------------------------------------------
void SynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(COL_BG);

    // ヘッダー下線
    g.setColour(COL_ACCENT.withAlpha(0.4f));
    g.drawHorizontalLine(HDR_H - 1, 0.0f, static_cast<float>(getWidth()));

    // === DIAGNOSTIC v2: Typeface と Font の中身を確認 ===
    auto tp = synth::getEmbeddedJapaneseTypeface();
    const juce::String tpName  = tp ? tp->getName()  : juce::String("(null-tp)");
    const juce::String tpStyle = tp ? tp->getStyle() : juce::String("");

    juce::Font fontA(tp);
    fontA.setHeight(13.0f);
    const juce::String fontAName = fontA.getTypefaceName();

    juce::Font fontB(juce::FontOptions(tp).withHeight(13.0f));
    const juce::String fontBName = fontB.getTypefaceName();

    juce::String diag;
    diag << "TF=" << (tp == nullptr ? "NULL" : "OK")
         << " sz=" << (int) BinaryData::NotoSansJPRegular_ttfSize
         << " tpName=[" << tpName << "/" << tpStyle << "]"
         << " fontA=[" << fontAName << "]"
         << " fontB=[" << fontBName << "]";

    g.setColour(juce::Colours::lime);
    g.setFont(juce::Font(juce::FontOptions(11.0f)));
    g.drawText(diag, 4, 0, getWidth() - 8, 14, juce::Justification::left);

    // 方法 A: Font(Typeface::Ptr) 直接
    g.setFont(fontA);
    g.setColour(juce::Colours::yellow);
    g.drawText("[A]こんにちは シンセ", 4, 16, getWidth(), 14, juce::Justification::left);

    // 方法 B: FontOptions(Typeface).withHeight
    g.setFont(fontB);
    g.setColour(juce::Colours::cyan);
    g.drawText("[B]こんにちは シンセ", 320, 16, getWidth(), 14, juce::Justification::left);

    // 方法 C: GlyphArrangement（描画パイプラインを迂回）
    {
        juce::GlyphArrangement ga;
        ga.addLineOfText(fontB, "[C-GA]こんにちは シンセ", 4.0f, 44.0f);
        g.setColour(juce::Colours::magenta);
        ga.draw(g);
    }

    // 方法 D: AttributedString + Font(Typeface)
    {
        juce::AttributedString s("[D-Attr]こんにちは シンセ");
        s.setFont(fontB);
        s.setColour(juce::Colours::white);
        s.draw(g, juce::Rectangle<float>(320.0f, 30.0f,
                                         (float)getWidth() - 320.0f, 16.0f));
    }
}

// -----------------------------------------------------------------------------
// resized — レイアウト設計書 §5 準拠
// -----------------------------------------------------------------------------
void SynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    const int w = area.getWidth();

    // ── ヘッダー ────────────────────────────────────────────────
    auto hdr = area.removeFromTop(HDR_H);
    titleLabel.setBounds(hdr.removeFromLeft(w / 3).reduced(10, 12));
    resetButton.setBounds(hdr.removeFromRight(80).reduced(8, 12));
    presetBox.setBounds(hdr.removeFromRight(200).reduced(4, 12));

    // ── ビジュアライザ ──────────────────────────────────────────
    auto vizArea = area.removeFromTop(VIZ_H);
    auto leftViz = vizArea.removeFromLeft(vizArea.getWidth() / 2);
    waveformDisplay.setBounds(leftViz.reduced(4));
    spectrumDisplay.setBounds(vizArea.reduced(4));

    // ── セクション 第1行 ────────────────────────────────────────
    auto row1 = area.removeFromTop(SEC_H);
    const int panelW = row1.getWidth() / 7;

    oscPanel.setBounds      (row1.removeFromLeft(panelW));
    noisePanel.setBounds    (row1.removeFromLeft(panelW));
    filterPanel.setBounds   (row1.removeFromLeft(panelW));
    filterEnvPanel.setBounds(row1.removeFromLeft(panelW));
    ampEnvPanel.setBounds   (row1.removeFromLeft(panelW));
    lfoPanel.setBounds      (row1.removeFromLeft(panelW));
    drivePanel.setBounds    (row1); // 7番目: 残りを使い切る

    // ── セクション 第2行 ────────────────────────────────────────
    auto row2 = area.removeFromTop(SEC_H);
    const int tw      = row2.getWidth();
    const int delayW  = juce::roundToInt(tw * 0.21f);
    const int reverbW = juce::roundToInt(tw * 0.17f);
    const int masterW = juce::roundToInt(tw * 0.21f);

    delayPanel.setBounds (row2.removeFromLeft(delayW));
    reverbPanel.setBounds(row2.removeFromLeft(reverbW));
    masterPanel.setBounds(row2.removeFromRight(masterW));
    voicePanel.setBounds (row2); // 残り中央（最も広い）

    // ── InfoPanel ───────────────────────────────────────────────
    infoPanel.setBounds(area.removeFromTop(INFO_H));

    // ── MIDIキーボード ──────────────────────────────────────────
    keyboard.setBounds(area.reduced(8, 4));
}

} // namespace synth
