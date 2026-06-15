// =============================================================================
// SynthVoice.cpp — ボイス実装（DSP 信号フロー）
// 設計: docs/desktop-design.md §4, §7 参照
// =============================================================================
#include "SynthVoice.h"
#include "SynthSound.h"
#include "Oscillator.h"
#include "Filter.h"
#include "Envelope.h"
#include "LFO.h"
#include "Noise.h"
#include "../Parameters.h"
#include <cmath>

namespace synth {

SynthVoice::SynthVoice()
    : oscillator(std::make_unique<Oscillator>()),
      filter    (std::make_unique<Filter>()),
      ampEnv    (std::make_unique<Envelope>()),
      filterEnv (std::make_unique<Envelope>()),
      lfo       (std::make_unique<LFO>()),
      noise     (std::make_unique<Noise>())
{}

SynthVoice::~SynthVoice() = default;

// -----------------------------------------------------------------------------
bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

// -----------------------------------------------------------------------------
void SynthVoice::prepare(const juce::dsp::ProcessSpec& spec,
                         juce::AudioProcessorValueTreeState& apvts)
{
    sampleRate = spec.sampleRate;

    oscillator->prepare(spec);
    filter->prepare(spec);
    ampEnv->prepare(sampleRate);
    filterEnv->prepare(sampleRate);
    lfo->prepare(sampleRate);
    noise->prepare(spec);

    const int maxBlock = static_cast<int>(spec.maximumBlockSize);
    voiceBuffer.setSize(1, maxBlock);
    filterEnvBuffer.setSize(1, maxBlock);

    // パラメータポインタを一度だけ取得（processBlock 内では load() のみ）
    auto get = [&](const char* id) -> std::atomic<float>*
    {
        return apvts.getRawParameterValue(id);
    };

    pOscWaveform  = get(ParamID::OscWaveform);
    pOscDetune    = get(ParamID::OscDetune);
    pOscOctave    = get(ParamID::OscOctave);
    pOscLevel     = get(ParamID::OscLevel);
    pNoiseType    = get(ParamID::NoiseType);
    pNoiseLevel   = get(ParamID::NoiseLevel);
    pFilterType   = get(ParamID::FilterType);
    pFilterCutoff = get(ParamID::FilterCutoff);
    pFilterReso   = get(ParamID::FilterReso);
    pFEnvAttack   = get(ParamID::FEnvAttack);
    pFEnvDecay    = get(ParamID::FEnvDecay);
    pFEnvSustain  = get(ParamID::FEnvSustain);
    pFEnvRelease  = get(ParamID::FEnvRelease);
    pFEnvAmount   = get(ParamID::FEnvAmount);
    pAmpAttack    = get(ParamID::AmpAttack);
    pAmpDecay     = get(ParamID::AmpDecay);
    pAmpSustain   = get(ParamID::AmpSustain);
    pAmpRelease   = get(ParamID::AmpRelease);
    pLfoWaveform  = get(ParamID::LfoWaveform);
    pLfoRate      = get(ParamID::LfoRate);
    pLfoDepth     = get(ParamID::LfoDepth);
    pLfoTarget    = get(ParamID::LfoTarget);
    pUnisonCount  = get(ParamID::UnisonCount);
    pUnisonDetune = get(ParamID::UnisonDetune);
}

// -----------------------------------------------------------------------------
void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound*, int /*pitchWheel*/)
{
    currentFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    currentVelocity  = velocity;

    oscillator->setFrequency(currentFrequency);
    oscillator->reset();

    ampEnv->reset();
    filterEnv->reset();
    ampEnv->noteOn();
    filterEnv->noteOn();

    lfo->reset();
}

// -----------------------------------------------------------------------------
void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        ampEnv->noteOff();
        filterEnv->noteOff();
    }
    else
    {
        ampEnv->reset();
        filterEnv->reset();
        clearCurrentNote();
    }
}

void SynthVoice::pitchWheelMoved(int) {}
void SynthVoice::controllerMoved(int, int) {}

