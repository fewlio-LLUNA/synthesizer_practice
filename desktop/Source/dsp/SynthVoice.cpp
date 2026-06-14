// =============================================================================
// SynthVoice.cpp — ボイス実装（最小スタブ）
//
// TODO: Session A 担当 — サイン波だけでもまず音が鳴るところから開始。
// =============================================================================
#include "SynthVoice.h"
#include "SynthSound.h"

namespace synth {

SynthVoice::SynthVoice() = default;
SynthVoice::~SynthVoice() = default;

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int /*midiNoteNumber*/, float /*velocity*/,
                           juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
    // TODO: Session A — エンベロープ trigger、周波数設定
}

void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (!allowTailOff)
        clearCurrentNote();
    // TODO: Session A — エンベロープ release
}

void SynthVoice::pitchWheelMoved(int) {}
void SynthVoice::controllerMoved(int, int) {}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& /*outputBuffer*/,
                                 int /*startSample*/, int /*numSamples*/)
{
    // TODO: Session A — サンプルを生成して outputBuffer に加算
}

} // namespace synth
