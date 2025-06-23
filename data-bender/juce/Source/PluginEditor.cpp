#include "PluginEditor.h"

DataBenderJuceAudioProcessorEditor::DataBenderJuceAudioProcessorEditor(DataBenderJuceAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(500, 350);
    
    // Setup labels
    levelLabelL.setText("L", juce::dontSendNotification);
    levelLabelL.setJustificationType(juce::Justification::centred);
    levelLabelL.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(levelLabelL);
    
    levelLabelR.setText("R", juce::dontSendNotification);
    levelLabelR.setJustificationType(juce::Justification::centred);
    levelLabelR.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(levelLabelR);
    
    // Setup gain controls
    inputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inputGainSlider.setRange(0.0, 4.0, 0.01);
    inputGainSlider.setValue(1.0);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    inputGainSlider.addListener(this);
    inputGainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::orange);
    inputGainSlider.setColour(juce::Slider::trackColourId, juce::Colours::darkorange);
    addAndMakeVisible(inputGainSlider);
    
    outputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputGainSlider.setRange(0.0, 4.0, 0.01);
    outputGainSlider.setValue(1.0);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    outputGainSlider.addListener(this);
    outputGainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::lightblue);
    outputGainSlider.setColour(juce::Slider::trackColourId, juce::Colours::darkblue);
    addAndMakeVisible(outputGainSlider);
    
    inputGainLabel.setText("Input Gain", juce::dontSendNotification);
    inputGainLabel.setJustificationType(juce::Justification::centred);
    inputGainLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    inputGainLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(inputGainLabel);
    
    outputGainLabel.setText("Output Gain", juce::dontSendNotification);
    outputGainLabel.setJustificationType(juce::Justification::centred);
    outputGainLabel.setColour(juce::Label::textColourId, juce::Colours::lightblue);
    outputGainLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(outputGainLabel);
    
    // Setup freeze button
    freezeButton.setButtonText("FREEZE");
    freezeButton.setClickingTogglesState(true);
    freezeButton.addListener(this);
    freezeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    freezeButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    freezeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    freezeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible(freezeButton);
    
    freezeLabel.setText("Buffer Freeze", juce::dontSendNotification);
    freezeLabel.setJustificationType(juce::Justification::centred);
    freezeLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    freezeLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(freezeLabel);
    
    // Setup playback speed slider
    speedSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    speedSlider.setRange(-2, 2, 1); // 5 discrete steps: -2, -1, 0, +1, +2
    speedSlider.setValue(0);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    speedSlider.addListener(this);
    speedSlider.setColour(juce::Slider::thumbColourId, juce::Colours::yellow);
    speedSlider.setColour(juce::Slider::trackColourId, juce::Colours::gold);
    addAndMakeVisible(speedSlider);
    
    speedLabel.setText("Playback Speed (Octaves)", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centred);
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    speedLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(speedLabel);
    
    // Setup repeats slider
    repeatsSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    repeatsSlider.setRange(0.0, 1.0, 0.01);
    repeatsSlider.setValue(0.0);
    repeatsSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    repeatsSlider.addListener(this);
    repeatsSlider.setColour(juce::Slider::thumbColourId, juce::Colours::purple);
    repeatsSlider.setColour(juce::Slider::trackColourId, juce::Colours::darkmagenta);
    addAndMakeVisible(repeatsSlider);
    
    repeatsLabel.setText("Repeats", juce::dontSendNotification);
    repeatsLabel.setJustificationType(juce::Justification::centred);
    repeatsLabel.setColour(juce::Label::textColourId, juce::Colours::purple);
    repeatsLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(repeatsLabel);
    
    // Start timer for VU meter updates
    startTimerHz(30); // Update 30 times per second
}

DataBenderJuceAudioProcessorEditor::~DataBenderJuceAudioProcessorEditor() {
    stopTimer();
}

void DataBenderJuceAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawFittedText("_Data Bender (Echo Devices) - Auto-Rebuild Test", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);
    
    // Draw VU meters
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // Space for title
    bounds.removeFromBottom(280); // Increased space for knobs and labels
    
    auto meterWidth = bounds.getWidth() / 6; // Smaller meters to make room for knobs
    auto meterHeight = bounds.getHeight(); // Use remaining height
    auto centerX = bounds.getCentreX();
    
    // Left meter
    auto meterL = juce::Rectangle<int>(centerX - meterWidth - 10, bounds.getY(), meterWidth, meterHeight);
    drawVUMeter(g, meterL, levelL, "L");
    
    // Right meter
    auto meterR = juce::Rectangle<int>(centerX + 10, bounds.getY(), meterWidth, meterHeight);
    drawVUMeter(g, meterR, levelR, "R");
}

void DataBenderJuceAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // Space for title
    bounds.removeFromBottom(280); // Increased space for knobs and labels
    
    // Layout labels - position them above the knobs
    auto meterWidth = bounds.getWidth() / 6;
    auto centerX = bounds.getCentreX();
    
    // Left label - position above the input gain knob
    levelLabelL.setBounds(centerX - meterWidth - 150, bounds.getBottom() + 20, meterWidth, 20);
    
    // Right label - position above the output gain knob
    levelLabelR.setBounds(centerX + 150, bounds.getBottom() + 20, meterWidth, 20);
    
    // Layout gain controls - make them more visible
    auto knobSize = 100; // Larger knobs
    auto knobY = bounds.getBottom() + 50; // Position below the VU meters
    
    // Input gain (left side) - position more clearly
    inputGainSlider.setBounds(centerX - knobSize - 150, knobY, knobSize, knobSize);
    inputGainLabel.setBounds(centerX - knobSize - 150, knobY - 25, knobSize, 25);
    
    // Output gain (right side) - position more clearly
    outputGainSlider.setBounds(centerX + 150, knobY, knobSize, knobSize);
    outputGainLabel.setBounds(centerX + 150, knobY - 25, knobSize, 25);
    
    // Layout freeze button - center below the VU meters
    auto buttonWidth = 120;
    auto buttonHeight = 40;
    auto buttonX = centerX - buttonWidth / 2;
    auto buttonY = bounds.getBottom() + 10;
    
    freezeButton.setBounds(buttonX, buttonY, buttonWidth, buttonHeight);
    freezeLabel.setBounds(buttonX, buttonY - 25, buttonWidth, 25);
    
    // Layout playback speed slider (centered below freeze button)
    auto speedKnobSize = 80;
    auto speedKnobY = bounds.getBottom() + 110; // Position below freeze button
    speedSlider.setBounds(centerX - speedKnobSize / 2, speedKnobY, speedKnobSize, speedKnobSize);
    speedLabel.setBounds(centerX - speedKnobSize / 2, speedKnobY - 25, speedKnobSize, 25);
    
    // Layout repeats slider (centered below playback speed slider)
    auto repeatsKnobSize = 80;
    auto repeatsKnobY = bounds.getBottom() + 220; // Increased spacing - position further below speed knob
    repeatsSlider.setBounds(centerX - repeatsKnobSize / 2, repeatsKnobY, repeatsKnobSize, repeatsKnobSize);
    repeatsLabel.setBounds(centerX - repeatsKnobSize / 2, repeatsKnobY - 25, repeatsKnobSize, 25);
}

