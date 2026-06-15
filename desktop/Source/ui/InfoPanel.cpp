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

    // === DIAGNOSTIC: PluginEditor::paint と完全に同じ書き方の日本語描画テスト ===
    g.setColour(juce::Colours::lime);
    g.setFont(juce::Font(juce::FontOptions(11.0f)));
    g.drawText("[InfoPanel-diag]こんにちは シンセ", 4, 0, getWidth() - 8, 14,
               juce::Justification::left);

    auto area = getLocalBounds().reduced(8, 4);

    // 左端の "INFO" ラベル
    g.setColour(juce::Colour(0xff555555));
    g.setFont(juce::Font(juce::FontOptions(10.0f)));
    g.drawText("INFO", area.removeFromLeft(40), juce::Justification::topLeft);

    // パラメータ ID（アクセント色、英字）
    auto paramArea = area.removeFromLeft(180);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::Font(juce::FontOptions(11.0f)));
    g.drawText(currentParamId, paramArea, juce::Justification::topLeft);

    // 解説文（日本語）
    // 重要: g.setFont は juce::Font 経由で渡す。
    // FontOptions を直接 setFont に渡すと JUCE 8 で別パスを通り日本語が化ける。
    g.setColour(juce::Colour(0xffe0e0e0));
    g.setFont(juce::Font(juce::FontOptions(11.0f)));

    const int lineH = 13;
    int y = area.getY();
    for (auto& line : juce::StringArray::fromLines(currentDescription))
    {
        if (y + lineH > area.getBottom()) break;
        g.drawText(line, area.getX(), y, area.getWidth(), lineH,
                   juce::Justification::topLeft);
        y += lineH;
    }
}

} // namespace synth
