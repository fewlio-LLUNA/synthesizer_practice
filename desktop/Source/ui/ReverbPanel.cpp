// =============================================================================
// ReverbPanel.cpp
// =============================================================================
#include "ReverbPanel.h"

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

ReverbPanel::ReverbPanel(juce::AudioProcessorValueTreeState& apvts)
{
    setupRotary(decaySlider);
    addAndMakeVisible(decaySlider);
    decaySlider.addMouseListener(this, false);
    setupLabel(decayLabel, "Decay");
    addAndMakeVisible(decayLabel);

    setupRotary(mixSlider);
    addAndMakeVisible(mixSlider);
    mixSlider.addMouseListener(this, false);
    setupLabel(mixLabel, "Mix");
    addAndMakeVisible(mixLabel);

    decayAttachment = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::ReverbDecay, decaySlider);
    mixAttachment   = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::ReverbMix,   mixSlider);
}

void ReverbPanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &decaySlider && onParamHover) onParamHover(ParamID::ReverbDecay);
    if (e.eventComponent == &mixSlider   && onParamHover) onParamHover(ParamID::ReverbMix);
}

void ReverbPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("REVERB", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void ReverbPanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    const int labelH = 13;
    const int halfW  = area.getWidth() / 2;

    auto leftCol = area.removeFromLeft(halfW);
    decayLabel.setBounds(leftCol.removeFromBottom(labelH));
    decaySlider.setBounds(leftCol.reduced(4));

    mixLabel.setBounds(area.removeFromBottom(labelH));
    mixSlider.setBounds(area.reduced(4));
}

} // namespace synth
