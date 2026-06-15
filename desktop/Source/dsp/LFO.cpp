// =============================================================================
// LFO.cpp
// =============================================================================
#include "LFO.h"
#include <cmath>

namespace synth {

void LFO::prepare(double sr)
{
    sampleRate = sr;
    phase = 0.0;
}

void LFO::reset()
{
    phase = 0.0;
}

void LFO::setRate(float hz)
{
    phaseInc = static_cast<double>(hz) / sampleRate;
}

void LFO::setDepth(float d)
{
    depth = juce::jlimit(0.0f, 1.0f, d);
}

void LFO::setWaveform(LfoWaveform w)
{
    waveform = w;
}

void LFO::setTarget(LfoTarget t)
{
    target = t;
}

float LFO::getNextSample() noexcept
{
    const float p = static_cast<float>(phase);
    float sample = 0.0f;

    switch (waveform)
    {
    case LfoWaveform::Sine:
        sample = std::sin(juce::MathConstants<float>::twoPi * p);
        break;
    case LfoWaveform::Sawtooth:
        sample = 2.0f * p - 1.0f;
        break;
    case LfoWaveform::Square:
        sample = p < 0.5f ? 1.0f : -1.0f;
        break;
    case LfoWaveform::Triangle:
        sample = p < 0.5f ? 4.0f * p - 1.0f
                          : -4.0f * p + 3.0f;
        break;
    }

    phase += phaseInc;
    if (phase >= 1.0) phase -= 1.0;

    return sample * depth;
}

} // namespace synth
