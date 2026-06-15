// =============================================================================
// InfoPanel.cpp
// =============================================================================
#include "InfoPanel.h"

namespace synth {

void InfoPanel::setInfo(const juce::String& paramId, const juce::String& description)
{
    currentParamId = paramId;
    currentDescription = description;
    repaint();
}

void InfoPanel::paint(juce::Graphics& g)
{
    // 背景 + 上端の区切り線
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawHorizontalLine(0, 0.0f, static_cast<float>(getWidth()));

    auto area = getLocalBounds().reduced(8, 4);

    // 左端の "INFO" ラベル
    g.setColour(juce::Colour(0xff555555));
    g.setFont(juce::FontOptions(10.0f));
    g.drawText("INFO", area.removeFromLeft(40), juce::Justification::topLeft);

    // パラメータ ID（アクセント色、英字）
    auto paramArea = area.removeFromLeft(180);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText(currentParamId, paramArea, juce::Justification::topLeft);

    // 解説文（日本語、複数行折り返し、最大4行）
    g.setColour(juce::Colour(0xffe0e0e0));
    g.setFont(juce::FontOptions(10.0f));
    g.drawFittedText(currentDescription, area,
                     juce::Justification::topLeft, 4);
}

} // namespace synth
