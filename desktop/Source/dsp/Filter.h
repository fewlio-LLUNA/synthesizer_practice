// =============================================================================
// Filter.h — juce::dsp::StateVariableTPTFilter ベース
// =============================================================================
#pragma once

#include <juce_dsp/juce_dsp.h>
#include "../Parameters.h"

namespace synth {

class Filter
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setType(FilterType type);
    void setBaseCutoff(float hz);
    void setResonance(float q);

    // カットオフ変調係数（フィルターエンベロープ + LFO から）
    void setCutoffModFactor(float factor);

    void process(juce::AudioBuffer<float>& buffer, int numSamples);

private:
    juce::dsp::StateVariableTPTFilter<float> tptFilter;

    float       baseCutoff  { 2000.0f };
    float       modFactor   { 1.0f };
    float       resonance   { 1.0f };
    double      sampleRate  { 44100.0 };
    FilterType  filterType  { FilterType::LowPass };

    void applyParameters();
};

} // namespace synth
