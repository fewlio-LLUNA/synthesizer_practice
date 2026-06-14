// =============================================================================
// Parameters.cpp — APVTS Layout 構築（スタブ）
//
// TODO: Session E 担当
// 全パラメータの定義（範囲、デフォルト値、表示フォーマット）を実装する。
// Web 版の src/types.ts と src/data/defaults.ts を設計参考にすること。
// 詳細は docs/desktop-design.md §3 のパラメータ一覧表を参照。
// =============================================================================
#include "Parameters.h"

namespace synth {

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    // TODO: Session E がすべてのパラメータを定義する
    // 仮で master_volume だけ追加（他セッションがビルド可能な状態を保つ）
    layout.add(std::make_unique<AudioParameterFloat>(
        ParameterID{ ParamID::MasterVolume, 1 },
        "Master Volume",
        NormalisableRange<float>{ 0.0f, 1.0f, 0.001f },
        0.5f));

    return layout;
}

} // namespace synth
