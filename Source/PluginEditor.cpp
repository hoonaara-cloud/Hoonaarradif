#include "PluginEditor.h"
#include <cmath>

namespace
{
    const juce::Colour ink       { 0xff172731 };
    const juce::Colour mutedInk  { 0xff5a6a70 };
    const juce::Colour gold      { 0xffd5a84b };
    const juce::Colour teal      { 0xff54b7ae };
    const juce::Colour cream     { 0xfff4f0e7 };
    const juce::Colour darkPanel { 0xff101f29 };

    juce::String degreeText(int degree)
    {
        if (degree <= 0)
            return "Not encoded";
        return "Degree " + juce::String(degree);
    }

    juce::String accidentalSymbolFor(int alteration)
    {
        switch (alteration)
        {
            case -4: return juce::String::fromUTF8("\xF0\x9D\x84\xAB"); // double flat
            case -3: return juce::String::fromUTF8("\xE2\x99\xAD") + juce::String::fromUTF8("\xF0\x9D\x84\xB3");
            case -2: return juce::String::fromUTF8("\xE2\x99\xAD"); // flat
            case -1: return juce::String::fromUTF8("\xF0\x9D\x84\xB3"); // quarter-tone flat (Koron)
            case  1: return juce::String::fromUTF8("\xF0\x9D\x84\xB2"); // quarter-tone sharp (Sori)
            case  2: return juce::String::fromUTF8("\xE2\x99\xAF"); // sharp
            case  3: return juce::String::fromUTF8("\xE2\x99\xAF") + juce::String::fromUTF8("\xF0\x9D\x84\xB2");
            case  4: return juce::String::fromUTF8("\xF0\x9D\x84\xAA"); // double sharp
            default: return {};
        }
    }

    juce::String accidentalFallbackFor(int alteration)
    {
        if (alteration == -1) return "K";
        if (alteration ==  1) return "S";
        if (alteration == -2) return "b";
        if (alteration ==  2) return "#";
        if (alteration == -3) return "bK";
        if (alteration ==  3) return "#S";
        if (alteration == -4) return "bb";
        if (alteration ==  4) return "x";
        return "";
    }
}

HoonaarRadifAtlasAudioProcessorEditor::HoonaarRadifAtlasAudioProcessorEditor(
    HoonaarRadifAtlasAudioProcessor& processor)
    : juce::AudioProcessorEditor(&processor), audioProcessor(processor)
{
    setSize(1000, 780);
    setResizable(true, true);
    setResizeLimits(860, 700, 1400, 1000);

    auto styleLabel = [](juce::Label& label, const juce::String& text)
    {
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colour(0xffa8bbc2));
        label.setJustificationType(juce::Justification::centredLeft);
    };

    styleLabel(modeLabel, "DASTGAH / AVAZ");
    styleLabel(gushehLabel, "GUSHEH");
    styleLabel(tonicLabel, "TONIC / SPELLING");

    for (auto* component : { static_cast<juce::Component*>(&modeLabel),
                             static_cast<juce::Component*>(&gushehLabel),
                             static_cast<juce::Component*>(&tonicLabel),
                             static_cast<juce::Component*>(&modeBox),
                             static_cast<juce::Component*>(&gushehBox),
                             static_cast<juce::Component*>(&tonicBox) })
        addAndMakeVisible(component);

    const auto comboBackground = juce::Colour(0xff172a35);
    const auto comboText = juce::Colour(0xfff3f5ef);
    for (auto* box : { &modeBox, &gushehBox, &tonicBox })
    {
        box->setColour(juce::ComboBox::backgroundColourId, comboBackground);
        box->setColour(juce::ComboBox::textColourId, comboText);
        box->setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff36505c));
        box->setColour(juce::ComboBox::arrowColourId, gold);
    }

    populateModeBox();
    populateTonicBox();

    modeBox.onChange = [this]
    {
        const auto newIndex = modeBox.getSelectedId() - 1;
        if (newIndex < 0)
            return;

        audioProcessor.setModeIndex(newIndex);
        const auto& currentMode = Hoonaar::Radif::modes()[(size_t) newIndex];
        for (size_t i = 0; i < Hoonaar::Radif::tonics().size(); ++i)
        {
            const auto& option = Hoonaar::Radif::tonics()[i];
            if (option.name == currentMode.referenceTonicName)
            {
                audioProcessor.setTonicIndex((int) i);
                tonicBox.setSelectedId((int) i + 1, juce::dontSendNotification);
                break;
            }
        }
        refreshGushehBox();
        repaint();
    };

    tonicBox.onChange = [this]
    {
        const auto selected = tonicBox.getSelectedId() - 1;
        if (selected >= 0)
        {
            audioProcessor.setTonicIndex(selected);
            repaint();
        }
    };

    gushehBox.onChange = [this]
    {
        const auto selected = gushehBox.getSelectedId() - 1;
        if (selected >= 0)
        {
            audioProcessor.setGushehIndex(selected);
            repaint();
        }
    };

    modeBox.setSelectedId(audioProcessor.getModeIndex() + 1, juce::dontSendNotification);
    tonicBox.setSelectedId(audioProcessor.getTonicIndex() + 1, juce::dontSendNotification);
    refreshGushehBox();
}

