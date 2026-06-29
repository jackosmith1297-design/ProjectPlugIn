cmake_minimum_required(VERSION 3.22)

project(ProjectPlugIn VERSION 0.1.0)

# We need JUCE itself. This expects a folder called "JUCE" to exist
# one level above this project (we'll fetch it via git submodule - 
# instructions for this come later, don't worry about it yet).
add_subdirectory(JUCE)

# This declares the actual plugin: its name, what formats it builds as,
# and basic identifying info that Logic Pro / macOS use to recognise it.
juce_add_plugin(ProjectPlugIn
    COMPANY_NAME "YourCompanyName"
    BUNDLE_ID "com.yourcompany.projectplugin"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Ycmp
    PLUGIN_CODE Plg1
    FORMATS AU VST3
    PRODUCT_NAME "Project PlugIn"
)

# Tells the compiler to use the C++20 language standard, as planned.
target_compile_features(ProjectPlugIn PRIVATE cxx_std_20)

# List of our own source code files. We will add more to this list
# as we build new parts of the plugin.
target_sources(ProjectPlugIn PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/VocoderBand.cpp
    Source/VocoderEngine.cpp
    Source/OutputStage.cpp
)

# Standard JUCE modules (pre-built building blocks) every audio plugin needs.
target_link_libraries(ProjectPlugIn PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

target_compile_definitions(ProjectPlugIn PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