// -----------------------------------------------------------------------------
void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                 int startSample, int numSamples)
{
    if (!isVoiceActive()) return;

    updateFromParameters();

    // 1. ボイスバッファをクリア
    voiceBuffer.clear();

    // 2. フィルターエンベロープ値を展開してカットオフ変調係数を算出
    //    filterEnvBuffer を 1.0f で初期化し applyEnvelopeToBuffer で乗算
    {
        juce::FloatVectorOperations::fill(
            filterEnvBuffer.getWritePointer(0), 1.0f, numSamples);
        filterEnv->applyEnvelopeToBuffer(filterEnvBuffer, 0, numSamples);
    }
    const float fenvLevel  = filterEnvBuffer.getSample(0, 0);
    const float fenvAmount = (pFEnvAmount != nullptr) ? pFEnvAmount->load() : 0.0f;
    // カットオフ変調: 2^(amount * level * 4オクターブ)
    const float cutoffMod  = std::pow(2.0f, fenvAmount * fenvLevel * 4.0f);

    // 3. LFO 値（numSamples 分進めて最終値を変調に使用）
    float lfoVal = 0.0f;
    for (int s = 0; s < numSamples; ++s)
        lfoVal = lfo->getNextSample();

    // 4. Pitch LFO 変調（±100 cent/depth）
    {
        double freq = currentFrequency;
        if (lfo->getTarget() == LfoTarget::Pitch)
            freq *= static_cast<double>(std::pow(2.0f, lfoVal * 100.0f / 1200.0f));
        oscillator->setFrequency(freq);
    }

    // 5. オシレーター + ノイズをボイスバッファに生成
    oscillator->process(voiceBuffer, numSamples);
    noise->addTo(voiceBuffer, numSamples);

    // 6. フィルター（FilterEnv + LFO 変調）
    {
        float totalMod = cutoffMod;
        if (lfo->getTarget() == LfoTarget::Filter)
            totalMod *= std::pow(2.0f, lfoVal * 2.0f); // ±2 oct
        filter->setCutoffModFactor(totalMod);
        filter->process(voiceBuffer, numSamples);
    }

    // 7. Amp エンベロープを適用
    ampEnv->applyEnvelopeToBuffer(voiceBuffer, 0, numSamples);

    // 8. Amp LFO（トレモロ）
    if (lfo->getTarget() == LfoTarget::Amp)
    {
        const float ampMod = 1.0f + lfoVal * 0.8f;
        voiceBuffer.applyGain(0, 0, numSamples, ampMod);
    }

    // 9. ベロシティを乗算してメインバッファに加算
    for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
        outputBuffer.addFrom(ch, startSample, voiceBuffer, 0, 0, numSamples,
                             currentVelocity);

    // 10. エンベロープ終了でボイス停止
    if (!ampEnv->isActive())
        clearCurrentNote();
}

// -----------------------------------------------------------------------------
void SynthVoice::updateFromParameters()
{
    // ポインタが null の場合はデフォルト値を使用（Session E 未実装パラメータ対応）
    auto load = [](std::atomic<float>* p, float def) -> float
    {
        return (p != nullptr) ? p->load() : def;
    };

    // オシレーター
    oscillator->setWaveform(static_cast<OscWaveform>(
        static_cast<int>(load(pOscWaveform, 1.0f)))); // デフォルト Sawtooth
    oscillator->setDetuneCents(load(pOscDetune, 0.0f));
    oscillator->setOctave(static_cast<int>(load(pOscOctave, 0.0f)));
    oscillator->setLevel(load(pOscLevel, 1.0f));

    const int uniCount = juce::jlimit(1, Oscillator::MAX_UNISON,
                                      static_cast<int>(load(pUnisonCount, 1.0f)));
    oscillator->setUnisonCount(uniCount);
    oscillator->setUnisonDetuneCents(load(pUnisonDetune, 20.0f));

    // ノイズ
    noise->setType(static_cast<NoiseType>(static_cast<int>(load(pNoiseType, 0.0f))));
    noise->setLevel(load(pNoiseLevel, 0.0f));

    // フィルター
    filter->setType(static_cast<FilterType>(static_cast<int>(load(pFilterType, 0.0f))));
    filter->setBaseCutoff(load(pFilterCutoff, 2000.0f));
    filter->setResonance(load(pFilterReso, 1.0f));

    // フィルターエンベロープ
    filterEnv->setParameters(load(pFEnvAttack,  0.01f),
                             load(pFEnvDecay,   0.3f),
                             load(pFEnvSustain, 0.4f),
                             load(pFEnvRelease, 0.3f));

    // アンプエンベロープ
    ampEnv->setParameters(load(pAmpAttack,  0.01f),
                          load(pAmpDecay,   0.2f),
                          load(pAmpSustain, 0.7f),
                          load(pAmpRelease, 0.3f));

    // LFO
    lfo->setWaveform(static_cast<LfoWaveform>(static_cast<int>(load(pLfoWaveform, 0.0f))));
    lfo->setRate(load(pLfoRate, 5.0f));
    lfo->setDepth(load(pLfoDepth, 0.0f));
    lfo->setTarget(static_cast<LfoTarget>(static_cast<int>(load(pLfoTarget, 0.0f))));
}

} // namespace synth
