#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide pkginfo AU /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_AU/PkgInfo
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide pkginfo AU /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_AU/PkgInfo
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide pkginfo AU /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_AU/PkgInfo
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/JUCE/tools/extras/Build/juceaide/juceaide_artefacts/Custom/juceaide pkginfo AU /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/ProjectPlugIn_artefacts/JuceLibraryCode/ProjectPlugIn_AU/PkgInfo
fi

