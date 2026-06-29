#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "VocoderEngine.h"
#include "OutputStage.h"

// This class is the "engine" of the plugin - it's what actually
// processes audio.
//
// IMPORTANT CONCEPT: a vocoder needs TWO incoming sounds to work:
//   1. The VOICE (e.g. a vocal track) - this provides the "shape"/rhythm
//   2. The CARRIER (e.g. a synth track) - this provides the actual tone
//      that gets reshaped into sounding like the voice
//
// In Logic Pro, the main input is the voice, and the carrier comes in
// via a second ("side-chain") input that your husband will route a
// synth track into.
class ProjectPlugInAudioProcessor : public juce::AudioProcessor
{
public:
    ProjectPlugInAudioProcessor();
    ~ProjectPlugInAudioProcessor() override;

    // Called once before playback starts. Used to get things ready.
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;

    // Called when playback stops.
    void releaseResources() override;

    // Checks whether a given input/output channel setup is supported.
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    // THE MOST IMPORTANT FUNCTION: called repeatedly while audio plays.
    // This is where the actual vocoder processing happens.
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // Creates the window/editor you see when you open the plugin.
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // Basic plugin info JUCE/Logic Pro needs.
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    // Preset/program handling - we are not using presets yet, so these
    // are minimal "do nothing" implementations for now.
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    // Saving/loading plugin state (so settings persist between sessions).
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // This holds all of the plugin's user-adjustable controls
    // (Character, Strength, and more added later). JUCE's
    // AudioProcessorValueTreeState handles host automation, undo,
    // saving/loading, and connecting to on-screen knobs/switches for us.
    juce::AudioProcessorValueTreeState parameters;

    // Parameter ID constants - used both here and later in the editor,
    // so we always refer to the same parameter by the same name.
    static constexpr const char* characterParamID = "character";
    static constexpr const char* strengthParamID = "strength";
    static constexpr const char* dryWetParamID = "dryWet";
    static constexpr const char* outputGainParamID = "outputGain";

private:
    // Builds the list of parameters this plugin exposes. Called once
    // from the constructor.
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // The actual DSP engines that do the processing, in order:
    // voice + carrier -> VocoderEngine -> OutputStage -> final output.
    VocoderEngine vocoderEngine;
    OutputStage outputStage;

    // A temporary holding space for the original, untouched voice
    // signal. We need this because VocoderEngine overwrites the voice
    // buffer in place with the vocoded result - but OutputStage's
    // Dry/Wet control later needs to compare against the ORIGINAL
    // dry voice, so we keep a copy of it before vocoding happens.
    juce::AudioBuffer<float> dryVoiceCopy;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectPlugInAudioProcessor)
};

