// =============================================================================
// CustomLookAndFeel.h — ハードシンセ風スタイル（黒パネル + オレンジアクセント）
// =============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace synth {

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // カラーパレット（他コンポーネントからも参照可能）
    static const juce::Colour Background;  // #1a1a1a
    static const juce::Colour Panel;       // #2a2a2a
    static const juce::Colour Accent;      // #ff8c00
    static const juce::Colour TextPrimary; // #e0e0e0
    static const juce::Colour TextDim;     // #888888

    CustomLookAndFeel();
    ~CustomLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override;

    juce::Font getLabelFont(juce::Label& label) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool isMouseOverButton,
                              bool isButtonDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool isMouseOverButton,
                        bool isButtonDown) override;

    void drawComboBox(juce::Graphics& g, int width, int height,
                      bool isButtonDown, int buttonX, int buttonY,
                      int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawPopupMenuItem(juce::Graphics& g,
                           const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive,
                           bool isHighlighted, bool isTicked, bool hasSubMenu,
                           const juce::String& text,
                           const juce::String& shortcutKeyText,
                           const juce::Drawable* icon,
                           const juce::Colour* textColour) override;

    /** すべてのフォント要求に対し、日本語対応 Typeface を返す。
     *  JUCE 8 では setDefaultSansSerifTypefaceName だけでは
     *  匿名指定の Font に対して typeface が切り替わらないため、
     *  ここで強制的に日本語フォントを差し込む。 */
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font& font) override;

private:
    juce::String japaneseTypefaceName; // ctor で解決した実在フォント名を保持

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};

} // namespace synth
