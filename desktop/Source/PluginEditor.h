// =============================================================================
// PluginEditor.h — GUI エントリポイント
//
// Session D 担当: 全セクションパネルの配置とレイアウト実装。
// Session B の RotaryKnob / SectionPanel が揃い次第パネル内部を更新。
// Session C の Visualizer が揃い次第ビジュアライザ領域に組み込む。
// Session E の Descriptions が揃い次第 InfoPanel のホバー文言を実化する。
// =============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>  // MidiKeyboardComponent
#include <juce_gui_extra/juce_gui_extra.h>

#include "PluginProcessor.h"
#include "ui/OscillatorPanel.h"
#include "ui/NoisePanel.h"
#include "ui/FilterPanel.h"
#include "ui/FilterEnvelopePanel.h"
#include "ui/AmpEnvelopePanel.h"
#include "ui/LfoPanel.h"
#include "ui/DrivePanel.h"
#include "ui/DelayPanel.h"
#include "ui/ReverbPanel.h"
#include "ui/VoicePanel.h"
#include "ui/MasterPanel.h"
#include "ui/InfoPanel.h"

namespace synth {

class SynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SynthAudioProcessorEditor(SynthAudioProcessor&);
    ~SynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SynthAudioProcessor& processorRef;

    // ヘッダー
    juce::Label      titleLabel;
    juce::ComboBox   presetBox;
    juce::TextButton resetButton;

    // セクションパネル — 宣言順に初期化されるため processorRef より後に配置
    OscillatorPanel     oscPanel;
    NoisePanel          noisePanel;
    FilterPanel         filterPanel;
    FilterEnvelopePanel filterEnvPanel;
    AmpEnvelopePanel    ampEnvPanel;
    LfoPanel            lfoPanel;
    DrivePanel          drivePanel;
    DelayPanel          delayPanel;
    ReverbPanel         reverbPanel;
    VoicePanel          voicePanel;
    MasterPanel         masterPanel;

    // パラメータ解説表示
    InfoPanel infoPanel;

    // MIDIキーボード
    juce::MidiKeyboardComponent keyboard;

    // ビジュアライザ領域（Session C の成果物が入るまでのプレースホルダー）
    juce::Rectangle<int> visualizerBounds;

    // 全パネルのホバーコールバックを InfoPanel に接続する
    void setupHoverCallbacks();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessorEditor)
};

} // namespace synth
