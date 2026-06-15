// =============================================================================
// SpectrumDisplay.h — 周波数スペクトラムビジュアライザ（Session C 担当）
//
// juce::dsp::FFT で周波数解析し、対数軸（左=低音、右=高音）で描画する。
// startTimerHz(60) で再描画。
// オーディオデータは PluginProcessor::getSpectrumFifo() / getSpectrumFifoBuffer()
// 経由で lock-free に取得する。
// =============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace synth {

class SynthAudioProcessor; // 前方宣言

class SpectrumDisplay : public juce::Component,
                        private juce::Timer
{
public:
    explicit SpectrumDisplay(SynthAudioProcessor& processor);
    ~SpectrumDisplay() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    SynthAudioProcessor& processorRef;

    // FFT パラメータ
    static constexpr int kFftOrder = 11;              // 2^11 = 2048 点
    static constexpr int kFftSize  = 1 << kFftOrder;  // 2048

    // 周波数軸の範囲
    static constexpr float kMinFreq = 20.0f;
    static constexpr float kMaxFreq = 20000.0f;
    static constexpr float kMinDb   = -80.0f;
    static constexpr float kMaxDb   =   0.0f;

    // ヘッダー高さ
    static constexpr int kHeaderHeight = 34;

    juce::dsp::FFT forwardFFT { kFftOrder };

    // FFT 入力バッファ（FIFO から積み上げ、kFftSize 溜まったら処理）
    std::array<float, kFftSize * 2> fftData {};  // 後半は FFT の作業領域
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;

    // 表示用スムージング済みスペクトラム（dB 値）
    std::array<float, kFftSize / 2> scopeData {};

    // タイマーコールバック（60 Hz）
    void timerCallback() override;

    // FIFO からサンプルを読み出し、FFT ブロックを構築
    void drainFifoIntoBuffer();

    // FFT を実行してスコープデータを更新
    void computeFFT();

    // スペクトラム描画（対数軸）
    void drawSpectrum(juce::Graphics& g, juce::Rectangle<int> bounds);

    // 周波数から画面 x 座標（0.0〜1.0）への対数マッピング
    static float freqToX(float freq) noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumDisplay)
};

} // namespace synth
