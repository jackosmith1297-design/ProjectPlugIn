#include "PluginProcessor.h"
#include "PluginEditor.h"

// ─────────────────────────────────────────────────────────────────────────────
// Colour palette (matches mockup CSS variables exactly)
// ─────────────────────────────────────────────────────────────────────────────
namespace Pal
{
    // Classic
    static const juce::Colour amber      { 0xFFD98E3Eu };
    static const juce::Colour amberGlow  { 0xFFFFB15Cu };
    static const juce::Colour amberDim   { 0xFF6B4A26u };
    static const juce::Colour cream      { 0xFFE8DCC4u };
    static const juce::Colour creamDim   { 0xFFA89C84u };
    static const juce::Colour panelDark  { 0xFF1A1816u };
    static const juce::Colour panelMid   { 0xFF252220u };
    static const juce::Colour panelLight { 0xFF312D2Au };

    // Modern
    static const juce::Colour lavender   { 0xFFB8A4E8u };
    static const juce::Colour mint       { 0xFF9EEED1u };
    static const juce::Colour sky        { 0xFF9CCDF0u };
    static const juce::Colour peach      { 0xFFF5C298u };
    static const juce::Colour pink       { 0xFFF0A8C8u };
    static const juce::Colour lilac      { 0xFFD4B3E8u };
    static const juce::Colour mTextPri   { 0xFF2E2C38u };
    static const juce::Colour mTextSec   { 0xFF767288u };
    static const juce::Colour mHousing   { 0xFFC9C6D2u };
    static const juce::Colour mPanelLt   { 0xFFF4F3F6u };
    static const juce::Colour mPanelMid  { 0xFFE6E4EAu };
    static const juce::Colour mPanelDk   { 0xFFD6D3DCu };
}

