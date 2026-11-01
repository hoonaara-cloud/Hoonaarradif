#pragma once

#include <JuceHeader.h>
#include "RadifData.h"
#include <atomic>

class HoonaarRadifAtlasAudioProcessor final : public juce::AudioProcessor
{
public:
    HoonaarRadifAtlasAudioProcessor();
    ~HoonaarRadifAtlasAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destinationData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    int getModeIndex() const noexcept { return modeIndex.load(); }
    int getTonicIndex() const noexcept { return tonicIndex.load(); }
    int getGushehIndex() const noexcept { return gushehIndex.load(); }

    void setModeIndex(int value) noexcept;
    void setTonicIndex(int value) noexcept;
    void setGushehIndex(int value) noexcept;

private:
    std::atomic<int> modeIndex { 0 };
    std::atomic<int> tonicIndex { 10 }; // G is a useful first view for Shur.
    std::atomic<int> gushehIndex { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HoonaarRadifAtlasAudioProcessor)
};
