// =============================================================================
// PresetManager.cpp — Session E 実装
//
// 内蔵プリセット5種: Basic Lead, Super Saw, Wobble Bass, Acid Bass, Dreamy Pad
// Web 版 web/src/data/presets.ts の値を C++ 構造体に移植。
// ユーザープリセットは APVTS 状態を XML としてファイルに保存。
// =============================================================================
#include "PresetManager.h"
#include "Parameters.h"

namespace synth {

// -----------------------------------------------------------------------------
// OscWaveform インデックス定数（enum class との対応）
// -----------------------------------------------------------------------------
[[maybe_unused]] static constexpr int kSine     = 0;
[[maybe_unused]] static constexpr int kSaw      = 1;
[[maybe_unused]] static constexpr int kSquare   = 2;
[[maybe_unused]] static constexpr int kTriangle = 3;

// NoiseType
[[maybe_unused]] static constexpr int kWhite = 0;
[[maybe_unused]] static constexpr int kPink  = 1;

// FilterType
[[maybe_unused]] static constexpr int kLP = 0;
[[maybe_unused]] static constexpr int kHP = 1;
[[maybe_unused]] static constexpr int kBP = 2;

// LfoWaveform（osc と同じ順）、LfoTarget
[[maybe_unused]] static constexpr int kPitchTarget  = 0;
[[maybe_unused]] static constexpr int kFilterTarget = 1;
[[maybe_unused]] static constexpr int kAmpTarget    = 2;

// DriveType
[[maybe_unused]] static constexpr int kSoft = 0;
[[maybe_unused]] static constexpr int kHard = 1;

// -----------------------------------------------------------------------------
// コンストラクタ — 内蔵プリセット定義
// -----------------------------------------------------------------------------
PresetManager::PresetManager()
{
    // --- Basic Lead ---
    // ノコギリ波 + LPF の基本的なリード音
    builtInPresets.push_back({
        "Basic Lead",
        "ノコギリ波 + LPF で作る基本的なリード音。シンセの基本",
        kSaw, 0.0f, 0, 1.0f,           // osc
        kWhite, 0.0f,                   // noise
        kLP, 2500.0f, 3.0f,             // filter
        0.01f, 0.3f, 0.4f, 0.3f, 0.3f, // fenv
        0.01f, 0.2f, 0.7f, 0.3f,       // amp env
        kSine, 5.0f, 0.0f, kPitchTarget, // lfo
        kSoft, 0.0f, 0.0f,             // drive
        0.3f, 0.2f, 0.15f,             // delay
        1.5f, 0.1f,                    // reverb
        1, 20.0f, 0.0f,                // voice
        0.5f                           // master
    });

    // --- Super Saw ---
    // ユニゾン7個の分厚いスーパーソウ
    builtInPresets.push_back({
        "Super Saw",
        "ユニゾン7個で作る分厚いスーパーソウ。EDMリードの定番",
        kSaw, 0.0f, 0, 1.0f,
        kWhite, 0.0f,
        kLP, 4000.0f, 1.0f,
        0.01f, 0.3f, 0.5f, 0.3f, 0.2f,
        0.05f, 0.3f, 0.8f, 0.5f,
        kSine, 5.0f, 0.0f, kPitchTarget,
        kSoft, 0.2f, 0.5f,
        0.25f, 0.2f, 0.2f,
        2.5f, 0.3f,
        7, 30.0f, 0.0f,
        0.4f
    });

    // --- Wobble Bass ---
    // 矩形波 + フィルターLFO によるウォブルベース
    builtInPresets.push_back({
        "Wobble Bass",
        "矩形波 + フィルターLFO によるウォブルベース",
        kSquare, 0.0f, -1, 1.0f,
        kWhite, 0.0f,
        kLP, 400.0f, 10.0f,
        0.01f, 0.3f, 0.4f, 0.3f, 0.0f,
        0.01f, 0.3f, 0.8f, 0.2f,
        kSine, 4.0f, 0.7f, kFilterTarget,
        kSoft, 0.3f, 0.5f,
        0.3f, 0.3f, 0.0f,
        1.5f, 0.0f,
        1, 20.0f, 0.0f,
        0.5f
    });

    // --- Acid Bass ---
    // フィルターエンベロープを活かした TB-303 風アシッドベース
    builtInPresets.push_back({
        "Acid Bass",
        "フィルターエンベロープを活かしたTB-303風アシッドベース",
        kSaw, 0.0f, -1, 1.0f,
        kWhite, 0.0f,
        kLP, 300.0f, 18.0f,
        0.005f, 0.25f, 0.05f, 0.2f, 0.7f,
        0.005f, 0.4f, 0.4f, 0.2f,
        kSine, 5.0f, 0.0f, kPitchTarget,
        kSoft, 0.3f, 0.7f,
        0.25f, 0.3f, 0.15f,
        1.0f, 0.0f,
        1, 20.0f, 0.08f,
        0.5f
    });

    // --- Dreamy Pad ---
    // ゆっくりしたアタックと長いリリース、リバーブで広がるパッド音
    builtInPresets.push_back({
        "Dreamy Pad",
        "ゆっくりしたアタックと長いリリース、リバーブで広がるパッド音",
        kSaw, 10.0f, 0, 1.0f,
        kPink, 0.05f,
        kLP, 1500.0f, 1.0f,
        1.5f, 0.5f, 0.5f, 2.0f, 0.2f,
        1.5f, 0.5f, 0.8f, 2.5f,
        kSine, 0.5f, 0.15f, kAmpTarget,
        kSoft, 0.0f, 0.0f,
        0.5f, 0.4f, 0.25f,
        5.0f, 0.5f,
        4, 20.0f, 0.0f,
        0.4f
    });
}

// -----------------------------------------------------------------------------
// 内蔵プリセットを APVTS へ適用
// -----------------------------------------------------------------------------
void PresetManager::applyBuiltInPreset(juce::AudioProcessorValueTreeState& apvts,
                                       int index)
{
    if (index < 0 || index >= static_cast<int>(builtInPresets.size()))
        return;

    applyPresetValues(apvts, builtInPresets[static_cast<size_t>(index)]);
}

// -----------------------------------------------------------------------------
// Preset 構造体の値を APVTS パラメータへ書き込む
// setValueNotifyingHost() は正規化値(0-1)を期待するため convertTo0to1() で変換。
// -----------------------------------------------------------------------------
void PresetManager::applyPresetValues(juce::AudioProcessorValueTreeState& apvts,
                                       const Preset& p)
{
    auto set = [&apvts](const char* id, float rawValue) {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(rawValue));
    };

