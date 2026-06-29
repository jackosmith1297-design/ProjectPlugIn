#include "VocoderEngine.h"
#include <cmath>

VocoderEngine::VocoderEngine()
{
}

void VocoderEngine::prepare (double newSampleRate)
{
    sampleRate = newSampleRate;
    rebuildBands();
}

void VocoderEngine::reset()
{
    for (auto& band : bands)
        band.reset();
}

void VocoderEngine::setCharacter (Character newCharacter)
{
    if (newCharacter == currentCharacter)
        return; // Nothing to do - avoid unnecessary rebuilding.

    currentCharacter = newCharacter;
    rebuildBands();
}

void VocoderEngine::setStrength (float newStrength)
{
    // Keep the value safely between 0 and 1, in case something further
    // up the chain sends us an out-of-range number.
    strength = juce::jlimit (0.0f, 1.0f, newStrength);
}

void VocoderEngine::rebuildBands()
{
    // PLAIN ENGLISH: this function decides HOW MANY frequency slices we
    // use, WHERE they sit, and HOW FAST they react - and that's the
    // entire difference between "Classic" and "Modern" under the hood.

    bands.clear();

    int numberOfBands = 0;
    float lowestFrequency = 0.0f;
    float highestFrequency = 0.0f;

    if (currentCharacter == Character::Classic)
    {
        // Classic: fewer, wider bands. This is less precise at tracking
        // the voice, which is exactly what gives old-school vocoders
        // their blunter, more obviously "robotic" character.
        numberOfBands = 12;
        lowestFrequency = 80.0f;
        highestFrequency = 4000.0f;

        // Faster envelope follower = snappier, choppier response.
        followerSpeed = 0.7f;
    }
    else // Character::Modern
    {
        // Modern: more, narrower bands. This tracks the voice's
        // frequency content far more precisely, giving a smoother,
        // more natural-sounding result.
        numberOfBands = 24;
        lowestFrequency = 60.0f;
        highestFrequency = 8000.0f;

        // Slower envelope follower = smoother, less choppy response.
        followerSpeed = 0.35f;
    }

    // We space the bands out logarithmically (rather than evenly),
    // because human hearing perceives frequency that way too - e.g. the
    // jump from 100Hz to 200Hz sounds like a similarly-sized "step" as
    // the jump from 1000Hz to 2000Hz, even though the second gap is far
    // bigger in raw numbers. Spacing bands evenly in raw Hz would waste
    // most of them on frequencies too high to matter for voice.
    const float logLow = std::log10 (lowestFrequency);
    const float logHigh = std::log10 (highestFrequency);

    for (int i = 0; i < numberOfBands; ++i)
    {
        const float t = static_cast<float> (i) / static_cast<float> (numberOfBands - 1);
        const float logFrequency = logLow + t * (logHigh - logLow);
        const float frequency = std::pow (10.0f, logFrequency);

        // Narrower bands (higher Q) for Modern, since we have more of
        // them and want each to be more precise. Wider bands for
        // Classic, matching its blunter character.
        const float qFactor = (currentCharacter == Character::Classic) ? 3.0f : 6.0f;

        VocoderBand band;
        band.prepare (sampleRate, frequency, qFactor);
        bands.push_back (std::move (band));
    }
}

void VocoderEngine::processBlock (juce::AudioBuffer<float>& voiceBuffer,
                                   const juce::AudioBuffer<float>& carrierBuffer)
{
    const int numChannels = voiceBuffer.getNumChannels();
    const int numSamples = voiceBuffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* voiceData = voiceBuffer.getWritePointer (channel);

        // If the carrier doesn't have as many channels as the voice
        // (e.g. carrier is mono, voice is stereo), we just reuse its
        // first channel rather than crashing.
        const int carrierChannel = juce::jmin (channel, carrierBuffer.getNumChannels() - 1);
        const auto* carrierData = carrierBuffer.getReadPointer (carrierChannel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float dryVoice = voiceData[sample];
            const float carrierSample = carrierData[sample];

            // Add up every band's contribution to build the full vocoded sound.
            float vocodedSample = 0.0f;
            for (auto& band : bands)
                vocodedSample += band.processSample (dryVoice, carrierSample, followerSpeed);

            // Blend between the original dry voice and the fully
            // vocoded sound, based on the Strength knob. At
            // strength = 0 you hear just the original voice; at
            // strength = 1 you hear the full vocoder effect.
            voiceData[sample] = (dryVoice * (1.0f - strength)) + (vocodedSample * strength);
        }
    }
}
