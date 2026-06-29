#include "PluginProcessor.h"
#include "PluginEditor.h"

ProjectPlugInAudioProcessor::ProjectPlugInAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Voice",    juce::AudioChannelSet::stereo(), true)
                        .withInput  ("Carrier",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output",   juce::AudioChannelSet::stereo(), true)),
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

    if (layouts.getChannelSet (true, 0) != juce::AudioChannelSet::stereo())  // Voice bus
        return false;

    if (layouts.getChannelSet (true, 1) != juce::AudioChannelSet::stereo())  // Carrier bus
        return false;

    return true;
}

void ProjectPlugInAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    // Read the current parameter values. These are updated automatically
    // by JUCE whenever the user moves a knob/switch in the editor, or
    // when Logic Pro automates them.
    const float strengthValue = parameters.getRawParameterValue (strengthParamID)->load();
    const int characterChoice = static_cast<int> (parameters.getRawParameterValue (characterParamID)->load());
    const float dryWetValue = parameters.getRawParameterValue (dryWetParamID)->load();
    const float outputGainValue = parameters.getRawParameterValue (outputGainParamID)->load();

    vocoderEngine.setStrength (strengthValue);
    vocoderEngine.setCharacter (characterChoice == 0 ? VocoderEngine::Character::Classic
                                                       : VocoderEngine::Character::Modern);
    outputStage.setDryWetMix (dryWetValue);
    outputStage.setOutputGainDb (outputGainValue);

    // `buffer` arrives containing ALL input buses joined together.
    // We use getBusBuffer to get a view of just the Voice bus (bus 0)
    // and just the Carrier bus (bus 1), without copying any audio data.
    auto voiceBlock = getBusBuffer (buffer, true, 0);
    auto carrierBlock = getBusBuffer (buffer, true, 1);

    // Keep an untouched copy of the original voice BEFORE the vocoder
    // changes it - the Dry/Wet control later needs this original
    // version to blend against.
    dryVoiceCopy.makeCopyOf (voiceBlock, true);

    vocoderEngine.processBlock (voiceBlock, carrierBlock);

    // Blend the original dry voice back in (per the Dry/Wet knob) and
    // apply final output gain. This writes the finished result
    // straight into voiceBlock, which is a view onto the actual output bus.
    outputStage.processBlock (dryVoiceCopy, voiceBlock);
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
