// =============================================================================
// CustomLookAndFeel.cpp — ハードシンセ風スタイル実装
// =============================================================================
#include "CustomLookAndFeel.h"
#include <cmath>

namespace synth {

const juce::Colour CustomLookAndFeel::Background  { 0xFF1a1a1a };
const juce::Colour CustomLookAndFeel::Panel       { 0xFF2a2a2a };
const juce::Colour CustomLookAndFeel::Accent      { 0xFFff8c00 };
const juce::Colour CustomLookAndFeel::TextPrimary { 0xFFe0e0e0 };
const juce::Colour CustomLookAndFeel::TextDim     { 0xFF888888 };

// 利用可能な日本語フォントを優先順に試して採用する
static juce::String pickJapaneseTypefaceName()
{
    const juce::StringArray candidates {
        "Yu Gothic UI",
        "Yu Gothic",
        "Meiryo UI",
        "Meiryo",
        "MS UI Gothic",
        "MS Gothic",
        "ＭＳ ゴシック",
        "BIZ UDGothic"
    };
    const auto available = juce::Font::findAllTypefaceNames();
    for (const auto& name : candidates)
        if (available.contains(name))
            return name;
    return "Yu Gothic"; // 最終フォールバック
}

CustomLookAndFeel::CustomLookAndFeel()
    : japaneseTypefaceName(pickJapaneseTypefaceName())
{
    DBG("[Font] CustomLookAndFeel using: " << japaneseTypefaceName);
    setDefaultSansSerifTypefaceName(japaneseTypefaceName);

    setColour(juce::ResizableWindow::backgroundColourId, Background);
    setColour(juce::DocumentWindow::backgroundColourId,  Background);

    setColour(juce::Slider::thumbColourId,               Accent);
    setColour(juce::Slider::rotarySliderFillColourId,    Accent);
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xFF444444));
    setColour(juce::Slider::backgroundColourId,          Panel);
    setColour(juce::Slider::textBoxTextColourId,         TextPrimary);
    setColour(juce::Slider::textBoxBackgroundColourId,   Background);
    setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);

    setColour(juce::Label::textColourId,            TextPrimary);
    setColour(juce::Label::backgroundColourId,      juce::Colours::transparentBlack);
    setColour(juce::Label::outlineColourId,         juce::Colours::transparentBlack);
    setColour(juce::Label::textWhenEditingColourId, TextPrimary);

    setColour(juce::TextButton::buttonColourId,  Panel);
    setColour(juce::TextButton::textColourOffId, TextPrimary);
    setColour(juce::TextButton::textColourOnId,  Accent);

    setColour(juce::ComboBox::backgroundColourId, Panel);
    setColour(juce::ComboBox::textColourId,       TextPrimary);
    setColour(juce::ComboBox::outlineColourId,    juce::Colour(0xFF444444));
    setColour(juce::ComboBox::arrowColourId,      TextDim);

    setColour(juce::PopupMenu::backgroundColourId,           Panel);
    setColour(juce::PopupMenu::textColourId,                 TextPrimary);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, Accent.withAlpha(0.3f));
    setColour(juce::PopupMenu::highlightedTextColourId,      TextPrimary);
}

