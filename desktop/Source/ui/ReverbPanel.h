// =============================================================================
// ReverbPanel.h — リバーブセクション (Decay / Mix)
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace synth {

class ReverbPanel : public juce::Component
{
public:
    explicit ReverbPanel(juce::AudioProcessorValueTreeState& apvts);

    std::function<void(juce::String)> onParamHover;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    void mouseEnter(const juce::MouseEvent& e) override;

    // UI コンポーネント
    juce::Slider decaySlider, mixSlider;
    juce::Label  decayLabel,  mixLabel;

    // Attachments
    std::unique_ptr<APVTS::SliderAttachment> decayAttachment;
    std::unique_ptr<APVTS::SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbPanel)
};

} // namespace synth
