// =============================================================================
// Reverb.cpp
// =============================================================================
#include "Reverb.h"

namespace synth {

void ReverbEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    reverb.prepare(spec);

    const int maxBlock = static_cast<int>(spec.maximumBlockSize);
    dryBuffer.setSize(static_cast<int>(spec.numChannels), maxBlock);
}

void ReverbEffect::reset()
{
    reverb.reset();
}

void ReverbEffect::process(juce::AudioBuffer<float>& buffer, float decaySeconds, float mix)
{
    if (mix <= 0.0f) return;

    const int numCh      = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < numCh; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    // juce::dsp::Reverb のパラメータ（decay を room size に変換）
    juce::dsp::Reverb::Parameters params;
    params.roomSize   = juce::jlimit(0.0f, 1.0f, decaySeconds / 10.0f);
    params.damping    = 0.5f;
    params.wetLevel   = mix;
    params.dryLevel   = 1.0f - mix;
    params.width      = 1.0f;
    params.freezeMode = 0.0f;
    reverb.setParameters(params);

    auto block = juce::dsp::AudioBlock<float>(buffer).getSubBlock(0, (size_t)numSamples);
    auto ctx   = juce::dsp::ProcessContextReplacing<float>(block);
    reverb.process(ctx);
}

} // namespace synth
