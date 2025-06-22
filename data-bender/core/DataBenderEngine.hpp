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
    
    // Parameter setters for future effects
    void setParameter(int paramId, float value);
    float getParameter(int paramId) const;
    
    // Sample rate management
    void setSampleRate(float sampleRate);
    float getSampleRate() const;
    
private:
    float sampleRate;
    float parameters[16]; // Space for future parameters
    
    // Internal processing state
    void processFrame(float inputL, float inputR, float& outputL, float& outputR);
}; 