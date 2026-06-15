// =============================================================================
// WaveformDisplay.h — 時間波形ビジュアライザ（Session C 担当）
//
// juce::AudioVisualiserComponent をラップし、
// 上部にタイトル「時間波形」とキャプションを常時表示する。
// =============================================================================
#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

namespace synth {

class WaveformDisplay : public juce::Component
{
public:
    /** audioVisualiser は PluginProcessor::getAudioVisualiser() の戻り値を渡す */
    explicit WaveformDisplay(juce::AudioVisualiserComponent& audioVisualiser);
    ~WaveformDisplay() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioVisualiserComponent& visualiserRef;

    // ヘッダー高さ（タイトル + キャプション）
    static constexpr int kHeaderHeight = 34;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};

} // namespace synth
