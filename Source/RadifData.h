#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

namespace Hoonaar::Radif
{
    struct Gusheh
    {
        juce::String name;
        int shahidDegree = 0; // 1-based scale degree; 0 means not source-coded.
        int istDegree = 0;
    };

    struct Mode
    {
        juce::String name;
        juce::String family;
        juce::String referenceTonicName;
        int referenceTonic = 0; // MIDI pitch class: C=0 ... B=11.
        std::array<int, 7> scale24{}; // 24-EDO quarter-step offsets from the tonic.
        std::vector<Gusheh> gushehs;
    };

    struct TonicOption
    {
        juce::String name;
        int pitchClass = 0;
        int letterIndex = 0; // C,D,E,F,G,A,B = 0..6; used to spell the staff.
    };

    const std::vector<Mode>& modes();
    const std::vector<TonicOption>& tonics();
    juce::String noteNameForQuarterStep(int absoluteQuarterStep, bool preferFlats = false);
    juce::String accidentalName(int quarterStepAlteration);
}
