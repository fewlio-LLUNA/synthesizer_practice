// =============================================================================
// OscillatorPanel.h — オシレーターセクション (Waveform / Detune / Octave / Level)
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace synth {

class OscillatorPanel : public juce::Component
{
public:
    explicit OscillatorPanel(juce::AudioProcessorValueTreeState& apvts);

    std::function<void(juce::String)> onParamHover;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    void mouseEnter(const juce::MouseEvent& e) override;

    // UI コンポーネント
    juce::ComboBox waveformBox;
    juce::Slider   detuneSlider, octaveSlider, levelSlider;
    juce::Label    waveformLabel, detuneLabel, octaveLabel, levelLabel;

    // Attachments
    std::unique_ptr<APVTS::ComboBoxAttachment> waveformAttachment;
    std::unique_ptr<APVTS::SliderAttachment>   detuneAttachment;
    std::unique_ptr<APVTS::SliderAttachment>   octaveAttachment;
    std::unique_ptr<APVTS::SliderAttachment>   levelAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorPanel)
};

} // namespace synth
