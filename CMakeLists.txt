cmake_minimum_required(VERSION 3.15)
project(DelayPlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# Download CPM.cmake to manage dependencies automatically
if(NOT EXISTS ${CMAKE_BINARY_DIR}/cmake/CPM.cmake)
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake ${CMAKE_BINARY_DIR}/cmake/CPM.cmake)
endif()
include(${CMAKE_BINARY_DIR}/cmake/CPM.cmake)

# Fetch JUCE 7.0.12
CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG 7.0.12
)

juce_add_plugin(DelayPlugin
    COMPANY_NAME "CustomDSP"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Cstm
    PLUGIN_CODE Dly1
    FORMATS VST3
    PRODUCT_NAME "Delay Plugin"
)

target_sources(DelayPlugin PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(DelayPlugin PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(DelayPlugin PRIVATE
    juce::juce_audio_utils
    juce::juce_gui_extra
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
