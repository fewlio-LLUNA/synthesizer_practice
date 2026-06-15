// =============================================================================
// SectionPanel.cpp — タイトル + コンテンツ枠の実装
// =============================================================================
#include "SectionPanel.h"
#include "CustomLookAndFeel.h"

namespace synth {

SectionPanel::SectionPanel(const juce::String& title)
{
    titleLabel_.setText(title, juce::dontSendNotification);
    titleLabel_.setJustificationType(juce::Justification::centredLeft);
    titleLabel_.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(titleLabel_);
}

void SectionPanel::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    // パネル背景（角丸）
    g.setColour(CustomLookAndFeel::Panel);
    g.fillRoundedRectangle(bounds, 6.0f);

    // タイトル帯のアクセントカラー（上角丸を保持するためクリップ）
    g.saveState();
    g.reduceClipRegion(getLocalBounds().removeFromTop(TitleHeight));
    g.setColour(CustomLookAndFeel::Accent.withAlpha(0.12f));
    g.fillRoundedRectangle(bounds, 6.0f);
    g.restoreState();

    // タイトルとコンテンツの境界線
    g.setColour(juce::Colour(0xFF404040));
    g.drawHorizontalLine(TitleHeight, 0.0f, (float)getWidth());

    // 外枠
    g.setColour(juce::Colour(0xFF404040));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);
}

void SectionPanel::resized()
{
    titleLabel_.setBounds(
        getLocalBounds().removeFromTop(TitleHeight).reduced(Padding + 2, 0));
}

juce::Rectangle<int> SectionPanel::getContentArea() const
{
    return getLocalBounds().withTrimmedTop(TitleHeight).reduced(Padding);
}

} // namespace synth
