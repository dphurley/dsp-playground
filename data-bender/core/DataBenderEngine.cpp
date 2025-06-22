#include "DataBenderEngine.hpp"
#include <cmath>

// DataBenderEngine implementation
DataBenderEngine::DataBenderEngine() : sampleRate(44100.0f) {
    // Initialize parameters to default values
    for (int i = 0; i < 16; ++i) {
        parameters[i] = 0.0f;
    }
}

DataBenderEngine::~DataBenderEngine() {
    // Cleanup if needed
}

void DataBenderEngine::init(float sampleRate) {
    this->sampleRate = sampleRate;
    // Initialize any DSP state here
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
    // For now, just pass through the audio
    // This is where you'll add your DSP effects later
    outputL = inputL;
    outputR = inputR;
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