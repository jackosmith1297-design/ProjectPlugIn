#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// This class is the "dashboard" - the window you actually see and click
// on inside Logic Pro. This is a deliberately plain/functional first
// version: a switch for Character, a slider for Strength. No visual
// design has been applied yet - that comes later, once we know the
// controls actually work correctly.
class ProjectPlugInAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit ProjectPlugInAudioProcessorEditor (ProjectPlugInAudioProcessor&);
    ~ProjectPlugInAudioProcessorEditor() override;

    // Draws the window's contents.
    void paint (juce::Graphics&) override;

    // Called whenever the window is resized, to reposition controls.
    void resized() override;

private:
    // A reference back to the engine, so the dashboard can read/control it.
    ProjectPlugInAudioProcessor& audioProcessor;

    // The actual on-screen controls.
    juce::ComboBox characterSelector;
    juce::Slider strengthSlider;
    juce::Slider dryWetSlider;
    juce::Slider outputGainSlider;

    juce::Label characterLabel;
    juce::Label strengthLabel;
    juce::Label dryWetLabel;
    juce::Label outputGainLabel;

    // These "attachments" are what link an on-screen control directly
    // to a plugin parameter - JUCE handles keeping them in sync in both
    // directions (turning the knob updates the parameter, and the
    // parameter changing - e.g. via host automation - updates the knob).
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> characterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> strengthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectPlugInAudioProcessorEditor)
};
