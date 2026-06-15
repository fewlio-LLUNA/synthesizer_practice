// =============================================================================
// Drive.h — juce::dsp::WaveShaper ベースのドライブエフェクト
// =============================================================================
#pragma once

#include <juce_dsp/juce_dsp.h>
#include "../Parameters.h"

namespace synth {

class Drive
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    // ステレオバッファに適用
    void process(juce::AudioBuffer<float>& buffer,
                 DriveType type, float amount, float mix);

private:
    juce::dsp::WaveShaper<float> softShaper;
    juce::dsp::WaveShaper<float> hardShaper;

    // dry 信号を保持するバッファ（wet/dry ミックス用）
    juce::AudioBuffer<float> dryBuffer;

    static float softClip(float x) noexcept;
    static float hardClip(float x) noexcept;
};

} // namespace synth
