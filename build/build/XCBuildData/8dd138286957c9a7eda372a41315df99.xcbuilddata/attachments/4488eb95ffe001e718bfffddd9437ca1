#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E copy /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_VST3/PkgInfo "/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/VST3/Project PlugIn.vst3/Contents"
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/VST3/Project PlugIn.vst3" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/VST3/Project\ PlugIn.vst3/Contents/moduleinfo.json /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E echo creating\ /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  /opt/homebrew/bin/cmake -E make_directory /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/VST3/Project\ PlugIn.vst3/Contents/Resources
  /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/vst3_helpers/ProjectPlugIn/vst3_helper > /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/VST3/Project PlugIn.vst3" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/VST3/Project PlugIn.vst3" -Ddest=/Users/cameronpye/Library/Audio/Plug-Ins/VST3 -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E copy /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_VST3/PkgInfo "/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/VST3/Project PlugIn.vst3/Contents"
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/VST3/Project PlugIn.vst3" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/VST3/Project\ PlugIn.vst3/Contents/moduleinfo.json /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E echo creating\ /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  /opt/homebrew/bin/cmake -E make_directory /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/VST3/Project\ PlugIn.vst3/Contents/Resources
  /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/vst3_helpers/ProjectPlugIn/vst3_helper > /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/VST3/Project PlugIn.vst3" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/VST3/Project PlugIn.vst3" -Ddest=/Users/cameronpye/Library/Audio/Plug-Ins/VST3 -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E copy /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_VST3/PkgInfo "/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/VST3/Project PlugIn.vst3/Contents"
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/VST3/Project PlugIn.vst3" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/VST3/Project\ PlugIn.vst3/Contents/moduleinfo.json /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E echo creating\ /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  /opt/homebrew/bin/cmake -E make_directory /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/VST3/Project\ PlugIn.vst3/Contents/Resources
  /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/vst3_helpers/ProjectPlugIn/vst3_helper > /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/VST3/Project PlugIn.vst3" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/VST3/Project PlugIn.vst3" -Ddest=/Users/cameronpye/Library/Audio/Plug-Ins/VST3 -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E copy /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_VST3/PkgInfo "/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/VST3/Project PlugIn.vst3/Contents"
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/VST3/Project PlugIn.vst3" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/VST3/Project\ PlugIn.vst3/Contents/moduleinfo.json /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E echo creating\ /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  /opt/homebrew/bin/cmake -E make_directory /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/VST3/Project\ PlugIn.vst3/Contents/Resources
  /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/vst3_helpers/ProjectPlugIn/vst3_helper > /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/VST3/Project\ PlugIn.vst3/Contents/Resources/moduleinfo.json
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/VST3/Project PlugIn.vst3" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/VST3/Project PlugIn.vst3" -Ddest=/Users/cameronpye/Library/Audio/Plug-Ins/VST3 -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/copyDir.cmake
fi

