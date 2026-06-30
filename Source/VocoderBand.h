#pragma once

#include <juce_dsp/juce_dsp.h>

// ============================================================================
// VocoderBand — one frequency slice of the vocoder.
//
// Each band band-passes the voice and carrier to the same centre frequency,
// envelope-follows the voice energy in that slice, then stamps that envelope
// onto the filtered carrier.  Attack and release are independent so that
// consonants are caught quickly (fast attack) while vowels decay gracefully
// (slower release) — a critical difference from the original single-speed
// follower.
// ============================================================================
class VocoderBand
{
public:
    VocoderBand();

    // attackMs  — how quickly the envelope rises  (1–5 ms recommended)
    // releaseMs — how quickly the envelope falls  (30–80 ms recommended)
    void prepare (double sampleRate, float centreFrequencyHz, float qFactor,
                  float attackMs, float releaseMs);

    void reset();

    // Returns this band's contribution to the vocoded output.
    float processSample (float voiceSample, float carrierSample);

private:
    juce::dsp::IIR::Filter<float> voiceFilter;
    juce::dsp::IIR::Filter<float> carrierFilter;

    double currentSampleRate { 44100.0 };
    float  centreFrequency   { 1000.0f };
    float  qualityFactor     {    4.0f };

    float envelopeValue  { 0.0f };
    float attackCoeff    { 0.0f }; // close to 0 = fastest attack
    float releaseCoeff   { 0.0f }; // close to 1 = slowest release
};
