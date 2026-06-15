// =============================================================================
// PresetManager.h — 内蔵プリセット + ユーザープリセット保存/読込
//
// Session E 担当。InfoPanel のプリセットセレクターと連動する。
// 内蔵プリセットは applyBuiltInPreset() で適用。
// ユーザープリセットは XML ファイルとして userApplicationDataDirectory に保存。
// =============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

namespace synth {

// -----------------------------------------------------------------------------
// プリセットデータ構造体
// 各フィールドは「実際の値」（非正規化）。
// -----------------------------------------------------------------------------
struct Preset {
    juce::String name;
    juce::String description;

    // オシレーター（OscWaveform enum のインデックス）
    int   oscWaveform;
    float oscDetune;    // cent
    int   oscOctave;
    float oscLevel;

    // ノイズ（NoiseType enum のインデックス）
    int   noiseType;
    float noiseLevel;

    // フィルター（FilterType enum のインデックス）
    int   filterType;
    float filterCutoff;    // Hz
    float filterResonance; // Q

    // フィルターエンベロープ
    float fEnvAttack;
    float fEnvDecay;
    float fEnvSustain;
    float fEnvRelease;
    float fEnvAmount;

    // アンプエンベロープ
    float ampAttack;
    float ampDecay;
    float ampSustain;
    float ampRelease;

    // LFO（LfoWaveform / LfoTarget enum のインデックス）
    int   lfoWaveform;
    float lfoRate;   // Hz
    float lfoDepth;
    int   lfoTarget;

    // ドライブ（DriveType enum のインデックス）
    int   driveType;
    float driveAmount;
    float driveMix;

    // ディレイ
    float delayTime;     // s
    float delayFeedback;
    float delayMix;

    // リバーブ
    float reverbDecay;   // s
    float reverbMix;

    // ボイス
    int   unisonCount;
    float unisonDetune;  // cent
    float glide;         // s

    // マスター
    float masterVolume;
};

// -----------------------------------------------------------------------------
// PresetManager
// -----------------------------------------------------------------------------
class PresetManager
{
public:
    PresetManager();

    // 内蔵プリセット一覧を返す
    const std::vector<Preset>& getBuiltInPresets() const { return builtInPresets; }

    // 内蔵プリセットを APVTS へ適用する
    void applyBuiltInPreset(juce::AudioProcessorValueTreeState& apvts, int index);

    // 現在の APVTS 状態をユーザープリセットとして XML ファイルに保存する
    void saveUserPreset(juce::AudioProcessorValueTreeState& apvts,
                        const juce::String& name);

    // ユーザープリセットを XML ファイルから読み込み APVTS へ適用する
    void loadUserPreset(juce::AudioProcessorValueTreeState& apvts,
                        const juce::String& name);

    // 保存済みユーザープリセット名の一覧を返す
    juce::StringArray getUserPresetNames() const;

private:
    std::vector<Preset> builtInPresets;

    juce::File getUserPresetsDir() const;

    // Preset 構造体の各値を APVTS パラメータへ書き込む
    void applyPresetValues(juce::AudioProcessorValueTreeState& apvts,
                           const Preset& preset);
};

} // namespace synth
