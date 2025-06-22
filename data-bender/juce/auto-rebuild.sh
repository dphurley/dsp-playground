#!/bin/bash

# Data Bender JUCE Auto-Rebuild Script
# Watches source files and automatically rebuilds the plugin

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_debug() {
    echo -e "${BLUE}[DEBUG]${NC} $1"
}

# Check if JUCE_DIR is set
if [ -z "$JUCE_DIR" ]; then
    print_error "JUCE_DIR environment variable is not set."
    echo "Please set it to the path of your JUCE folder:"
    echo "export JUCE_DIR=~/workspace/SDKs/JUCE"
    exit 1
fi

# Check if JUCE directory exists
if [ ! -d "$JUCE_DIR" ]; then
    print_error "JUCE directory not found: $JUCE_DIR"
    exit 1
fi

# Get number of CPU cores
CPU_CORES=$(sysctl -n hw.ncpu)

print_status "Starting Data Bender JUCE auto-rebuild..."
print_status "Watching for changes in Source/ and ../core/"
print_status "Using $CPU_CORES CPU cores for parallel builds"

# Function to build the plugin
build_plugin() {
    local start_time=$(date +%s)
    print_status "Building plugin..."
    
    mkdir -p build && cd build
    
    # Use Ninja if available for faster builds
    if command -v ninja &> /dev/null; then
        print_debug "Using Ninja build system"
        if [ ! -f "build.ninja" ]; then
            cmake .. -DCMAKE_BUILD_TYPE=Debug -G Ninja
        fi
        ninja -j$CPU_CORES
    else
        print_debug "Using Make build system"
        if [ ! -f "Makefile" ]; then
            cmake .. -DCMAKE_BUILD_TYPE=Debug
        fi
        cmake --build . --parallel $CPU_CORES
    fi
    
    # Install to user Components directory
    mkdir -p ~/Library/Audio/Plug-Ins/Components/
    cp -r "DataBenderJuce_artefacts/Debug/AU/_Data Bender JUCE.component" ~/Library/Audio/Plug-Ins/Components/
    
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    print_status "Build completed in ${duration}s"
    print_status "Plugin installed to ~/Library/Audio/Plug-Ins/Components/"
}

# Initial build
print_status "Performing initial build..."
build_plugin

# Check if fswatch is available (faster than inotifywait)
if command -v fswatch &> /dev/null; then
    print_status "Using fswatch for file watching (fastest)"
    print_status "Press Ctrl+C to stop watching"
    
    # Watch for changes in source files
    fswatch -o Source/ ../core/ | while read f; do
        print_status "File change detected, rebuilding..."
        build_plugin
    done
elif command -v inotifywait &> /dev/null; then
    print_status "Using inotifywait for file watching"
    print_status "Press Ctrl+C to stop watching"
    
    while true; do
        inotifywait -r -e modify,create,delete Source/ ../core/
        print_status "File change detected, rebuilding..."
        build_plugin
    done
else
    print_warning "No file watcher found. Install fswatch for best performance:"
    echo "  brew install fswatch"
    echo ""
    print_status "Falling back to manual rebuild. Run './build.sh fast' to rebuild."
    exit 0
fi 