// ─────────────────────────────────────────────────────────────────────────────
// Custom LookAndFeel — rotary knobs
// ─────────────────────────────────────────────────────────────────────────────
class PluginLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void setClassicMode (bool c) { isClassic = c; }

    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& slider) override
    {
        const float cx     = x + width  * 0.5f;
        const float cy     = y + height * 0.5f;
        const float outerR = juce::jmin (width, height) * 0.5f - 4.0f; // knob-wrap radius
        const float innerR = outerR * (60.0f / 76.0f);                 // knob radius (60/76 ratio)

        // ── Ambient glow pool BELOW the knob (.knob-wrap::after) ─────────────
        {
            const juce::Colour gc = isClassic ? juce::Colour (0x72D98E3E)  // warm amber
                                              : juce::Colour (0x66B8A4E8); // cool lavender
            const float gw = outerR * 2.4f;
            const float gh = 14.0f;
            const float gcy = cy + outerR + 2.0f;
            juce::ColourGradient gl (gc, cx, gcy,
                                     juce::Colours::transparentBlack, cx + gw * 0.5f, gcy, true);
            g.setGradientFill (gl);
            g.fillEllipse (cx - gw * 0.5f, cy + outerR - 3.0f, gw, gh);
        }

        // ── Outer rim (.knob-wrap) — linear-gradient(155deg, ...) ────────────
        // CSS 155deg: gradient direction is (sin155°, −cos155°) ≈ (0.423, 0.906)
        {
            const float dx = std::sin (juce::degreesToRadians (155.0f)) * outerR;
            const float dy = -std::cos (juce::degreesToRadians (155.0f)) * outerR;
            juce::ColourGradient rim;
            if (isClassic)
            {
                rim = juce::ColourGradient (juce::Colour (0xFF6B6358), cx - dx, cy - dy,
                                            juce::Colour (0xFF1A1714), cx + dx, cy + dy, false);
                rim.addColour (0.6, juce::Colour (0xFF2C2824));
            }
            else
            {
                rim = juce::ColourGradient (juce::Colours::white,      cx - dx, cy - dy,
                                            juce::Colour (0xFFB8B4C2), cx + dx, cy + dy, false);
                rim.addColour (0.6, juce::Colour (0xFFC9C6D2));
            }
            g.setGradientFill (rim);
            g.fillEllipse (cx - outerR, cy - outerR, outerR * 2.0f, outerR * 2.0f);

            // inset 0 0 0 1px rgba(0,0,0,0.5)
            g.setColour (juce::Colours::black.withAlpha (0.5f));
            g.drawEllipse (cx - outerR + 0.5f, cy - outerR + 0.5f,
                           (outerR - 0.5f) * 2.0f, (outerR - 0.5f) * 2.0f, 1.0f);
        }

        // ── Tick marks (on the rim between innerR and outerR) ────────────────
        {
            const int numTicks = 11;
            const juce::Colour tickCol = isClassic ? Pal::creamDim.withAlpha (0.4f)
                                                   : Pal::mTextSec.withAlpha (0.35f);
            for (int i = 0; i <= numTicks; ++i)
            {
                const float a  = rotaryStartAngle + (float) i / numTicks * (rotaryEndAngle - rotaryStartAngle);
                const float r1 = innerR + 1.5f;
                const float r2 = outerR - 1.5f;
                g.setColour (tickCol);
                g.drawLine (cx + r1 * std::sin (a), cy - r1 * std::cos (a),
                            cx + r2 * std::sin (a), cy - r2 * std::cos (a), 1.2f);
            }
        }

        // ── Inner dial (.knob) — radial-gradient(circle at 38% 32%, ...) ─────
        // 38%,32% of the 60px diameter maps to offset from center:
        // x: (0.38 − 0.50) × 2×innerR = −0.24×innerR
        // y: (0.32 − 0.50) × 2×innerR = −0.36×innerR
        {
            const float hx = cx - 0.24f * innerR;
            const float hy = cy - 0.36f * innerR;
            juce::ColourGradient face;
            if (isClassic)
            {
                face = juce::ColourGradient (juce::Colour (0xFF4A443C), hx, hy,
                                              juce::Colour (0xFF161412), cx + innerR * 0.7f, cy + innerR * 0.7f, true);
                face.addColour (0.5, juce::Colour (0xFF28241F));
            }
            else
            {
                face = juce::ColourGradient (juce::Colour (0xFFF6F4F9), hx, hy,
                                              juce::Colour (0xFFCDC8D6), cx + innerR * 0.7f, cy + innerR * 0.7f, true);
                face.addColour (0.5, juce::Colour (0xFFE2DEE9));
            }
            g.setGradientFill (face);
            g.fillEllipse (cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f);

            // inset 0 0 0 1px rgba(0,0,0,0.6) border
            g.setColour (juce::Colours::black.withAlpha (isClassic ? 0.6f : 0.3f));
            g.drawEllipse (cx - innerR + 0.5f, cy - innerR + 0.5f,
                           (innerR - 0.5f) * 2.0f, (innerR - 0.5f) * 2.0f, 1.0f);

            // inset 0 -3px 5px rgba(0,0,0,0.5) — bottom-of-face shadow
            juce::ColourGradient btm (juce::Colours::transparentBlack, cx, cy - innerR,
                                      juce::Colours::black.withAlpha (isClassic ? 0.4f : 0.18f),
                                      cx, cy + innerR, false);
            g.setGradientFill (btm);
            g.fillEllipse (cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f);

            // inset 0 2px 3px rgba(255,255,255,...) — top specular highlight
            juce::ColourGradient top (juce::Colours::white.withAlpha (isClassic ? 0.04f : 0.55f),
                                      cx, cy - innerR,
                                      juce::Colours::transparentWhite, cx, cy, false);
            g.setGradientFill (top);
            g.fillEllipse (cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f);
        }

        // ── Indicator needle with glow ────────────────────────────────────────
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Per-mode / per-knob needle colours
        juce::Colour indicTop, indicBot, glowCol;
        if (isClassic)
        {
            indicTop = juce::Colour (0xFFFFB15C);  // warm amber
            indicBot = juce::Colour (0xFFD98E3E);
            glowCol  = juce::Colour (0x99FFB15C);  // rgba(255,177,92,0.6)
        }
        else
        {
            const auto name = slider.getName();
            if (name == "dryWet")
            {
                indicTop = juce::Colour (0xFF9CCDF0); indicBot = juce::Colour (0xFF6BADE0);
                glowCol  = juce::Colour (0xB29CCDF0); // sky
            }
            else if (name == "outputGain")
            {
                indicTop = juce::Colour (0xFFF5C298); indicBot = juce::Colour (0xFFE89A5C);
                glowCol  = juce::Colour (0xB2F5C298); // peach
            }
            else
            {
                indicTop = juce::Colour (0xFF9EEED1); indicBot = juce::Colour (0xFF5FCF9E);
                glowCol  = juce::Colour (0xB29EEED1); // mint (strength)
            }
        }

        // Needle geometry: top: 6px from edge, width: 3px, height: 18px
        // transform-origin: knob center (already correct since we rotate around cx,cy)
        const float needleTop = -(innerR - 5.0f);
        const float needleLen = innerR * 0.52f;
        const float needleW   = 3.0f;

        const juce::AffineTransform t = juce::AffineTransform::rotation (angle).translated (cx, cy);

        // World-space gradient endpoints (tip → base after rotation)
        const auto tip  = juce::Point<float> (0.0f, needleTop).transformedBy (t);
        const auto base = juce::Point<float> (0.0f, needleTop + needleLen).transformedBy (t);

        // Glow pass — wider semi-transparent halo (box-shadow: 0 0 4px glowCol)
        {
            juce::Path glow;
            glow.addRoundedRectangle (-needleW * 2.2f, needleTop - 2.0f,
                                       needleW * 4.4f, needleLen + 4.0f, needleW * 2.2f);
            g.setColour (glowCol.withAlpha (0.28f));
            g.fillPath (glow, t);
        }

        // Needle — gradient from tip (bright) to base (darker)
        {
            juce::Path needle;
            needle.addRoundedRectangle (-needleW * 0.5f, needleTop, needleW, needleLen, 1.5f);
            juce::ColourGradient ng (indicTop, tip.x, tip.y, indicBot, base.x, base.y, false);
            g.setGradientFill (ng);
            g.fillPath (needle, t);
        }
    }

private:
    bool isClassic = true;
};

// ─────────────────────────────────────────────────────────────────────────────
// CharacterToggle — Classic ←→ Modern toggle switch
// ─────────────────────────────────────────────────────────────────────────────
class CharacterToggle : public juce::Component
{
public:
    explicit CharacterToggle (juce::AudioProcessorValueTreeState& apvts)
        : param (*apvts.getParameter (ProjectPlugInAudioProcessor::characterParamID))
    {
        setMouseCursor (juce::MouseCursor::PointingHandCursor);
    }

    bool isClassic() const { return param.getValue() < 0.5f; }

