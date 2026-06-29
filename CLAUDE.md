# Project PlugIn — Working Notes for Claude

## Who I am, and how to work with me

- I have ADHD and find dense technical jargon hard to follow. Keep explanations
  short, plain-English, step-by-step. Use everyday words and short sentences.
  Analogies are welcome if they make something click faster.
- I have no coding background. I work in 1st-line IT support, so I'm
  comfortable in a terminal and clicking around, but I don't know C++ or JUCE.
- Don't over-explain by default. Move fast, just build things, and only stop
  to explain when it's a decision that actually matters or I ask. I'd rather
  you just do the thing than narrate theory first.
- When you want me to run something or check something, give me the exact
  command to copy-paste, not a description of what to type.
- If something might be ambiguous, make a reasonable call yourself and tell me
  what you assumed, rather than stopping to ask, unless it's a big decision.
- I will be minimal/short in my replies sometimes — that's just me being
  efficient, not being short with you.
- When things go wrong, verify each step directly (e.g. checking file
  contents, checking installed binaries) rather than guessing or assuming a
  fix worked. We've had repeated issues this project where a fix was assumed
  to have worked, but the underlying file/setting never actually changed —
  always verify with a direct check (grep, file size, strings, plutil, etc)
  before moving on or declaring something fixed.

## What this project is

A vocoder plugin for Logic Pro (AU + VST3), built with JUCE + C++20, CMake,
Xcode. My husband is an independent music artist; the goal is a genuinely
good, original vocoder plugin so he doesn't have to buy expensive ones.

Two "characters":
- **Classic** — old-school, obviously robotic vocoder sound. Visually: dark
  wood-grain border, perforated metal grille panel, amber accents.
- **Modern** — smoother, more natural/contemporary vocoder sound. Visually:
  light, glassy, brushed-aluminium panel, luminous pastel accents
  (lavender/mint/sky/peach/pink/lilac).

Both share Strength, Dry/Wet, and Output controls.

Company/brand name shown in the plugin: **Smith-Pye Audio**. Plugin display
name: **Project PlugIn**. There's a subtle handwritten-script signature
("Smith–Pye") in the corner of the GUI — keep it small and tasteful, not a
branding element.

## Current state (update this section as things progress)

- Full DSP signal chain is written and has compiled successfully:
  `VocoderBand`, `VocoderEngine` (Classic/Modern), `OutputStage` (Dry/Wet +
  Output gain), wired into `PluginProcessor` with real automatable parameters
  (Character, Strength, Dry/Wet, Output) and two input buses (Voice, Carrier).
- A full HTML/CSS mockup of the intended GUI exists at `mockup/gui_mockup.html`
  — this is the visual source of truth for what the real JUCE GUI should look
  like (wood/grille Classic, glassy pastel Modern, VU meter, rotary knobs,
  toggle switch, status sticker, signature).
- "Layer 1" of translating that mockup into real JUCE C++ is done:
  `PluginEditor.cpp` draws the wood border / perforated grille (Classic) and
  glassy light panel (Modern) backgrounds, with a timer that repaints when
  Character changes. The actual controls (dropdown, sliders) are still
  JUCE's plain default widgets — NOT yet styled to match the mockup (no
  rotary knobs, no toggle switch, no VU meter yet). That's the next layer
  of work.
- Known currently-unresolved issue: Logic Pro / macOS keeps showing the
  plugin's manufacturer as the old placeholder "YourCompanyName" instead of
  "Smith-Pye Audio", even though the actual installed binary and Info.plist
  on disk are confirmed correct. We were mid-troubleshooting this (tried
  Reset & Rescan, Full Audio Unit Reset, neither worked yet; suspect Loopcloud
  may be interfering, was about to search the whole disk for stray copies of
  the .component file). Pick this up before doing anything else GUI-related.

## House rules for how we build

- Build in small, testable layers/milestones. Don't do a giant rewrite in one
  go. After each real chunk of work, it should still compile and load in
  Logic Pro before moving on.
- Never allocate memory on the audio thread. No locking on the audio thread.
- Real-time audio safety and clean modern C++20 matters more than cleverness.
- After making a change, verify the actual file on disk reflects it (don't
  just trust that an edit landed) before rebuilding, and verify the actual
  build artifact contains the change (e.g. `strings` search) before testing
  in Logic Pro, especially if something seems not to have taken effect.
- The GitHub repo (jackosmith1297-design/ProjectPlugIn) is the safe backup —
  commit and push at the end of any real chunk of work.
