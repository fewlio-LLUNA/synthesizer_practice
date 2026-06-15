// =============================================================================
// Delay.cpp
// =============================================================================
#include "Delay.h"

namespace synth {

void Delay::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = static_cast<float>(spec.sampleRate);

    const int maxDelaySamples = static_cast<int>(MAX_DELAY_SECONDS * sampleRate) + 1;
    delayLineL.setMaximumDelayInSamples(maxDelaySamples);
    delayLineR.setMaximumDelayInSamples(maxDelaySamples);

    juce::dsp::ProcessSpec monoSpec { spec.sampleRate, spec.maximumBlockSize, 1 };
    delayLineL.prepare(monoSpec);
    delayLineR.prepare(monoSpec);

    const int maxBlock = static_cast<int>(spec.maximumBlockSize);
    dryBuffer.setSize(static_cast<int>(spec.numChannels), maxBlock);
}

void Delay::reset()
{
    delayLineL.reset();
    delayLineR.reset();
}

void Delay::process(juce::AudioBuffer<float>& buffer,
                    float delaySeconds, float feedback, float mix)
{
    if (mix <= 0.0f) return;

    const int numCh      = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < numCh; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    const float delaySamples = juce::jlimit(1.0f,
                                            MAX_DELAY_SECONDS * sampleRate,
                                            delaySeconds * sampleRate);

    auto& lineL = (numCh > 1) ? delayLineL : delayLineL;
    auto& lineR = (numCh > 1) ? delayLineR : delayLineL;

    for (int s = 0; s < numSamples; ++s)
    {
        // L チャンネル
        {
            auto* data = buffer.getWritePointer(0);
            const float delayed = lineL.popSample(0, delaySamples, true);
            lineL.pushSample(0, data[s] + delayed * feedback);
            data[s] = data[s] * (1.0f - mix) + delayed * mix;
        }
        // R チャンネル（ステレオの場合）
        if (numCh > 1)
        {
            auto* data = buffer.getWritePointer(1);
            const float delayed = lineR.popSample(0, delaySamples, true);
            lineR.pushSample(0, data[s] + delayed * feedback);
            data[s] = data[s] * (1.0f - mix) + delayed * mix;
        }
    }
}

} // namespace synth
