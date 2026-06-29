#pragma once

#include <juce_dsp/juce_dsp.h>

// ============================================================================
// VocoderBand
//
// PLAIN ENGLISH: A vocoder splits sound into multiple frequency "slices"
// (like a graphic equaliser with many bands). This class represents ONE
// of those slices. Each band does three jobs:
//
//   1. Listens to the VOICE and measures how loud just this slice of
//      frequency is, moment to moment (this is the "envelope follower").
//   2. Takes the SYNTH (carrier) sound and filters it down to just this
//      same frequency slice.
//   3. Multiplies the filtered synth by the voice's loudness pattern,
//      so the synth "pulses" in time with the voice.
//
// A full vocoder is just many of these bands added together afterward.
// ============================================================================
class VocoderBand
{
public:
    VocoderBand();

    // Sets up the band for a given centre frequency (in Hz) and how
    // "wide" a slice of frequency it covers (Q = narrowness; higher Q
    // means a thinner, more precise slice).
    void prepare (double sampleRate, float centreFrequencyHz, float qFactor);

    // Resets internal state (called when playback stops/starts).
    void reset();

    // Processes one sample of audio.
    //   voiceSample  = the input voice signal for this instant
    //   carrierSample = the input synth/carrier signal for this instant
    //   followerSpeed = how quickly the envelope follower reacts
    //                   (0 = very slow/smooth, 1 = very fast/snappy)
    // Returns: this band's contribution to the final vocoded sound.
    float processSample (float voiceSample, float carrierSample, float followerSpeed);

private:
    // Filters used to isolate this frequency slice from the voice and
    // from the carrier respectively. JUCE's IIR filter is a standard,
    // efficient building block for this - no need to write filter
    // maths by hand.
    juce::dsp::IIR::Filter<float> voiceFilter;
    juce::dsp::IIR::Filter<float> carrierFilter;

    double currentSampleRate { 44100.0 };
    float centreFrequency { 1000.0f };
    float qualityFactor { 4.0f };

    // The envelope follower's "memory" of how loud the voice was a
    // moment ago. Smoothing this over time is what avoids harsh
    // clicking/crackling sounds.
    float envelopeValue { 0.0f };
};
