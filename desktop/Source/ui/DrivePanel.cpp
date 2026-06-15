// =============================================================================
// DrivePanel.cpp
// =============================================================================
#include "DrivePanel.h"

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

DrivePanel::DrivePanel(juce::AudioProcessorValueTreeState& apvts)
{
    typeBox.addItem("Soft", 1);
    typeBox.addItem("Hard", 2);
    addAndMakeVisible(typeBox);
    typeBox.addMouseListener(this, false);
    setupLabel(typeLabel, "Type");
    addAndMakeVisible(typeLabel);

    setupRotary(amountSlider);
    addAndMakeVisible(amountSlider);
    amountSlider.addMouseListener(this, false);
    setupLabel(amountLabel, "Amount");
    addAndMakeVisible(amountLabel);

    setupRotary(mixSlider);
    addAndMakeVisible(mixSlider);
    mixSlider.addMouseListener(this, false);
    setupLabel(mixLabel, "Mix");
    addAndMakeVisible(mixLabel);

    typeAttachment   = std::make_unique<APVTS::ComboBoxAttachment>(apvts, ParamID::DriveType,   typeBox);
    amountAttachment = std::make_unique<APVTS::SliderAttachment>  (apvts, ParamID::DriveAmount, amountSlider);
    mixAttachment    = std::make_unique<APVTS::SliderAttachment>  (apvts, ParamID::DriveMix,    mixSlider);
}

void DrivePanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &typeBox      && onParamHover) onParamHover(ParamID::DriveType);
    if (e.eventComponent == &amountSlider && onParamHover) onParamHover(ParamID::DriveAmount);
    if (e.eventComponent == &mixSlider    && onParamHover) onParamHover(ParamID::DriveMix);
}

void DrivePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("DRIVE", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void DrivePanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    const int labelH = 13;
    const int comboH = 22;

    typeLabel.setBounds(area.removeFromTop(labelH));
    typeBox.setBounds(area.removeFromTop(comboH));
    area.removeFromTop(4);

    const int halfW = area.getWidth() / 2;

    auto amountCol = area.removeFromLeft(halfW);
    amountLabel.setBounds(amountCol.removeFromBottom(labelH));
    amountSlider.setBounds(amountCol.reduced(3));

    mixLabel.setBounds(area.removeFromBottom(labelH));
    mixSlider.setBounds(area.reduced(3));
}

} // namespace synth
