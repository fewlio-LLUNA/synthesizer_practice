// =============================================================================
// NoisePanel.h — ノイズセクション (Type / Level)
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace synth {

class NoisePanel : public juce::Component
{
public:
    explicit NoisePanel(juce::AudioProcessorValueTreeState& apvts);

    std::function<void(juce::String)> onParamHover;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    void mouseEnter(const juce::MouseEvent& e) override;

    // UI コンポーネント（Attachment より前に宣言）
    juce::ComboBox typeBox;
    juce::Slider   levelSlider;
    juce::Label    typeLabel, levelLabel;

    // Attachments（UI コンポーネントより後に宣言）
    std::unique_ptr<APVTS::ComboBoxAttachment> typeAttachment;
    std::unique_ptr<APVTS::SliderAttachment>   levelAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoisePanel)
};

} // namespace synth
