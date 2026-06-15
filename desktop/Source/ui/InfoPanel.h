// =============================================================================
// InfoPanel.h — ホバー中パラメータの解説を表示するパネル
//
// JUCE 8 では juce::Label の描画パスで日本語が化けるため、内部で Label は
// 使わず paint() で直接 g.drawText / g.drawFittedText を呼ぶ。
// =============================================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace synth {

class InfoPanel : public juce::Component
{
public:
    InfoPanel() = default;

    /** パラメータ ID と説明文をセット（PluginEditor のホバーコールバックから呼ぶ） */
    void setInfo(const juce::String& paramId, const juce::String& description);

    void paint(juce::Graphics& g) override;

private:
    juce::String currentParamId;
    juce::String currentDescription;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InfoPanel)
};

} // namespace synth
