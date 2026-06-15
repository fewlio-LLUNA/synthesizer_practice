// =============================================================================
// Oscillator.cpp
// =============================================================================
#include "Oscillator.h"
#include <cmath>

namespace synth {

void Oscillator::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    updateFrequencies();
}

void Oscillator::reset()
{
    for (auto& osc : oscs)
        osc.phase = 0.0;
}

void Oscillator::setFrequency(double hz)
{
    baseFrequency = hz;
    updateFrequencies();
}

void Oscillator::setWaveform(OscWaveform newWaveform)
{
    waveform = newWaveform;
}

void Oscillator::setDetuneCents(float cents)
{
    detuneCents = cents;
    updateFrequencies();
}

void Oscillator::setOctave(int oct)
{
    octave = oct;
    updateFrequencies();
}

void Oscillator::setLevel(float l)
{
    level = l;
}

void Oscillator::setUnisonCount(int count)
{
    unisonCount = juce::jlimit(1, MAX_UNISON, count);
    updateFrequencies();
}

void Oscillator::setUnisonDetuneCents(float cents)
{
    unisonDetune = cents;
    updateFrequencies();
}

void Oscillator::process(juce::AudioBuffer<float>& buffer, int numSamples)
{
    if (sampleRate <= 0.0) return;

    const float gain = level / static_cast<float>(unisonCount);
    float* data = buffer.getWritePointer(0);

    for (int s = 0; s < numSamples; ++s)
    {
        float sample = 0.0f;
        for (int u = 0; u < unisonCount; ++u)
            sample += generateSample(oscs[u]);
        data[s] += sample * gain;
    }
}

float Oscillator::generateSample(UnisonOsc& osc) noexcept
{
    const float p = static_cast<float>(osc.phase);
    float sample = 0.0f;

    switch (waveform)
    {
    case OscWaveform::Sine:
        sample = std::sin(juce::MathConstants<float>::twoPi * p);
        break;
    case OscWaveform::Sawtooth:
        sample = 2.0f * p - 1.0f;
        break;
    case OscWaveform::Square:
        sample = p < 0.5f ? 1.0f : -1.0f;
        break;
    case OscWaveform::Triangle:
        sample = p < 0.5f ? 4.0f * p - 1.0f
                          : -4.0f * p + 3.0f;
        break;
    }

    osc.phase += osc.phaseInc;
    if (osc.phase >= 1.0)
        osc.phase -= 1.0;

    return sample;
}

void Oscillator::updateFrequencies()
{
    if (sampleRate <= 0.0) return;

    const double octaveMult = std::pow(2.0, static_cast<double>(octave));
    const double detuneRatio = static_cast<double>(centsToRatio(detuneCents));
    const double baseHz = baseFrequency * octaveMult * detuneRatio;

    if (unisonCount == 1)
    {
        oscs[0].phaseInc = baseHz / sampleRate;
        return;
    }

    for (int u = 0; u < unisonCount; ++u)
    {
        const float frac = static_cast<float>(u) / static_cast<float>(unisonCount - 1) - 0.5f;
        const double uHz = baseHz * static_cast<double>(centsToRatio(frac * unisonDetune));
        oscs[u].phaseInc = uHz / sampleRate;
    }
}

float Oscillator::centsToRatio(float cents) noexcept
{
    return std::pow(2.0f, cents / 1200.0f);
}

} // namespace synth