void HoonaarRadifAtlasAudioProcessorEditor::populateModeBox()
{
    modeBox.clear(juce::dontSendNotification);
    const auto& allModes = Hoonaar::Radif::modes();
    for (size_t i = 0; i < allModes.size(); ++i)
        modeBox.addItem(allModes[i].name, (int) i + 1);
}

void HoonaarRadifAtlasAudioProcessorEditor::populateTonicBox()
{
    tonicBox.clear(juce::dontSendNotification);
    const auto& allTonics = Hoonaar::Radif::tonics();
    for (size_t i = 0; i < allTonics.size(); ++i)
        tonicBox.addItem(allTonics[i].name, (int) i + 1);
}

void HoonaarRadifAtlasAudioProcessorEditor::refreshGushehBox()
{
    gushehBox.clear(juce::dontSendNotification);
    const auto& allModes = Hoonaar::Radif::modes();
    if (allModes.empty())
        return;

    const auto modeIndex = juce::jlimit(0, (int) allModes.size() - 1, audioProcessor.getModeIndex());
    const auto& gushehs = allModes[(size_t) modeIndex].gushehs;
    for (size_t i = 0; i < gushehs.size(); ++i)
        gushehBox.addItem(gushehs[i].name, (int) i + 1);

    const auto selected = juce::jlimit(0, juce::jmax(0, (int) gushehs.size() - 1), audioProcessor.getGushehIndex());
    if (!gushehs.empty())
        gushehBox.setSelectedId(selected + 1, juce::dontSendNotification);
}

std::array<HoonaarRadifAtlasAudioProcessorEditor::SpelledNote, 7>
HoonaarRadifAtlasAudioProcessorEditor::buildSpelledNotes(
    const Hoonaar::Radif::Mode& mode, const Hoonaar::Radif::TonicOption& tonic) const
{
    static const juce::String letters[] = { "C", "D", "E", "F", "G", "A", "B" };
    static constexpr int naturalQuarterSteps[] = { 0, 4, 8, 10, 14, 18, 22 };

    std::array<SpelledNote, 7> result;
    const int rootAbsoluteQuarterStep = 4 * 24 + tonic.pitchClass * 2;

    for (int i = 0; i < 7; ++i)
    {
        auto& note = result[(size_t) i];
        const int totalLetter = tonic.letterIndex + i;
        const int letterIndex = totalLetter % 7;
        const int octave = 4 + totalLetter / 7;
        const int targetPitch = rootAbsoluteQuarterStep + mode.scale24[(size_t) i];
        const int naturalPitch = octave * 24 + naturalQuarterSteps[letterIndex];

        int alteration = targetPitch - naturalPitch;
        while (alteration > 12) alteration -= 24;
        while (alteration < -12) alteration += 24;

        note.accidentalQuarterSteps = alteration;
        note.accidentalName = Hoonaar::Radif::accidentalName(alteration);
        note.accidentalSymbol = accidentalSymbolFor(alteration);
        note.scaleDegree = i + 1;
        note.centsFromTonic = mode.scale24[(size_t) i] * 50;
        note.staffSteps = (octave * 7 + letterIndex) - (4 * 7 + 2); // E4 is the bottom line.
        note.label = letters[letterIndex];
        if (alteration != 0)
            note.label += " " + note.accidentalName;
    }
    return result;
}

