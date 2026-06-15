// =============================================================================
// Descriptions.h — 各パラメータの解説文
//
// Session E 担当。InfoPanel が getDescription(paramId) で取得して表示する。
// 「数値の意味」と「人間の感覚での意味」を併記した日本語の解説。
// =============================================================================
#pragma once

#include <juce_core/juce_core.h>

namespace synth {

/** paramId（ParamID::* と一致する文字列）に対応する解説文を返す。
 *  未知の paramId の場合は空文字列を返す。 */
juce::String getDescription(const juce::String& paramId);

} // namespace synth
