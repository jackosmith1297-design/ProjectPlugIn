#include "PluginProcessor.h"
#include "PluginEditor.h"

// PolyBLEP correction term for a band-limited sawtooth oscillator.
// Subtracting this from the naive (2*phase - 1) waveform removes the
// worst aliasing artefacts at the discontinuity, at negligible CPU cost.
static float polyBlep (float phase, float phaseInc) noexcept
{
    if (phase < phaseInc)
    {
        const float t = phase / phaseInc;
        return t + t - t * t - 1.0f;
    }
    if (phase > 1.0f - phaseInc)
    {
        const float t = (phase - 1.0f) / phaseInc;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

ProjectPlugInAudioProcessor::ProjectPlugInAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Voice",   juce::AudioChannelSet::stereo(), true)
                        .withInput  ("Carrier", juce::AudioChannelSet::stereo(), false) // optional, kept for Logic Pro bus compat
                        .withOutput ("Output",  juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

ProjectPlugInAudioProcessor::~ProjectPlugInAudioProcessor()
{
}

void ProjectPlugInAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    vocoderEngine.prepare (sampleRate);
    outputStage.prepare (sampleRate);

    // Make sure our "holding space" for the dry voice signal is the
    // right size to copy a full block of stereo audio into.
    dryVoiceCopy.setSize (2, samplesPerBlock);
    internalCarrier.setSize (2, samplesPerBlock);
    carrierPhase  = 0.0f;
    carrierPhase2 = 0.0f;
    noiseHPFIn    = 0.0f;
    noiseHPFOut   = 0.0f;
    // 1-pole HPF coefficient for ~1.5 kHz: alpha = 1 / (1 + 2π·fc/fs)
    noiseHPFAlpha = 1.0f / (1.0f + juce::MathConstants<float>::twoPi * 1500.0f / float (sampleRate));
}

void ProjectPlugInAudioProcessor::releaseResources()
{
}

bool ProjectPlugInAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // We need exactly: stereo Voice input, stereo Carrier input,
    // stereo output. This keeps things simple and predictable for now.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getChannelSet (true, 0) != juce::AudioChannelSet::stereo())  // Voice bus
        return false;

    // Carrier bus is optional — accept stereo or disabled
    const auto& carrier = layouts.getChannelSet (true, 1);
    if (!carrier.isDisabled() && carrier != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void ProjectPlugInAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    const float strengthValue   = parameters.getRawParameterValue (strengthParamID)->load();
    const int   characterChoice = static_cast<int> (parameters.getRawParameterValue (characterParamID)->load());
    const float pitchHz         = parameters.getRawParameterValue (pitchParamID)->load();
    const float dryWetValue     = parameters.getRawParameterValue (dryWetParamID)->load();
    const float outputGainValue = parameters.getRawParameterValue (outputGainParamID)->load();

    const bool isModern = (characterChoice != 0);
    vocoderEngine.setStrength (strengthValue);
    vocoderEngine.setCharacter (isModern ? VocoderEngine::Character::Modern
                                         : VocoderEngine::Character::Classic);
    outputStage.setDryWetMix (dryWetValue);
    outputStage.setOutputGainDb (outputGainValue);

    auto voiceBlock = getBusBuffer (buffer, true, 0);
    const int numSamples = voiceBlock.getNumSamples();

    // ── Generate internal carrier oscillator ──────────────────────────────
    // Classic: single band-limited (PolyBLEP) sawtooth — richer, cleaner buzz
    // Modern:  two detuned PolyBLEP saws + a small high-passed noise layer
    //          for added "air" and presence above the saw's harmonic range
    internalCarrier.setSize (2, numSamples, false, false, true);
    const float fs        = float (getSampleRate());
    const float phaseInc  = pitchHz / fs;
    const float phaseInc2 = (pitchHz * 1.006f) / fs; // slight detune for Modern

    for (int s = 0; s < numSamples; ++s)
    {
        carrierPhase += phaseInc;
        if (carrierPhase >= 1.0f) carrierPhase -= 1.0f;

        float sample;

        if (isModern)
        {
            carrierPhase2 += phaseInc2;
            if (carrierPhase2 >= 1.0f) carrierPhase2 -= 1.0f;

            // Two PolyBLEP saws, blended
            const float saw1 = (2.0f * carrierPhase  - 1.0f) - polyBlep (carrierPhase,  phaseInc);
            const float saw2 = (2.0f * carrierPhase2 - 1.0f) - polyBlep (carrierPhase2, phaseInc2);
            sample = saw1 * 0.6f + saw2 * 0.4f;

            // Small high-passed noise layer adds "air" above 1.5 kHz
            // where the saw's harmonics become sparse at low pitch settings.
            noiseRng ^= noiseRng << 13;
            noiseRng ^= noiseRng >> 17;
            noiseRng ^= noiseRng << 5;
            const float rawNoise = float (int32_t (noiseRng)) * (1.0f / float (1u << 31));
            noiseHPFOut = noiseHPFAlpha * (noiseHPFOut + rawNoise - noiseHPFIn);
            noiseHPFIn  = rawNoise;
            sample += noiseHPFOut * 0.08f; // 8 % noise blend
        }
        else
        {
            // Classic: single PolyBLEP sawtooth
            sample = (2.0f * carrierPhase - 1.0f) - polyBlep (carrierPhase, phaseInc);
        }

        internalCarrier.setSample (0, s, sample);
        internalCarrier.setSample (1, s, sample);
    }

    // ── Vocode: shape the internal carrier with the voice's envelope ──────
    dryVoiceCopy.makeCopyOf (voiceBlock, true);
    vocoderEngine.processBlock (voiceBlock, internalCarrier);
    outputStage.processBlock (dryVoiceCopy, voiceBlock);

    // ── VU meter ──────────────────────────────────────────────────────────
    float peak = 0.0f;
    for (int ch = 0; ch < voiceBlock.getNumChannels(); ++ch)
        peak = std::max (peak, voiceBlock.getMagnitude (ch, 0, numSamples));
    const float dB = peak > 1e-6f ? 20.0f * std::log10 (peak) : -60.0f;
    outputLevelDb.store (juce::jlimit (-60.0f, 0.0f, dB));
}

juce::AudioProcessorEditor* ProjectPlugInAudioProcessor::createEditor()
{
    return new ProjectPlugInAudioProcessorEditor (*this);
}

bool ProjectPlugInAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String ProjectPlugInAudioProcessor::getName() const
{
    return "Project PlugIn";
}

bool ProjectPlugInAudioProcessor::acceptsMidi() const
{
    return false;
}

bool ProjectPlugInAudioProcessor::producesMidi() const
{
    return false;
}

double ProjectPlugInAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ProjectPlugInAudioProcessor::getNumPrograms()
{
    return 1;
}

int ProjectPlugInAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ProjectPlugInAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const juce::String ProjectPlugInAudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void ProjectPlugInAudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/)
{
}

void ProjectPlugInAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Saves all current knob/switch positions, so they're restored
    // correctly when the Logic Pro project is reopened later.
    if (auto state = parameters.copyState(); state.isValid())
    {
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
    }
}

void ProjectPlugInAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout ProjectPlugInAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> paramList;

    // CHARACTER: a simple two-way choice between our two vocoder
    // personalities. Stored as a choice parameter so it shows up as a
    // dropdown/switch rather than a continuous knob.
    paramList.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { characterParamID, 1 },
        "Character",
        juce::StringArray { "Classic", "Modern" },
        0)); // Default to "Classic" (index 0)

    // STRENGTH: how much of the vocoder effect is applied, from
    // completely dry voice (0%) to fully vocoded (100%).
    paramList.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { strengthParamID, 1 },
        "Strength",
        juce::NormalisableRange<float> (0.0f, 1.0f),
        1.0f)); // Default to 100% - full vocoder effect

    // PITCH: the frequency of the internal carrier oscillator in Hz.
    // This controls how high or low the robot voice sounds. The range
    // is skewed so the knob feels natural — most useful values (80–200 Hz)
    // sit comfortably in the first half of the knob travel.
    {
        auto pitchRange = juce::NormalisableRange<float> (50.0f, 400.0f);
        pitchRange.setSkewForCentre (130.0f);
        paramList.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { pitchParamID, 1 },
            "Pitch",
            pitchRange,
            120.0f)); // Default 120 Hz — a natural-sounding robot pitch
    }

    // DRY/WET: final blend between the original voice and the vocoded
    // result. Separate from Strength - see OutputStage.h for why.
    paramList.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { dryWetParamID, 1 },
        "Dry/Wet",
        juce::NormalisableRange<float> (0.0f, 1.0f),
        1.0f)); // Default to 100% wet (fully vocoded)

    // OUTPUT GAIN: final volume adjustment, in decibels. Range chosen
    // to allow meaningful boost/cut without being excessive:
    // -24dB (much quieter) up to +12dB (notably louder).
    paramList.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { outputGainParamID, 1 },
        "Output",
        juce::NormalisableRange<float> (-24.0f, 12.0f),
        0.0f)); // Default to 0dB - no change

    return { paramList.begin(), paramList.end() };
}

// This is required by JUCE: it's the function that creates an instance
// of our processor. Every JUCE plugin needs exactly one of these.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProjectPlugInAudioProcessor();
}
