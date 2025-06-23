#include "DataBenderEngine.hpp"
#include <cmath>
#include <cstring>
#include <iostream>

// DataBenderEngine implementation
DataBenderEngine::DataBenderEngine() : sampleRate(44100.0f), writePosition(0), readPosition(0), audioStartPosition(0), isFrozen(false), bufferInitialized(false), totalTrimmedLength(0), segmentsInitialized(false) {
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
    
    // Cleanup trimmed segments
    clearTrimmedSegments();
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
    
    // Reset trimming state
    clearTrimmedSegments();
    
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
    // If we have trimmed segments, use them for playback
    if (segmentsInitialized && !trimmedSegments.empty()) {
        readFromTrimmedBuffer(outputL, outputR);
        return;
    }
    
    // Fallback to original buffer reading (for backward compatibility)
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
        // Analyze and trim silence from the buffer
        analyzeAndTrimSilence();
        
        // Start reading from the beginning of trimmed audio
        readPosition = 0;
        
        std::cout << "FREEZE: Starting trimmed playback. Total trimmed length: " 
                 << totalTrimmedLength << " samples (" << (totalTrimmedLength / sampleRate) << "s)" << std::endl;
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
    
    // Clear trimmed segments
    clearTrimmedSegments();
}

void DataBenderEngine::clearTrimmedSegments() {
    // Clean up existing segments
    for (auto& segment : trimmedSegments) {
        delete[] segment.dataL;
        delete[] segment.dataR;
    }
    trimmedSegments.clear();
    totalTrimmedLength = 0;
    segmentsInitialized = false;
}

bool DataBenderEngine::isSilence(int start, int length) const {
    // Check if a block of audio is silence
    for (int i = 0; i < length && (start + i) < BUFFER_SIZE; ++i) {
        int pos = (start + i) % BUFFER_SIZE;
        float levelL = std::abs(bufferL[pos]);
        float levelR = std::abs(bufferR[pos]);
        
        if (levelL > SILENCE_THRESHOLD || levelR > SILENCE_THRESHOLD) {
            return false;
        }
    }
    return true;
}

void DataBenderEngine::analyzeAndTrimSilence() {
    clearTrimmedSegments();
    
    // Determine how much audio we have captured
    int capturedSamples = writePosition;
    if (bufferInitialized) {
        capturedSamples = BUFFER_SIZE; // Full buffer
    }
    
    if (capturedSamples == 0) {
        return;
    }
    
    std::cout << "ANALYZING: Scanning " << capturedSamples << " samples for silence trimming..." << std::endl;
    
    int currentPos = 0;
    bool inAudio = false;
    int audioStart = 0;
    
    while (currentPos < capturedSamples) {
        // Check if current position is silence
        bool currentIsSilence = isSilence(currentPos, MIN_SILENCE_LENGTH);
        
        if (!inAudio && !currentIsSilence) {
            // Transition from silence to audio
            audioStart = currentPos;
            inAudio = true;
        } else if (inAudio && currentIsSilence) {
            // Transition from audio to silence
            int audioLength = currentPos - audioStart;
            
            if (audioLength >= MIN_AUDIO_LENGTH) {
                // Create segment for this audio block
                AudioSegment segment;
                segment.start = audioStart;
                segment.length = audioLength;
                segment.dataL = new float[audioLength];
                segment.dataR = new float[audioLength];
                
                // Copy audio data to segment
                for (int i = 0; i < audioLength; ++i) {
                    int bufferPos = (audioStart + i) % BUFFER_SIZE;
                    segment.dataL[i] = bufferL[bufferPos];
                    segment.dataR[i] = bufferR[bufferPos];
                }
                
                trimmedSegments.push_back(segment);
                totalTrimmedLength += audioLength;
                
                std::cout << "SEGMENT: Audio block " << (audioStart / sampleRate) << "s to " 
                         << ((audioStart + audioLength) / sampleRate) << "s (" << audioLength << " samples)" << std::endl;
            }
            
            inAudio = false;
        }
        
        currentPos += MIN_SILENCE_LENGTH;
    }
    
    // Handle final audio block if we end in audio
    if (inAudio) {
        int audioLength = capturedSamples - audioStart;
        if (audioLength >= MIN_AUDIO_LENGTH) {
            AudioSegment segment;
            segment.start = audioStart;
            segment.length = audioLength;
            segment.dataL = new float[audioLength];
            segment.dataR = new float[audioLength];
            
            for (int i = 0; i < audioLength; ++i) {
                int bufferPos = (audioStart + i) % BUFFER_SIZE;
                segment.dataL[i] = bufferL[bufferPos];
                segment.dataR[i] = bufferR[bufferPos];
            }
            
            trimmedSegments.push_back(segment);
            totalTrimmedLength += audioLength;
            
            std::cout << "SEGMENT: Final audio block " << (audioStart / sampleRate) << "s to " 
                     << ((audioStart + audioLength) / sampleRate) << "s (" << audioLength << " samples)" << std::endl;
        }
    }
    
    segmentsInitialized = true;
    std::cout << "TRIMMING: Created " << trimmedSegments.size() << " segments, total length: " 
             << totalTrimmedLength << " samples (" << (totalTrimmedLength / sampleRate) << "s)" << std::endl;
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

void DataBenderEngine::readFromTrimmedBuffer(float& outputL, float& outputR) {
    if (!segmentsInitialized || trimmedSegments.empty()) {
        outputL = 0.0f;
        outputR = 0.0f;
        return;
    }
    
    // Find which segment we're currently reading from
    int currentSample = readPosition % totalTrimmedLength;
    int segmentIndex = 0;
    int segmentOffset = 0;
    
    // Find the segment containing our current sample
    for (int i = 0; i < trimmedSegments.size(); ++i) {
        if (currentSample < segmentOffset + trimmedSegments[i].length) {
            segmentIndex = i;
            break;
        }
        segmentOffset += trimmedSegments[i].length;
    }
    
    // Calculate position within the current segment
    int segmentSample = currentSample - segmentOffset;
    
    // Read from the segment
    if (segmentSample < trimmedSegments[segmentIndex].length) {
        outputL = trimmedSegments[segmentIndex].dataL[segmentSample];
        outputR = trimmedSegments[segmentIndex].dataR[segmentSample];
    } else {
        outputL = 0.0f;
        outputR = 0.0f;
    }
    
    // Debug output (only occasionally to avoid spam)
    static int debugCounter = 0;
    debugCounter++;
    if (debugCounter % 1000 == 0) {
        std::cout << "TRIMMED READ: sample=" << currentSample << "/" << totalTrimmedLength 
                 << " segment=" << segmentIndex << "/" << trimmedSegments.size() 
                 << " pos=" << segmentSample << " L=" << outputL << " R=" << outputR << std::endl;
    }
    
    // Advance read position
    readPosition++;
} 