#include "PluginProcessor.h"
#include "PluginEditor.h"

HoonaarRadifAtlasAudioProcessor::HoonaarRadifAtlasAudioProcessor()
    : juce::AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void HoonaarRadifAtlasAudioProcessor::prepareToPlay(double, int)
{
}

void HoonaarRadifAtlasAudioProcessor::releaseResources()
{
}

bool HoonaarRadifAtlasAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();
    return input == output && (input == juce::AudioChannelSet::mono()
                            || input == juce::AudioChannelSet::stereo());
}

void HoonaarRadifAtlasAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    // This is a silent educational guide, not a sound generator or analyser.
    // Leaving the input untouched makes it safe to insert on an existing REAPER track.
    const auto inputChannels = getTotalNumInputChannels();
    const auto outputChannels = getTotalNumOutputChannels();
    for (int channel = inputChannels; channel < outputChannels; ++channel)
        buffer.clear(channel, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* HoonaarRadifAtlasAudioProcessor::createEditor()
{
    return new HoonaarRadifAtlasAudioProcessorEditor(*this);
}

void HoonaarRadifAtlasAudioProcessor::setModeIndex(int value) noexcept
{
    const auto count = (int) Hoonaar::Radif::modes().size();
    modeIndex.store(juce::jlimit(0, juce::jmax(0, count - 1), value));
    gushehIndex.store(0);
}

void HoonaarRadifAtlasAudioProcessor::setTonicIndex(int value) noexcept
{
    const auto count = (int) Hoonaar::Radif::tonics().size();
    tonicIndex.store(juce::jlimit(0, juce::jmax(0, count - 1), value));
}

void HoonaarRadifAtlasAudioProcessor::setGushehIndex(int value) noexcept
{
    const auto& allModes = Hoonaar::Radif::modes();
    const auto mode = juce::jlimit(0, juce::jmax(0, (int) allModes.size() - 1), modeIndex.load());
    const auto count = allModes.empty() ? 0 : (int) allModes[(size_t) mode].gushehs.size();
    gushehIndex.store(juce::jlimit(0, juce::jmax(0, count - 1), value));
}

void HoonaarRadifAtlasAudioProcessor::getStateInformation(juce::MemoryBlock& destinationData)
{
    juce::XmlElement state("HOONAAR_RADIF_ATLAS_STATE");
    state.setAttribute("mode", modeIndex.load());
    state.setAttribute("tonic", tonicIndex.load());
    state.setAttribute("gusheh", gushehIndex.load());
    copyXmlToBinary(state, destinationData);
}

void HoonaarRadifAtlasAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> state(getXmlFromBinary(data, sizeInBytes));
    if (state == nullptr || !state->hasTagName("HOONAAR_RADIF_ATLAS_STATE"))
        return;

    const auto mode = state->getIntAttribute("mode", 0);
    const auto tonic = state->getIntAttribute("tonic", 10);
    const auto gusheh = state->getIntAttribute("gusheh", 0);

    setModeIndex(mode);
    setTonicIndex(tonic);
    setGushehIndex(gusheh);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HoonaarRadifAtlasAudioProcessor();
}
