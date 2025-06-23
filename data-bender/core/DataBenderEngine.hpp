#pragma once

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
    
    // Internal processing state
    void processFrame(float inputL, float inputR, float& outputL, float& outputR);
    void updateBuffer(float inputL, float inputR);
    void readFromBuffer(float& outputL, float& outputR);
}; 