#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <array>

class HoonaarRadifAtlasAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit HoonaarRadifAtlasAudioProcessorEditor(HoonaarRadifAtlasAudioProcessor&);
    ~HoonaarRadifAtlasAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    struct SpelledNote
    {
        juce::String label;
        juce::String accidentalName;
        juce::String accidentalSymbol;
        int accidentalQuarterSteps = 0;
        int staffSteps = 0;
        int scaleDegree = 1;
        int centsFromTonic = 0;
    };

    HoonaarRadifAtlasAudioProcessor& audioProcessor;
    juce::ComboBox modeBox;
    juce::ComboBox gushehBox;
    juce::ComboBox tonicBox;
    juce::Label modeLabel;
    juce::Label gushehLabel;
    juce::Label tonicLabel;

    void populateModeBox();
    void populateTonicBox();
    void refreshGushehBox();
    std::array<SpelledNote, 7> buildSpelledNotes(const Hoonaar::Radif::Mode&,
                                                  const Hoonaar::Radif::TonicOption&) const;
    void drawTrebleClef(juce::Graphics&, float x, float top) const;
    void drawAccidental(juce::Graphics&, const juce::String& symbol,
                        const juce::String& fallback, float x, float y,
                        float width = 24.0f) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HoonaarRadifAtlasAudioProcessorEditor)
};
