#include "PluginProcessor.h"
#include "PluginEditor.h"

DataBenderJuceAudioProcessor::DataBenderJuceAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

DataBenderJuceAudioProcessor::~DataBenderJuceAudioProcessor()
{
}

const juce::String DataBenderJuceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DataBenderJuceAudioProcessor::acceptsMidi() const
{
    return false;
}

bool DataBenderJuceAudioProcessor::producesMidi() const
{
    return false;
}

bool DataBenderJuceAudioProcessor::isMidiEffect() const
{
    return false;
}

double DataBenderJuceAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DataBenderJuceAudioProcessor::getNumPrograms()
{
    return 1;
}

int DataBenderJuceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DataBenderJuceAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String DataBenderJuceAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void DataBenderJuceAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void DataBenderJuceAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    dspEngine.init((float)sampleRate);
    
    // Initialize level monitoring
    inputLevelL.reset(sampleRate, 0.1);
    inputLevelR.reset(sampleRate, 0.1);
    outputLevelL.reset(sampleRate, 0.1);
    outputLevelR.reset(sampleRate, 0.1);
    inputLevelL.setCurrentAndTargetValue(0.0f);
    inputLevelR.setCurrentAndTargetValue(0.0f);
    outputLevelL.setCurrentAndTargetValue(0.0f);
    outputLevelR.setCurrentAndTargetValue(0.0f);
}

void DataBenderJuceAudioProcessor::releaseResources()
{
    // DSP engine doesn't have a reset method, so we'll just reinitialize
    dspEngine.init(dspEngine.getSampleRate());
}

bool DataBenderJuceAudioProcessor::isBusesLayoutSupported(const BusesLayout& busesLayout) const
{
    if (busesLayout.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && busesLayout.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (busesLayout.getMainOutputChannelSet() != busesLayout.getMainInputChannelSet())
        return false;

    return true;
}

void DataBenderJuceAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Calculate input levels BEFORE processing
    if (buffer.getNumChannels() > 0) {
        float maxL = 0.0f;
        float maxR = 0.0f;
        
        // Read from the buffer directly
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            float sampleL = buffer.getSample(0, sample);
            float sampleR = buffer.getNumChannels() > 1 ? buffer.getSample(1, sample) : sampleL;
            
            maxL = juce::jmax(maxL, std::abs(sampleL));
            maxR = juce::jmax(maxR, std::abs(sampleR));
        }
        
        // Debug output every 1000 samples
        static int debugCounter = 0;
        debugCounter++;
        if (debugCounter >= 1000) {
            debugCounter = 0;
            std::cout << "Processor Input - L: " << maxL << " R: " << maxR << " Channels: " << buffer.getNumChannels() << " Samples: " << buffer.getNumSamples() << std::endl;
        }
        
        // Apply input gain and set smoothed levels
        inputLevelL.setTargetValue(maxL * inputGain);
        inputLevelR.setTargetValue(maxR * inputGain);
        
        // Store raw values for immediate access
        rawInputLevelL = maxL * inputGain;
        rawInputLevelR = maxR * inputGain;
    }

    // Apply input gain to the buffer
    if (inputGain != 1.0f) {
        buffer.applyGain(inputGain);
    }

    // Process with DSP engine - extract pointers and call with correct interface
    if (buffer.getNumChannels() > 0) {
        const float* inputs[2] = {
            buffer.getReadPointer(0),
            buffer.getNumChannels() > 1 ? buffer.getReadPointer(1) : buffer.getReadPointer(0)
        };
        float* outputs[2] = {
            buffer.getWritePointer(0),
            buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : buffer.getWritePointer(0)
        };
        
        dspEngine.process(inputs, outputs, buffer.getNumSamples());
    }
    
    // Calculate output levels AFTER processing
    if (buffer.getNumChannels() > 0) {
        float maxL = 0.0f;
        float maxR = 0.0f;
        
        // Read from the buffer directly
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            float sampleL = buffer.getSample(0, sample);
            float sampleR = buffer.getNumChannels() > 1 ? buffer.getSample(1, sample) : sampleL;
            
            maxL = juce::jmax(maxL, std::abs(sampleL));
            maxR = juce::jmax(maxR, std::abs(sampleR));
        }
        
        // Debug output every 1000 samples
        static int outputDebugCounter = 0;
        outputDebugCounter++;
        if (outputDebugCounter >= 1000) {
            outputDebugCounter = 0;
            std::cout << "Processor Output - L: " << maxL << " R: " << maxR << std::endl;
        }
        
        // Set smoothed levels (output gain already applied to audio)
        outputLevelL.setTargetValue(maxL);
        outputLevelR.setTargetValue(maxR);
    }

    // Apply output gain
    if (outputGain != 1.0f) {
        buffer.applyGain(outputGain);
    }
}

bool DataBenderJuceAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* DataBenderJuceAudioProcessor::createEditor()
{
    return new DataBenderJuceAudioProcessorEditor(*this);
}

void DataBenderJuceAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ignoreUnused(destData);
}

void DataBenderJuceAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

float DataBenderJuceAudioProcessor::getLevel(int channel) const
{
    if (channel == 0) {
        return rawInputLevelL; // Return raw input level immediately
    } else if (channel == 1) {
        return rawInputLevelR; // Return raw input level immediately
    }
    return 0.0f;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DataBenderJuceAudioProcessor();
} 