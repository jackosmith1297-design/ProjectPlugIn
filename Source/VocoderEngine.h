#pragma once

#include "VocoderBand.h"
#include <vector>

class VocoderEngine
{
public:
    enum class Character { Classic, Modern };

    VocoderEngine();

    void prepare (double sampleRate);
    void reset();
    void setCharacter (Character newCharacter);
    void setStrength (float newStrength);

    void processBlock (juce::AudioBuffer<float>& voiceBuffer,
                       const juce::AudioBuffer<float>& carrierBuffer);

private:
    void rebuildBands();

    std::vector<VocoderBand> bands;

    Character currentCharacter { Character::Classic };
    float     strength         { 1.0f };
    double    sampleRate       { 44100.0 };

    // ── Sibilance enhancement ───────────────────────────────────────────────
    // High-pass filters: one pair per channel (max stereo = 2).
    // voiceHPF  detects HF energy in the voice (sibilants: s, t, f, sh).
    // noiseHPF  shapes white noise to match that same frequency region.
    juce::dsp::IIR::Filter<float> sibilanceVoiceHPF[2];
    juce::dsp::IIR::Filter<float> sibilanceNoiseHPF[2];

    float    sibilanceEnv[2]        { 0.0f, 0.0f };
    float    sibilanceAttackCoeff   { 0.0f };
    float    sibilanceReleaseCoeff  { 0.0f };
    float    sibilanceAmount        { 0.0f }; // set per character in rebuildBands()

    // Minimal xorshift32 for real-time noise generation (no heap allocation).
    uint32_t noiseSeed { 2463534242u };
};
