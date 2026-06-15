// =============================================================================
// AmpEnvelopePanel.h — アンプエンベロープ (A / D / S / R)
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace synth {

class AmpEnvelopePanel : public juce::Component
{
public:
    explicit AmpEnvelopePanel(juce::AudioProcessorValueTreeState& apvts);

    std::function<void(juce::String)> onParamHover;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    void mouseEnter(const juce::MouseEvent& e) override;

    // UI コンポーネント
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label  attackLabel,  decayLabel,  sustainLabel,  releaseLabel;

    // Attachments
    std::unique_ptr<APVTS::SliderAttachment> attackAttachment;
    std::unique_ptr<APVTS::SliderAttachment> decayAttachment;
    std::unique_ptr<APVTS::SliderAttachment> sustainAttachment;
    std::unique_ptr<APVTS::SliderAttachment> releaseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpEnvelopePanel)
};

} // namespace synth
