// =============================================================================
// Noise.h — ホワイト / ピンクノイズ（Paul Kellet's algorithm）
// =============================================================================
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>  // ProcessSpec
#include "../Parameters.h"

namespace synth {

class Noise
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void setType(NoiseType type);
    void setLevel(float level);

    // モノラルバッファにノイズを加算
    void addTo(juce::AudioBuffer<float>& buffer, int numSamples);

private:
    NoiseType   noiseType   { NoiseType::White };
    float       level       { 0.0f };

    // Paul Kellet's pink noise フィルター状態
    float b0 { 0.0f }, b1 { 0.0f }, b2 { 0.0f };
    float b3 { 0.0f }, b4 { 0.0f }, b5 { 0.0f }, b6 { 0.0f };

    juce::Random rng;

    float getWhiteSample() noexcept;
    float getPinkSample() noexcept;
};

} // namespace synth
