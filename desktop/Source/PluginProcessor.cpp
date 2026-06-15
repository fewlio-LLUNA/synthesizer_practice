// =============================================================================
// PluginProcessor.cpp — AudioProcessor 実装
// =============================================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/SynthVoice.h"
#include "dsp/SynthSound.h"
#include "dsp/Drive.h"
#include "dsp/Delay.h"
#include "dsp/Reverb.h"

namespace synth {

// PluginProcessor.h で前方宣言した EffectsChain の実体
struct SynthAudioProcessor::EffectsChain
{
    Drive        drive;
    Delay        delay;
    ReverbEffect reverb;
};

// -----------------------------------------------------------------------------
SynthAudioProcessor::SynthAudioProcessor()
    : juce::AudioProcessor(BusesProperties().withOutput("Output",
                                                       juce::AudioChannelSet::stereo(),
                                                       true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout()),
      effectsChain(std::make_unique<EffectsChain>())
{
    // MAX_VOICES 分のボイスとサウンドを登録
    synth.addSound(new SynthSound());
    for (int i = 0; i < MAX_VOICES; ++i)
        synth.addVoice(new SynthVoice());

    audioVisualiser.setBufferSize(512);
    audioVisualiser.setSamplesPerBlock(64);
}

SynthAudioProcessor::~SynthAudioProcessor() = default;

// -----------------------------------------------------------------------------
void SynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    audioVisualiser.clear();
    spectrumFifo.reset();
    spectrumFifoBuffer.clear();

    // ボイスを準備（モノラル ProcessSpec を渡す）
    const juce::dsp::ProcessSpec voiceSpec
    {
        sampleRate,
        static_cast<juce::uint32>(samplesPerBlock),
        1  // モノラルで処理
    };
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            voice->prepare(voiceSpec, apvts);

    // マスターエフェクトチェーンを準備（ステレオ ProcessSpec）
    const juce::dsp::ProcessSpec stereoSpec
    {
        sampleRate,
        static_cast<juce::uint32>(samplesPerBlock),
        2
    };
    effectsChain->drive.prepare(stereoSpec);
    effectsChain->delay.prepare(stereoSpec);
    effectsChain->reverb.prepare(stereoSpec);

    // パラメータポインタを取得
    auto get = [&](const char* id) -> std::atomic<float>*
    {
        return apvts.getRawParameterValue(id);
    };

    pMasterVolume  = get(ParamID::MasterVolume);
    pDriveType     = get(ParamID::DriveType);
    pDriveAmount   = get(ParamID::DriveAmount);
    pDriveMix      = get(ParamID::DriveMix);
    pDelayTime     = get(ParamID::DelayTime);
    pDelayFeedback = get(ParamID::DelayFeedback);
    pDelayMix      = get(ParamID::DelayMix);
    pReverbDecay   = get(ParamID::ReverbDecay);
    pReverbMix     = get(ParamID::ReverbMix);
}

// -----------------------------------------------------------------------------
void SynthAudioProcessor::releaseResources()
{
    effectsChain->drive.reset();
    effectsChain->delay.reset();
    effectsChain->reverb.reset();
}

// -----------------------------------------------------------------------------
bool SynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOut = layouts.getMainOutputChannelSet();
    return mainOut == juce::AudioChannelSet::stereo()
        || mainOut == juce::AudioChannelSet::mono();
}

// -----------------------------------------------------------------------------
void SynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // 鍵盤コンポーネントから来る MIDI メッセージを統合
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    // シンセに渡して音を生成
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // マスターエフェクトチェーン（Drive → Delay → Reverb）
    {
        auto load = [](std::atomic<float>* p, float def) -> float
        {
            return (p != nullptr) ? p->load() : def;
        };

        const auto driveType = static_cast<DriveType>(
            static_cast<int>(load(pDriveType, 0.0f)));

        effectsChain->drive.process(buffer,
            driveType,
            load(pDriveAmount,   0.0f),
            load(pDriveMix,      0.0f));

        effectsChain->delay.process(buffer,
            load(pDelayTime,     0.3f),
            load(pDelayFeedback, 0.3f),
            load(pDelayMix,      0.0f));

        effectsChain->reverb.process(buffer,
            load(pReverbDecay,   1.5f),
            load(pReverbMix,     0.0f));
    }

    // マスターボリューム
    const float masterVol = (pMasterVolume != nullptr) ? pMasterVolume->load() : 0.5f;
    buffer.applyGain(masterVol);

    // ビジュアライザに送る
    audioVisualiser.pushBuffer(buffer);

    // スペクトラム FIFO に書き込み（ch0 のみ、UI スレッドの SpectrumDisplay が消費）
    {
        const int numSamples = buffer.getNumSamples();
        const int numToWrite = juce::jmin(numSamples, spectrumFifo.getFreeSpace());
        if (numToWrite > 0)
        {
            int start1, size1, start2, size2;
            spectrumFifo.prepareToWrite(numToWrite, start1, size1, start2, size2);
            const float* src = buffer.getReadPointer(0);
            if (size1 > 0)
                spectrumFifoBuffer.copyFrom(0, start1, src, size1);
            if (size2 > 0)
                spectrumFifoBuffer.copyFrom(0, start2, src + size1, size2);
            spectrumFifo.finishedWrite(size1 + size2);
        }
    }
}

// -----------------------------------------------------------------------------
juce::AudioProcessorEditor* SynthAudioProcessor::createEditor()
{
    return new SynthAudioProcessorEditor(*this);
}

void SynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
        if (auto xml = state.createXml())
            copyXmlToBinary(*xml, destData);
}

void SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

} // namespace synth

// プラグインのエントリポイント（JUCE が必須要求）
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new synth::SynthAudioProcessor();
}
