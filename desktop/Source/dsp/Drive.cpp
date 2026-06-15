// =============================================================================
// Drive.cpp
// =============================================================================
#include "Drive.h"
#include <cmath>

namespace synth {

float Drive::softClip(float x) noexcept
{
    // tanh ベースのソフトクリッピング
    return std::tanh(x);
}

float Drive::hardClip(float x) noexcept
{
    return juce::jlimit(-1.0f, 1.0f, x);
}

void Drive::prepare(const juce::dsp::ProcessSpec& spec)
{
    softShaper.functionToUse = softClip;
    hardShaper.functionToUse = hardClip;

    softShaper.prepare(spec);
    hardShaper.prepare(spec);

    const int maxBlock = static_cast<int>(spec.maximumBlockSize);
    dryBuffer.setSize(static_cast<int>(spec.numChannels), maxBlock);
}

void Drive::reset()
{
    softShaper.reset();
    hardShaper.reset();
}

void Drive::process(juce::AudioBuffer<float>& buffer,
                    DriveType type, float amount, float mix)
{
    if (mix <= 0.0f) return;

    const int numCh      = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // dry を保存
    for (int ch = 0; ch < numCh; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    // ゲイン増幅してシェーパーに入力
    const float driveGain = 1.0f + amount * 9.0f; // 1x..10x
    buffer.applyGain(driveGain);

    auto block = juce::dsp::AudioBlock<float>(buffer).getSubBlock(0, (size_t)numSamples);
    auto ctx   = juce::dsp::ProcessContextReplacing<float>(block);

    if (type == DriveType::Soft)
        softShaper.process(ctx);
    else
        hardShaper.process(ctx);

    // 出力レベルを正規化してから wet/dry ミックス
    for (int ch = 0; ch < numCh; ++ch)
    {
        auto* wet = buffer.getWritePointer(ch);
        const auto* dry = dryBuffer.getReadPointer(ch);
        for (int s = 0; s < numSamples; ++s)
            wet[s] = wet[s] * mix + dry[s] * (1.0f - mix);
    }
}

} // namespace synth
