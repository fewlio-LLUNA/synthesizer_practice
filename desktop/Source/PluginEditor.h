// =============================================================================
// PluginEditor.h — GUI のエントリポイント
//
// TODO: Session D が各セクションコンポーネントを配置する。
// =============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

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

    // 仮の鍵盤（Session D で本格的なレイアウトに置き換え）
    juce::MidiKeyboardComponent keyboard {
        processorRef.getKeyboardState(),
        juce::MidiKeyboardComponent::horizontalKeyboard
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessorEditor)
};

} // namespace synth