void DataBenderJuceAudioProcessorEditor::timerCallback() {
    // Get current levels from the processor
    levelL = processor.getLevel(0);
    levelR = processor.getLevel(1);
    
    // Sync freeze button state with processor
    bool processorFreezeState = processor.getFreeze();
    if (freezeButton.getToggleState() != processorFreezeState) {
        freezeButton.setToggleState(processorFreezeState, juce::dontSendNotification);
        if (processorFreezeState) {
            freezeButton.setButtonText("UNFREEZE");
        } else {
            freezeButton.setButtonText("FREEZE");
        }
    }
    
    // Trigger repaint to update VU meters
    repaint();
    
    // Debug output every 30 frames (about once per second)
    static int debugCounter = 0;
    debugCounter++;
    if (debugCounter >= 30) {
        debugCounter = 0;
        // Print to console for debugging
        juce::Logger::writeToLog("VU Levels - L: " + juce::String(levelL, 6) + " R: " + juce::String(levelR, 6));
        // Also print to stdout for immediate visibility
        std::cout << "VU Levels - L: " << levelL << " R: " << levelR << std::endl;
    }
}

void DataBenderJuceAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    if (slider == &inputGainSlider) {
        processor.setInputGain((float)slider->getValue());
    } else if (slider == &outputGainSlider) {
        processor.setOutputGain((float)slider->getValue());
    } else if (slider == &speedSlider) {
        int octave = static_cast<int>(slider->getValue());
        float speed = std::pow(2.0f, octave);
        processor.setPlaybackSpeed(speed);
        // Optionally update the text box to show the speed value
        speedSlider.setTextValueSuffix(" (" + juce::String(speed, 2) + "x)");
    } else if (slider == &repeatsSlider) {
        processor.setRepeats((float)slider->getValue());
    }
}

void DataBenderJuceAudioProcessorEditor::buttonClicked(juce::Button* button) {
    if (button == &freezeButton) {
        bool freezeState = freezeButton.getToggleState();
        processor.setFreeze(freezeState);
        
        // Update button text based on state
        if (freezeState) {
            freezeButton.setButtonText("UNFREEZE");
        } else {
            freezeButton.setButtonText("FREEZE");
        }
    }
}

void DataBenderJuceAudioProcessorEditor::drawVUMeter(juce::Graphics& g, juce::Rectangle<int> bounds, float level, const juce::String& label) {
    // Draw meter background
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(bounds);
    
    // Draw meter border
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(bounds, 1);
    
    // Make the meter much more sensitive to low levels
    float normalizedLevel = 0.0f;
    
    if (level > 0.0f) {
        // For very low levels, use a more sensitive scale
        if (level < 0.001f) {
            normalizedLevel = level * 1000.0f; // Scale up very low levels
        } else if (level < 0.01f) {
            normalizedLevel = level * 100.0f; // Scale up low levels
        } else {
            // For higher levels, use normal scale
            normalizedLevel = juce::jlimit(0.0f, 1.0f, level);
        }
    }
    
    normalizedLevel = juce::jlimit(0.0f, 1.0f, normalizedLevel);
    
    // Draw level bar
    auto levelBounds = bounds.reduced(2);
    auto levelHeight = (int)(levelBounds.getHeight() * normalizedLevel);
    auto levelY = levelBounds.getBottom() - levelHeight;
    
    if (levelHeight > 0) {
        // Choose color based on level
        juce::Colour meterColour;
        if (level > 0.8f) {
            meterColour = juce::Colours::red; // High level
        } else if (level > 0.5f) {
            meterColour = juce::Colours::yellow; // Medium level
        } else if (level > 0.1f) {
            meterColour = juce::Colours::green; // Low level
        } else {
            meterColour = juce::Colours::darkgreen; // Very low level
        }
        
        g.setColour(meterColour);
        g.fillRect(levelBounds.getX(), levelY, levelBounds.getWidth(), levelHeight);
    }
    
    // Draw level value for debugging
    g.setColour(juce::Colours::white);
    g.setFont(8.0f);
    g.drawText(juce::String(level, 6), bounds.getX(), bounds.getY() - 12, bounds.getWidth(), 12, juce::Justification::centred);
} // Test comment for file watching
// Another test comment
// Test fswatch detection
// Test change
// Test file watching fix
// Test change for fswatch
// Test polling-based file watching
