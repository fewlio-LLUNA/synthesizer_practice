// =============================================================================
// RotaryKnob.h — ロータリーノブ（Slider + Label + ホバーコールバック）
// =============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

namespace synth {

class RotaryKnob : public juce::Component
{
public:
    // ホバー時コールバック（InfoPanel 連動用）。paramId を渡す
    std::function<void(const juce::String&)> onHover;

    // APVTS::SliderAttachment に渡すために public で公開
    juce::Slider slider;

    RotaryKnob(const juce::String& title,
               const juce::String& paramId = {},
               const juce::String& suffix  = {});
    ~RotaryKnob() override;

    void resized() override;
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e)  override;

    // SliderAttachment 後に呼んでダブルクリックリセット値を設定する
    void setDefaultValue(double value);

private:
    juce::String paramId_;
    juce::Label  titleLabel_;
    juce::Label  valueLabel_;
    bool         hovering_ = false;

    void updateValueLabel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnob)
};

} // namespace synth
