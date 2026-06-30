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

Both share Strength, Pitch, Dry/Wet, and Output controls.

Company/brand name shown in the plugin: **Smith-Pye Audio**. Plugin display
name: **Project PlugIn**. There's a subtle handwritten-script signature
("Smith–Pye") in the corner of the GUI — keep it small and tasteful, not a
branding element.

## Current state (updated 2026-06-30)

### DSP / Processor — CONFIRMED WORKING IN LOGIC PRO
- Single Voice input bus. A Carrier bus exists but is marked optional and
  ignored — the plugin generates its own internal carrier oscillator, so no
  sidechain or MIDI is needed. Just drop it on a vocal track.
- **Internal carrier:** Classic = single PolyBLEP band-limited sawtooth.
  Modern = two slightly-detuned PolyBLEP saws + 8% high-passed noise layer
  for added presence/air. PolyBLEP removes the harshest aliasing artefacts
  from the naive sawtooth.
- **Parameters (all automatable):** Character, Strength, Pitch (50–400 Hz,
  skewed so 130 Hz is mid-knob), Dry/Wet, Output gain.
- **VocoderEngine — DSP improvements as of 2026-06-30:**
  - Classic: 16 bands, 80 Hz–8 kHz, Q=2.5, attack=2 ms, release=60 ms
  - Modern:  24 bands, 60 Hz–12 kHz, Q=5.0, attack=3 ms, release=80 ms
  - Asymmetric envelope follower (separate attack/release) in every band —
    catches consonants fast, releases smoothly.
  - Sibilance enhancement path: 4 kHz HPF detects S/T/F/SH energy in the
    dry voice; injects proportional high-passed noise into the output.
    Amount: 25% Classic, 18% Modern.
- Peak output level exposed as `std::atomic<float> outputLevelDb` for VU meter.
- AU validation: passes `auval -v aufx Plg2 SmPy`. Logic Pro loads correctly.

### GUI — COMPLETE AND CONFIRMED WORKING
- Full custom GUI matching `mockup/gui_mockup.html` (visual source of truth).
- `PluginLookAndFeel`: two-layer rotary knobs — outer rim linear-gradient
  (155°), inner dial radial-gradient at 38%/32% highlight, PolyBLEP-aware
  needle with per-mode glow (amber Classic; mint/sky/peach Modern), ambient
  glow pool below each knob.
- Four knobs in Controls section (80px, 4-column): STRENGTH / PITCH /
  DRY-WET / OUTPUT.
- `CharacterToggle`, `VuMeter` (28-LED bar reading live audio level),
  styled header, footer with power LED + Snell Roundhand script signature,
  section panels, IO row, status sticker, holographic stickers.

### Plugin identity
- Manufacturer: Smith-Pye Audio. Bundle: com.smithpyeaudio.projectplugin.
- AU codes: aufx / Plg2 / SmPy. DO NOT change these — every change forces
  Logic Pro to treat it as a new plugin, losing saved project state.
- Installed to: `~/Library/Audio/Plug-Ins/Components/Project PlugIn.component`

### Known gaps / next steps
- Output level normalisation — vocoded signal level is unpredictable across
  pitch/character settings; needs makeup gain after band summation.
- VU meter has no peak-hold or falloff animation (shows raw live level only).
- No preset system yet (important for commercial release).
- Release build not yet made — all testing on Debug builds.
- Apple Developer signing not yet set up (needs £79/year account).

### RESOLVED: Logic Pro manufacturer name
- RESOLVED: Logic Pro was persistently showing the plugin's manufacturer as
  the old placeholder "YourCompanyName" instead of "Smith-Pye Audio", even
  though the installed binary/Info.plist on disk were confirmed correct, and
  even after Reset & Rescan Selection and Full Audio Unit Reset in Logic's
  Plug-in Manager. Root cause was never fully confirmed, but the fix that
  worked: changed PLUGIN_MANUFACTURER_CODE (was "Ycmp") and PLUGIN_CODE (was
  "Plg1") in CMakeLists.txt to new values ("SmPy" / "Plg2"), forcing Logic to
  treat it as a genuinely new, never-seen plugin identity rather than trying
  to update/rescan its old cached record of the previous identity. This
  worked immediately after a clean rebuild. LESSON: if Logic/macOS ever shows
  stale plugin metadata again after a legitimate source change, changing the
  manufacturer/plugin codes is a faster, more reliable fix than fighting the
  cache via Reset & Rescan or Full Audio Unit Reset.
- Current PLUGIN_MANUFACTURER_CODE / PLUGIN_CODE: "SmPy" / "Plg2". DO NOT
  change these again now that real testing has started, except deliberately
  and with the same understanding as above — every change forces Logic to
  treat it as a new plugin (losing any saved automation/state tied to the
  old identity in any test projects).
