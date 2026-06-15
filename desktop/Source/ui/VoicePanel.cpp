// =============================================================================
// VoicePanel.cpp
// =============================================================================
#include "VoicePanel.h"

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

VoicePanel::VoicePanel(juce::AudioProcessorValueTreeState& apvts)
{
    setupRotary(unisonCountSlider);
    addAndMakeVisible(unisonCountSlider);
    unisonCountSlider.addMouseListener(this, false);
    setupLabel(unisonCountLabel, "Unison");
    addAndMakeVisible(unisonCountLabel);

    setupRotary(unisonDetuneSlider);
    addAndMakeVisible(unisonDetuneSlider);
    unisonDetuneSlider.addMouseListener(this, false);
    setupLabel(unisonDetuneLabel, "Detune");
    addAndMakeVisible(unisonDetuneLabel);

    setupRotary(glideSlider);
    addAndMakeVisible(glideSlider);
    glideSlider.addMouseListener(this, false);
    setupLabel(glideLabel, "Glide");
    addAndMakeVisible(glideLabel);

    unisonCountAttachment  = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::UnisonCount,  unisonCountSlider);
    unisonDetuneAttachment = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::UnisonDetune, unisonDetuneSlider);
    glideAttachment        = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::Glide,        glideSlider);
}

void VoicePanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &unisonCountSlider  && onParamHover) onParamHover(ParamID::UnisonCount);
    if (e.eventComponent == &unisonDetuneSlider && onParamHover) onParamHover(ParamID::UnisonDetune);
    if (e.eventComponent == &glideSlider        && onParamHover) onParamHover(ParamID::Glide);
}

void VoicePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("VOICE", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void VoicePanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    const int labelH = 13;
    const int thirdW = area.getWidth() / 3;

    auto unisonCountCol = area.removeFromLeft(thirdW);
    unisonCountLabel.setBounds(unisonCountCol.removeFromBottom(labelH));
    unisonCountSlider.setBounds(unisonCountCol.reduced(4));

    auto unisonDetuneCol = area.removeFromLeft(thirdW);
    unisonDetuneLabel.setBounds(unisonDetuneCol.removeFromBottom(labelH));
    unisonDetuneSlider.setBounds(unisonDetuneCol.reduced(4));

    glideLabel.setBounds(area.removeFromBottom(labelH));
    glideSlider.setBounds(area.reduced(4));
}

} // namespace synth
