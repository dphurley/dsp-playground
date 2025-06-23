#pragma once

#include <vector>

// Core DSP engine - designed to be portable across platforms
class DataBenderEngine {
public:
    DataBenderEngine();
    ~DataBenderEngine();
    
    // Initialize the DSP engine
    void init(float sampleRate);
    
    // Process audio - designed to be called from any platform
    void process(const float* inputs[2], float* outputs[2], int numFrames);
    
    // Buffer freeze controls
    void setFreeze(bool freeze);
    bool getFreeze() const;
    void clearBuffer();
    
    // Helper method to find start of audio in buffer
    int findAudioStart() const;
    
    // Parameter setters for future effects
    void setParameter(int paramId, float value);
    float getParameter(int paramId) const;
    
    // Sample rate management
    void setSampleRate(float sampleRate);
    float getSampleRate() const;
    
    // Progressive silence trimming methods
    void analyzeAndTrimSilence();
    void clearTrimmedSegments();
    void readFromTrimmedBuffer(float& outputL, float& outputR);
    bool isSilence(int start, int length) const;
    
private:
    float sampleRate;
    float parameters[16]; // Space for future parameters
    
    // Buffer management
    static const int BUFFER_SIZE = 60 * 44100; // 60 seconds at 44.1kHz
    float* bufferL;
    float* bufferR;
    int writePosition;
    int readPosition;
    int audioStartPosition; // Store where audio starts (trim silence)
    bool isFrozen;
    bool bufferInitialized;
    
    // Progressive silence trimming
    struct AudioSegment {
        int start;
        int length;
        float* dataL;
        float* dataR;
    };
    
    std::vector<AudioSegment> trimmedSegments;
    int totalTrimmedLength;
    bool segmentsInitialized;
    
    // Silence detection parameters
    static constexpr float SILENCE_THRESHOLD = 0.001f;
    static constexpr int MIN_SILENCE_LENGTH = 1024; // Minimum silence block to trim (about 23ms at 44.1kHz)
    static constexpr int MIN_AUDIO_LENGTH = 512; // Minimum audio block to keep (about 12ms at 44.1kHz)
    
    // Internal processing state
    void processFrame(float inputL, float inputR, float& outputL, float& outputR);
    void updateBuffer(float inputL, float inputR);
    void readFromBuffer(float& outputL, float& outputR);
}; 