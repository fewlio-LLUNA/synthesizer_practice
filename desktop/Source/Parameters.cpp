// =============================================================================
// Parameters.cpp — APVTS Layout 構築
//
// Session A が最低限のパラメータを追加（受け入れ条件の動作確認用）。
// Session E が全パラメータの完全実装を行う。
// 設計: docs/desktop-design.md §3 参照
// =============================================================================
#include "Parameters.h"

namespace synth {

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    // ----- オシレーター -----
    layout.add(std::make_unique<AudioParameterChoice>(
        ParameterID{ ParamID::OscWaveform, 1 }, "Osc Waveform",
        StringArray{ "Sine", "Saw", "Square", "Triangle" }, 1));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::OscDetune, 1 }, "Osc Detune",
        NormalisableRange<float>{ -1200.0f, 1200.0f, 1.0f }, 0.0f));

    layout.add(std::make_unique<AudioParameterInt>(
        ParameterID{ ParamID::OscOctave, 1 }, "Osc Octave", -2, 2, 0));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::OscLevel, 1 }, "Osc Level",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f }, 1.0f));

    // ----- ノイズ -----
    layout.add(std::make_unique<AudioParameterChoice>(
        ParameterID{ ParamID::NoiseType, 1 }, "Noise Type",
        StringArray{ "White", "Pink" }, 0));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::NoiseLevel, 1 }, "Noise Level",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f }, 0.0f));

    // ----- フィルター -----
    layout.add(std::make_unique<AudioParameterChoice>(
        ParameterID{ ParamID::FilterType, 1 }, "Filter Type",
        StringArray{ "LP", "HP", "BP" }, 0));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::FilterCutoff, 1 }, "Filter Cutoff",
        NormalisableRange<float>{ 20.0f, 20000.0f, 0.1f, 0.3f }, 2000.0f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::FilterReso, 1 }, "Filter Resonance",
        NormalisableRange<float>{ 0.1f, 30.0f, 0.01f }, 1.0f));

    // ----- フィルターエンベロープ -----
    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::FEnvAttack, 1 }, "FEnv Attack",
        NormalisableRange<float>{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.01f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::FEnvDecay, 1 }, "FEnv Decay",
        NormalisableRange<float>{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.3f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::FEnvSustain, 1 }, "FEnv Sustain",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f }, 0.4f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::FEnvRelease, 1 }, "FEnv Release",
        NormalisableRange<float>{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.3f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::FEnvAmount, 1 }, "FEnv Amount",
        NormalisableRange<float>{ -1.0f, 1.0f, 0.001f }, 0.0f));

    // ----- アンプエンベロープ -----
    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::AmpAttack, 1 }, "Amp Attack",
        NormalisableRange<float>{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.01f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::AmpDecay, 1 }, "Amp Decay",
        NormalisableRange<float>{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.2f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::AmpSustain, 1 }, "Amp Sustain",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f }, 0.7f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::AmpRelease, 1 }, "Amp Release",
        NormalisableRange<float>{ 0.001f, 5.0f, 0.001f, 0.5f }, 0.3f));

    // ----- LFO -----
    layout.add(std::make_unique<AudioParameterChoice>(
        ParameterID{ ParamID::LfoWaveform, 1 }, "LFO Waveform",
        StringArray{ "Sine", "Saw", "Square", "Triangle" }, 0));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::LfoRate, 1 }, "LFO Rate",
        NormalisableRange<float>{ 0.1f, 20.0f, 0.01f }, 5.0f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::LfoDepth, 1 }, "LFO Depth",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f }, 0.0f));

    layout.add(std::make_unique<AudioParameterChoice>(
        ParameterID{ ParamID::LfoTarget, 1 }, "LFO Target",
        StringArray{ "Pitch", "Filter", "Amp" }, 0));

    // ----- ドライブ -----
    layout.add(std::make_unique<AudioParameterChoice>(
        ParameterID{ ParamID::DriveType, 1 }, "Drive Type",
        StringArray{ "Soft", "Hard" }, 0));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::DriveAmount, 1 }, "Drive Amount",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f }, 0.0f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::DriveMix, 1 }, "Drive Mix",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f }, 0.0f));

    // ----- ディレイ -----
    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::DelayTime, 1 }, "Delay Time",
        NormalisableRange<float>{ 0.01f, 2.0f, 0.001f }, 0.3f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::DelayFeedback, 1 }, "Delay Feedback",
        NormalisableRange<float>{ 0.0f, 0.95f, 0.001f }, 0.3f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::DelayMix, 1 }, "Delay Mix",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f }, 0.0f));

    // ----- リバーブ -----
    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::ReverbDecay, 1 }, "Reverb Decay",
        NormalisableRange<float>{ 0.1f, 10.0f, 0.01f }, 1.5f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::ReverbMix, 1 }, "Reverb Mix",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f }, 0.0f));

    // ----- Voice -----
    layout.add(std::make_unique<AudioParameterInt>(
        ParameterID{ ParamID::UnisonCount, 1 }, "Unison Count", 1, 8, 1));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::UnisonDetune, 1 }, "Unison Detune",
        NormalisableRange<float>{ 0.0f, 100.0f, 0.1f }, 20.0f));

    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::Glide, 1 }, "Glide",
        NormalisableRange<float>{ 0.0f, 2.0f, 0.001f }, 0.0f));

    // ----- マスター -----
    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::MasterVolume, 1 }, "Master Volume",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f }, 0.5f));

    return layout;
}

} // namespace synth
