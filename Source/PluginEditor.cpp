#include "PluginProcessor.h"
#include "PluginEditor.h"

ProjectPlugInAudioProcessorEditor::ProjectPlugInAudioProcessorEditor (ProjectPlugInAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // --- Character control (Classic / Modern switch) ---
    characterSelector.addItem ("Classic", 1); // JUCE ComboBox IDs start at 1, not 0
    characterSelector.addItem ("Modern", 2);
    addAndMakeVisible (characterSelector);

    characterLabel.setText ("Character", juce::dontSendNotification);
    characterLabel.attachToComponent (&characterSelector, false);
    addAndMakeVisible (characterLabel);

    // This attachment links the dropdown directly to the "character"
    // parameter we defined in PluginProcessor - no manual wiring needed.
    characterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.parameters, ProjectPlugInAudioProcessor::characterParamID, characterSelector);

    // --- Strength control (0-100% slider) ---
    strengthSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    strengthSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible (strengthSlider);

    strengthLabel.setText ("Strength", juce::dontSendNotification);
    strengthLabel.attachToComponent (&strengthSlider, false);
    addAndMakeVisible (strengthLabel);

    strengthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, ProjectPlugInAudioProcessor::strengthParamID, strengthSlider);

    // --- Dry/Wet control (0-100% slider) ---
    dryWetSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    dryWetSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible (dryWetSlider);

    dryWetLabel.setText ("Dry/Wet", juce::dontSendNotification);
    dryWetLabel.attachToComponent (&dryWetSlider, false);
    addAndMakeVisible (dryWetLabel);

    dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, ProjectPlugInAudioProcessor::dryWetParamID, dryWetSlider);

    // --- Output Gain control (-24dB to +12dB slider) ---
    outputGainSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    outputGainSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible (outputGainSlider);

    outputGainLabel.setText ("Output", juce::dontSendNotification);
    outputGainLabel.attachToComponent (&outputGainSlider, false);
    addAndMakeVisible (outputGainLabel);

    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, ProjectPlugInAudioProcessor::outputGainParamID, outputGainSlider);

    setSize (400, 400);
}

ProjectPlugInAudioProcessorEditor::~ProjectPlugInAudioProcessorEditor()
{
}

void ProjectPlugInAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkslategrey);
}

void ProjectPlugInAudioProcessorEditor::resized()
{
    // Simple, functional layout for now - just stacking the controls
    // vertically with some breathing room. Real visual design comes later.
    auto area = getLocalBounds().reduced (20);

    area.removeFromTop (20); // space for the label above the first control
    characterSelector.setBounds (area.removeFromTop (30));

    area.removeFromTop (30); // space for the label above the next control
    strengthSlider.setBounds (area.removeFromTop (30));

    area.removeFromTop (30);
    dryWetSlider.setBounds (area.removeFromTop (30));

    area.removeFromTop (30);
    outputGainSlider.setBounds (area.removeFromTop (30));
}
