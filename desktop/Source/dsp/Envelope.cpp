// =============================================================================
// Envelope.cpp
// =============================================================================
#include "Envelope.h"

namespace synth {

void Envelope::prepare(double sampleRate)
{
    adsr.setSampleRate(sampleRate);
}

void Envelope::setParameters(float attack, float decay, float sustain, float release)
{
    juce::ADSR::Parameters params;
    params.attack  = juce::jmax(0.001f, attack);
    params.decay   = juce::jmax(0.001f, decay);
    params.sustain = juce::jlimit(0.0f, 1.0f, sustain);
    params.release = juce::jmax(0.001f, release);
    adsr.setParameters(params);
}

void Envelope::noteOn()
{
    adsr.noteOn();
}

void Envelope::noteOff()
{
    adsr.noteOff();
}

void Envelope::reset()
{
    adsr.reset();
}

bool Envelope::isActive() const
{
    return adsr.isActive();
}

float Envelope::getNextSample()
{
    return adsr.getNextSample();
}

void Envelope::applyEnvelopeToBuffer(juce::AudioBuffer<float>& buffer,
                                     int startSample, int numSamples)
{
    adsr.applyEnvelopeToBuffer(buffer, startSample, numSamples);
}

} // namespace synth
