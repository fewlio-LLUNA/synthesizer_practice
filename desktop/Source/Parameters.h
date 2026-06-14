// =============================================================================
// Parameters.h — 全パラメータの ID 定義と APVTS Layout 構築
//
// このファイルは「共有資産」。全セッションが参照するため、
// パラメータ ID の追加・変更時は TASKS.md の「I/F 変更ルール」に従うこと。
// =============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace synth {

// -----------------------------------------------------------------------------
// パラメータ ID
// 文字列定数として定義。APVTS から RawParameterValue を取るときに使う。
// 命名規則: <セクション>_<パラメータ>。例: filter_cutoff, env_attack
// -----------------------------------------------------------------------------
namespace ParamID {
    // オシレーター
    inline constexpr auto OscWaveform   = "osc_waveform";
    inline constexpr auto OscDetune     = "osc_detune";
    inline constexpr auto OscOctave     = "osc_octave";
    inline constexpr auto OscLevel      = "osc_level";

    // ノイズ
    inline constexpr auto NoiseType     = "noise_type";
    inline constexpr auto NoiseLevel    = "noise_level";

    // フィルター
    inline constexpr auto FilterType    = "filter_type";
    inline constexpr auto FilterCutoff  = "filter_cutoff";
    inline constexpr auto FilterReso    = "filter_resonance";

    // フィルターエンベロープ
    inline constexpr auto FEnvAttack    = "fenv_attack";
    inline constexpr auto FEnvDecay     = "fenv_decay";
    inline constexpr auto FEnvSustain   = "fenv_sustain";
    inline constexpr auto FEnvRelease   = "fenv_release";
    inline constexpr auto FEnvAmount    = "fenv_amount";

    // アンプエンベロープ
    inline constexpr auto AmpAttack     = "amp_attack";
    inline constexpr auto AmpDecay      = "amp_decay";
    inline constexpr auto AmpSustain    = "amp_sustain";
    inline constexpr auto AmpRelease    = "amp_release";

    // LFO
    inline constexpr auto LfoWaveform   = "lfo_waveform";
    inline constexpr auto LfoRate       = "lfo_rate";
    inline constexpr auto LfoDepth      = "lfo_depth";
    inline constexpr auto LfoTarget     = "lfo_target";

    // ドライブ
    inline constexpr auto DriveType     = "drive_type";
    inline constexpr auto DriveAmount   = "drive_amount";
    inline constexpr auto DriveMix      = "drive_mix";

    // ディレイ
    inline constexpr auto DelayTime     = "delay_time";
    inline constexpr auto DelayFeedback = "delay_feedback";
    inline constexpr auto DelayMix      = "delay_mix";

    // リバーブ
    inline constexpr auto ReverbDecay   = "reverb_decay";
    inline constexpr auto ReverbMix     = "reverb_mix";

    // Voice（ユニゾン + 演奏）
    inline constexpr auto UnisonCount   = "unison_count";
    inline constexpr auto UnisonDetune  = "unison_detune";
    inline constexpr auto Glide         = "glide";

    // マスター
    inline constexpr auto MasterVolume  = "master_volume";
}

// -----------------------------------------------------------------------------
// 列挙型: choice パラメータの選択肢
// AudioParameterChoice の index と1対1対応させる。
// -----------------------------------------------------------------------------
enum class OscWaveform : int { Sine = 0, Sawtooth, Square, Triangle };
enum class NoiseType   : int { White = 0, Pink };
enum class FilterType  : int { LowPass = 0, HighPass, BandPass };
enum class LfoWaveform : int { Sine = 0, Sawtooth, Square, Triangle };
enum class LfoTarget   : int { Pitch = 0, Filter, Amp };
enum class DriveType   : int { Soft = 0, Hard };

// -----------------------------------------------------------------------------
// APVTS Layout 構築
// Session E が実装する。createParameterLayout() は全パラメータを定義する。
// -----------------------------------------------------------------------------
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

} // namespace synth
