#pragma once

#include "VocoderBand.h"
#include <vector>

// ============================================================================
// VocoderEngine
//
// PLAIN ENGLISH: This is the actual "brain" of the plugin. It owns a whole
// row of VocoderBand slices covering the full range of human voice
// frequencies, runs the voice and carrier signal through ALL of them, and
// adds the results together to produce the final vocoded sound.
//
// It also implements the "Character" switch (Classic vs Modern) by
// changing two things under the hood:
//   - How many bands are used, and how they're spaced
//   - How fast the envelope followers react
//
// From the musician's point of view, they just flip a switch and turn a
// Strength knob - all this complexity stays hidden.
// ============================================================================
class VocoderEngine
{
public:
    enum class Character
    {
        Classic,
        Modern
    };

    VocoderEngine();

    // Must be called once before playback starts (and again if the
    // sample rate ever changes).
    void prepare (double sampleRate);

    // Clears all internal state - called when playback stops.
    void reset();

    // Switches between the two vocoder personalities. Safe to call
    // while audio is playing.
    void setCharacter (Character newCharacter);

    // 0.0 = no vocoder effect at all (dry signal), 1.0 = full effect.
    // This maps directly to the "Strength" knob the musician sees.
    void setStrength (float newStrength);

    // Processes one block of audio.
    //   voiceBuffer   = the input microphone/vocal track
    //   carrierBuffer = the input synth/carrier track
    // The result is written back into voiceBuffer in place.
    void processBlock (juce::AudioBuffer<float>& voiceBuffer,
                        const juce::AudioBuffer<float>& carrierBuffer);

private:
    // Rebuilds the band layout based on the currently selected Character.
    // Called whenever the character changes.
    void rebuildBands();

    std::vector<VocoderBand> bands;

    Character currentCharacter { Character::Classic };
    float strength { 1.0f };
    double sampleRate { 44100.0 };

    // How quickly each band's envelope follower reacts. Set automatically
    // based on the selected Character (Classic = faster, Modern = slower).
    float followerSpeed { 0.5f };
};
