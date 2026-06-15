// =============================================================================
// WaveformDisplay.cpp — 時間波形ビジュアライザ実装
// =============================================================================
#include "WaveformDisplay.h"

namespace synth {

// カラー定数（デザイン書 §5 に準拠）
static const juce::Colour kBackground  { 0xff1a1a1a };
static const juce::Colour kAccent      { 0xffff8c00 }; // オレンジ
static const juce::Colour kTextDim     { 0xff888888 };
static const juce::Colour kTextBright  { 0xffe0e0e0 };

WaveformDisplay::WaveformDisplay(juce::AudioVisualiserComponent& audioVisualiser)
    : visualiserRef(audioVisualiser)
{
    // 波形色: オレンジ、背景色: ダークパネル
    visualiserRef.setColours(juce::Colour(0xff2a2a2a), kAccent);
    visualiserRef.setOpaque(true);
    addAndMakeVisible(visualiserRef);
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    // 全体背景
    g.fillAll(kBackground);

    // ヘッダー領域
    auto header = getLocalBounds().removeFromTop(kHeaderHeight).reduced(8, 4);

    // タイトル「時間波形」 — bold 付きだと日本語フォントが解決できないため plain で表示
    g.setFont(juce::Font(juce::FontOptions(13.0f)));
    g.setColour(kAccent);
    const int titleWidth = 60;
    g.drawText("時間波形", header.removeFromLeft(titleWidth),
               juce::Justification::centredLeft, true);

    // キャプション
    g.setFont(juce::Font(juce::FontOptions(11.0f)));
    g.setColour(kTextDim);
    g.drawText("Time Domain", header, juce::Justification::centredLeft, true);
}

void WaveformDisplay::resized()
{
    // ヘッダー分を除いた領域に AudioVisualiserComponent を配置
    visualiserRef.setBounds(getLocalBounds().withTrimmedTop(kHeaderHeight));
}

} // namespace synth
