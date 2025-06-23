#include "DataBenderEngine.hpp"
#include <cmath>
#include <cstring>
#include <iostream>

// DataBenderEngine implementation
DataBenderEngine::DataBenderEngine() : sampleRate(44100.0f), writePosition(0), readPosition(0), audioStartPosition(0), isFrozen(false), bufferInitialized(false) {
    // Initialize parameters to default values
    for (int i = 0; i < 16; ++i) {
        parameters[i] = 0.0f;
    }
    
    // Allocate buffer memory
    bufferL = new float[BUFFER_SIZE];
    bufferR = new float[BUFFER_SIZE];
    
    // Clear buffers
    std::memset(bufferL, 0, BUFFER_SIZE * sizeof(float));
    std::memset(bufferR, 0, BUFFER_SIZE * sizeof(float));
}

DataBenderEngine::~DataBenderEngine() {
    // Cleanup buffer memory
    delete[] bufferL;
    delete[] bufferR;
}

void DataBenderEngine::init(float sampleRate) {
    this->sampleRate = sampleRate;
    
    // Recalculate buffer size based on new sample rate
    // Note: For simplicity, we keep the 60-second buffer size
    // In a real implementation, you might want to reallocate based on sample rate
    
    // Reset buffer positions
    writePosition = 0;
    readPosition = 0;
    audioStartPosition = 0;
    isFrozen = false;
    bufferInitialized = false;
    
    // Clear buffers
    std::memset(bufferL, 0, BUFFER_SIZE * sizeof(float));
    std::memset(bufferR, 0, BUFFER_SIZE * sizeof(float));
}

void DataBenderEngine::process(const float* inputs[2], float* outputs[2], int numFrames) {
    // Process each frame
    for (int i = 0; i < numFrames; ++i) {
        float inputL = inputs[0] ? inputs[0][i] : 0.0f;
        float inputR = inputs[1] ? inputs[1][i] : 0.0f;
        float outputL, outputR;
        
        processFrame(inputL, inputR, outputL, outputR);
        
        outputs[0][i] = outputL;
        outputs[1][i] = outputR;
    }
}

void DataBenderEngine::processFrame(float inputL, float inputR, float& outputL, float& outputR) {
    if (isFrozen) {
        // When frozen, read from the buffer
        readFromBuffer(outputL, outputR);
    } else {
        // When not frozen, pass through and update buffer
        outputL = inputL;
        outputR = inputR;
        updateBuffer(inputL, inputR);
    }
}

void DataBenderEngine::updateBuffer(float inputL, float inputR) {
    // Write to buffer
    bufferL[writePosition] = inputL;
    bufferR[writePosition] = inputR;
    
    // Advance write position
    writePosition = (writePosition + 1) % BUFFER_SIZE;
    
    // Mark buffer as initialized after first complete cycle
    if (writePosition == 0) {
        bufferInitialized = true;
    }
    
    // When not frozen, read position follows write position
    if (!isFrozen) {
        readPosition = writePosition;
    }
}

void DataBenderEngine::readFromBuffer(float& outputL, float& outputR) {
    // Determine how much audio we have captured
    int capturedSamples = writePosition;
    if (bufferInitialized) {
        capturedSamples = BUFFER_SIZE; // Full buffer
    }
    
    // If no audio captured yet, output silence
    if (capturedSamples == 0) {
        outputL = 0.0f;
        outputR = 0.0f;
        return;
    }
    
    // If we've reached the end of captured audio, loop back to audio start
    if (readPosition >= capturedSamples) {
        readPosition = audioStartPosition;
    }
    
    // Read from buffer
    outputL = bufferL[readPosition];
    outputR = bufferR[readPosition];
    
    // Debug output (only occasionally to avoid spam)
    static int debugCounter = 0;
    debugCounter++;
    if (debugCounter % 1000 == 0) {
        std::cout << "BUFFER READ: pos=" << readPosition << "/" << capturedSamples << " (audio starts at " << audioStartPosition << ") L=" << outputL << " R=" << outputR << std::endl;
    }
    
    // Advance read position
    readPosition++;
}

void DataBenderEngine::setFreeze(bool freeze) {
    if (freeze && !isFrozen) {
        // Find where the actual audio starts (trim silence) and store it
        audioStartPosition = findAudioStart();
        readPosition = audioStartPosition;
        
        // Calculate how much audio we have captured
        int capturedSamples = writePosition;
        if (bufferInitialized) {
            capturedSamples = BUFFER_SIZE; // Full buffer
        }
        
        std::cout << "FREEZE: Starting playback from position " << readPosition << ". Captured " << (capturedSamples / sampleRate) << " seconds of audio" << std::endl;
    }
    isFrozen = freeze;
    std::cout << "FREEZE: State changed to " << (freeze ? "FROZEN" : "UNFROZEN") << std::endl;
}

bool DataBenderEngine::getFreeze() const {
    return isFrozen;
}

void DataBenderEngine::clearBuffer() {
    // Clear buffers
    std::memset(bufferL, 0, BUFFER_SIZE * sizeof(float));
    std::memset(bufferR, 0, BUFFER_SIZE * sizeof(float));
    
    // Reset positions
    writePosition = 0;
    readPosition = 0;
    bufferInitialized = false;
}

int DataBenderEngine::findAudioStart() const {
    // Determine how much audio we have captured
    int capturedSamples = writePosition;
    if (bufferInitialized) {
        capturedSamples = BUFFER_SIZE; // Full buffer
    }
    
    if (capturedSamples == 0) {
        return 0;
    }
    
    // Threshold for silence detection (adjust as needed)
    const float silenceThreshold = 0.001f;
    
    // Look for the first sample that's above the silence threshold
    for (int i = 0; i < capturedSamples; ++i) {
        float levelL = std::abs(bufferL[i]);
        float levelR = std::abs(bufferR[i]);
        
        if (levelL > silenceThreshold || levelR > silenceThreshold) {
            std::cout << "AUDIO START: Found at position " << i << " (L=" << levelL << " R=" << levelR << ")" << std::endl;
            return i;
        }
    }
    
    // If no audio found, return the end of buffer
    std::cout << "AUDIO START: No audio found, returning end of buffer" << std::endl;
    return capturedSamples;
}

void DataBenderEngine::setParameter(int paramId, float value) {
    if (paramId >= 0 && paramId < 16) {
        parameters[paramId] = value;
    }
}

float DataBenderEngine::getParameter(int paramId) const {
    if (paramId >= 0 && paramId < 16) {
        return parameters[paramId];
    }
    return 0.0f;
}

void DataBenderEngine::setSampleRate(float sampleRate) {
    this->sampleRate = sampleRate;
}

float DataBenderEngine::getSampleRate() const {
    return sampleRate;
} 