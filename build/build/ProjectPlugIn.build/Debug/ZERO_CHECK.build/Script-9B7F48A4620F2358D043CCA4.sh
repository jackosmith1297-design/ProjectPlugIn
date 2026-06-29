#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  make -f /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  make -f /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  make -f /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/cameronpye/Downloads/ProjectPlugIn_update2/build
  make -f /Users/cameronpye/Downloads/ProjectPlugIn_update2/build/CMakeScripts/ReRunCMake.make
fi

