// =============================================================================
// Filter.cpp
// =============================================================================
#include "Filter.h"

namespace synth {

void Filter::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    tptFilter.prepare(spec);
    applyParameters();
}

void Filter::reset()
{
    tptFilter.reset();
}

void Filter::setType(FilterType type)
{
    filterType = type;
    switch (type)
    {
    case FilterType::LowPass:
        tptFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        break;
    case FilterType::HighPass:
        tptFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        break;
    case FilterType::BandPass:
        tptFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        break;
    }
}

void Filter::setBaseCutoff(float hz)
{
    baseCutoff = juce::jlimit(20.0f, 20000.0f, hz);
    applyParameters();
}

void Filter::setResonance(float q)
{
    resonance = juce::jlimit(0.1f, 30.0f, q);
    tptFilter.setResonance(resonance);
}

void Filter::setCutoffModFactor(float factor)
{
    modFactor = juce::jlimit(0.01f, 100.0f, factor);
    applyParameters();
}

void Filter::process(juce::AudioBuffer<float>& buffer, int numSamples)
{
    auto block = juce::dsp::AudioBlock<float>(buffer).getSubBlock(0, (size_t)numSamples);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    tptFilter.process(context);
}

void Filter::applyParameters()
{
    const float cutoff = juce::jlimit(20.0f, 20000.0f, baseCutoff * modFactor);
    tptFilter.setCutoffFrequency(cutoff);
}

} // namespace synth
