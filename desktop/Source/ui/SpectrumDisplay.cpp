// =============================================================================
// SpectrumDisplay.cpp — 周波数スペクトラムビジュアライザ実装
// =============================================================================
#include "SpectrumDisplay.h"
#include "PluginProcessor.h"

namespace synth {

// カラー定数（デザイン書 §5 に準拠）
static const juce::Colour kBackground { 0xff1a1a1a };
static const juce::Colour kPanel      { 0xff2a2a2a };
static const juce::Colour kAccent     { 0xffff8c00 }; // オレンジ
static const juce::Colour kGrid       { 0xff404040 };
static const juce::Colour kTextDim    { 0xff888888 };

SpectrumDisplay::SpectrumDisplay(SynthAudioProcessor& processor)
    : processorRef(processor)
{
    fftData.fill(0.0f);
    scopeData.fill(kMinDb);
    startTimerHz(60);
}

SpectrumDisplay::~SpectrumDisplay()
{
    stopTimer();
}

// -----------------------------------------------------------------------------
// タイマーコールバック（UI スレッド、60 Hz）
// -----------------------------------------------------------------------------
void SpectrumDisplay::timerCallback()
{
    drainFifoIntoBuffer();

    if (nextFFTBlockReady)
    {
        computeFFT();
        nextFFTBlockReady = false;
        repaint();
    }
}

// -----------------------------------------------------------------------------
// FIFO からサンプルを読み出し、FFT 入力バッファに積み上げる
// kFftSize 個溜まったら nextFFTBlockReady = true をセットし中断する。
// -----------------------------------------------------------------------------
void SpectrumDisplay::drainFifoIntoBuffer()
{
    auto& fifo   = processorRef.getSpectrumFifo();
    auto& srcBuf = processorRef.getSpectrumFifoBuffer();

    const int available = fifo.getNumReady();
    if (available <= 0)
        return;

    int start1, size1, start2, size2;
    fifo.prepareToRead(available, start1, size1, start2, size2);

    const float* src = srcBuf.getReadPointer(0);
    int totalRead = 0;

    // region のサンプルを fftData に積み上げる。kFftSize 到達で true を返す。
    auto writeSlice = [&](int srcStart, int count) -> bool
    {
        for (int i = 0; i < count; ++i)
        {
            fftData[static_cast<std::size_t>(fifoIndex)] = src[srcStart + i];
            ++fifoIndex;
            ++totalRead;

            if (fifoIndex >= kFftSize)
            {
                fifoIndex = 0;
                nextFFTBlockReady = true;
                return true;
            }
        }
        return false;
    };

    if (!writeSlice(start1, size1))
        writeSlice(start2, size2);

    fifo.finishedRead(totalRead);
}

// -----------------------------------------------------------------------------
// FFT を実行してスコープデータ（dB）を更新する
// -----------------------------------------------------------------------------
void SpectrumDisplay::computeFFT()
{
    // Hanning 窓を乗算してスペクトル漏れを低減
    for (int i = 0; i < kFftSize; ++i)
    {
        const float phase  = static_cast<float>(i) / static_cast<float>(kFftSize - 1);
        const float window = 0.5f * (1.0f - std::cos(
            juce::MathConstants<float>::twoPi * phase));
        fftData[static_cast<std::size_t>(i)] *= window;
    }

    // 後半（FFT 作業領域）をゼロクリア
    std::fill(fftData.begin() + kFftSize, fftData.end(), 0.0f);

    // FFT 実行 → fftData[0..kFftSize/2-1] に振幅が格納される
    forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());

    // 振幅を dB に変換し、指数移動平均でスムージング（下降は緩やか）
    constexpr float kSmooth   = 0.15f;
    const float invFFTSize    = 1.0f / static_cast<float>(kFftSize);

    for (int i = 0; i < kFftSize / 2; ++i)
    {
        const float magnitude = fftData[static_cast<std::size_t>(i)] * invFFTSize;
        const float db        = juce::Decibels::gainToDecibels(magnitude, kMinDb);
        const float clamped   = juce::jlimit(kMinDb, kMaxDb, db);

        const float prev = scopeData[static_cast<std::size_t>(i)];
        scopeData[static_cast<std::size_t>(i)] = (clamped > prev)
            ? clamped
            : prev + kSmooth * (clamped - prev);
    }
}