    set(ParamID::OscWaveform,   static_cast<float>(p.oscWaveform));
    set(ParamID::OscDetune,     p.oscDetune);
    set(ParamID::OscOctave,     static_cast<float>(p.oscOctave));
    set(ParamID::OscLevel,      p.oscLevel);

    set(ParamID::NoiseType,     static_cast<float>(p.noiseType));
    set(ParamID::NoiseLevel,    p.noiseLevel);

    set(ParamID::FilterType,    static_cast<float>(p.filterType));
    set(ParamID::FilterCutoff,  p.filterCutoff);
    set(ParamID::FilterReso,    p.filterResonance);

    set(ParamID::FEnvAttack,    p.fEnvAttack);
    set(ParamID::FEnvDecay,     p.fEnvDecay);
    set(ParamID::FEnvSustain,   p.fEnvSustain);
    set(ParamID::FEnvRelease,   p.fEnvRelease);
    set(ParamID::FEnvAmount,    p.fEnvAmount);

    set(ParamID::AmpAttack,     p.ampAttack);
    set(ParamID::AmpDecay,      p.ampDecay);
    set(ParamID::AmpSustain,    p.ampSustain);
    set(ParamID::AmpRelease,    p.ampRelease);

    set(ParamID::LfoWaveform,   static_cast<float>(p.lfoWaveform));
    set(ParamID::LfoRate,       p.lfoRate);
    set(ParamID::LfoDepth,      p.lfoDepth);
    set(ParamID::LfoTarget,     static_cast<float>(p.lfoTarget));

    set(ParamID::DriveType,     static_cast<float>(p.driveType));
    set(ParamID::DriveAmount,   p.driveAmount);
    set(ParamID::DriveMix,      p.driveMix);

    set(ParamID::DelayTime,     p.delayTime);
    set(ParamID::DelayFeedback, p.delayFeedback);
    set(ParamID::DelayMix,      p.delayMix);

    set(ParamID::ReverbDecay,   p.reverbDecay);
    set(ParamID::ReverbMix,     p.reverbMix);

    set(ParamID::UnisonCount,   static_cast<float>(p.unisonCount));
    set(ParamID::UnisonDetune,  p.unisonDetune);
    set(ParamID::Glide,         p.glide);

    set(ParamID::MasterVolume,  p.masterVolume);
}

// -----------------------------------------------------------------------------
// ユーザープリセット保存
// APVTS の現在状態を XML ファイルとして保存する
// -----------------------------------------------------------------------------
void PresetManager::saveUserPreset(juce::AudioProcessorValueTreeState& apvts,
                                    const juce::String& name)
{
    auto dir = getUserPresetsDir();
    dir.createDirectory();

    auto state = apvts.copyState();
    if (auto xml = state.createXml())
    {
        auto file = dir.getChildFile(name + ".xml");
        xml->writeToFile(file, {});
    }
}

// -----------------------------------------------------------------------------
// ユーザープリセット読み込み
// XML ファイルを ValueTree に変換し apvts.replaceState() で適用する
// -----------------------------------------------------------------------------
void PresetManager::loadUserPreset(juce::AudioProcessorValueTreeState& apvts,
                                    const juce::String& name)
{
    auto file = getUserPresetsDir().getChildFile(name + ".xml");
    if (!file.existsAsFile())
        return;

    juce::XmlDocument doc(file);
    if (auto xml = doc.getDocumentElement())
    {
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

// -----------------------------------------------------------------------------
// 保存済みユーザープリセット名の一覧を返す
// -----------------------------------------------------------------------------
juce::StringArray PresetManager::getUserPresetNames() const
{
    juce::StringArray names;
    auto dir = getUserPresetsDir();

    juce::Array<juce::File> files;
    dir.findChildFiles(files, juce::File::findFiles, false, "*.xml");

    for (const auto& file : files)
        names.add(file.getFileNameWithoutExtension());

    names.sort(true);
    return names;
}

// -----------------------------------------------------------------------------
// ユーザープリセット保存先ディレクトリ
// %APPDATA%/SynthesizerPractice/Presets/
// -----------------------------------------------------------------------------
juce::File PresetManager::getUserPresetsDir() const
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("SynthesizerPractice")
        .getChildFile("Presets");
}

} // namespace synth
