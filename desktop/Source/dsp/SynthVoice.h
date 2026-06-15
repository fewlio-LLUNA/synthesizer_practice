// =============================================================================
// SynthVoice.h — 1ノート分のボイス
//
// 公開インターフェース（public section）は変更しない。
// Session A がプライベートメンバーと prepare() を追加。
// =============================================================================
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include <memory>

namespace synth {

// 前方宣言（.h が DSP ヘッダーを #include しないようにする）
class Oscillator;
class Filter;
class Envelope;
class LFO;
class Noise;

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();
    ~SynthVoice() override;

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override;

    // PluginProcessor::prepareToPlay から呼ぶ（モノラル ProcessSpec を渡す）
    void prepare(const juce::dsp::ProcessSpec& spec,
                 juce::AudioProcessorValueTreeState& apvts);

private:
    // DSP コンポーネント
    std::unique_ptr<Oscillator> oscillator;
    std::unique_ptr<Filter>     filter;
    std::unique_ptr<Envelope>   ampEnv;
    std::unique_ptr<Envelope>   filterEnv;
    std::unique_ptr<LFO>        lfo;
    std::unique_ptr<Noise>      noise;

    // 事前確保バッファ（renderNextBlock 内でアロケーション禁止）
    juce::AudioBuffer<float> voiceBuffer;
    juce::AudioBuffer<float> filterEnvBuffer;

    // 状態
    double  currentFrequency { 0.0 };
    float   currentVelocity  { 0.0f };
    double  sampleRate       { 44100.0 };

    // APVTS パラメータポインタ（prepare で取得、renderNextBlock で load()）
    std::atomic<float>* pOscWaveform  { nullptr };
    std::atomic<float>* pOscDetune    { nullptr };
    std::atomic<float>* pOscOctave    { nullptr };
    std::atomic<float>* pOscLevel     { nullptr };
    std::atomic<float>* pNoiseType    { nullptr };
    std::atomic<float>* pNoiseLevel   { nullptr };
    std::atomic<float>* pFilterType   { nullptr };
    std::atomic<float>* pFilterCutoff { nullptr };
    std::atomic<float>* pFilterReso   { nullptr };
    std::atomic<float>* pFEnvAttack   { nullptr };
    std::atomic<float>* pFEnvDecay    { nullptr };
    std::atomic<float>* pFEnvSustain  { nullptr };
    std::atomic<float>* pFEnvRelease  { nullptr };
    std::atomic<float>* pFEnvAmount   { nullptr };
    std::atomic<float>* pAmpAttack    { nullptr };
    std::atomic<float>* pAmpDecay     { nullptr };
    std::atomic<float>* pAmpSustain   { nullptr };
    std::atomic<float>* pAmpRelease   { nullptr };
    std::atomic<float>* pLfoWaveform  { nullptr };
    std::atomic<float>* pLfoRate      { nullptr };
    std::atomic<float>* pLfoDepth     { nullptr };
    std::atomic<float>* pLfoTarget    { nullptr };
    std::atomic<float>* pUnisonCount  { nullptr };
    std::atomic<float>* pUnisonDetune { nullptr };

    void updateFromParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};

} // namespace synth
