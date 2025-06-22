#include "plugin.hpp"
#include "DataBenderModule.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
    pluginInstance = p;
    
    // Add modules here
    p->addModel(modelDataBender);
}

// Module model declaration
Model* modelDataBender = createModel<DataBenderModule, DataBenderWidget>("DataBender"); 