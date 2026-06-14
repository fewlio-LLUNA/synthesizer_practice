// =============================================================================
// SynthSound.h — Synthesiser に登録する音の判定クラス
// 「このノートはこの Voice で鳴らすか」を判定する役割。
// 全 MIDI ノート・全 MIDI チャンネルに反応する単純実装。
// =============================================================================
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace synth {

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override     { return true; }
    bool appliesToChannel(int) override  { return true; }
};

} // namespace synth
