// =============================================================================
// AmpEnvelopePanel.cpp
// =============================================================================
#include "AmpEnvelopePanel.h"

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

AmpEnvelopePanel::AmpEnvelopePanel(juce::AudioProcessorValueTreeState& apvts)
{
    auto setupSlider = [this](juce::Slider& s, juce::Label& l, const juce::String& name) {
        setupRotary(s);
        addAndMakeVisible(s);
        s.addMouseListener(this, false);
        setupLabel(l, name);
        addAndMakeVisible(l);
    };

    setupSlider(attackSlider,  attackLabel,  "A");
    setupSlider(decaySlider,   decayLabel,   "D");
    setupSlider(sustainSlider, sustainLabel, "S");
    setupSlider(releaseSlider, releaseLabel, "R");

    attackAttachment  = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::AmpAttack,  attackSlider);
    decayAttachment   = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::AmpDecay,   decaySlider);
    sustainAttachment = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::AmpSustain, sustainSlider);
    releaseAttachment = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::AmpRelease, releaseSlider);
}

void AmpEnvelopePanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &attackSlider  && onParamHover) onParamHover(ParamID::AmpAttack);
    if (e.eventComponent == &decaySlider   && onParamHover) onParamHover(ParamID::AmpDecay);
    if (e.eventComponent == &sustainSlider && onParamHover) onParamHover(ParamID::AmpSustain);
    if (e.eventComponent == &releaseSlider && onParamHover) onParamHover(ParamID::AmpRelease);
}

void AmpEnvelopePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("AMP.ENV", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void AmpEnvelopePanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    const int labelH  = 13;
    const int quarterW = area.getWidth() / 4;

    juce::Slider* sliders[] = { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider };
    juce::Label*  labels[]  = { &attackLabel,  &decayLabel,  &sustainLabel,  &releaseLabel  };

    for (int i = 0; i < 4; ++i)
    {
        auto col = (i < 3) ? area.removeFromLeft(quarterW) : area;
        labels[i]->setBounds(col.removeFromBottom(labelH));
        sliders[i]->setBounds(col.reduced(2));
    }
}

} // namespace synth