    void paint (juce::Graphics& g) override
    {
        const bool classic  = isClassic();
        const auto b        = getLocalBounds().toFloat();
        const float labelW  = 56.0f;
        const float trackW  = 64.0f;
        const float trackH  = 28.0f;
        const float totalW  = labelW + 16.0f + trackW + 16.0f + labelW;
        const float startX  = (b.getWidth()  - totalW) * 0.5f;
        const float centreY = b.getCentreY();

        // Left label: CLASSIC
        g.setFont (juce::FontOptions (9.5f, juce::Font::bold));
        g.setColour (classic ? Pal::amberGlow : Pal::creamDim.withAlpha (0.5f));
        g.drawText ("CLASSIC", startX, centreY - 8.0f, labelW, 16.0f, juce::Justification::centred);

        // Right label: MODERN
        g.setColour (!classic ? Pal::lilac : Pal::creamDim.withAlpha (0.5f));
        g.drawText ("MODERN", startX + labelW + 16.0f + trackW + 16.0f, centreY - 8.0f,
                    labelW, 16.0f, juce::Justification::centred);

        // Track
        const float trackX = startX + labelW + 16.0f;
        const float trackY = centreY - trackH * 0.5f;

        if (classic)
        {
            juce::ColourGradient tg (juce::Colour (0xFF0D0B09), trackX, trackY,
                                     juce::Colour (0xFF1C1813), trackX, trackY + trackH, false);
            g.setGradientFill (tg);
        }
        else
        {
            juce::ColourGradient tg (juce::Colour (0xFFE8E5EE), trackX, trackY,
                                     juce::Colour (0xFFF5F3F8), trackX, trackY + trackH, false);
            g.setGradientFill (tg);
        }
        g.fillRoundedRectangle (trackX, trackY, trackW, trackH, 14.0f);
        g.setColour (juce::Colours::black.withAlpha (0.35f));
        g.drawRoundedRectangle (trackX, trackY, trackW, trackH, 14.0f, 1.0f);

        // Knob
        const float knobSize = 24.0f;
        const float knobX    = classic ? trackX + 2.0f : trackX + trackW - knobSize - 2.0f;
        const float knobY    = trackY + 2.0f;

        {
            juce::ColourGradient kg;
            if (classic)
                kg = juce::ColourGradient (juce::Colour (0xFF4A4540), knobX, knobY,
                                           juce::Colour (0xFF221F1C), knobX + knobSize, knobY + knobSize, true);
            else
                kg = juce::ColourGradient (juce::Colours::white, knobX, knobY,
                                           juce::Colour (0xFFD4CFE0), knobX + knobSize, knobY + knobSize, true);
            g.setGradientFill (kg);
        }
        g.fillEllipse (knobX, knobY, knobSize, knobSize);

        // Glow ring around knob
        g.setColour (classic ? Pal::amber.withAlpha (0.35f) : Pal::lilac.withAlpha (0.5f));
        g.drawEllipse (knobX - 1.5f, knobY - 1.5f, knobSize + 3.0f, knobSize + 3.0f, 2.0f);
    }

    void mouseDown (const juce::MouseEvent&) override
    {
        param.beginChangeGesture();
        param.setValueNotifyingHost (param.getValue() < 0.5f ? 1.0f : 0.0f);
        param.endChangeGesture();
        repaint();
    }

private:
    juce::RangedAudioParameter& param;
};

// ─────────────────────────────────────────────────────────────────────────────
// VuMeter — LED bar display
// ─────────────────────────────────────────────────────────────────────────────
class VuMeter : public juce::Component
{
public:
    void setLevel    (float dB) { levelDb  = dB;      repaint(); }
    void setClassicMode (bool c) { isClassic = c;     repaint(); }

    void paint (juce::Graphics& g) override
    {
        const auto b = getLocalBounds().toFloat();

        // Housing
        juce::ColourGradient hg (juce::Colour (0xFF0A0908), b.getTopLeft(),
                                  juce::Colour (0xFF050404), b.getBottomRight(), false);
        if (!isClassic)
        {
            hg = juce::ColourGradient (juce::Colours::white, b.getTopLeft(),
                                       Pal::mHousing, b.getBottomRight(), false);
        }
        g.setGradientFill (hg);
        g.fillRoundedRectangle (b, 3.0f);
        g.setColour (juce::Colours::black.withAlpha (isClassic ? 0.6f : 0.2f));
        g.drawRoundedRectangle (b, 3.0f, 1.0f);

        // Face
        const auto face = b.reduced (7.0f, 7.0f);
        {
            juce::ColourGradient fg;
            if (isClassic)
                fg = juce::ColourGradient (juce::Colour (0xFF2A2520), face.getTopLeft(),
                                           juce::Colour (0xFF1C1813), face.getBottomRight(), false);
            else
                fg = juce::ColourGradient (juce::Colour (0xFFFBFAFF), face.getTopLeft(),
                                           juce::Colour (0xFFECE9F2), face.getBottomRight(), false);
            g.setGradientFill (fg);
            g.fillRoundedRectangle (face, 2.0f);
        }

        // Scale labels
        g.setFont (juce::FontOptions (8.0f));
        g.setColour (isClassic ? Pal::creamDim : Pal::mTextSec);
        g.drawText ("-30", (int) face.getX(),                       (int) face.getY() + 1, 24, 11, juce::Justification::left);
        g.drawText ("-10", (int) face.getCentreX() - 12,            (int) face.getY() + 1, 24, 11, juce::Justification::centred);
        g.drawText ("0",   (int)(face.getRight() - 16),             (int) face.getY() + 1, 16, 11, juce::Justification::right);

        // LED track
        const float trackY = face.getY() + 16.0f;
        const float trackH = 14.0f;
        const auto  track  = juce::Rectangle<float> (face.getX(), trackY, face.getWidth(), trackH);

        g.setColour (isClassic ? juce::Colour (0xFF0D0B09) : juce::Colour (0xFFD8D4E0));
        g.fillRoundedRectangle (track, 2.0f);

        const int   numLeds  = 28;
        const float minDb    = -30.0f;
        const float maxDb    = 0.0f;
        const float norm     = (levelDb - minDb) / (maxDb - minDb);
        const int   numLit   = juce::jlimit (0, numLeds, (int) (norm * numLeds));
        const float gap      = 1.5f;
        const float ledW     = (track.getWidth() - gap) / numLeds - gap;

        for (int i = 0; i < numLeds; ++i)
        {
            const float lx = track.getX() + gap + i * (ledW + gap);
            const float ly = track.getY() + 2.0f;
            const float lh = track.getHeight() - 4.0f;

            juce::Colour col;
            if (i < numLit)
            {
                const float ledNorm = (float) i / numLeds;
                if (isClassic)
                    col = ledNorm < 0.6f ? Pal::amberDim : (ledNorm < 0.85f ? Pal::amber : Pal::amberGlow);
                else
                    col = ledNorm < 0.6f ? Pal::lavender : (ledNorm < 0.85f ? Pal::sky : Pal::pink);
            }
            else
            {
                col = isClassic ? juce::Colour (0xFF1A1612) : juce::Colour (0xFFCFCBD6);
            }
            g.setColour (col);
            g.fillRoundedRectangle (lx, ly, ledW, lh, 0.8f);
        }

        // Bottom label
        g.setFont (juce::FontOptions (8.5f));
        g.setColour (isClassic ? Pal::creamDim : Pal::mTextSec);
        g.drawText ("VOICE", (int) face.getX(), (int)(trackY + trackH + 3.0f),
                    (int) face.getWidth(), 11, juce::Justification::left);
        g.drawText ("dB", (int) face.getX(), (int)(trackY + trackH + 3.0f),
                    (int) face.getWidth(), 11, juce::Justification::right);
    }

private:
    float levelDb  = -30.0f;
    bool  isClassic = true;
};

