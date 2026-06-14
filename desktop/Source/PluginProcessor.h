// =============================================================================
// PluginProcessor.h — AudioProcessor 本体（音処理のエントリポイント）
//
// このファイルは共有資産。public I/F の変更は周知必須。
// =============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters.h"

namespace synth {

class SynthAudioProcessor : public juce::AudioProcessor
{
public:
    SynthAudioProcessor();
    ~SynthAudioProcessor() override;

    // AudioProcessor 必須インターフェース
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Synthesizer Practice"; }
    bool acceptsMidi() const override  { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    // 状態の保存・復元（プリセット保存とDAWセッションの両方で使われる）
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // ----- カスタム I/F（他セッションから利用） -----
    /** APVTS への参照。GUI 側はこれで SliderAttachment などを作る */
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    /** 鍵盤コンポーネントが取得する MIDI キーボード状態 */
    juce::MidiKeyboardState& getKeyboardState() { return keyboardState; }

    /** ビジュアライザがオーディオデータを取得するための AudioVisualiserComponent */
    juce::AudioVisualiserComponent& getAudioVisualiser() { return audioVisualiser; }

    /** スペクトラム分析用 FIFO（SpectrumDisplay が timerCallback で読み出す）
     *  オーディオスレッドが書き込み、UI スレッドが読み出す lock-free 構成。 */
    static constexpr int kSpectrumFifoSize = 4096;
    juce::AbstractFifo& getSpectrumFifo() noexcept       { return spectrumFifo; }
    juce::AudioBuffer<float>& getSpectrumFifoBuffer() noexcept { return spectrumFifoBuffer; }

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::MidiKeyboardState keyboardState;
    juce::Synthesiser synth;

    // ビジュアライザ用（Session C が描画読み出し、Processor が processBlock 時に書き込み）
    juce::AudioVisualiserComponent audioVisualiser { 1 }; // モノラル

    // スペクトラム表示用 FIFO（Session C / SpectrumDisplay が消費）
    juce::AbstractFifo spectrumFifo { kSpectrumFifoSize };
    juce::AudioBuffer<float> spectrumFifoBuffer { 1, kSpectrumFifoSize };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessor)
};

} // namespace synth
