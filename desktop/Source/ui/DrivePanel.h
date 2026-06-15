// =============================================================================
// DrivePanel.h — ドライブセクション (Type / Amount / Mix)
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace synth {

class DrivePanel : public juce::Component
{
public:
    explicit DrivePanel(juce::AudioProcessorValueTreeState& apvts);

    std::function<void(juce::String)> onParamHover;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    void mouseEnter(const juce::MouseEvent& e) override;

    // UI コンポーネント
    juce::ComboBox typeBox;
    juce::Slider   amountSlider, mixSlider;
    juce::Label    typeLabel, amountLabel, mixLabel;

    // Attachments
    std::unique_ptr<APVTS::ComboBoxAttachment> typeAttachment;
    std::unique_ptr<APVTS::SliderAttachment>   amountAttachment;
    std::unique_ptr<APVTS::SliderAttachment>   mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrivePanel)
};

} // namespace synth
