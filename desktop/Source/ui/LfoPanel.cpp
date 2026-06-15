// =============================================================================
// LfoPanel.cpp
// =============================================================================
#include "LfoPanel.h"

namespace {

void setupRotary(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
}

void setupLabel(juce::Label& l, const juce::String& text)
{
    l.setText(text, juce::dontSendNotification);
    l.setFont(juce::FontOptions(10.0f));
    l.setColour(juce::Label::textColourId, juce::Colour(0xff888888));
    l.setJustificationType(juce::Justification::centred);
}

} // anonymous namespace

namespace synth {

LfoPanel::LfoPanel(juce::AudioProcessorValueTreeState& apvts)
{
    waveformBox.addItem("Sine",     1);
    waveformBox.addItem("Saw",      2);
    waveformBox.addItem("Square",   3);
    waveformBox.addItem("Triangle", 4);
    addAndMakeVisible(waveformBox);
    waveformBox.addMouseListener(this, false);
    setupLabel(waveformLabel, "Wave");
    addAndMakeVisible(waveformLabel);

    setupRotary(rateSlider);
    addAndMakeVisible(rateSlider);
    rateSlider.addMouseListener(this, false);
    setupLabel(rateLabel, "Rate");
    addAndMakeVisible(rateLabel);

    setupRotary(depthSlider);
    addAndMakeVisible(depthSlider);
    depthSlider.addMouseListener(this, false);
    setupLabel(depthLabel, "Depth");
    addAndMakeVisible(depthLabel);

    targetBox.addItem("Pitch",  1);
    targetBox.addItem("Filter", 2);
    targetBox.addItem("Amp",    3);
    addAndMakeVisible(targetBox);
    targetBox.addMouseListener(this, false);
    setupLabel(targetLabel, "Target");
    addAndMakeVisible(targetLabel);

    waveformAttachment = std::make_unique<APVTS::ComboBoxAttachment>(apvts, ParamID::LfoWaveform, waveformBox);
    rateAttachment     = std::make_unique<APVTS::SliderAttachment>  (apvts, ParamID::LfoRate,     rateSlider);
    depthAttachment    = std::make_unique<APVTS::SliderAttachment>  (apvts, ParamID::LfoDepth,    depthSlider);
    targetAttachment   = std::make_unique<APVTS::ComboBoxAttachment>(apvts, ParamID::LfoTarget,   targetBox);
}

void LfoPanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &waveformBox  && onParamHover) onParamHover(ParamID::LfoWaveform);
    if (e.eventComponent == &rateSlider   && onParamHover) onParamHover(ParamID::LfoRate);
    if (e.eventComponent == &depthSlider  && onParamHover) onParamHover(ParamID::LfoDepth);
    if (e.eventComponent == &targetBox    && onParamHover) onParamHover(ParamID::LfoTarget);
}

void LfoPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("LFO", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void LfoPanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    const int labelH = 13;
    const int comboH = 22;
    const int halfH  = area.getHeight() / 2;
    const int halfW  = area.getWidth()  / 2;

    // 上段: Wave combo / Target combo
    auto topRow = area.removeFromTop(halfH);

    auto waveCol = topRow.removeFromLeft(halfW);
    waveformLabel.setBounds(waveCol.removeFromTop(labelH));
    waveformBox.setBounds(waveCol.removeFromTop(comboH));

    targetLabel.setBounds(topRow.removeFromTop(labelH));
    targetBox.setBounds(topRow.removeFromTop(comboH));

    // 下段: Rate / Depth スライダー
    const int sliderHalfW = area.getWidth() / 2;

    auto rateCol = area.removeFromLeft(sliderHalfW);
    rateLabel.setBounds(rateCol.removeFromBottom(labelH));
    rateSlider.setBounds(rateCol.reduced(3));

    depthLabel.setBounds(area.removeFromBottom(labelH));
    depthSlider.setBounds(area.reduced(3));
}

} // namespace synth
