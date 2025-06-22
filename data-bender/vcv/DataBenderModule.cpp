#include "DataBenderModule.hpp"

// DataBenderModule implementation
DataBenderModule::DataBenderModule() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    
    // Configure inputs
    configInput(INPUT_L, "Left");
    configInput(INPUT_R, "Right");
    
    // Configure outputs
    configOutput(OUTPUT_L, "Left");
    configOutput(OUTPUT_R, "Right");
    
    // Initialize the DSP engine
    engine.init(APP->engine->getSampleRate());
}

void DataBenderModule::process(const ProcessArgs& args) {
    // Prepare input arrays
    const float* inputs[2] = {
        inputs[INPUT_L].getVoltages(),
        inputs[INPUT_R].getVoltages()
    };
    
    // Prepare output arrays
    float* outputs[2] = {
        outputs[OUTPUT_L].getVoltages(),
        outputs[OUTPUT_R].getVoltages()
    };
    
    // Process audio through the engine
    engine.process(inputs, outputs, args.samples);
}

void DataBenderModule::onSampleRateChange() {
    engine.setSampleRate(APP->engine->getSampleRate());
}

// DataBenderWidget implementation
DataBenderWidget::DataBenderWidget(DataBenderModule* module) {
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/DataBender.svg")));
    
    // Add screws
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    
    // Add inputs
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.5, 25)), module, DataBenderModule::INPUT_L));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.5, 25)), module, DataBenderModule::INPUT_R));
    
    // Add outputs
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.5, 85)), module, DataBenderModule::OUTPUT_L));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.5, 85)), module, DataBenderModule::OUTPUT_R));
} 