// =============================================================================
// PluginEditor.cpp — GUI 実装（最小スタブ）
//
// TODO: Session D — 11セクション（Osc, Noise, Filter, ...）を配置する。
// Session B の RotaryKnob、Session C の Visualizer もここに統合される。
// =============================================================================
#include "PluginEditor.h"

namespace synth {

SynthAudioProcessorEditor::SynthAudioProcessorEditor(SynthAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), processorRef(p)
{
    setSize(1200, 720);

    addAndMakeVisible(keyboard);
}

SynthAudioProcessorEditor::~SynthAudioProcessorEditor() = default;

void SynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(26, 26, 26));

    g.setColour(juce::Colour::fromRGB(136, 136, 136));
    g.setFont(juce::FontOptions(16.0f));
    g.drawText("Synthesizer Practice (Desktop)",
               getLocalBounds().reduced(16).removeFromTop(40),
               juce::Justification::topLeft);

    g.setColour(juce::Colour::fromRGB(68, 68, 68));
    g.setFont(juce::FontOptions(12.0f));
    g.drawText("各セクション UI は Session D が実装します",
               getLocalBounds().reduced(16).removeFromTop(80).removeFromBottom(30),
               juce::Justification::topLeft);
}

void SynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    keyboard.setBounds(area.removeFromBottom(120).reduced(16));
}

} // namespace synth
