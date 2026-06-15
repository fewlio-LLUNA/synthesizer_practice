// =============================================================================
// FilterEnvelopePanel.cpp
// =============================================================================
#include "FilterEnvelopePanel.h"

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

FilterEnvelopePanel::FilterEnvelopePanel(juce::AudioProcessorValueTreeState& apvts)
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
    setupSlider(amountSlider,  amountLabel,  "Amt");

    attackAttachment  = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::FEnvAttack,  attackSlider);
    decayAttachment   = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::FEnvDecay,   decaySlider);
    sustainAttachment = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::FEnvSustain, sustainSlider);
    releaseAttachment = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::FEnvRelease, releaseSlider);
    amountAttachment  = std::make_unique<APVTS::SliderAttachment>(apvts, ParamID::FEnvAmount,  amountSlider);
}

void FilterEnvelopePanel::mouseEnter(const juce::MouseEvent& e)
{
    if (e.eventComponent == &attackSlider  && onParamHover) onParamHover(ParamID::FEnvAttack);
    if (e.eventComponent == &decaySlider   && onParamHover) onParamHover(ParamID::FEnvDecay);
    if (e.eventComponent == &sustainSlider && onParamHover) onParamHover(ParamID::FEnvSustain);
    if (e.eventComponent == &releaseSlider && onParamHover) onParamHover(ParamID::FEnvRelease);
    if (e.eventComponent == &amountSlider  && onParamHover) onParamHover(ParamID::FEnvAmount);
}

void FilterEnvelopePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colour(0xffff8c00));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("F.ENV", getLocalBounds().reduced(4).removeFromTop(20),
               juce::Justification::centredLeft);
}

void FilterEnvelopePanel::resized()
{
    auto area = getLocalBounds().reduced(4);
    area.removeFromTop(20);

    // A/D/S/R/Amount を上段 (A/D/S/R) + 下段 (Amount) に分ける
    const int labelH = 13;
    const int halfH  = area.getHeight() / 2;
    const int quarterW = area.getWidth() / 4;

    // 上段: A / D / S / R
    auto topRow = area.removeFromTop(halfH);
    juce::Slider* adsr[]   = { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider };
    juce::Label*  adsr_l[] = { &attackLabel,  &decayLabel,  &sustainLabel,  &releaseLabel  };

    for (int i = 0; i < 4; ++i)
    {
        auto col = (i < 3) ? topRow.removeFromLeft(quarterW) : topRow;
        adsr_l[i]->setBounds(col.removeFromBottom(labelH));
        adsr[i]->setBounds(col.reduced(2));
    }

    // 下段: Amount
    amountLabel.setBounds(area.removeFromBottom(labelH));
    amountSlider.setBounds(area.reduced(8));
}

} // namespace synth
