#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

class DataBenderJuceAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer, public juce::Slider::Listener
{
public:
    DataBenderJuceAudioProcessorEditor(DataBenderJuceAudioProcessor&);
    ~DataBenderJuceAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    DataBenderJuceAudioProcessor& processor;
    
    // VU Meter components
    juce::Label levelLabelL;
    juce::Label levelLabelR;
    
    // Gain controls
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    juce::Label inputGainLabel;
    juce::Label outputGainLabel;
    
    // Custom level meter display
    float levelL = 0.0f;
    float levelR = 0.0f;
    
    // Helper method to draw VU meters
    void drawVUMeter(juce::Graphics& g, juce::Rectangle<int> bounds, float level, const juce::String& label);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataBenderJuceAudioProcessorEditor)
}; 