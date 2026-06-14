// =============================================================================
// RotaryKnob.cpp — ロータリーノブ実装
// =============================================================================
#include "RotaryKnob.h"

namespace synth {

RotaryKnob::RotaryKnob(const juce::String& title,
                       const juce::String& paramId,
                       const juce::String& suffix)
    : paramId_(paramId)
{
    // スライダー設定
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    if (suffix.isNotEmpty())
        slider.setTextValueSuffix(" " + suffix);
    slider.setDoubleClickReturnValue(true, 0.0);
    slider.onValueChange = [this] { updateValueLabel(); };
    // スライダー上のマウスイベントも受け取るためにリスナー登録
    slider.addMouseListener(this, false);
    addAndMakeVisible(slider);

    // タイトルラベル
    titleLabel_.setText(title, juce::dontSendNotification);
    titleLabel_.setJustificationType(juce::Justification::centred);
    titleLabel_.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(titleLabel_);

    // 値表示ラベル
    valueLabel_.setJustificationType(juce::Justification::centred);
    valueLabel_.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(valueLabel_);
}

RotaryKnob::~RotaryKnob()
{
    slider.removeMouseListener(this);
}

void RotaryKnob::resized()
{
    static constexpr int TitleH = 18;
    static constexpr int ValueH = 16;

    auto bounds = getLocalBounds();
    titleLabel_.setBounds(bounds.removeFromTop(TitleH));
    valueLabel_.setBounds(bounds.removeFromBottom(ValueH));
    slider.setBounds(bounds);
}

void RotaryKnob::mouseEnter(const juce::MouseEvent& /*e*/)
{
    if (!hovering_)
    {
        hovering_ = true;
        if (onHover && paramId_.isNotEmpty())
            onHover(paramId_);
    }
}

void RotaryKnob::mouseExit(const juce::MouseEvent& e)
{
    // スライダーからのイベントで、まだ RotaryKnob 内にいる場合は無視
    const auto localPos = getLocalPoint(e.eventComponent, e.getPosition());
    if (!getLocalBounds().contains(localPos))
        hovering_ = false;
}

void RotaryKnob::setDefaultValue(double value)
{
    slider.setDoubleClickReturnValue(true, value);
}

void RotaryKnob::updateValueLabel()
{
    valueLabel_.setText(slider.getTextFromValue(slider.getValue()),
                        juce::dontSendNotification);
}

} // namespace synth
