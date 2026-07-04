#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayPluginAudioProcessorEditor::DelayPluginAudioProcessorEditor(DelayPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p),
      studioToggle("STUDIO", "CREATIVE", juce::Colour::fromString("FF60A5FA")),
      autoGainToggle("AUTO", "GAIN", juce::Colour::fromString("FF4ADE80")) {
    setSize(300, 400);
    setLookAndFeel(&lookAndFeel);
    
    addAndMakeVisible(header);
    addAndMakeVisible(studioToggle);
    addAndMakeVisible(autoGainToggle);
    
    tempoKnob.setLookAndFeel(&lookAndFeel);
    regenKnob.setLookAndFeel(&lookAndFeel);
    mixKnob.setLookAndFeel(&lookAndFeel);
    outputKnob.setLookAndFeel(&lookAndFeel);
    
    tempoKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromString("FF5EEAD4"));
    regenKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromString("FF374151"));
    mixKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromString("FF374151"));
    outputKnob.setColour(juce::Slider::thumbColourId, juce::Colour::fromString("FF374151"));

    addAndMakeVisible(tempoKnob); addAndMakeVisible(regenKnob);
    addAndMakeVisible(mixKnob); addAndMakeVisible(outputKnob);
    
    addAndMakeVisible(brightnessBtn); addAndMakeVisible(colorBtn); addAndMakeVisible(sparkleBtn);
    
    inLabel.setText("IN", juce::dontSendNotification);
    outLabel.setText("OUT", juce::dontSendNotification);
    inLabel.setColour(juce::Label::textColourId, juce::Colour::fromString("FF6B7280"));
    outLabel.setColour(juce::Label::textColourId, juce::Colour::fromString("FF6B7280"));
    inLabel.setFont(juce::Font(10.0f)); outLabel.setFont(juce::Font(10.0f));
    inLabel.setJustificationType(juce::Justification::centred); outLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(inLabel); addAndMakeVisible(outLabel);
    addAndMakeVisible(inMeter); addAndMakeVisible(outMeter);
    
    tempoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.parameters, "tempo", tempoKnob);
    regenAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.parameters, "regen", regenKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.parameters, "mix", mixKnob);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.parameters, "output", outputKnob);
}

DelayPluginAudioProcessorEditor::~DelayPluginAudioProcessorEditor() { setLookAndFeel(nullptr); }

void DelayPluginAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour::fromString("FFF4F5FF"));
    auto bounds = getLocalBounds();
    bounds.removeFromTop(120); 
    auto toolbarB = bounds.removeFromTop(40);
    
    g.setColour(juce::Colour::fromString("FFFFFFFF"));
    g.fillRect(toolbarB);
    g.fillRect(bounds);
    
    g.setColour(juce::Colour::fromString("FFE5E7EB"));
    g.drawLine(0.0f, 120.0f, 300.0f, 120.0f, 1.0f);
    g.drawLine(0.0f, 160.0f, 300.0f, 160.0f, 1.0f);
}

void DelayPluginAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();
    header.setBounds(bounds.removeFromTop(120));
    
    auto toolbarBounds = bounds.removeFromTop(40);
    studioToggle.setBounds(toolbarBounds.removeFromLeft(150).reduced(10, 5));
    autoGainToggle.setBounds(toolbarBounds.removeFromRight(120).reduced(10, 5));
    
    auto controlsBounds = bounds;
    auto mainControlsBounds = controlsBounds.removeFromLeft(210);
    auto outputBounds = controlsBounds;
    
    auto row1 = mainControlsBounds.removeFromTop(100).reduced(10, 10);
    auto row2 = mainControlsBounds.reduced(10, 10);
    
    tempoKnob.setBounds(row1.removeFromLeft(70));
    regenKnob.setBounds(row1.removeFromLeft(70));
    mixKnob.setBounds(row1.removeFromLeft(70));
    
    brightnessBtn.setBounds(row2.removeFromLeft(65).reduced(5));
    colorBtn.setBounds(row2.removeFromLeft(65).reduced(5));
    sparkleBtn.setBounds(row2.removeFromLeft(65).reduced(5));
    
    auto outTop = outputBounds.removeFromTop(20);
    inLabel.setBounds(outTop.removeFromLeft(45));
    outLabel.setBounds(outTop);
    
    auto meters = outputBounds.removeFromTop(160);
    inMeter.setBounds(meters.removeFromLeft(45).reduced(15, 10));
    outMeter.setBounds(meters.reduced(15, 10));
    
    outputKnob.setBounds(outputBounds.reduced(20));
}
