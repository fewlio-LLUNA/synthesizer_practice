// =============================================================================
// LfoPanel.h — LFO セクション (Waveform / Rate / Depth / Target)
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace synth {

class LfoPanel : public juce::Component
{
public:
    explicit LfoPanel(juce::AudioProcessorValueTreeState& apvts);

    std::function<void(juce::String)> onParamHover;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    void mouseEnter(const juce::MouseEvent& e) override;

    // UI コンポーネント
    juce::ComboBox waveformBox, targetBox;
    juce::Slider   rateSlider, depthSlider;
    juce::Label    waveformLabel, rateLabel, depthLabel, targetLabel;

    // Attachments
    std::unique_ptr<APVTS::ComboBoxAttachment> waveformAttachment;
    std::unique_ptr<APVTS::SliderAttachment>   rateAttachment;
    std::unique_ptr<APVTS::SliderAttachment>   depthAttachment;
    std::unique_ptr<APVTS::ComboBoxAttachment> targetAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfoPanel)
};

} // namespace synth
