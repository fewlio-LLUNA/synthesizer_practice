// =============================================================================
// NoisePanel.cpp
// =============================================================================
#include "NoisePanel.h"

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

NoisePanel::NoisePanel(juce::AudioProcessorValueTreeState& apvts)
{
    typeBox.addItem("White", 1);
    typeBox.addItem("Pink",  2);
    addAndMakeVisible(typeBox);
    typeBox.addMouseListener(this, false);
    setupLabel(typeLabel, "Type");
    addAndMakeVisible(typeLabel);

    setupRotary(levelSlider);
    addAndMakeVisible(levelSlider);
    levelSlider.addMouseListener(this, false);
    setupLabel(levelLabel, "Level");
    addAndMakeVisible(levelLabel);

    typeAttachment  = std::make_unique<APVTS::ComboBoxAttachment>(apvts, ParamID::NoiseType,  typeBox);
    levelAttachment = std::make_unique<APVTS::SliderAttachment>  (apvts, ParamID::NoiseLevel, levelSlider);
}

void NoisePanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &typeBox    && onParamHover) onParamHover(ParamID::NoiseType);
    if (e.eventComponent == &levelSlider && onParamHover) onParamHover(ParamID::NoiseLevel);
}

void NoisePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("NOISE", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void NoisePanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    const int labelH = 13;
    const int comboH = 22;

    typeLabel.setBounds(area.removeFromTop(labelH));
    typeBox.setBounds(area.removeFromTop(comboH));
    area.removeFromTop(6);

    levelLabel.setBounds(area.removeFromBottom(labelH));
    levelSlider.setBounds(area.reduced(4));
}

} // namespace synth
