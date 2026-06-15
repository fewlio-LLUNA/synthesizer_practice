// =============================================================================
// DelayPanel.cpp
// =============================================================================
#include "DelayPanel.h"

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

DelayPanel::DelayPanel(juce::AudioProcessorValueTreeState& apvts)
{
    setupRotary(timeSlider);
    addAndMakeVisible(timeSlider);
    timeSlider.addMouseListener(this, false);
    setupLabel(timeLabel, "Time");
    addAndMakeVisible(timeLabel);

    setupRotary(feedbackSlider);
    addAndMakeVisible(feedbackSlider);
    feedbackSlider.addMouseListener(this, false);
    setupLabel(feedbackLabel, "Feedback");
    addAndMakeVisible(feedbackLabel);

    setupRotary(mixSlider);
    addAndMakeVisible(mixSlider);
    mixSlider.addMouseListener(this, false);
    setupLabel(mixLabel, "Mix");
    addAndMakeVisible(mixLabel);

    timeAttachment     = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::DelayTime,     timeSlider);
    feedbackAttachment = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::DelayFeedback, feedbackSlider);
    mixAttachment      = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::DelayMix,      mixSlider);
}

void DelayPanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &timeSlider     && onParamHover) onParamHover(ParamID::DelayTime);
    if (e.eventComponent == &feedbackSlider && onParamHover) onParamHover(ParamID::DelayFeedback);
    if (e.eventComponent == &mixSlider      && onParamHover) onParamHover(ParamID::DelayMix);
}

void DelayPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("DELAY", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void DelayPanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    const int labelH  = 13;
    const int thirdW  = area.getWidth() / 3;

    auto timeCol = area.removeFromLeft(thirdW);
    timeLabel.setBounds(timeCol.removeFromBottom(labelH));
    timeSlider.setBounds(timeCol.reduced(4));

    auto fbCol = area.removeFromLeft(thirdW);
    feedbackLabel.setBounds(fbCol.removeFromBottom(labelH));
    feedbackSlider.setBounds(fbCol.reduced(4));

    mixLabel.setBounds(area.removeFromBottom(labelH));
    mixSlider.setBounds(area.reduced(4));
}

} // namespace synth