// ─────────────────────────────────────────────────────────────────────────────
// Editor
// ─────────────────────────────────────────────────────────────────────────────
ProjectPlugInAudioProcessorEditor::ProjectPlugInAudioProcessorEditor (ProjectPlugInAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    laf = std::make_unique<PluginLookAndFeel>();
    setLookAndFeel (laf.get());

    // Character toggle
    characterToggle = std::make_unique<CharacterToggle> (audioProcessor.parameters);
    addAndMakeVisible (*characterToggle);

    // VU meter
    vuMeter = std::make_unique<VuMeter>();
    addAndMakeVisible (*vuMeter);

    // Knobs — name them so the LookAndFeel can colour indicators per knob
    strengthKnob.setName   ("strength");
    pitchKnob.setName      ("pitch");
    dryWetKnob.setName     ("dryWet");
    outputGainKnob.setName ("outputGain");

    addAndMakeVisible (strengthKnob);
    addAndMakeVisible (pitchKnob);
    addAndMakeVisible (dryWetKnob);
    addAndMakeVisible (outputGainKnob);

    strengthAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, ProjectPlugInAudioProcessor::strengthParamID,   strengthKnob);
    pitchAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, ProjectPlugInAudioProcessor::pitchParamID,      pitchKnob);
    dryWetAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, ProjectPlugInAudioProcessor::dryWetParamID,     dryWetKnob);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, ProjectPlugInAudioProcessor::outputGainParamID, outputGainKnob);

    // Repaint when any knob moves so value labels stay in sync
    strengthKnob.onValueChange   = [this] { repaint(); };
    pitchKnob.onValueChange      = [this] { repaint(); };
    dryWetKnob.onValueChange     = [this] { repaint(); };
    outputGainKnob.onValueChange = [this] { repaint(); };

    setSize (720, 480);
    startTimerHz (15); // 15Hz is smooth enough for VU meter + character switch response
}

ProjectPlugInAudioProcessorEditor::~ProjectPlugInAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
    stopTimer();
}

bool ProjectPlugInAudioProcessorEditor::isClassicMode() const
{
    return static_cast<int> (
        audioProcessor.parameters.getRawParameterValue (
            ProjectPlugInAudioProcessor::characterParamID)->load()) == 0;
}

