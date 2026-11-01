cmake_minimum_required(VERSION 3.22)
project(HoonaarRadifAtlas VERSION 0.1.0 LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 9.0.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_binary_data(AtlasData
    SOURCES
        Data/radif_data.json
)

juce_add_plugin(HoonaarRadifAtlas
    COMPANY_NAME "Hoonaar Audio"
    BUNDLE_ID com.hoonaaraudio.radifatlas
    PLUGIN_MANUFACTURER_CODE HoAu
    PLUGIN_CODE RdAt
    PRODUCT_NAME "Radif Atlas"
    DESCRIPTION "Iranian dastgah and avaz staff notation guide"
    FORMATS VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
)

juce_generate_juce_header(HoonaarRadifAtlas)

target_sources(HoonaarRadifAtlas
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/RadifData.cpp
)

target_compile_features(HoonaarRadifAtlas PRIVATE cxx_std_17)
target_compile_definitions(HoonaarRadifAtlas
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(HoonaarRadifAtlas
    PRIVATE
        AtlasData
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
