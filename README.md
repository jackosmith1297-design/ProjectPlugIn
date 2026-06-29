# Project PlugIn

A vocoder plugin for Logic Pro (and other DAWs via VST3), built with JUCE + C++20.

## Status: Vocoder DSP + output controls written, awaiting first build (blocked on Xcode)

The actual vocoder sound-processing logic is now written:

- `VocoderBand` - one frequency "slice" of the vocoder (filters + envelope follower)
- `VocoderEngine` - combines many bands, implements the Classic/Modern
  character switch, and the Strength blend
- `OutputStage` - final Dry/Wet blend and Output gain, applied after
  the vocoder, with smoothing to avoid clicks when knobs are turned
- `PluginProcessor` - wires everything together, exposes **Character**,
  **Strength**, **Dry/Wet**, and **Output** as real, host-automatable
  parameters, and accepts two inputs: **Voice** and **Carrier**
- `PluginEditor` - a plain (undesigned) window with working controls
  for all four parameters above

This has NOT been compiled or tested yet - that requires Xcode, which
isn't installed yet. Once Xcode is available, the next step is:

```
cmake -B build -G Xcode
cmake --build build
```

## Two vocoder characters

- **Classic** - 12 wider frequency bands, faster envelope follower =>
  blunter, more obviously robotic sound
- **Modern** - 24 narrower frequency bands, slower envelope follower =>
  smoother, more natural-sounding blend

## The four controls

- **Character** - Classic or Modern (see above)
- **Strength** - how aggressively the vocoder effect behaves internally
- **Dry/Wet** - final blend between original voice and vocoded result
- **Output** - final volume adjustment (-24dB to +12dB)

## How the inputs work in Logic Pro (once built)

This plugin will need TWO audio sources routed into it:
1. **Voice** - the vocal track (main input)
2. **Carrier** - a synth track, routed in as a side-chain/second input

## Requirements to build this project

- macOS with Xcode installed
- CMake (already installed)
- JUCE 8.0.14 (already included as a git submodule)
