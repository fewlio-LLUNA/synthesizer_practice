// =============================================================================
// InfoPanel.h — ホバー中パラメータの解説を表示するパネル
// Session E の Descriptions::getDescription と連動予定
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace synth {

class InfoPanel : public juce::Component
{
public:
    InfoPanel();

    // パラメータ ID と説明文をセット（PluginEditor のホバーコールバックから呼ぶ）
    void setInfo(const juce::String& paramId, const juce::String& description);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Label paramLabel;
    juce::Label descLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InfoPanel)
};

} // namespace synth