- A ruled-out theory, for the record: suspected Loopcloud (an old,
  no-longer-used app) might have installed a stray duplicate .component
  file somewhere. Did a full disk search (`sudo find / -iname "Project
  PlugIn.component"`) and confirmed only one real installed copy exists, at
  the standard `~/Library/Audio/Plug-Ins/Components/` location (plus the
  expected build-output copy inside the project's own build folder). Not the
  cause of anything — don't re-investigate this.

## Build/test loop reminders specific to this project

- Xcode, CMake, and JUCE 8.0.14 (as a git submodule) are all installed and
  working on this Mac. The standard rebuild-and-test loop is:
  ```
  cd ~/Downloads/ProjectPlugIn
  cmake --build build
  ```
  (only `rm -rf build && cmake -B build -G Xcode` again if something
  structural changed, e.g. CMakeLists.txt edits, new source files added,
  or plugin codes changed — a plain `cmake --build build` is enough for
  most source file edits)
- ALWAYS fully quit Logic Pro (Cmd+Q) before testing a rebuilt plugin, then
  reopen it and start a NEW empty project rather than reusing an old one,
  to avoid any risk of Logic showing stale cached state.
- To verify a code change actually reached the compiled plugin (don't just
  trust the build succeeded): search the real installed binary for a known
  string from the new code, e.g.
  ```
  strings "/Users/cameronpye/Library/Audio/Plug-Ins/Components/Project PlugIn.component/Contents/MacOS/Project PlugIn" | grep -i "SOME_KNOWN_STRING"
  ```
- To check the plugin's actual installed metadata directly (bypassing any
  Logic Pro UI caching question entirely):
  ```
  plutil -p "/Users/cameronpye/Library/Audio/Plug-Ins/Components/Project PlugIn.component/Contents/Info.plist"
  ```

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


## Future goal: selling this plugin online

This plugin is being built with eventual commercial sale in mind — not just
personal use. This doesn't change how we build day-to-day, but it means
certain decisions should be made correctly from the start rather than
retrofitted later. Keep this context in mind when making architectural or
design decisions.

**What this means practically while building:**
- Design any new features (presets, settings, installer) as if a stranger
  with no context will use them, not just Cameron's husband who knows the
  project well
- Keep the codebase clean and documented enough that future maintenance
  (bug fixes after release, future updates) is manageable
- Don't hardcode anything that would need changing per-user or per-machine
- The GUI, branding (Smith-Pye Audio, Project PlugIn), and dual-character
  concept are all intentional commercial decisions — treat them as such

**Things to build now because they're harder to add later:**
- **Preset system** — factory presets are essential for commercial plugins;
  strangers need to get results immediately without knowing the plugin.
  Build a basic preset save/load system as part of the plugin proper, not
  as an afterthought. Include at least a handful of named factory presets
  covering both Classic and Modern characters at different Strength settings.
- **Installer** — the current build auto-copies to the right folder on
  Cameron's Mac (COPY_PLUGIN_AFTER_BUILD TRUE in CMakeLists). For end users,
  a proper double-clickable .pkg installer is needed. Use the free Mac app
  "Packages" to build this. Design for it from the start.

**Things that need money — NOT blocking current development:**
- **Apple Developer Account (£79/year)** — required for proper code signing
  and notarisation so the plugin isn't blocked by Gatekeeper on other Macs.
  Cameron cannot afford this right now and will purchase it when ready.
  Until then, continue building and testing on Cameron's own Mac using the
  current ad-hoc signing (which works fine locally). Do NOT block any
  feature work waiting for this — just flag clearly in any installer/
  distribution work that "this step requires Apple Developer signing which
  will be added when the account is purchased." Build everything else
  distribution-ready so signing can be dropped in as one final step later.
- **JUCE commercial licence** — only required once revenue exceeds JUCE's
  free tier threshold. Not a concern yet, handle when relevant.

**Distribution plan (for reference, not immediate action):**
- Primary: own website or Gumroad (simple, low fees, full control)
- Secondary: consider Plugin Boutique / Splice for reach once established
- Realistic pricing range for this type of plugin: £15–£49
- This is a side income / passion project, not a business replacement


- Repo: https://github.com/jackosmith1297-design/ProjectPlugIn
- `git` is already set up and authenticated on this Mac — you can run
  `git add`, `git commit`, and `git push` directly yourself when a real chunk
  of work is finished, rather than just telling me to do it. Use clear commit
  messages describing what changed.
- The repo is the safe backup of this project — commit and push at the end of
  any real chunk of work so nothing is ever only sitting locally unsaved.