void CustomLookAndFeel::drawRotarySlider(
    juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& /*slider*/)
{
    const float cx = (float)x + (float)width  * 0.5f;
    const float cy = (float)y + (float)height * 0.5f;
    const float outerR    = juce::jmin(width, height) * 0.5f - 3.0f;
    const float trackW    = 3.0f;
    const float knobR     = outerR - trackW * 2.5f;

    // トラック背景（全円弧）
    {
        juce::Path track;
        track.addCentredArc(cx, cy, outerR, outerR, 0.0f,
                            rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xFF444444));
        g.strokePath(track, juce::PathStrokeType(
            trackW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // 値の弧（アクセントカラー）
    const float currentAngle = rotaryStartAngle
        + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    if (sliderPosProportional > 0.0f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc(cx, cy, outerR, outerR, 0.0f,
                               rotaryStartAngle, currentAngle, true);
        g.setColour(Accent);
        g.strokePath(valueArc, juce::PathStrokeType(
            trackW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // ノブ本体（グラデーション円）
    {
        juce::ColourGradient grad(
            juce::Colour(0xFF3c3c3c), cx, cy - knobR,
            juce::Colour(0xFF1c1c1c), cx, cy + knobR, false);
        g.setGradientFill(grad);
        g.fillEllipse(cx - knobR, cy - knobR, knobR * 2.0f, knobR * 2.0f);

        g.setColour(juce::Colour(0xFF555555));
        g.drawEllipse(cx - knobR, cy - knobR, knobR * 2.0f, knobR * 2.0f, 1.0f);
    }

    // インジケーター（中心 → 外側方向の線 + 先端ドット）
    {
        const float len = knobR * 0.55f;
        const float ix  = cx + std::sin(currentAngle) * len;
        const float iy  = cy - std::cos(currentAngle) * len;
        g.setColour(Accent);
        g.drawLine(cx, cy, ix, iy, 2.0f);
        g.fillEllipse(ix - 2.5f, iy - 2.5f, 5.0f, 5.0f);
    }
}

// JUCE 8 では「Yu Gothic UI」のような name 明示指定だと解決失敗で化けるが、
// name 未指定だと Windows のフォントフォールバックで日本語が表示される。
// よって全フォント取得は name を渡さず、サイズのみの FontOptions を返す。
juce::Font CustomLookAndFeel::getLabelFont(juce::Label& /*label*/)
{
    return juce::Font(juce::FontOptions(11.0f));
}

juce::Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox& box)
{
    return juce::Font(juce::FontOptions(juce::jmin(15.0f, (float)box.getHeight() * 0.85f)));
}

juce::Font CustomLookAndFeel::getPopupMenuFont()
{
    return juce::Font(juce::FontOptions(13.0f));
}

juce::Font CustomLookAndFeel::getTextButtonFont(juce::TextButton& /*button*/, int buttonHeight)
{
    return juce::Font(juce::FontOptions(juce::jmin(15.0f, (float)buttonHeight * 0.6f)));
}

void CustomLookAndFeel::drawButtonBackground(
    juce::Graphics& g, juce::Button& button,
    const juce::Colour& /*backgroundColour*/,
    bool isMouseOver, bool isButtonDown)
{
    const auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
    const auto base   = isButtonDown ? Accent.darker(0.2f)
                      : isMouseOver  ? Panel.brighter(0.08f)
                                     : Panel;
    g.setColour(base);
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(isMouseOver ? Accent.withAlpha(0.7f) : juce::Colour(0xFF444444));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

void CustomLookAndFeel::drawButtonText(
    juce::Graphics& g, juce::TextButton& button,
    bool /*isMouseOver*/, bool /*isButtonDown*/)
{
    g.setFont(juce::Font(juce::FontOptions(12.0f)));
    g.setColour(button.getToggleState() ? Accent : TextPrimary);
    g.drawFittedText(button.getButtonText(),
                     button.getLocalBounds().reduced(4, 2),
                     juce::Justification::centred, 1);
}

void CustomLookAndFeel::drawComboBox(
    juce::Graphics& g, int width, int height,
    bool /*isButtonDown*/, int /*buttonX*/, int /*buttonY*/,
    int /*buttonW*/, int /*buttonH*/, juce::ComboBox& /*box*/)
{
    const juce::Rectangle<int> boxBounds(0, 0, width, height);
    g.setColour(Panel);
    g.fillRoundedRectangle(boxBounds.toFloat(), 4.0f);

    g.setColour(juce::Colour(0xFF444444));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f), 4.0f, 1.0f);

    // 下向き矢印
    const float ax = (float)(width - 14);
    const float ay = (float)(height / 2);
    juce::Path arrow;
    arrow.addTriangle(ax, ay - 3.0f,
                      ax + 6.0f, ay - 3.0f,
                      ax + 3.0f, ay + 3.0f);
    g.setColour(TextDim);
    g.fillPath(arrow);
}

void CustomLookAndFeel::drawPopupMenuItem(
    juce::Graphics& g, const juce::Rectangle<int>& area,
    bool isSeparator, bool isActive, bool isHighlighted,
    bool isTicked, bool /*hasSubMenu*/,
    const juce::String& text, const juce::String& /*shortcutKeyText*/,
    const juce::Drawable* /*icon*/, const juce::Colour* /*textColour*/)
{
    if (isSeparator)
    {
        const int midY = area.getCentreY();
        g.setColour(juce::Colour(0xFF444444));
        g.fillRect(area.getX() + 8, midY, area.getWidth() - 16, 1);
        return;
    }

    if (isHighlighted && isActive)
    {
        g.setColour(Accent.withAlpha(0.25f));
        g.fillRect(area);
    }

    g.setFont(juce::Font(juce::FontOptions(12.0f)));
    g.setColour(isTicked ? Accent : (isActive ? TextPrimary : TextDim));
    g.drawFittedText(text, area.reduced(8, 0),
                     juce::Justification::centredLeft, 1);
}

// -----------------------------------------------------------------------------
// すべてのフォント要求に日本語対応 Typeface を返す
// -----------------------------------------------------------------------------
juce::Typeface::Ptr CustomLookAndFeel::getTypefaceForFont(const juce::Font& font)
{
    ++typefaceCallCount;
    // JUCE 8 では Font::withTypefaceName が削除されたため、FontOptions で再構築する。
    // サイズ・スタイルは元のフォントから継承し、typeface 名だけ日本語対応に差し替える。
    return juce::Typeface::createSystemTypefaceFor(
        juce::FontOptions(japaneseTypefaceName,
                          font.getHeight(),
                          font.getStyleFlags()));
}

} // namespace synth
