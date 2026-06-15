// =============================================================================
// LFO.h — 低周波変調器（Pitch / Filter / Amp の3ターゲット）
// =============================================================================
#pragma once

#include <juce_dsp/juce_dsp.h>
#include "../Parameters.h"

namespace synth {

class LFO
{
public:
    void prepare(double sampleRate);
    void reset();

    void setRate(float hz);
    void setDepth(float depth);   // 0..1
    void setWaveform(LfoWaveform waveform);
    void setTarget(LfoTarget target);

    LfoTarget getTarget() const noexcept { return target; }

    // 次のサンプル値を返す（-depth .. +depth）
    float getNextSample() noexcept;

private:
    double      phase       { 0.0 };
    double      phaseInc    { 0.0 };
    double      sampleRate  { 44100.0 };
    float       depth       { 0.0f };
    LfoWaveform waveform    { LfoWaveform::Sine };
    LfoTarget   target      { LfoTarget::Pitch };
};

} // namespace synth
