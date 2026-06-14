// =============================================================================
// PluginProcessor.cpp — AudioProcessor 実装（スタブ）
//
// TODO: Session A が DSP 結線（Synthesiser へのボイス登録、エフェクトチェーン）、
//       Session D が GUI 連動を実装する。
// 詳細は docs/desktop-design.md §4（DSP 信号フロー） を参照。
// =============================================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/SynthVoice.h"
#include "dsp/SynthSound.h"

namespace synth {

SynthAudioProcessor::SynthAudioProcessor()
    : juce::AudioProcessor(BusesProperties().withOutput("Output",
                                                       juce::AudioChannelSet::stereo(),
                                                       true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    // ボイスとサウンドの登録（Session A が拡張）
    synth.addSound(new SynthSound());
    synth.addVoice(new SynthVoice());

    audioVisualiser.setBufferSize(512);
    audioVisualiser.setSamplesPerBlock(64);
}

SynthAudioProcessor::~SynthAudioProcessor() = default;

void SynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    audioVisualiser.clear();

    // TODO: Session A — 各 DSP モジュール（Filter, Envelope, Effects）を prepare
    juce::ignoreUnused(samplesPerBlock);
}

void SynthAudioProcessor::releaseResources()
{
    // TODO: 必要に応じてリソース解放
}

bool SynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOut = layouts.getMainOutputChannelSet();
    return mainOut == juce::AudioChannelSet::stereo()
        || mainOut == juce::AudioChannelSet::mono();
}

void SynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // 鍵盤コンポーネントから来る MIDI メッセージを統合
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    // シンセに渡して音を生成
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // TODO: Session A — エフェクトチェーン（drive → delay → reverb）を適用

    // ビジュアライザに送る（モノラル化）
    audioVisualiser.pushBuffer(buffer);
}

juce::AudioProcessorEditor* SynthAudioProcessor::createEditor()
{
    return new SynthAudioProcessorEditor(*this);
}

void SynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // APVTS の状態をシリアライズ
    if (auto state = apvts.copyState(); state.isValid())
    {
        if (auto xml = state.createXml())
            copyXmlToBinary(*xml, destData);
    }
}

void SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

} // namespace synth

// プラグインのエントリポイント（JUCE が必須要求）
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new synth::SynthAudioProcessor();
}
