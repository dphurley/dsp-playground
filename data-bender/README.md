# Data Bender - Portable DSP Audio Effect

A portable digital audio effects module designed to run across multiple platforms including VCV Rack, VST3, AU, CLAP, Max For Live, and more.

## Project Structure

```
data-bender/
├── core/                   # Platform-agnostic DSP code
│   ├── DataBenderEngine.hpp
│   └── DataBenderEngine.cpp
├── vcv/                    # VCV Rack specific code
│   ├── DataBenderModule.hpp
│   ├── DataBenderModule.cpp
│   ├── plugin.hpp
│   └── plugin.cpp
├── juce/                   # JUCE plugin code
│   ├── Source/
│   │   ├── PluginProcessor.h
│   │   ├── PluginProcessor.cpp
│   │   ├── PluginEditor.h
│   │   └── PluginEditor.cpp
│   ├── CMakeLists.txt
│   └── build.sh
├── res/                    # Resources
│   └── DataBender.svg      # Module panel
├── plugin.json            # VCV Rack plugin manifest
├── Makefile               # VCV Rack build system
├── CMakeLists.txt         # Alternative build system
├── build.sh               # VCV Rack build script
├── .gitignore            # Git ignore rules
└── README.md             # This file
```

## Architecture

The module is designed with maximum portability in mind:

### Core DSP Engine (`core/DataBenderEngine`)
- **Platform-agnostic** audio processing
- Parameter management system
- Sample rate handling
- **No dependencies** on any specific platform
- Designed to be easily ported to other platforms

### VCV Rack Integration (`vcv/`)
- `DataBenderModule`: Handles VCV Rack-specific I/O
- `DataBenderWidget`: UI components and layout
- Clean separation between DSP and platform code
- Wraps the core engine for VCV Rack

### JUCE Integration (`juce/`)
- `DataBenderJuceAudioProcessor`: JUCE AudioProcessor implementation
- `DataBenderJuceAudioProcessorEditor`: JUCE GUI implementation
- Targets AU and CLAP formats (VST3 temporarily disabled due to conflicts)
- Uses the same core DSP engine

## Building

### Prerequisites
- VCV Rack source code (for VCV builds)
- JUCE SDK (for JUCE builds)
- C++ compiler (GCC, Clang, or MSVC)
- Make and CMake

### VCV Rack Build

```bash
# Set Rack directory (for VCV builds)
export RACK_DIR=/path/to/Rack

# Development build for VCV Rack (debug symbols)
./build.sh dev

# Release build for VCV Rack (optimized)
./build.sh release

# Build core DSP library only (platform-agnostic)
./build.sh core

# Install to Rack plugins directory
./build.sh install

# Clean build artifacts
./build.sh clean
```

### JUCE Build

```bash
# Set JUCE directory
export JUCE_DIR=~/workspace/SDKs/JUCE

# Development build (debug symbols)
cd juce && ./build.sh dev

# Release build (optimized)
cd juce && ./build.sh release

# Install to system plugin directories
cd juce && ./build.sh install

# Clean build artifacts
cd juce && ./build.sh clean
```

### Alternative Build Systems

```bash
# Using Make directly (VCV Rack)
make dev
make release
make core

# Using CMake (JUCE)
cd juce
mkdir build && cd build
cmake ..
make
```

## Adding Effects

To add new audio effects, modify the `processFrame` method in `core/DataBenderEngine.cpp`:

```cpp
void DataBenderEngine::processFrame(float inputL, float inputR, float& outputL, float& outputR) {
    // Add your DSP effects here
    // Example: Simple distortion
    outputL = tanh(inputL * parameters[0]);
    outputR = tanh(inputR * parameters[0]);
}
```

## Portability Features

### Core Library (`core/`)
- **Zero platform dependencies**
- Standard C++ only
- Parameter system with 16 slots
- Stereo I/O support
- Sample rate management

### Platform Integration
The `DataBenderEngine` class can be easily integrated into:
- **VST3/AU/CLAP** (desktop) - Use JUCE build
- **AUv3** (iOS/iPadOS) - Use core library with AudioKit
- **Max For Live** - Use core library with Max API
- **Reason rack effects** - Use core library with Reason SDK
- **VCV Rack modules** - Already implemented in `vcv/`

### Current Platform Support
- ✅ **VCV Rack** - Full implementation with stereo I/O
- ✅ **JUCE AU** - Full implementation with GUI
- 🔄 **JUCE CLAP** - Build system ready (CLAP support in progress)
- 🔄 **JUCE VST3** - Temporarily disabled due to parameter automation conflicts

### Future Platform Examples

```cpp
// VST3 Example (future)
class DataBenderVST : public AudioEffect {
    DataBenderEngine engine;
    
    void processReplacing(float** inputs, float** outputs, int numFrames) {
        engine.process(inputs, outputs, numFrames);
    }
};

// AUv3 Example (future)
class DataBenderAU : public AUAudioUnit {
    DataBenderEngine engine;
    
    void process(AudioBufferList* inputBufferList, AudioBufferList* outputBufferList) {
        // Convert buffers and call engine.process()
    }
};
```

## Development Workflow

1. **Add Effects**: Modify `processFrame()` in `core/DataBenderEngine.cpp`
2. **Add Parameters**: Use the parameter array and add UI controls in platform-specific code
3. **Test**: Build and test in VCV Rack using `./build.sh dev` or JUCE using `cd juce && ./build.sh dev`
4. **Port**: Use the core library for other platforms

## Platform Porting Guide

### Step 1: Build Core Library
```bash
./build.sh core
```

### Step 2: Include Core Headers
```cpp
#include "core/DataBenderEngine.hpp"
```

### Step 3: Integrate with Platform
```cpp
DataBenderEngine engine;
engine.init(sampleRate);

// In your audio processing callback:
engine.process(inputs, outputs, numFrames);
```

## Hot Reload Development

For rapid DSP prototyping with hot reload capabilities:

### JUCE with AudioPluginHost
- Build the JUCE plugin: `cd juce && ./build.sh dev`
- Use JUCE's AudioPluginHost for instant plugin reloading
- Edit DSP code in `core/DataBenderEngine.cpp` and rebuild
- AudioPluginHost will reload the plugin automatically

### Alternative Hot Reload Options
- **Max/MSP with gen~**: Write DSP in gen~ for instant feedback
- **Faust**: Use Faust IDE for rapid DSP prototyping
- **SuperCollider**: For algorithmic DSP development

## License

MIT License - see LICENSE file for details.

## Contributing

This is a personal DSP playground. Feel free to fork and experiment! 