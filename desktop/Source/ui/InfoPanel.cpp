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

    // 解説文（日本語）
    // JUCE 8 の drawFittedText では日本語が化けるため、
    // 改行で行分割して drawText で1行ずつ描画する（PluginEditor::paint と
    // 同じ描画パスを通すと Windows のフォントフォールバックで日本語が出る）。
    g.setColour(juce::Colour(0xffe0e0e0));
    g.setFont(juce::FontOptions(11.0f));

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
