#!/bin/bash

# Data Bender JUCE Build Script
# This script helps build the JUCE plugin with optimized build times

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
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
    echo ""
    echo "Or run this script with:"
    echo "JUCE_DIR=~/workspace/SDKs/JUCE ./build.sh"
    exit 1
fi

# Check if JUCE directory exists
if [ ! -d "$JUCE_DIR" ]; then
    print_error "JUCE directory not found: $JUCE_DIR"
    exit 1
fi

print_status "Building Data Bender JUCE plugin..."
print_status "JUCE directory: $JUCE_DIR"

# Get number of CPU cores for parallel builds
CPU_CORES=$(sysctl -n hw.ncpu)
print_status "Using $CPU_CORES CPU cores for parallel build"

# Build type
BUILD_TYPE=${1:-fast}
case $BUILD_TYPE in
    fast|dev|development)
        print_status "Building in fast development mode (optimized for speed)"
        mkdir -p build && cd build
        
        # Use Ninja if available for faster builds
        if command -v ninja &> /dev/null; then
            print_debug "Using Ninja build system for faster builds"
            cmake .. -DCMAKE_BUILD_TYPE=Debug -G Ninja
            ninja -j$CPU_CORES
        else
            print_debug "Using Make build system"
            cmake .. -DCMAKE_BUILD_TYPE=Debug
            cmake --build . --parallel $CPU_CORES
        fi
        
        # Install to user Components directory
        mkdir -p ~/Library/Audio/Plug-Ins/Components/
        cp -r "DataBenderJuce_artefacts/Debug/AU/_Data Bender JUCE.component" ~/Library/Audio/Plug-Ins/Components/
        print_status "AU plugin installed to ~/Library/Audio/Plug-Ins/Components/"
        ;;
    release)
        print_status "Building in release mode (optimized)"
        mkdir -p build && cd build
        
        if command -v ninja &> /dev/null; then
            cmake .. -DCMAKE_BUILD_TYPE=Release -G Ninja
            ninja -j$CPU_CORES
        else
            cmake .. -DCMAKE_BUILD_TYPE=Release
            cmake --build . --parallel $CPU_CORES
        fi
        
        # Install to user Components directory
        mkdir -p ~/Library/Audio/Plug-Ins/Components/
        cp -r "DataBenderJuce_artefacts/AU/_Data Bender JUCE.component" ~/Library/Audio/Plug-Ins/Components/
        print_status "AU plugin installed to ~/Library/Audio/Plug-Ins/Components/"
        ;;
    clean)
        print_status "Cleaning build artifacts"
        rm -rf build
        # Optionally remove from user Components directory
        if [ -d "~/Library/Audio/Plug-Ins/Components/_Data Bender JUCE.component" ]; then
            rm -rf ~/Library/Audio/Plug-Ins/Components/_Data\ Bender\ JUCE.component
            print_status "Removed plugin from user Components directory"
        fi
        exit 0
        ;;
    install)
        print_status "Building and installing to user plugin directories"
        mkdir -p build && cd build
        
        if command -v ninja &> /dev/null; then
            cmake .. -DCMAKE_BUILD_TYPE=Release -G Ninja
            ninja -j$CPU_CORES
        else
            cmake .. -DCMAKE_BUILD_TYPE=Release
            cmake --build . --parallel $CPU_CORES
        fi
        
        # Copy AU plugin to user location
        mkdir -p ~/Library/Audio/Plug-Ins/Components/
        cp -r "DataBenderJuce_artefacts/AU/_Data Bender JUCE.component" ~/Library/Audio/Plug-Ins/Components/
        print_status "AU plugin installed to ~/Library/Audio/Plug-Ins/Components/"
        exit 0
        ;;
    *)
        print_error "Unknown build type: $BUILD_TYPE"
        echo "Available options: fast, release, clean, install"
        echo ""
        echo "  fast     - Fast development build (optimized for speed) + install to user"
        echo "  release  - Release build (optimized) + install to user"
        echo "  clean    - Clean build artifacts and remove from user"
        echo "  install  - Build and install to user plugin directories"
        exit 1
        ;;
esac

if [ $? -eq 0 ]; then
    print_status "Build completed successfully!"
    print_status "AU plugin installed to ~/Library/Audio/Plug-Ins/Components/"
    print_status "You can now load the plugin in any AU-compatible DAW or AudioPluginHost"
    print_status "For hot reload: rebuild and the plugin will be automatically updated"
else
    print_error "Build failed!"
    exit 1
fi 