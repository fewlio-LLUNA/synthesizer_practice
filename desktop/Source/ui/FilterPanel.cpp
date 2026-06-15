// =============================================================================
// FilterPanel.cpp
// =============================================================================
#include "FilterPanel.h"

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

FilterPanel::FilterPanel(juce::AudioProcessorValueTreeState& apvts)
{
    typeBox.addItem("LP", 1);
    typeBox.addItem("HP", 2);
    typeBox.addItem("BP", 3);
    addAndMakeVisible(typeBox);
    typeBox.addMouseListener(this, false);
    setupLabel(typeLabel, "Type");
    addAndMakeVisible(typeLabel);

    setupRotary(cutoffSlider);
    addAndMakeVisible(cutoffSlider);
    cutoffSlider.addMouseListener(this, false);
    setupLabel(cutoffLabel, "Cutoff");
    addAndMakeVisible(cutoffLabel);

    setupRotary(resoSlider);
    addAndMakeVisible(resoSlider);
    resoSlider.addMouseListener(this, false);
    setupLabel(resoLabel, "Reso");
    addAndMakeVisible(resoLabel);

    typeAttachment   = std::make_unique<APVTS::ComboBoxAttachment>(apvts, ParamID::FilterType,   typeBox);
    cutoffAttachment = std::make_unique<APVTS::SliderAttachment>  (apvts, ParamID::FilterCutoff, cutoffSlider);
    resoAttachment   = std::make_unique<APVTS::SliderAttachment>  (apvts, ParamID::FilterReso,   resoSlider);
}

void FilterPanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &typeBox      && onParamHover) onParamHover(ParamID::FilterType);
    if (e.eventComponent == &cutoffSlider && onParamHover) onParamHover(ParamID::FilterCutoff);
    if (e.eventComponent == &resoSlider   && onParamHover) onParamHover(ParamID::FilterReso);
}

void FilterPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("FILTER", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void FilterPanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    const int labelH = 13;
    const int comboH = 22;

    // Type ComboBox
    typeLabel.setBounds(area.removeFromTop(labelH));
    typeBox.setBounds(area.removeFromTop(comboH));
    area.removeFromTop(4);

    // Cutoff / Reso を横並び
    const int halfW = area.getWidth() / 2;

    auto cutoffCol = area.removeFromLeft(halfW);
    cutoffLabel.setBounds(cutoffCol.removeFromBottom(labelH));
    cutoffSlider.setBounds(cutoffCol.reduced(3));

    resoLabel.setBounds(area.removeFromBottom(labelH));
    resoSlider.setBounds(area.reduced(3));
}

} // namespace synth
