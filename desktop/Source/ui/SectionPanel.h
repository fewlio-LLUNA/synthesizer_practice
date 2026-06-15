// =============================================================================
// SectionPanel.h — タイトル + コンテンツ領域の共通枠コンポーネント
// =============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace synth {

class SectionPanel : public juce::Component
{
public:
    static constexpr int TitleHeight = 22;
    static constexpr int Padding     = 4;

    explicit SectionPanel(const juce::String& title);
    ~SectionPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // タイトル領域を除いたコンテンツ矩形（子コンポーネント配置に使用）
    juce::Rectangle<int> getContentArea() const;

private:
    juce::Label titleLabel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SectionPanel)
};

} // namespace synth
