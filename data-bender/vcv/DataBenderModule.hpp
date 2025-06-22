#pragma once

#include "rack.hpp"
#include "../core/DataBenderEngine.hpp"

using namespace rack;

// VCV Rack Module
struct DataBenderModule : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    
    enum InputIds {
        INPUT_L,
        INPUT_R,
        NUM_INPUTS
    };
    
    enum OutputIds {
        OUTPUT_L,
        OUTPUT_R,
        NUM_OUTPUTS
    };
    
    enum LightIds {
        NUM_LIGHTS
    };
    
    DataBenderEngine engine;
    
    DataBenderModule();
    
    void process(const ProcessArgs& args) override;
    void onSampleRateChange() override;
};

// VCV Rack Widget
struct DataBenderWidget : ModuleWidget {
    DataBenderWidget(DataBenderModule* module);
}; 