// -----------------------------------------------------------------------------
// 周波数 → 画面 x 比率（0.0〜1.0）の対数マッピング
// -----------------------------------------------------------------------------
float SpectrumDisplay::freqToX(float freq) noexcept
{
    return std::log(freq / kMinFreq) / std::log(kMaxFreq / kMinFreq);
}

// -----------------------------------------------------------------------------
// paint
// -----------------------------------------------------------------------------
void SpectrumDisplay::paint(juce::Graphics& g)
{
    // 全体背景
    g.fillAll(kBackground);

    // ヘッダー
    auto bounds = getLocalBounds();
    auto header = bounds.removeFromTop(kHeaderHeight).reduced(8, 4);

    // bold 付きだと日本語フォント解決に失敗するため plain で表示
    g.setFont(juce::Font(juce::FontOptions(13.0f)));
    g.setColour(kAccent);
    g.drawText("スペクトラム", header.removeFromLeft(80),
               juce::Justification::centredLeft, true);

    g.setFont(juce::Font(juce::FontOptions(11.0f)));
    g.setColour(kTextDim);
    g.drawText("Frequency Domain", header, juce::Justification::centredLeft, true);

    // スペクトラム描画エリア（パネル背景）
    g.setColour(kPanel);
    g.fillRect(bounds);

    drawSpectrum(g, bounds);
}

// -----------------------------------------------------------------------------
// スペクトラム描画（対数周波数軸）
// sampleRate を 48000 Hz と仮定（可視化目的の近似）
// -----------------------------------------------------------------------------
void SpectrumDisplay::drawSpectrum(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    const float w  = static_cast<float>(bounds.getWidth());
    const float h  = static_cast<float>(bounds.getHeight());
    const float x0 = static_cast<float>(bounds.getX());
    const float y0 = static_cast<float>(bounds.getY());

    // 垂直グリッドと周波数ラベル
    struct FreqLabel { float freq; const char* label; };
    const FreqLabel freqLabels[] = {
        { 100.0f,   "100"  },
        { 500.0f,   "500"  },
        { 1000.0f,  "1k"   },
        { 5000.0f,  "5k"   },
        { 10000.0f, "10k"  },
    };

    g.setFont(juce::Font(juce::FontOptions(9.0f)));
    for (const auto& fl : freqLabels)
    {
        const float xNorm = freqToX(fl.freq);
        if (xNorm < 0.0f || xNorm > 1.0f) continue;
        const float gx = x0 + xNorm * w;

        g.setColour(kGrid);
        g.drawVerticalLine(static_cast<int>(gx), y0, y0 + h);

        g.setColour(kTextDim);
        g.drawText(fl.label,
                   static_cast<int>(gx) - 12,
                   static_cast<int>(y0 + h) - 14,
                   24, 12, juce::Justification::centred, false);
    }

    // スペクトラム曲線（画面幅 1px ずつサンプリング）
    juce::Path spectrumPath;
    bool pathStarted = false;

    for (int px = 0; px < static_cast<int>(w); ++px)
    {
        const float xNorm = static_cast<float>(px) / w;
        const float freq  = kMinFreq * std::pow(kMaxFreq / kMinFreq, xNorm);
        // 48000 Hz サンプルレートを前提とした bin 計算
        const float binF  = freq * static_cast<float>(kFftSize) / 48000.0f;
        const int   bin   = juce::jlimit(0, kFftSize / 2 - 1,
                                         static_cast<int>(binF));

        const float db    = scopeData[static_cast<std::size_t>(bin)];
        const float yNorm = 1.0f - (db - kMinDb) / (kMaxDb - kMinDb);
        const float py    = y0 + yNorm * h;

        if (!pathStarted)
        {
            spectrumPath.startNewSubPath(x0 + static_cast<float>(px), py);
            pathStarted = true;
        }
        else
        {
            spectrumPath.lineTo(x0 + static_cast<float>(px), py);
        }
    }

    // 塗りつぶし領域（曲線の下部）
    if (pathStarted)
    {
        juce::Path fillPath = spectrumPath;
        fillPath.lineTo(x0 + w, y0 + h);
        fillPath.lineTo(x0,     y0 + h);
        fillPath.closeSubPath();

        g.setColour(kAccent.withAlpha(0.12f));
        g.fillPath(fillPath);

        // アウトライン
        g.setColour(kAccent);
        g.strokePath(spectrumPath, juce::PathStrokeType(1.5f));
    }
}

void SpectrumDisplay::resized() {}

} // namespace synth
