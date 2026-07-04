#pragma once
#include "PluginProcessor.h"
#include "CustomComponents.h"

class DelayPluginAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    DelayPluginAudioProcessorEditor(DelayPluginAudioProcessor&);
    ~DelayPluginAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DelayPluginAudioProcessor& processorRef;
    DelayLookAndFeel lookAndFeel;
    
    Header header;
    ToggleSwitch studioToggle;
    ToggleSwitch autoGainToggle;
    
    AnimatedKnob tempoKnob {"TEMPO", "1/8"};
    AnimatedKnob regenKnob {"REGEN"};
    AnimatedKnob mixKnob {"MIX"};
    AnimatedKnob outputKnob {"OUTPUT"};
    
    IconButton brightnessBtn {"BRIGHTNESS"};
    IconButton colorBtn {"COLOR"};
    IconButton sparkleBtn {"SPARKLE"};
    
    juce::Label inLabel, outLabel;
    LevelMeter inMeter, outMeter;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tempoAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> regenAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayPluginAudioProcessorEditor)
};
