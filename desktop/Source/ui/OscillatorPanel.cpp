// =============================================================================
// OscillatorPanel.cpp
// =============================================================================
#include "OscillatorPanel.h"

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

OscillatorPanel::OscillatorPanel(juce::AudioProcessorValueTreeState& apvts)
{
    waveformBox.addItem("Sine",     1);
    waveformBox.addItem("Saw",      2);
    waveformBox.addItem("Square",   3);
    waveformBox.addItem("Triangle", 4);
    addAndMakeVisible(waveformBox);
    waveformBox.addMouseListener(this, false);
    setupLabel(waveformLabel, "Wave");
    addAndMakeVisible(waveformLabel);

    setupRotary(detuneSlider);
    addAndMakeVisible(detuneSlider);
    detuneSlider.addMouseListener(this, false);
    setupLabel(detuneLabel, "Detune");
    addAndMakeVisible(detuneLabel);

    setupRotary(octaveSlider);
    addAndMakeVisible(octaveSlider);
    octaveSlider.addMouseListener(this, false);
    setupLabel(octaveLabel, "Octave");
    addAndMakeVisible(octaveLabel);

    setupRotary(levelSlider);
    addAndMakeVisible(levelSlider);
    levelSlider.addMouseListener(this, false);
    setupLabel(levelLabel, "Level");
    addAndMakeVisible(levelLabel);

    waveformAttachment = std::make_unique<APVTS::ComboBoxAttachment>(apvts, ParamID::OscWaveform, waveformBox);
    detuneAttachment   = std::make_unique<APVTS::SliderAttachment>  (apvts, ParamID::OscDetune,   detuneSlider);
    octaveAttachment   = std::make_unique<APVTS::SliderAttachment>  (apvts, ParamID::OscOctave,   octaveSlider);
    levelAttachment    = std::make_unique<APVTS::SliderAttachment>  (apvts, ParamID::OscLevel,    levelSlider);
}

void OscillatorPanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &waveformBox   && onParamHover) onParamHover(ParamID::OscWaveform);
    if (e.eventComponent == &detuneSlider  && onParamHover) onParamHover(ParamID::OscDetune);
    if (e.eventComponent == &octaveSlider  && onParamHover) onParamHover(ParamID::OscOctave);
    if (e.eventComponent == &levelSlider   && onParamHover) onParamHover(ParamID::OscLevel);
}

void OscillatorPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("OSC", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void OscillatorPanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    const int labelH = 13;
    const int comboH = 22;

    // Waveform ComboBox
    waveformLabel.setBounds(area.removeFromTop(labelH));
    waveformBox.setBounds(area.removeFromTop(comboH));
    area.removeFromTop(4);

    // Detune / Octave / Level を横並び
    const int thirdW = area.getWidth() / 3;

    auto detuneCol = area.removeFromLeft(thirdW);
    detuneLabel.setBounds(detuneCol.removeFromBottom(labelH));
    detuneSlider.setBounds(detuneCol.reduced(2));

    auto octaveCol = area.removeFromLeft(thirdW);
    octaveLabel.setBounds(octaveCol.removeFromBottom(labelH));
    octaveSlider.setBounds(octaveCol.reduced(2));

    levelLabel.setBounds(area.removeFromBottom(labelH));
    levelSlider.setBounds(area.reduced(2));
}

} // namespace synth
