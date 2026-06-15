// =============================================================================
// InfoPanel.cpp
// =============================================================================
#include "InfoPanel.h"

namespace synth {

InfoPanel::InfoPanel()
{
    paramLabel.setFont(juce::FontOptions(11.0f));
    paramLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff8c00));
    paramLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(paramLabel);

    descLabel.setFont(juce::FontOptions(11.0f));
    descLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe0e0e0));
    descLabel.setJustificationType(juce::Justification::centredLeft);
    descLabel.setMinimumHorizontalScale(0.5f);
    addAndMakeVisible(descLabel);
}

void InfoPanel::setInfo(const juce::String& paramId, const juce::String& description)
{
    paramLabel.setText(paramId, juce::dontSendNotification);
    descLabel.setText(description, juce::dontSendNotification);
}

void InfoPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawHorizontalLine(0, 0.0f, static_cast<float>(getWidth()));

    g.setColour(juce::Colour(0xff555555));
    g.setFont(juce::FontOptions(10.0f));
    g.drawText("INFO", getLocalBounds().reduced(8, 0).removeFromLeft(40),
               juce::Justification::centredLeft);
}

void InfoPanel::resized()
{
    auto area = getLocalBounds().reduced(8, 6);
    area.removeFromLeft(48); // "INFO" ラベル分のスペース

    paramLabel.setBounds(area.removeFromLeft(150));
    descLabel.setBounds(area);
}

} // namespace synth
