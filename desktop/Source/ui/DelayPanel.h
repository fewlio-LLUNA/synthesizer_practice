// =============================================================================
// DelayPanel.h — ディレイセクション (Time / Feedback / Mix)
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace synth {

class DelayPanel : public juce::Component
{
public:
    explicit DelayPanel(juce::AudioProcessorValueTreeState& apvts);

    std::function<void(juce::String)> onParamHover;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    void mouseEnter(const juce::MouseEvent& e) override;

    // UI コンポーネント
    juce::Slider timeSlider, feedbackSlider, mixSlider;
    juce::Label  timeLabel,  feedbackLabel,  mixLabel;

    // Attachments
    std::unique_ptr<APVTS::SliderAttachment> timeAttachment;
    std::unique_ptr<APVTS::SliderAttachment> feedbackAttachment;
    std::unique_ptr<APVTS::SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayPanel)
};

} // namespace synth
