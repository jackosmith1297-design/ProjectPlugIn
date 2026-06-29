#include "OutputStage.h"

OutputStage::OutputStage()
{
}

void OutputStage::prepare (double sampleRate)
{
    // The "0.02" below means: whenever the mix or gain changes, glide
    // to the new value over 20 milliseconds rather than jumping
    // instantly. 20ms is fast enough that it still feels immediate to
    // a person turning a knob, but slow enough to avoid audible clicks.
    constexpr double smoothingTimeSeconds = 0.02;

    dryWetMix.reset (sampleRate, smoothingTimeSeconds);
    outputGain.reset (sampleRate, smoothingTimeSeconds);
}

void OutputStage::reset()
{
    dryWetMix.setCurrentAndTargetValue (dryWetMix.getCurrentValue());
    outputGain.setCurrentAndTargetValue (outputGain.getCurrentValue());
}

void OutputStage::setDryWetMix (float newMix)
{
    dryWetMix.setTargetValue (juce::jlimit (0.0f, 1.0f, newMix));
}

void OutputStage::setOutputGainDb (float newGainDb)
{
    // Converts from decibels (what the musician sees and understands)
    // into a plain multiplier (what the actual audio maths needs).
    // E.g. 0dB becomes x1.0 (no change), +6dB becomes roughly x2.0 (twice as loud).
    const float gainAsMultiplier = juce::Decibels::decibelsToGain (newGainDb);
    outputGain.setTargetValue (gainAsMultiplier);
}

void OutputStage::processBlock (const juce::AudioBuffer<float>& drySignal,
                                 juce::AudioBuffer<float>& wetBuffer)
{
    const int numChannels = wetBuffer.getNumChannels();
    const int numSamples = wetBuffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Getting the next smoothed value advances the glide by one
        // step. We do this once per sample (not once per channel) so
        // that all channels move together in sync.
        const float currentMix = dryWetMix.getNextValue();
        const float currentGain = outputGain.getNextValue();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float dry = drySignal.getSample (channel, sample);
            const float wet = wetBuffer.getSample (channel, sample);

            const float blended = (dry * (1.0f - currentMix)) + (wet * currentMix);
            wetBuffer.setSample (channel, sample, blended * currentGain);
        }
    }
}
