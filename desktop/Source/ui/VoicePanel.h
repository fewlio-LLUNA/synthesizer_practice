// =============================================================================
// VoicePanel.h — ボイスセクション (Unison Count / Unison Detune / Glide)
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace synth {

class VoicePanel : public juce::Component
{
public:
    explicit VoicePanel(juce::AudioProcessorValueTreeState& apvts);

    std::function<void(juce::String)> onParamHover;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    void mouseEnter(const juce::MouseEvent& e) override;

    // UI コンポーネント
    juce::Slider unisonCountSlider, unisonDetuneSlider, glideSlider;
    juce::Label  unisonCountLabel,  unisonDetuneLabel,  glideLabel;

    // Attachments
    std::unique_ptr<APVTS::SliderAttachment> unisonCountAttachment;
    std::unique_ptr<APVTS::SliderAttachment> unisonDetuneAttachment;
    std::unique_ptr<APVTS::SliderAttachment> glideAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoicePanel)
};

} // namespace synth
