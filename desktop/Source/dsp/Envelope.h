// =============================================================================
// Envelope.h — juce::ADSR ラッパー（Amp / Filter 2系統で使用）
// =============================================================================
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace synth {

class Envelope
{
public:
    void prepare(double sampleRate);
    void setParameters(float attack, float decay, float sustain, float release);
    void noteOn();
    void noteOff();
    void reset();

    bool isActive() const;

    // 1サンプル進めてエンベロープ値を返す
    float getNextSample();

    // バッファ全体にエンベロープを乗算（AmpEnv 用）
    void applyEnvelopeToBuffer(juce::AudioBuffer<float>& buffer,
                               int startSample, int numSamples);

private:
    juce::ADSR adsr;
};

} // namespace synth
