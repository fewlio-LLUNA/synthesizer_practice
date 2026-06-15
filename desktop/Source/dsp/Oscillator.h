// =============================================================================
// Oscillator.h — 波形生成（juce::dsp::Oscillator ベース、ユニゾン対応）
// =============================================================================
#pragma once

#include <juce_dsp/juce_dsp.h>
#include "../Parameters.h"
#include <array>

namespace synth {

class Oscillator
{
public:
    static constexpr int MAX_UNISON = 8;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setFrequency(double hz);
    void setWaveform(OscWaveform waveform);
    void setDetuneCents(float cents);
    void setOctave(int octave);
    void setLevel(float level);
    void setUnisonCount(int count);
    void setUnisonDetuneCents(float cents);

    // モノラルバッファにサンプルを加算
    void process(juce::AudioBuffer<float>& buffer, int numSamples);

private:
    // juce::dsp::Oscillator の代わりに位相アキュムレーターを使用
    // （ランタイムでの波形切り替えを RT-safe に行うため）
    struct UnisonOsc
    {
        double phase        { 0.0 };
        double phaseInc     { 0.0 };
    };

    std::array<UnisonOsc, MAX_UNISON> oscs;

    int         unisonCount     { 1 };
    double      baseFrequency   { 440.0 };
    double      sampleRate      { 44100.0 };
    float       detuneCents     { 0.0f };
    int         octave          { 0 };
    float       level           { 1.0f };
    float       unisonDetune    { 20.0f };
    OscWaveform waveform        { OscWaveform::Sawtooth };

    float generateSample(UnisonOsc& osc) noexcept;
    void  updateFrequencies();

    static float centsToRatio(float cents) noexcept;
};

} // namespace synth
