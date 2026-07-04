#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayPluginAudioProcessor::DelayPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", {
          std::make_unique<juce::AudioParameterFloat>("tempo", "Tempo", 0.0f, 1.0f, 0.5f),
          std::make_unique<juce::AudioParameterFloat>("regen", "Regen", 0.0f, 1.0f, 0.3f),
          std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 0.5f),
          std::make_unique<juce::AudioParameterFloat>("output", "Output", 0.0f, 1.0f, 0.8f),
          std::make_unique<juce::AudioParameterChoice>("mode", "Mode", juce::StringArray{"STUDIO", "CREATIVE"}, 0),
          std::make_unique<juce::AudioParameterBool>("autoGain", "Auto Gain", true)
      }) {}

DelayPluginAudioProcessor::~DelayPluginAudioProcessor() {}

void DelayPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    delayLine.prepare({ sampleRate, (juce::uint32) samplesPerBlock, 2 });
    delayLine.setMaximumDelayInSamples((int)(sampleRate * 4.0));
}

void DelayPluginAudioProcessor::releaseResources() {}
bool DelayPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) return false;
    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

void DelayPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) buffer.clear(i, 0, buffer.getNumSamples());

    feedback = *parameters.getRawParameterValue("regen");
    mix = *parameters.getRawParameterValue("mix");
    outputGain = *parameters.getRawParameterValue("output");
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto channelData = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            float dry = channelData[i];
            float wet = delayLine.popSample(channel);
            delayLine.pushSample(channel, dry + wet * feedback);
            channelData[i] = (dry * (1.0f - mix) + wet * mix) * outputGain;
        }
    }
}

juce::AudioProcessorEditor* DelayPluginAudioProcessor::createEditor() { return new DelayPluginAudioProcessorEditor(*this); }
void DelayPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}
void DelayPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr) if (xmlState->hasTagName(parameters.state.getType())) parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new DelayPluginAudioProcessor(); }
