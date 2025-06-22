cmake_minimum_required(VERSION 3.15)

project(DataBenderJuce VERSION 0.1.0)

# Enable parallel builds with maximum cores
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
    # Use all available cores for parallel builds
    set(CMAKE_JOB_POOLS compile=${N})
    set(CMAKE_JOB_POOL_COMPILE compile)
endif()

# Set compiler optimizations for faster builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# JUCE setup
set(JUCE_DIR "$ENV{JUCE_DIR}" CACHE PATH "Path to JUCE")
if(NOT JUCE_DIR)
    message(FATAL_ERROR "Please set JUCE_DIR to the path of your JUCE folder (export JUCE_DIR=/path/to/JUCE)")
endif()

add_subdirectory(${JUCE_DIR} juce)

# Plugin formats - temporarily disable VST3 to avoid conflicts
set(JUCE_PLUGINHOST_VST3 OFF)
set(JUCE_PLUGINHOST_AU ON)
set(JUCE_PLUGINHOST_CLAP ON)
set(JUCE_PLUGINHOST_VST2 OFF)

juce_add_plugin(DataBenderJuce
    COMPANY_NAME "Echo Devices"
    BUNDLE_ID "com.echodevices.databenderjuce"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE DBnd
    FORMATS AU CLAP
    PRODUCT_NAME "_Data Bender JUCE"
)

# Source files
target_sources(DataBenderJuce PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

# Include directories
target_include_directories(DataBenderJuce PRIVATE
    ../core
    ${JUCE_DIR}/modules
)

target_sources(DataBenderJuce PRIVATE
    ../core/DataBenderEngine.cpp
)

# Link JUCE modules
target_link_libraries(DataBenderJuce PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

# Set compile options for faster builds
target_compile_options(DataBenderJuce PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-fno-stack-protector>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-fno-common>
) 