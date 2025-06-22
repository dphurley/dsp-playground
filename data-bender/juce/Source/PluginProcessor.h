#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include "../../core/DataBenderEngine.hpp"

class DataBenderJuceAudioProcessor : public juce::AudioProcessor {
public:
    DataBenderJuceAudioProcessor();
    ~DataBenderJuceAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Level monitoring for VU meter
    float getLevel(int channel) const;

    // Gain parameters
    void setInputGain(float gain) { inputGain = gain; }
    void setOutputGain(float gain) { outputGain = gain; }
    float getInputGain() const { return inputGain; }
    float getOutputGain() const { return outputGain; }

private:
    DataBenderEngine dspEngine;
    
    // Level monitoring - input and output
    juce::LinearSmoothedValue<float> inputLevelL;
    juce::LinearSmoothedValue<float> inputLevelR;
    juce::LinearSmoothedValue<float> outputLevelL;
    juce::LinearSmoothedValue<float> outputLevelR;
    
    // Raw level values for immediate access
    float rawInputLevelL = 0.0f;
    float rawInputLevelR = 0.0f;
    float rawOutputLevelL = 0.0f;
    float rawOutputLevelR = 0.0f;
    
    // Gain parameters
    float inputGain = 1.0f;
    float outputGain = 1.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataBenderJuceAudioProcessor)
}; 