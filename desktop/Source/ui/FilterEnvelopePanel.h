// =============================================================================
// FilterEnvelopePanel.h — フィルターエンベロープ (A / D / S / R / Amount)
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace synth {

class FilterEnvelopePanel : public juce::Component
{
public:
    explicit FilterEnvelopePanel(juce::AudioProcessorValueTreeState& apvts);

    std::function<void(juce::String)> onParamHover;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    void mouseEnter(const juce::MouseEvent& e) override;

    // UI コンポーネント
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider, amountSlider;
    juce::Label  attackLabel,  decayLabel,  sustainLabel,  releaseLabel,  amountLabel;

    // Attachments
    std::unique_ptr<APVTS::SliderAttachment> attackAttachment;
    std::unique_ptr<APVTS::SliderAttachment> decayAttachment;
    std::unique_ptr<APVTS::SliderAttachment> sustainAttachment;
    std::unique_ptr<APVTS::SliderAttachment> releaseAttachment;
    std::unique_ptr<APVTS::SliderAttachment> amountAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterEnvelopePanel)
};

} // namespace synth
