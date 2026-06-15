// =============================================================================
// Noise.cpp
// =============================================================================
#include "Noise.h"

namespace synth {

void Noise::prepare(const juce::dsp::ProcessSpec& /*spec*/)
{
    b0 = b1 = b2 = b3 = b4 = b5 = b6 = 0.0f;
}

void Noise::setType(NoiseType type)
{
    noiseType = type;
}

void Noise::setLevel(float l)
{
    level = juce::jlimit(0.0f, 1.0f, l);
}

void Noise::addTo(juce::AudioBuffer<float>& buffer, int numSamples)
{
    if (level <= 0.0f) return;

    float* data = buffer.getWritePointer(0);
    for (int s = 0; s < numSamples; ++s)
    {
        const float sample = (noiseType == NoiseType::White)
                           ? getWhiteSample()
                           : getPinkSample();
        data[s] += sample * level;
    }
}

float Noise::getWhiteSample() noexcept
{
    return rng.nextFloat() * 2.0f - 1.0f;
}

// Paul Kellet's algorithm（ピンクノイズ近似）
float Noise::getPinkSample() noexcept
{
    const float white = getWhiteSample();

    b0 = 0.99886f * b0 + white * 0.0555179f;
    b1 = 0.99332f * b1 + white * 0.0750759f;
    b2 = 0.96900f * b2 + white * 0.1538520f;
    b3 = 0.86650f * b3 + white * 0.3104856f;
    b4 = 0.55000f * b4 + white * 0.5329522f;
    b5 = -0.7616f * b5 - white * 0.0168980f;

    const float pink = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362f) * 0.11f;
    b6 = white * 0.115926f;

    return juce::jlimit(-1.0f, 1.0f, pink);
}

} // namespace synth