void ProjectPlugInAudioProcessorEditor::timerCallback()
{
    const bool classic = isClassicMode();

    // Update sub-components with current mode
    laf->setClassicMode (classic);
    static_cast<VuMeter*> (vuMeter.get())->setLevel (audioProcessor.outputLevelDb.load());
    static_cast<VuMeter*> (vuMeter.get())->setClassicMode (classic);

    if (classic != wasClassicOnLastPaint)
    {
        wasClassicOnLastPaint = classic;
        repaint();
        strengthKnob.repaint();
        pitchKnob.repaint();
        dryWetKnob.repaint();
        outputGainKnob.repaint();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// paint()
// ─────────────────────────────────────────────────────────────────────────────
void ProjectPlugInAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (isClassicMode())
        paintClassicBackground (g);
    else
        paintModernBackground (g);

    paintSections     (g);
    paintHeader       (g);
    paintStatusSticker(g);
    paintFooter       (g);
    paintKnobLabels   (g);
    paintIoRow        (g);
    paintHoloStickers (g);
}

void ProjectPlugInAudioProcessorEditor::paintClassicBackground (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    // Wood border — thick outer frame
    const auto woodDark  = juce::Colour (0xFF432A16);
    const auto woodMid   = juce::Colour (0xFF6B4423);
    const auto woodLight = juce::Colour (0xFF8A5A35);

    g.setColour (woodMid);
    g.fillRect (b);

    // Near-horizontal grain stripes — 97° in the mockup means nearly
    // left-to-right, so we iterate in Y and offset slightly in X.
    {
        juce::Graphics::ScopedSaveState ss (g);
        g.reduceClipRegion (b.toNearestInt());
        const float sh      = 3.5f;
        const float xSlant  = b.getWidth() * 0.06f; // slight rightward tilt
        bool toggle = false;
        for (float y = 0; y < b.getHeight(); y += sh)
        {
            g.setColour (toggle ? woodLight.withAlpha (0.45f) : woodDark.withAlpha (0.35f));
            juce::Path stripe;
            stripe.addQuadrilateral (0,               y,
                                     b.getWidth(),    y + xSlant,
                                     b.getWidth(),    y + sh + xSlant,
                                     0,               y + sh);
            g.fillPath (stripe);
            toggle = !toggle;
        }
    }

    // Inner panel
    const float border = 26.0f;
    auto inner = b.reduced (border);

    // Panel gradient (brushed metal dark)
    juce::ColourGradient pg (juce::Colour (0xFF312D2A), inner.getTopLeft(),
                              juce::Colour (0xFF1A1816), inner.getBottomRight(), false);
    g.setGradientFill (pg);
    g.fillRoundedRectangle (inner, 4.0f);

    // Perforated dot grille
    {
        juce::Graphics::ScopedSaveState ss (g);
        juce::Path clip;
        clip.addRoundedRectangle (inner, 4.0f);
        g.reduceClipRegion (clip);
        g.setColour (juce::Colours::black.withAlpha (0.45f));
        const float gs = 7.0f, dr = 0.9f;
        for (float y = inner.getY(); y < inner.getBottom(); y += gs)
            for (float x = inner.getX(); x < inner.getRight(); x += gs)
                g.fillEllipse (x, y, dr * 2, dr * 2);
    }

    // Corner screws
    auto drawScrew = [&] (float cx, float cy)
    {
        g.setColour (juce::Colour (0xFF2A2622));
        g.fillEllipse (cx - 7, cy - 7, 14, 14);
        g.setColour (juce::Colours::black.withAlpha (0.6f));
        g.drawLine (cx - 4, cy - 2, cx + 4, cy + 2, 1.5f);
    };
    const float si = 13.0f;
    drawScrew (b.getX() + si, b.getY() + si);
    drawScrew (b.getRight() - si, b.getY() + si);
    drawScrew (b.getX() + si, b.getBottom() - si);
    drawScrew (b.getRight() - si, b.getBottom() - si);
}

void ProjectPlugInAudioProcessorEditor::paintModernBackground (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    // Light glassy panel
    juce::ColourGradient pg (Pal::mPanelLt, b.getTopLeft(),
                              Pal::mPanelDk, b.getBottomRight(), false);
    g.setGradientFill (pg);
    g.fillRoundedRectangle (b, 6.0f);

    // Sheen highlight near top
    juce::ColourGradient sheen (juce::Colours::white.withAlpha (0.55f),
                                 b.getX() + b.getWidth() * 0.3f, b.getY(),
                                 juce::Colours::white.withAlpha (0.0f),
                                 b.getX() + b.getWidth() * 0.3f, b.getY() + b.getHeight() * 0.45f, false);
    g.setGradientFill (sheen);
    g.fillRoundedRectangle (b, 6.0f);

    // Faint HUD grid
    {
        juce::Graphics::ScopedSaveState ss (g);
        juce::Path clip;
        clip.addRoundedRectangle (b, 6.0f);
        g.reduceClipRegion (clip);
        const float gs = 28.0f;
        g.setColour (juce::Colour (0x0F64C8F0)); // very faint cyan grid
        for (float y = b.getY(); y < b.getBottom(); y += gs)
            g.drawHorizontalLine ((int) y, b.getX(), b.getRight());
        for (float x = b.getX(); x < b.getRight(); x += gs)
            g.drawVerticalLine ((int) x, b.getY(), b.getBottom());
    }

    // Corner screws
    auto drawScrew = [&] (float cx, float cy)
    {
        g.setColour (juce::Colour (0xFF2A2622));
        g.fillEllipse (cx - 7, cy - 7, 14, 14);
        g.setColour (juce::Colours::black.withAlpha (0.5f));
        g.drawLine (cx - 4, cy - 2, cx + 4, cy + 2, 1.5f);
    };
    const float si = 13.0f;
    drawScrew (b.getX() + si, b.getY() + si);
    drawScrew (b.getRight() - si, b.getY() + si);
    drawScrew (b.getX() + si, b.getBottom() - si);
    drawScrew (b.getRight() - si, b.getBottom() - si);
}

void ProjectPlugInAudioProcessorEditor::paintSections (juce::Graphics& g)
{
    const bool classic = isClassicMode();

    auto drawSection = [&] (juce::Rectangle<int> r)
    {
        const auto rf = r.toFloat();
        if (classic)
        {
            juce::ColourGradient sg (juce::Colours::black.withAlpha (0.18f), rf.getTopLeft(),
                                     juce::Colours::black.withAlpha (0.32f), rf.getBottomRight(), false);
            g.setGradientFill (sg);
            g.fillRoundedRectangle (rf, 4.0f);
            g.setColour (juce::Colours::white.withAlpha (0.03f));
            g.drawRoundedRectangle (rf, 4.0f, 1.0f);
        }
        else
        {
            juce::ColourGradient sg (juce::Colours::white.withAlpha (0.55f), rf.getTopLeft(),
                                     juce::Colours::white.withAlpha (0.25f), rf.getBottomRight(), false);
            g.setGradientFill (sg);
            g.fillRoundedRectangle (rf, 4.0f);
            g.setColour (juce::Colours::white.withAlpha (0.55f));
            g.drawRoundedRectangle (rf.reduced (0.5f), 4.0f, 1.0f);
        }
    };

    // Section label helper
    auto drawLabel = [&] (const juce::String& text, juce::Rectangle<int> sectionR)
    {
        g.setFont (juce::FontOptions (9.5f, juce::Font::bold));
        g.setColour (classic ? Pal::amberDim : Pal::lilac);
        g.drawText (text, sectionR.getX(), sectionR.getY() + 8,
                    sectionR.getWidth(), 14, juce::Justification::centred);
        // Divider under label
        g.setColour (classic ? juce::Colours::white.withAlpha (0.05f)
                              : juce::Colours::black.withAlpha (0.08f));
        g.drawHorizontalLine (sectionR.getY() + 24,
                              (float) sectionR.getX() + 8, (float) sectionR.getRight() - 8);
    };

    drawSection (signalSectionBounds);
    drawLabel ("SIGNAL LEVEL", signalSectionBounds);

    drawSection (characterSectionBounds);
    drawLabel ("CHARACTER", characterSectionBounds);

    drawSection (controlsSectionBounds);
    drawLabel ("CONTROLS", controlsSectionBounds);
}

void ProjectPlugInAudioProcessorEditor::paintHeader (juce::Graphics& g)
{
    const bool classic = isClassicMode();
    const int  bw      = 26; // border width
    const auto inner   = getLocalBounds().reduced (bw);

    // Header solid backing plate (Classic only)
    if (classic)
    {
        juce::ColourGradient hg (juce::Colour (0xFF322E2B), inner.getX(), inner.getY(),
                                  juce::Colour (0xFF211D1B), inner.getX(), inner.getY() + 72, false);
        g.setGradientFill (hg);
        g.fillRoundedRectangle ((float) inner.getX(), (float) inner.getY(),
                                (float) inner.getWidth(), 72.0f, 3.0f);
        g.setColour (juce::Colours::white.withAlpha (0.05f));
        g.drawHorizontalLine (inner.getY() + 72,
                              (float) inner.getX() + 8, (float) inner.getRight() - 8);
    }

    const int lx = inner.getX() + 18;
    const int rx = inner.getRight() - 18;

    // Build strings with Unicode chars that can't be written as raw bytes safely
    const juce::String enDash  = juce::String::charToString (0x2013); // –
    const juce::String midDot  = juce::String::charToString (0x00B7); // ·
    const juce::String brandStr   = "SMITH" + enDash + "PYE AUDIO";
    const juce::String subtitleStr = "VOCAL  " + midDot + "  VOCODER  " + midDot + "  SYNTHESIS";
    const juce::String pvCode  = "PV" + enDash + "01";

    // Brand
    g.setFont (juce::FontOptions (10.0f));
    g.setColour (classic ? Pal::creamDim : Pal::mTextSec);
    g.drawText (brandStr, lx, inner.getY() + 10, 220, 13, juce::Justification::left);

    // Product name
    g.setFont (juce::FontOptions (26.0f, juce::Font::bold));
    g.setColour (classic ? Pal::cream : Pal::mTextPri);
    g.drawText ("PROJECT PLUGIN", lx, inner.getY() + 22, 380, 30, juce::Justification::left);

    // Subtitle
    g.setFont (juce::FontOptions (9.5f));
    g.setColour (classic ? Pal::amberDim : Pal::mTextSec);
    g.drawText (subtitleStr, lx, inner.getY() + 52, 300, 13, juce::Justification::left);

    // Model plate (right-aligned)
    g.setFont (juce::FontOptions (9.0f));
    g.setColour (classic ? Pal::creamDim : Pal::mTextSec);
    g.drawText ("MODEL  " + pvCode, rx - 140, inner.getY() + 12, 140, 13, juce::Justification::right);
    g.drawText ("ANALOGUE VOICE PROCESSOR", rx - 180, inner.getY() + 26, 180, 13, juce::Justification::right);

    g.setColour (classic ? Pal::amber : Pal::pink);
    g.drawText (pvCode, rx - 60, inner.getY() + 12, 60, 13, juce::Justification::right);
}

void ProjectPlugInAudioProcessorEditor::paintFooter (juce::Graphics& g)
{
    const bool classic = isClassicMode();
    const int  bw      = 26;
    const auto inner   = getLocalBounds().reduced (bw);

    const int footerY = inner.getBottom() - 46;

    // Divider
    g.setColour (classic ? juce::Colours::white.withAlpha (0.05f)
                         : juce::Colours::black.withAlpha (0.10f));
    g.drawHorizontalLine (footerY, (float) inner.getX() + 8, (float) inner.getRight() - 8);

    // Power LED
    const float ledX = (float) inner.getX() + 20;
    const float ledY = (float) footerY + 14;
    {
        juce::ColourGradient lg;
        if (classic)
            lg = juce::ColourGradient (Pal::amberGlow, ledX, ledY, Pal::amber, ledX + 8, ledY + 8, true);
        else
            lg = juce::ColourGradient (Pal::mint, ledX, ledY, Pal::sky, ledX + 8, ledY + 8, true);
        g.setGradientFill (lg);
        g.fillEllipse (ledX, ledY, 8, 8);
        // Glow
        g.setColour ((classic ? Pal::amberGlow : Pal::mint).withAlpha (0.4f));
        g.fillEllipse (ledX - 3, ledY - 3, 14, 14);
    }

    g.setFont (juce::FontOptions (9.0f));
    g.setColour (classic ? Pal::creamDim : Pal::mTextSec);
    g.drawText ("POWER", (int) ledX + 14, (int) ledY - 1, 50, 11, juce::Justification::left);

    // Signature — "Snell Roundhand" is a genuine calligraphic script built into macOS
    const juce::String sig = "Smith" + juce::String::charToString (0x2013) + "Pye";
    const juce::FontOptions sigFont ("Snell Roundhand", 26.0f, juce::Font::plain);
    const juce::Rectangle<int> sigBounds (inner.getRight() - 140, footerY + 6, 130, 34);

    // Soft ink-shadow pass (offset 1px down/right, very faint)
    g.setFont (sigFont);
    g.setColour (juce::Colours::black.withAlpha (classic ? 0.25f : 0.12f));
    g.drawText (sig, sigBounds.translated (1, 1), juce::Justification::right);

    // Main ink pass
    g.setColour ((classic ? Pal::creamDim : Pal::mTextSec).withAlpha (classic ? 0.82f : 0.72f));
    g.drawText (sig, sigBounds, juce::Justification::right);
}

void ProjectPlugInAudioProcessorEditor::paintKnobLabels (juce::Graphics& g)
{
    const bool classic = isClassicMode();
    const int knobR = 40; // half of knob component width (80/2)

    enum class Fmt { Percent, Hz, Db };
    struct KnobInfo { juce::Point<int> centre; juce::String name; double value; Fmt fmt; };
    const KnobInfo knobs[] = {
        { strengthKnobCentre,   "STRENGTH",  strengthKnob.getValue(),   Fmt::Percent },
        { pitchKnobCentre,      "PITCH",     pitchKnob.getValue(),      Fmt::Hz      },
        { dryWetKnobCentre,     "DRY / WET", dryWetKnob.getValue(),     Fmt::Percent },
        { outputGainKnobCentre, "OUTPUT",    outputGainKnob.getValue(), Fmt::Db      },
    };

    for (auto& k : knobs)
    {
        g.setFont (juce::FontOptions (9.5f, juce::Font::bold));
        g.setColour (classic ? Pal::cream : Pal::mTextPri);
        g.drawText (k.name, k.centre.x - 52, k.centre.y + knobR + 4, 104, 13, juce::Justification::centred);

        juce::String valStr;
        switch (k.fmt)
        {
            case Fmt::Db:      valStr = juce::String (k.value, 1) + " dB"; break;
            case Fmt::Hz:      valStr = juce::String ((int) juce::roundToInt (k.value)) + " Hz"; break;
            case Fmt::Percent: valStr = juce::String ((int) juce::roundToInt (k.value * 100.0)) + "%"; break;
        }

        g.setFont (juce::FontOptions (9.0f));
        g.setColour (classic ? Pal::amberDim : Pal::mTextSec);
        g.drawText (valStr, k.centre.x - 52, k.centre.y + knobR + 17, 104, 12, juce::Justification::centred);
    }
}

void ProjectPlugInAudioProcessorEditor::paintIoRow (juce::Graphics& g)
{
    if (ioRowBounds.isEmpty()) return;

    const bool classic = isClassicMode();
    const auto r = ioRowBounds.toFloat();

    // Divider
    g.setColour (classic ? juce::Colours::white.withAlpha (0.04f)
                         : juce::Colours::black.withAlpha (0.10f));
    g.drawHorizontalLine (ioRowBounds.getY(), r.getX() + 4, r.getRight() - 4);

    const juce::StringArray labels { "VOICE IN", "OUTPUT" };
    const float colW = r.getWidth() / 2.0f;

    for (int i = 0; i < 3; ++i)
    {
        const float itemX = r.getX() + i * colW;
        const float ledX  = itemX + 4;
        const float ledY  = r.getCentreY() - 3;

        // LED dot
        juce::Colour ledCol = classic ? Pal::amber : Pal::lilac;
        g.setColour (ledCol);
        g.fillEllipse (ledX, ledY, 6, 6);
        g.setColour (ledCol.withAlpha (0.4f));
        g.fillEllipse (ledX - 2, ledY - 2, 10, 10);

        // Label
        g.setFont (juce::FontOptions (8.5f));
        g.setColour (classic ? Pal::creamDim : Pal::mTextSec);
        g.drawText (labels[i], (int)(ledX + 12), (int) ledY - 1,
                    (int)(colW - 16), 11, juce::Justification::left);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// paintStatusSticker — "VOCODER ENGAGED" (Classic) / "VOICE DISTORTION
// ACTIVATED" (Modern). Top-right of panel, at subtitle level.
// ─────────────────────────────────────────────────────────────────────────────
void ProjectPlugInAudioProcessorEditor::paintStatusSticker (juce::Graphics& g)
{
    const bool classic = isClassicMode();
    const auto inner   = getLocalBounds().reduced (26);

    const juce::String text = classic ? "VOCODER ENGAGED" : "VOICE DISTORTION ACTIVATED";

    const float fontSize = 6.5f;
    const juce::Font font { juce::FontOptions (fontSize, juce::Font::bold) };
    const float sw = classic ? 118.0f : 158.0f;
    const float sh = 18.0f;

    // Position: right side of panel, aligned with subtitle row
    const float sx = (float)(inner.getRight()) - 22.0f - sw;
    const float sy = (float)(inner.getY()) + 56.0f;
    const auto  sr = juce::Rectangle<float> (sx, sy, sw, sh);

    if (classic)
    {
        // Vintage kraft-paper QC label — aged cream, faint double border
        {
            juce::Graphics::ScopedSaveState ss (g);
            const float angle = 0.014f; // 0.8 degrees
            g.addTransform (juce::AffineTransform::rotation (angle, sr.getCentreX(), sr.getCentreY()));

            // Background: aged cream gradient
            juce::ColourGradient bg (juce::Colour (0xFFD6C89E), sr.getTopLeft(),
                                     juce::Colour (0xFFD2C08E), sr.getBottomRight(), false);
            bg.addColour (0.5, juce::Colour (0xFFC9BB85));
            g.setGradientFill (bg);
            g.fillRect (sr);

            // Outer border
            g.setColour (juce::Colour (0x665A3E12));
            g.drawRect (sr, 1.0f);

            // Inner double-rule border (1.5px inset)
            g.setColour (juce::Colour (0x44503210));
            g.drawRect (sr.reduced (2.5f), 0.5f);

            // Text — dark brown, tight letter spacing
            g.setFont (font);
            g.setColour (juce::Colour (0xB537230A));
            g.drawText (text, sr.toNearestInt(), juce::Justification::centred);
        }
    }
    else
    {
        // Holographic pastel film — lavender → sky → pink gradient
        juce::ColourGradient bg (Pal::lavender.withAlpha (0.88f), sr.getX(), sr.getY(),
                                  Pal::pink.withAlpha (0.88f),    sr.getRight(), sr.getY(), false);
        bg.addColour (0.5, Pal::sky.withAlpha (0.88f));
        g.setGradientFill (bg);
        g.fillRoundedRectangle (sr, 2.0f);

        // Glassy sheen highlight on top half
        juce::ColourGradient sheen (juce::Colours::white.withAlpha (0.4f), sr.getTopLeft(),
                                    juce::Colours::white.withAlpha (0.0f),
                                    sr.getCentre(), false);
        g.setGradientFill (sheen);
        g.fillRoundedRectangle (sr, 2.0f);

        // Fine border
        g.setColour (juce::Colours::white.withAlpha (0.5f));
        g.drawRoundedRectangle (sr.reduced (0.5f), 2.0f, 0.5f);

        // Text
        g.setFont (font);
        g.setColour (juce::Colour (0xCC2E2C38));
        g.drawText (text, sr.toNearestInt(), juce::Justification::centred);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// paintHoloStickers — Modern-only decorative stickers.
//   • "LIMITED EDITION" rainbow bar (below Character section)
//   • Small circular iridescent badge (bottom-right)
// ─────────────────────────────────────────────────────────────────────────────
void ProjectPlugInAudioProcessorEditor::paintHoloStickers (juce::Graphics& g)
{
    if (isClassicMode()) return;

    const auto inner = getLocalBounds().reduced (26);

    // ── Sticker 1: "LIMITED EDITION" rectangular foil bar ──────────────────
    if (!characterSectionBounds.isEmpty())
    {
        const float sw = 80.0f;
        const float sh = 18.0f;
        const float sx = (float) characterSectionBounds.getX() + 10.0f;
        const float sy = (float) characterSectionBounds.getBottom() + 12.0f;
        const auto  sr = juce::Rectangle<float> (sx, sy, sw, sh);

        // Rainbow gradient
        juce::ColourGradient bg (juce::Colour (0xB3FF64C8), sr.getX(), sr.getY(),
                                  juce::Colour (0xB3C864FF), sr.getRight(), sr.getY(), false);
        bg.addColour (0.25f, juce::Colour (0xB364C8FF));
        bg.addColour (0.50f, juce::Colour (0xB3B4FF96));
        bg.addColour (0.75f, juce::Colour (0xB3FFC850));
        g.setGradientFill (bg);
        g.fillRoundedRectangle (sr, 2.0f);

        // Sheen
        juce::ColourGradient sheen (juce::Colours::white.withAlpha (0.4f), sr.getTopLeft(),
                                    juce::Colours::white.withAlpha (0.0f), sr.getBottomLeft(), false);
        g.setGradientFill (sheen);
        g.fillRoundedRectangle (sr, 2.0f);

        // Border
        g.setColour (juce::Colours::white.withAlpha (0.5f));
        g.drawRoundedRectangle (sr.reduced (0.5f), 2.0f, 0.5f);

        // Text
        g.setFont (juce::FontOptions (6.0f, juce::Font::bold));
        g.setColour (juce::Colour (0x8C000000));
        g.drawText ("LIMITED EDITION", sr.toNearestInt(), juce::Justification::centred);
    }

    // ── Sticker 2: small circular iridescent badge (bottom-right) ──────────
    {
        const float cr = 14.0f; // radius
        const float cx = (float) inner.getRight() - 40.0f;
        const float cy = (float) inner.getBottom() - 56.0f;

        // Conic-ish gradient approximated with multiple overlapping radials
        const juce::Colour cols[] = { juce::Colour (0xCCFF64C8), juce::Colour (0xCC64C8FF),
                                      juce::Colour (0xCCB4FF96), juce::Colour (0xCCFFC850),
                                      juce::Colour (0xCCC864FF) };
        for (int i = 0; i < 5; ++i)
        {
            const float angle = (float) i * juce::MathConstants<float>::twoPi / 5.0f;
            const float ox    = std::sin (angle) * cr * 0.5f;
            const float oy    = -std::cos (angle) * cr * 0.5f;
            juce::ColourGradient cg (cols[i], cx + ox, cy + oy,
                                     cols[(i + 2) % 5].withAlpha (0.0f), cx - ox, cy - oy, true);
            g.setGradientFill (cg);
            g.fillEllipse (cx - cr, cy - cr, cr * 2.0f, cr * 2.0f);
        }

        // Bright rim
        g.setColour (juce::Colours::white.withAlpha (0.5f));
        g.drawEllipse (cx - cr + 0.5f, cy - cr + 0.5f, (cr - 0.5f) * 2.0f, (cr - 0.5f) * 2.0f, 0.5f);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// resized()
// ─────────────────────────────────────────────────────────────────────────────
void ProjectPlugInAudioProcessorEditor::resized()
{
    const int bw = 26; // border / outer padding
    auto inner = getLocalBounds().reduced (bw);

    // Header: 74px
    inner.removeFromTop (74);

    // Footer: 48px — store for paint, don't place child components here
    const auto footer = inner.removeFromBottom (48);
    (void) footer;

    // Body
    auto body = inner.withTrimmedTop (8).withTrimmedBottom (6);

    // Split into left (260px) and right columns
    auto leftCol  = body.removeFromLeft (252);
    body.removeFromLeft (16); // gap
    auto rightCol = body;

    // Signal Level section
    const int signalH    = 158;
    const int sectionGap = 12;
    const int charH      = juce::jmax (80, leftCol.getHeight() - signalH - sectionGap);

    signalSectionBounds    = leftCol.removeFromTop (signalH);
    leftCol.removeFromTop (sectionGap);
    characterSectionBounds = leftCol.removeFromTop (charH);
    controlsSectionBounds  = rightCol;

    // VU meter inside signal section (below label)
    auto vuBounds = signalSectionBounds.reduced (10);
    vuBounds.removeFromTop (28);
    vuMeter->setBounds (vuBounds);

    // Character toggle inside character section (below label)
    auto charBounds = characterSectionBounds.reduced (10);
    charBounds.removeFromTop (28);
    characterToggle->setBounds (charBounds);

    // Controls section layout
    auto ctrlInner = controlsSectionBounds.reduced (12);
    ctrlInner.removeFromTop (28); // section label

    // IO row at bottom
    const int ioH = 28;
    ioRowBounds = ctrlInner.removeFromBottom (ioH);

    // 4-knob row: Strength / Pitch / Dry-Wet / Output
    const int knobSize = 80;
    const int knobColW = ctrlInner.getWidth() / 4;
    const int knobTopY = ctrlInner.getY() + juce::jmax (0, (ctrlInner.getHeight() - knobSize - 32) / 2);

    auto placeKnob = [&] (juce::Slider& knob, juce::Point<int>& centre, int col)
    {
        const int kx = ctrlInner.getX() + col * knobColW + (knobColW - knobSize) / 2;
        knob.setBounds (kx, knobTopY, knobSize, knobSize);
        centre = { kx + knobSize / 2, knobTopY + knobSize / 2 };
    };

    placeKnob (strengthKnob,   strengthKnobCentre,   0);
    placeKnob (pitchKnob,      pitchKnobCentre,      1);
    placeKnob (dryWetKnob,     dryWetKnobCentre,     2);
    placeKnob (outputGainKnob, outputGainKnobCentre, 3);
}
