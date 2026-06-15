// =============================================================================
// Reverb.h — juce::dsp::Reverb ラッパー
// =============================================================================
#pragma once

#include <juce_dsp/juce_dsp.h>

namespace synth {

class ReverbEffect
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void process(juce::AudioBuffer<float>& buffer, float decaySeconds, float mix);

private:
    juce::dsp::Reverb reverb;
    juce::AudioBuffer<float> dryBuffer;
};

} // namespace synth
