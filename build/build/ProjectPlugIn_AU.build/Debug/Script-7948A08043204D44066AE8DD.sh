#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E copy /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_AU/PkgInfo "/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/AU/Project PlugIn.component/Contents"
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/AU/Project PlugIn.component" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Debug/AU/Project PlugIn.component" -Ddest=/Users/cameronpye/Library/Audio/Plug-Ins/Components -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E copy /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_AU/PkgInfo "/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/AU/Project PlugIn.component/Contents"
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/AU/Project PlugIn.component" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/Release/AU/Project PlugIn.component" -Ddest=/Users/cameronpye/Library/Audio/Plug-Ins/Components -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E copy /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_AU/PkgInfo "/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/AU/Project PlugIn.component/Contents"
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/AU/Project PlugIn.component" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/MinSizeRel/AU/Project PlugIn.component" -Ddest=/Users/cameronpye/Library/Audio/Plug-Ins/Components -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake -E copy /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_AU/PkgInfo "/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/AU/Project PlugIn.component/Contents"
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/AU/Project PlugIn.component" -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /opt/homebrew/bin/cmake "-Dsrc=/Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/RelWithDebInfo/AU/Project PlugIn.component" -Ddest=/Users/cameronpye/Library/Audio/Plug-Ins/Components -P /Users/cameronpye/Downloads/ProjectPlugIn_update2/JUCE/extras/Build/CMake/copyDir.cmake
fi

