#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// Forward-declared here; full definitions live at the top of PluginEditor.cpp.
class PluginLookAndFeel;
class CharacterToggle;
class VuMeter;

class ProjectPlugInAudioProcessorEditor : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    explicit ProjectPlugInAudioProcessorEditor (ProjectPlugInAudioProcessor&);
    ~ProjectPlugInAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    void paintClassicBackground (juce::Graphics&);
    void paintModernBackground  (juce::Graphics&);
    void paintSections          (juce::Graphics&);
    void paintHeader            (juce::Graphics&);
    void paintFooter            (juce::Graphics&);
    void paintKnobLabels        (juce::Graphics&);
    void paintIoRow             (juce::Graphics&);
    void paintStatusSticker     (juce::Graphics&);
    void paintHoloStickers      (juce::Graphics&);

    bool isClassicMode() const;

    ProjectPlugInAudioProcessor& audioProcessor;
    bool wasClassicOnLastPaint { true };

    std::unique_ptr<PluginLookAndFeel> laf;

    // Character toggle switch (replaces old ComboBox)
    std::unique_ptr<CharacterToggle> characterToggle;

    // VU meter
    std::unique_ptr<VuMeter> vuMeter;

    // Rotary knobs (no text boxes — labels drawn in paint())
    juce::Slider strengthKnob  { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    juce::Slider pitchKnob     { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    juce::Slider dryWetKnob    { juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };
    juce::Slider outputGainKnob{ juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox };

    // Stored section bounds — computed in resized(), used in paint()
    juce::Rectangle<int> signalSectionBounds;
    juce::Rectangle<int> characterSectionBounds;
    juce::Rectangle<int> controlsSectionBounds;
    juce::Rectangle<int> ioRowBounds;

    // Knob centre positions for label painting
    juce::Point<int> strengthKnobCentre;
    juce::Point<int> pitchKnobCentre;
    juce::Point<int> dryWetKnobCentre;
    juce::Point<int> outputGainKnobCentre;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> strengthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectPlugInAudioProcessorEditor)
};
