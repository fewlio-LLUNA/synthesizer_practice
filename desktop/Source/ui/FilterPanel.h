// =============================================================================
// FilterPanel.h — フィルターセクション (Type / Cutoff / Resonance)
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace synth {

class FilterPanel : public juce::Component
{
public:
    explicit FilterPanel(juce::AudioProcessorValueTreeState& apvts);

    std::function<void(juce::String)> onParamHover;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    void mouseEnter(const juce::MouseEvent& e) override;

    // UI コンポーネント
    juce::ComboBox typeBox;
    juce::Slider   cutoffSlider, resoSlider;
    juce::Label    typeLabel, cutoffLabel, resoLabel;

    // Attachments
    std::unique_ptr<APVTS::ComboBoxAttachment> typeAttachment;
    std::unique_ptr<APVTS::SliderAttachment>   cutoffAttachment;
    std::unique_ptr<APVTS::SliderAttachment>   resoAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterPanel)
};

} // namespace synth
