#pragma once

#include <juce_dsp/juce_dsp.h>

// ============================================================================
// OutputStage
//
// PLAIN ENGLISH: This handles the LAST two controls in the signal chain,
// after the vocoder effect has already been calculated:
//
//   1. DRY/WET - a final safety-net blend. 0% = you hear only the
//      original untouched voice, 100% = you hear only the vocoded
//      result. This is deliberately separate from the "Strength"
//      control inside the vocoder itself - Strength shapes HOW the
//      effect behaves, Dry/Wet decides how much of that final result
//      gets mixed back with the original voice.
//
//   2. OUTPUT GAIN - a simple volume control, since combining many
//      frequency bands together can sometimes make the result quieter
//      or louder than expected. This lets the musician correct that
//      without having to add ANOTHER plugin afterward just for volume.
//
// IMPORTANT DETAIL: both controls "smooth" their changes over a few
// milliseconds rather than jumping instantly. Without this, turning a
// knob while audio is playing can cause an audible click or pop -
// smoothing avoids that.
// ============================================================================
class OutputStage
{
public:
    OutputStage();

    void prepare (double sampleRate);
    void reset();

    // 0.0 = fully dry (original voice only), 1.0 = fully wet (vocoded only).
    void setDryWetMix (float newMix);

    // Output volume, expressed in decibels (the standard unit for audio
    // volume - 0dB means "no change", positive boosts, negative reduces).
    void setOutputGainDb (float newGainDb);

    // Blends dry and wet signals together, then applies output gain.
    //   drySignal = the original, untouched voice (before vocoding)
    //   wetBuffer = the vocoded result (gets overwritten in place
    //               with the final output)
    void processBlock (const juce::AudioBuffer<float>& drySignal,
                        juce::AudioBuffer<float>& wetBuffer);

private:
    // JUCE's SmoothedValue automatically handles the "don't jump
    // instantly, ease into the new value over a few milliseconds" logic
    // for us, so we don't need to write that smoothing logic by hand.
    juce::SmoothedValue<float> dryWetMix { 1.0f };
    juce::SmoothedValue<float> outputGain { 1.0f };
};
