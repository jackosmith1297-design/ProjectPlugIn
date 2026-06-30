#include "VocoderEngine.h"
#include <cmath>

VocoderEngine::VocoderEngine() {}

void VocoderEngine::prepare (double newSampleRate)
{
    sampleRate = newSampleRate;

    // Sibilance HPFs — detect and replicate consonant energy above ~4 kHz.
    // Q of 0.7 gives a gentle 2nd-order Butterworth roll-off.
    for (int ch = 0; ch < 2; ++ch)
    {
        sibilanceVoiceHPF[ch].coefficients =
            juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, 4000.0f, 0.7f);
        sibilanceNoiseHPF[ch].coefficients =
            juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, 4000.0f, 0.7f);
    }

    // Sibilance envelope: very fast attack (catches transients), moderate release.
    sibilanceAttackCoeff  = std::exp (-1.0f / float (sampleRate * 0.001)); // 1 ms
    sibilanceReleaseCoeff = std::exp (-1.0f / float (sampleRate * 0.060)); // 60 ms

    rebuildBands();
}

void VocoderEngine::reset()
{
    for (auto& band : bands)
        band.reset();

    for (int ch = 0; ch < 2; ++ch)
    {
        sibilanceVoiceHPF[ch].reset();
        sibilanceNoiseHPF[ch].reset();
        sibilanceEnv[ch] = 0.0f;
    }
}

void VocoderEngine::setCharacter (Character newCharacter)
{
    if (newCharacter == currentCharacter)
        return;
    currentCharacter = newCharacter;
    rebuildBands();
}

void VocoderEngine::setStrength (float newStrength)
{
    strength = juce::jlimit (0.0f, 1.0f, newStrength);
}

void VocoderEngine::rebuildBands()
{
    bands.clear();

    int   numberOfBands   = 0;
    float lowestFreq      = 0.0f;
    float highestFreq     = 0.0f;
    float qFactor         = 0.0f;
    float attackMs        = 0.0f;
    float releaseMs       = 0.0f;

    if (currentCharacter == Character::Classic)
    {
        // 16 bands, 80 Hz – 8 kHz.
        // Wider bands (lower Q) preserve the blunt, robotic character while
        // the extended upper range now captures S and T consonants.
        numberOfBands = 16;
        lowestFreq    = 80.0f;
        highestFreq   = 8000.0f;
        qFactor       = 2.5f;
        attackMs      = 2.0f;  // snappy — gives classic choppy articulation
        releaseMs     = 60.0f;
        sibilanceAmount = 0.25f;
    }
    else
    {
        // 24 bands, 60 Hz – 12 kHz.
        // Narrower bands give finer frequency resolution for a more natural,
        // intelligible result. Upper limit raised to 12 kHz for presence/air.
        numberOfBands = 24;
        lowestFreq    = 60.0f;
        highestFreq   = 12000.0f;
        qFactor       = 5.0f;
        attackMs      = 3.0f;  // slightly softer attack for smoother transitions
        releaseMs     = 80.0f;
        sibilanceAmount = 0.18f;
    }

    // Logarithmic band spacing — matches how the ear perceives pitch.
    const float logLow  = std::log10 (lowestFreq);
    const float logHigh = std::log10 (highestFreq);

    for (int i = 0; i < numberOfBands; ++i)
    {
        const float t    = float (i) / float (numberOfBands - 1);
        const float freq = std::pow (10.0f, logLow + t * (logHigh - logLow));

        VocoderBand band;
        band.prepare (sampleRate, freq, qFactor, attackMs, releaseMs);
        bands.push_back (std::move (band));
    }
}

void VocoderEngine::processBlock (juce::AudioBuffer<float>& voiceBuffer,
                                   const juce::AudioBuffer<float>& carrierBuffer)
{
    const int numChannels = voiceBuffer.getNumChannels();
    const int numSamples  = voiceBuffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto*       voiceData   = voiceBuffer.getWritePointer (channel);
        const int   carrierCh   = juce::jmin (channel, carrierBuffer.getNumChannels() - 1);
        const auto* carrierData = carrierBuffer.getReadPointer (carrierCh);

        for (int s = 0; s < numSamples; ++s)
        {
            const float dryVoice    = voiceData[s];
            const float carrierSamp = carrierData[s];

            // ── Sibilance detection (on dry voice before vocoding) ──────────
            const float hfVoice  = sibilanceVoiceHPF[channel].processSample (dryVoice);
            const float hfEnergy = std::abs (hfVoice);

            if (hfEnergy > sibilanceEnv[channel])
                sibilanceEnv[channel] = sibilanceAttackCoeff  * sibilanceEnv[channel]
                                      + (1.0f - sibilanceAttackCoeff)  * hfEnergy;
            else
                sibilanceEnv[channel] = sibilanceReleaseCoeff * sibilanceEnv[channel]
                                      + (1.0f - sibilanceReleaseCoeff) * hfEnergy;

            // ── Main vocoder bands ──────────────────────────────────────────
            float vocodedSample = 0.0f;
            for (auto& band : bands)
                vocodedSample += band.processSample (dryVoice, carrierSamp);

            // Strength blend (0 = dry voice, 1 = full vocoder)
            float output = dryVoice * (1.0f - strength) + vocodedSample * strength;

            // ── Sibilance injection ─────────────────────────────────────────
            // Advance xorshift32 noise generator (no heap alloc, no locks).
            noiseSeed ^= noiseSeed << 13;
            noiseSeed ^= noiseSeed >> 17;
            noiseSeed ^= noiseSeed << 5;
            const float rawNoise      = float (int32_t (noiseSeed)) * (1.0f / float (1u << 31));
            const float filteredNoise = sibilanceNoiseHPF[channel].processSample (rawNoise);

            output += filteredNoise * sibilanceEnv[channel] * sibilanceAmount;

            voiceData[s] = output;
        }
    }
}
