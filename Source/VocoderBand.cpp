#include "VocoderBand.h"

VocoderBand::VocoderBand()
{
}

void VocoderBand::prepare (double sampleRate, float centreFrequencyHz, float qFactor)
{
    currentSampleRate = sampleRate;
    centreFrequency = centreFrequencyHz;
    qualityFactor = qFactor;

    // Configure both filters as band-pass filters centred on the same
    // frequency - one will filter the voice, the other the carrier.
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass (
        currentSampleRate, centreFrequency, qualityFactor);

    voiceFilter.coefficients = coefficients;
    carrierFilter.coefficients = coefficients;

    reset();
}

void VocoderBand::reset()
{
    voiceFilter.reset();
    carrierFilter.reset();
    envelopeValue = 0.0f;
}

float VocoderBand::processSample (float voiceSample, float carrierSample, float followerSpeed)
{
    // Step 1: isolate this frequency slice from the voice signal.
    const float filteredVoice = voiceFilter.processSample (voiceSample);

    // Step 2: measure the loudness of that slice right now.
    // We use the absolute value (ignoring whether the wave is currently
    // above or below zero) because we only care about volume, not direction.
    const float instantLoudness = std::abs (filteredVoice);

    // Step 3: smooth that loudness measurement over time so it doesn't
    // jump around harshly. This is the classic "envelope follower" -
    // think of it like a needle on a volume meter that can only move
    // at a limited speed rather than snapping instantly.
    //
    // followerSpeed close to 1.0 = needle moves fast = snappier, more
    //   choppy/robotic result (this is part of what makes "Classic" sound classic).
    // followerSpeed close to 0.0 = needle moves slowly = smoother,
    //   more natural blending (this is part of what makes "Modern" sound modern).
    const float smoothingAmount = 1.0f - followerSpeed;
    envelopeValue = (smoothingAmount * envelopeValue) + ((1.0f - smoothingAmount) * instantLoudness);

    // Step 4: isolate this same frequency slice from the carrier/synth signal.
    const float filteredCarrier = carrierFilter.processSample (carrierSample);

    // Step 5: shape the carrier by the voice's loudness pattern.
    // This is the actual "vocoding" moment - the synth's volume in this
    // frequency slice now follows the voice's volume in this slice.
    return filteredCarrier * envelopeValue;
}
