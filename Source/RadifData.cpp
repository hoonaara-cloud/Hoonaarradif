#include "RadifData.h"
#include <BinaryData.h>

namespace Hoonaar::Radif
{
    namespace
    {
        juce::var property(const juce::DynamicObject& object, const char* name)
        {
            return object.getProperty(juce::Identifier(name));
        }

        int integerOrZero(const juce::var& value)
        {
            return value.isInt() || value.isInt64() || value.isDouble() ? (int) value : 0;
        }

        juce::String pitchClassName(int pitchClass, bool preferFlats)
        {
            static const char* sharps[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
            static const char* flats[]  = { "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B" };
            const auto pc = (pitchClass % 12 + 12) % 12;
            return preferFlats ? flats[pc] : sharps[pc];
        }
    }

    const std::vector<Mode>& modes()
    {
        static const auto parsedModes = []
        {
            std::vector<Mode> result;
            const auto jsonText = juce::String::fromUTF8(BinaryData::radif_data_json,
                                                          BinaryData::radif_data_jsonSize);
            const auto root = juce::JSON::parse(jsonText);
            const auto* rootObject = root.getDynamicObject();
            if (rootObject == nullptr)
                return result;

            const auto modeArrayValue = property(*rootObject, "modes");
            const auto* modeArray = modeArrayValue.getArray();
            if (modeArray == nullptr)
                return result;

            for (const auto& modeValue : *modeArray)
            {
                const auto* modeObject = modeValue.getDynamicObject();
                if (modeObject == nullptr)
                    continue;

                Mode mode;
                mode.name = property(*modeObject, "name").toString();
                mode.family = property(*modeObject, "family").toString();
                mode.referenceTonicName = property(*modeObject, "referenceTonicName").toString();
                mode.referenceTonic = integerOrZero(property(*modeObject, "referenceTonic"));

                const auto scaleValue = property(*modeObject, "scale24");
                if (const auto* scaleArray = scaleValue.getArray())
                {
                    for (int i = 0; i < juce::jmin(7, (int) scaleArray->size()); ++i)
                        mode.scale24[(size_t) i] = integerOrZero((*scaleArray)[i]);
                }

                const auto gushehValue = property(*modeObject, "gushehs");
                if (const auto* gushehArray = gushehValue.getArray())
                {
                    for (const auto& gushehValueItem : *gushehArray)
                    {
                        const auto* gushehObject = gushehValueItem.getDynamicObject();
                        if (gushehObject == nullptr)
                            continue;

                        Gusheh gusheh;
                        gusheh.name = property(*gushehObject, "name").toString();
                        gusheh.shahidDegree = integerOrZero(property(*gushehObject, "shahid"));
                        gusheh.istDegree = integerOrZero(property(*gushehObject, "ist"));
                        mode.gushehs.push_back(std::move(gusheh));
                    }
                }

                if (mode.name.isNotEmpty())
                    result.push_back(std::move(mode));
            }
            return result;
        }();

        return parsedModes;
    }

    const std::vector<TonicOption>& tonics()
    {
        static const std::vector<TonicOption> options = {
            { "C", 0, 0 }, { "C#", 1, 0 }, { "Db", 1, 1 },
            { "D", 2, 1 }, { "D#", 3, 1 }, { "Eb", 3, 2 },
            { "E", 4, 2 }, { "F", 5, 3 }, { "F#", 6, 3 },
            { "Gb", 6, 4 }, { "G", 7, 4 }, { "G#", 8, 4 },
            { "Ab", 8, 5 }, { "A", 9, 5 }, { "A#", 10, 5 },
            { "Bb", 10, 6 }, { "B", 11, 6 }
        };
        return options;
    }

    juce::String noteNameForQuarterStep(int absoluteQuarterStep, bool preferFlats)
    {
        const auto pitch = (absoluteQuarterStep % 24 + 24) % 24;
        const auto lowerSemitone = pitch / 2;
        if ((pitch % 2) == 0)
            return pitchClassName(lowerSemitone, preferFlats);

        if (preferFlats)
            return pitchClassName(lowerSemitone + 1, true) + " Koron";
        return pitchClassName(lowerSemitone, false) + " Sori";
    }

    juce::String accidentalName(int quarterStepAlteration)
    {
        switch (quarterStepAlteration)
        {
            case -4: return "Double flat";
            case -3: return "Flat + Koron";
            case -2: return "Flat";
            case -1: return "Koron";
            case  0: return "Natural";
            case  1: return "Sori";
            case  2: return "Sharp";
            case  3: return "Sharp + Sori";
            case  4: return "Double sharp";
            default: return quarterStepAlteration < 0 ? "Lowered" : "Raised";
        }
    }
}
