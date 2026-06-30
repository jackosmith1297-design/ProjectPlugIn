#include "VocoderBand.h"

VocoderBand::VocoderBand() {}

void VocoderBand::prepare (double sampleRate, float centreFrequencyHz, float qFactor,
                            float attackMs, float releaseMs)
{
    currentSampleRate = sampleRate;
    centreFrequency   = centreFrequencyHz;
    qualityFactor     = qFactor;

    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass (
        currentSampleRate, centreFrequency, qualityFactor);

    voiceFilter.coefficients   = coefficients;
    carrierFilter.coefficients = coefficients;

    // Convert milliseconds to per-sample smoothing coefficients.
    // Formula: exp(-1 / (sampleRate * timeInSeconds))
    // A coefficient close to 1 means slow (heavy smoothing).
    // A coefficient close to 0 means fast (little smoothing).
    attackCoeff  = std::exp (-1.0f / float (sampleRate * attackMs  * 0.001));
    releaseCoeff = std::exp (-1.0f / float (sampleRate * releaseMs * 0.001));

    reset();
}

void VocoderBand::reset()
{
    voiceFilter.reset();
    carrierFilter.reset();
    envelopeValue = 0.0f;
}

float VocoderBand::processSample (float voiceSample, float carrierSample)
{
    // 1. Isolate this frequency slice from the voice.
    const float filteredVoice   = voiceFilter.processSample (voiceSample);
    const float instantLoudness = std::abs (filteredVoice);

    // 2. Asymmetric envelope follower.
    //    Attack path: follows rising energy quickly (catches consonants).
    //    Release path: falls slowly (avoids choppy dropouts between words).
    if (instantLoudness > envelopeValue)
        envelopeValue = attackCoeff  * envelopeValue + (1.0f - attackCoeff)  * instantLoudness;
    else
        envelopeValue = releaseCoeff * envelopeValue + (1.0f - releaseCoeff) * instantLoudness;

    // 3. Isolate the same frequency slice from the carrier.
    const float filteredCarrier = carrierFilter.processSample (carrierSample);

    // 4. Stamp the voice envelope onto the carrier slice.
    return filteredCarrier * envelopeValue;
}