void HoonaarRadifAtlasAudioProcessorEditor::drawTrebleClef(juce::Graphics& g,
                                                            float x, float top) const
{
    g.setColour(ink);
    g.setFont(juce::Font("Georgia", 62.0f, juce::Font::italic));
    g.drawText("G", juce::Rectangle<float>(x - 2.0f, top + 5.0f, 56.0f, 115.0f),
               juce::Justification::centred, false);

    // A simple spiral and vertical stroke makes the G-clef cue visible even if a music font is absent.
    juce::Path spiral;
    spiral.startNewSubPath(x + 28.0f, top + 46.0f);
    spiral.cubicTo(x + 10.0f, top + 41.0f, x + 10.0f, top + 65.0f, x + 29.0f, top + 70.0f);
    spiral.cubicTo(x + 47.0f, top + 75.0f, x + 47.0f, top + 96.0f, x + 31.0f, top + 105.0f);
    spiral.cubicTo(x + 15.0f, top + 114.0f, x + 5.0f, top + 102.0f, x + 13.0f, top + 91.0f);
    g.strokePath(spiral, juce::PathStrokeType(2.2f));
    g.drawLine(x + 30.0f, top + 4.0f, x + 30.0f, top + 112.0f, 2.2f);
    g.drawEllipse(x + 25.0f, top + 57.0f, 11.0f, 11.0f, 2.0f);
}

void HoonaarRadifAtlasAudioProcessorEditor::drawAccidental(juce::Graphics& g,
                                                            const juce::String& symbol,
                                                            const juce::String& fallback,
                                                            float x, float y,
                                                            float width) const
{
    if (symbol.isEmpty() && fallback.isEmpty())
        return;

    g.setColour(ink);
    g.setFont(juce::Font("Segoe UI Symbol", 19.0f, juce::Font::plain));
    g.drawText(symbol, juce::Rectangle<float>(x, y - 14.0f, width, 22.0f),
               juce::Justification::centred, false);

    g.setFont(juce::Font("Arial", 8.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff9b6d20));
    g.drawText(fallback, juce::Rectangle<float>(x, y + 5.0f, width, 11.0f),
               juce::Justification::centred, false);
}

void HoonaarRadifAtlasAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto w = (float) getWidth();
    const auto h = (float) getHeight();

    g.fillAll(juce::Colour(0xff08141b));
    g.setColour(juce::Colour(0xff11232d));
    g.fillRect(0.0f, 0.0f, w, 91.0f);
    g.setColour(gold);
    g.fillRect(0.0f, 88.0f, w, 3.0f);

    g.setColour(juce::Colour(0xffe6c273));
    g.setFont(juce::Font("Arial", 12.0f, juce::Font::bold));
    g.drawText("HOONAAR AUDIO", 30, 18, 220, 22, juce::Justification::centredLeft, false);
    g.setColour(juce::Colour(0xfff2f0e9));
    g.setFont(juce::Font("Arial", 29.0f, juce::Font::bold));
    g.drawText("Radif Atlas", 28, 38, 390, 38, juce::Justification::centredLeft, false);
    g.setColour(juce::Colour(0xff9bb0b8));
    g.setFont(juce::Font("Arial", 12.0f, juce::Font::plain));
    g.drawText("Iranian modal staff guide  •  Educational reference", 424, 48, 440, 24,
               juce::Justification::centredLeft, false);

    g.setColour(juce::Colour(0xff15313b));
    g.fillRoundedRectangle(w - 247.0f, 28.0f, 215.0f, 34.0f, 17.0f);
    g.setColour(teal);
    g.setFont(juce::Font("Arial", 10.0f, juce::Font::bold));
    g.drawText("24-EDO REFERENCE  •  v0.1", w - 239.0f, 37.0f, 200.0f, 16.0f,
               juce::Justification::centred, false);

    g.setColour(darkPanel);
    g.fillRoundedRectangle(20.0f, 98.0f, w - 40.0f, 80.0f, 12.0f);
    g.setColour(juce::Colour(0xff263e49));
    g.drawRoundedRectangle(20.0f, 98.0f, w - 40.0f, 80.0f, 12.0f, 1.0f);

    const auto& allModes = Hoonaar::Radif::modes();
    const auto& allTonics = Hoonaar::Radif::tonics();
    if (allModes.empty() || allTonics.empty())
    {
        g.setColour(juce::Colours::white);
        g.drawText("Radif catalog could not be loaded.", 30, 220, getWidth() - 60, 40,
                   juce::Justification::centred, false);
        return;
    }

    const auto modeIndex = juce::jlimit(0, (int) allModes.size() - 1, audioProcessor.getModeIndex());
    const auto tonicIndex = juce::jlimit(0, (int) allTonics.size() - 1, audioProcessor.getTonicIndex());
    const auto gushehIndex = juce::jlimit(0,
        juce::jmax(0, (int) allModes[(size_t) modeIndex].gushehs.size() - 1),
        audioProcessor.getGushehIndex());
    const auto& mode = allModes[(size_t) modeIndex];
    const auto& tonic = allTonics[(size_t) tonicIndex];
    const auto& gushehs = mode.gushehs;
    const auto notes = buildSpelledNotes(mode, tonic);
    const Hoonaar::Radif::Gusheh* gusheh = gushehs.empty() ? nullptr : &gushehs[(size_t) gushehIndex];

    const juce::Rectangle<float> staffPanel(20.0f, 190.0f, w - 40.0f, 352.0f);
    g.setColour(cream);
    g.fillRoundedRectangle(staffPanel, 13.0f);
    g.setColour(juce::Colour(0xffd6d0c5));
    g.drawRoundedRectangle(staffPanel, 13.0f, 1.0f);

    g.setColour(ink);
    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
    g.drawText(mode.family.toUpperCase() + "  /  " + mode.name.toUpperCase(),
               40, 205, 420, 20, juce::Justification::centredLeft, false);
    g.setColour(mutedInk);
    g.setFont(juce::Font("Arial", 10.0f, juce::Font::plain));
    g.drawText("GUSHEH  •  " + (gusheh != nullptr ? gusheh->name : juce::String("Not listed")),
               40, 226, 500, 18, juce::Justification::centredLeft, false);
    const auto roleNote = [&notes](int degree)
    {
        return degree > 0 ? notes[(size_t) ((degree - 1) % 7)].label : juce::String("Not encoded");
    };
    const auto shahidTop = gusheh != nullptr ? roleNote(gusheh->shahidDegree) : juce::String("Not encoded");
    const auto istTop = gusheh != nullptr ? roleNote(gusheh->istDegree) : juce::String("Not encoded");
    g.setColour(juce::Colour(0xff52666d));
    g.setFont(juce::Font("Arial", 9.0f, juce::Font::bold));
    g.drawText(juce::String("SHAHID  ") + shahidTop + "   /   IST  " + istTop,
               565, 226, (int) w - 605, 18, juce::Justification::centredRight, false);
    g.setColour(mutedInk);
    g.setFont(juce::Font("Arial", 10.0f, juce::Font::plain));
    g.drawText("Custom scale signature: explicit accidentals for this 7-note reference",
               510, 205, (int) w - 550, 18, juce::Justification::centredRight, false);

    const float staffTop = 282.0f;
    const float staffBottom = 378.0f;
    const float staffStepHeight = 12.0f;
    const float staffLeft = 46.0f;
    const float staffRight = w - 42.0f;

    g.setColour(juce::Colour(0xff26343a));
    for (int i = 0; i < 5; ++i)
    {
        const float y = staffTop + (float) i * 24.0f;
        g.drawLine(staffLeft, y, staffRight, y, 1.15f);
    }

    drawTrebleClef(g, 53.0f, staffTop - 7.0f);
    g.setColour(mutedInk);
    g.setFont(juce::Font("Arial", 9.0f, juce::Font::bold));
    g.drawText("TREBLE", 46, 411, 62, 14, juce::Justification::centred, false);

    g.setColour(juce::Colour(0xffb2b8b3));
    g.drawLine(124.0f, 252.0f, 124.0f, 423.0f, 1.0f);

    const float keyX = 139.0f;
    const float keySpacing = 27.0f;
    for (int i = 0; i < 7; ++i)
    {
        const auto& note = notes[(size_t) i];
        const float y = staffBottom - (float) note.staffSteps * staffStepHeight;
        drawAccidental(g, note.accidentalSymbol, accidentalFallbackFor(note.accidentalQuarterSteps),
                       keyX + (float) i * keySpacing, y, 25.0f);
    }
    g.setColour(mutedInk);
    g.setFont(juce::Font("Arial", 8.0f, juce::Font::plain));
    g.drawText("SCALE SIGNATURE", 134, 423, 200, 13, juce::Justification::centredLeft, false);

    const float noteStart = 364.0f;
    const float noteEnd = w - 64.0f;
    const float noteSpacing = (noteEnd - noteStart) / 6.0f;
    for (int i = 0; i < 7; ++i)
    {
        const auto& note = notes[(size_t) i];
        const float x = noteStart + (float) i * noteSpacing;
        const float y = staffBottom - (float) note.staffSteps * staffStepHeight;

        // Ledger lines for notes outside the five-line staff.
        if (note.staffSteps < 0)
        {
            for (int step = -2; step >= note.staffSteps; step -= 2)
            {
                const float ledgerY = staffBottom - (float) step * staffStepHeight;
                g.setColour(juce::Colour(0xff26343a));
                g.drawLine(x - 12.0f, ledgerY, x + 12.0f, ledgerY, 1.0f);
            }
        }
        else if (note.staffSteps > 8)
        {
            for (int step = 10; step <= note.staffSteps; step += 2)
            {
                const float ledgerY = staffBottom - (float) step * staffStepHeight;
                g.setColour(juce::Colour(0xff26343a));
                g.drawLine(x - 12.0f, ledgerY, x + 12.0f, ledgerY, 1.0f);
            }
        }

        const bool isShahid = gusheh != nullptr && gusheh->shahidDegree > 0
                           && ((gusheh->shahidDegree - 1) % 7) == i;
        const bool isIst = gusheh != nullptr && gusheh->istDegree > 0
                        && ((gusheh->istDegree - 1) % 7) == i;
        if (isShahid || isIst)
        {
            g.setColour(isShahid ? gold : teal);
            g.drawEllipse(x - 15.0f, y - 12.0f, 30.0f, 24.0f, 2.0f);
            if (isShahid && isIst)
            {
                g.setColour(teal);
                g.drawEllipse(x - 19.0f, y - 16.0f, 38.0f, 32.0f, 1.2f);
            }
        }

        drawAccidental(g, note.accidentalSymbol, accidentalFallbackFor(note.accidentalQuarterSteps),
                       x - 34.0f, y, 28.0f);
        g.setColour(ink);
        g.fillEllipse(x - 8.5f, y - 5.5f, 17.0f, 11.0f);
        if (note.staffSteps <= 4)
            g.drawLine(x + 7.0f, y - 1.0f, x + 7.0f, y - 34.0f, 1.6f);
        else
            g.drawLine(x - 7.0f, y + 1.0f, x - 7.0f, y + 34.0f, 1.6f);

        g.setColour(ink);
        g.setFont(juce::Font("Arial", 10.0f, juce::Font::bold));
        g.drawText(note.label, (int) (x - 43.0f), 435, 86, 16,
                   juce::Justification::centred, false);
        g.setColour(mutedInk);
        g.setFont(juce::Font("Arial", 8.5f, juce::Font::plain));
        g.drawText(juce::String(i + 1) + "  •  +" + juce::String(note.centsFromTonic) + "¢",
                   (int) (x - 43.0f), 452, 86, 13, juce::Justification::centred, false);
    }

    g.setColour(mutedInk);
    g.setFont(juce::Font("Arial", 9.0f, juce::Font::plain));
    g.drawText("Koron = quarter-tone flat  •  Sori = quarter-tone sharp  •  24-EDO reference only",
               42, 511, (int) w - 84, 18, juce::Justification::centredLeft, false);

    const float cardY = 558.0f;
    const float cardGap = 14.0f;
    const float cardW = (w - 40.0f - 2.0f * cardGap) / 3.0f;
    const float cardH = 112.0f;
    const float cardXs[] = { 20.0f, 20.0f + cardW + cardGap, 20.0f + 2.0f * (cardW + cardGap) };
    const juce::String cardLabels[] = { "TONIC", "SHAHID", "IST" };

    int referenceDegrees[] = { 0, gusheh != nullptr ? gusheh->shahidDegree : 0,
                                  gusheh != nullptr ? gusheh->istDegree : 0 };
    const juce::String tonicValue = tonic.name;
    juce::String values[3] = { tonicValue, "Not encoded", "Not encoded" };
    juce::String subValues[3] = { "Selected starting pitch", "Source-specific role", "Source-specific role" };
    if (referenceDegrees[1] > 0)
    {
        const int idx = (referenceDegrees[1] - 1) % 7;
        values[1] = notes[(size_t) idx].label;
        subValues[1] = degreeText(referenceDegrees[1]) + (referenceDegrees[1] > 7 ? " (upper octave)" : "");
    }
    if (referenceDegrees[2] > 0)
    {
        const int idx = (referenceDegrees[2] - 1) % 7;
        values[2] = notes[(size_t) idx].label;
        subValues[2] = degreeText(referenceDegrees[2]) + (referenceDegrees[2] > 7 ? " (upper octave)" : "");
    }

    for (int i = 0; i < 3; ++i)
    {
        const juce::Rectangle<float> card(cardXs[i], cardY, cardW, cardH);
        g.setColour(darkPanel);
        g.fillRoundedRectangle(card, 11.0f);
        g.setColour(juce::Colour(0xff29414b));
        g.drawRoundedRectangle(card, 11.0f, 1.0f);
        g.setColour(i == 1 ? gold : (i == 2 ? teal : juce::Colour(0xff9bb0b8)));
        g.setFont(juce::Font("Arial", 9.5f, juce::Font::bold));
        g.drawText(cardLabels[i], (int) card.getX() + 14, (int) card.getY() + 13,
                   (int) card.getWidth() - 28, 16, juce::Justification::centredLeft, false);
        g.setColour(juce::Colour(0xfff4f3ee));
        g.setFont(juce::Font("Arial", 20.0f, juce::Font::bold));
        g.drawText(values[i], (int) card.getX() + 14, (int) card.getY() + 37,
                   (int) card.getWidth() - 28, 30, juce::Justification::centredLeft, false);
        g.setColour(juce::Colour(0xff9bb0b8));
        g.setFont(juce::Font("Arial", 9.5f, juce::Font::plain));
        g.drawText(subValues[i], (int) card.getX() + 14, (int) card.getY() + 75,
                   (int) card.getWidth() - 28, 20, juce::Justification::centredLeft, false);
    }

    g.setColour(juce::Colour(0xff82979e));
    g.setFont(juce::Font("Arial", 8.5f, juce::Font::plain));
    g.drawText("Gusheh names and note functions vary by radif. Uncoded Shahid/Ist values are left blank rather than guessed.",
               25, h - 22.0f, w - 50.0f, 15.0f, juce::Justification::centred, false);
}

void HoonaarRadifAtlasAudioProcessorEditor::resized()
{
    const auto w = getWidth();
    modeLabel.setBounds(36, 103, 280, 18);
    modeBox.setBounds(34, 124, juce::jmin(290, w / 3), 36);

    gushehLabel.setBounds(351, 103, 370, 18);
    gushehBox.setBounds(349, 124, juce::jmin(390, w / 2), 36);

    tonicLabel.setBounds(w - 212, 103, 180, 18);
    tonicBox.setBounds(w - 214, 124, 180, 36);
}
