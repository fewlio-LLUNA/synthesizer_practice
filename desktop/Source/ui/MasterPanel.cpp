// =============================================================================
// MasterPanel.cpp
// =============================================================================
#include "MasterPanel.h"

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

MasterPanel::MasterPanel(juce::AudioProcessorValueTreeState& apvts)
{
    setupRotary(volumeSlider);
    addAndMakeVisible(volumeSlider);
    volumeSlider.addMouseListener(this, false);
    setupLabel(volumeLabel, "Volume");
    addAndMakeVisible(volumeLabel);

    volumeAttachment = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::MasterVolume, volumeSlider);
}

void MasterPanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &volumeSlider && onParamHover)
        onParamHover(ParamID::MasterVolume);
}

void MasterPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("MASTER", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void MasterPanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    const int labelH = 13;
    volumeLabel.setBounds(area.removeFromBottom(labelH));
    volumeSlider.setBounds(area.reduced(8));
}

} // namespace synth
