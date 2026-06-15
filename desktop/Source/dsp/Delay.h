// =============================================================================
// Delay.h — juce::dsp::DelayLine ベースのディレイ（フィードバック + wet/dry）
// =============================================================================
#pragma once

#include <juce_dsp/juce_dsp.h>

namespace synth {

class Delay
{
public:
    static constexpr float MAX_DELAY_SECONDS = 2.0f;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void process(juce::AudioBuffer<float>& buffer,
                 float delaySeconds, float feedback, float mix);

private:
    using DelayLine = juce::dsp::DelayLine<float,
        juce::dsp::DelayLineInterpolationTypes::Linear>;

    DelayLine delayLineL;
    DelayLine delayLineR;

    float sampleRate { 44100.0f };

    juce::AudioBuffer<float> dryBuffer;
};

} // namespace synth
