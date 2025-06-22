#!/bin/bash

# Data Bender Build Script
# This script helps build the VCV Rack module

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
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

# Check if RACK_DIR is set (only needed for VCV builds)
check_rack_dir() {
    if [ -z "$RACK_DIR" ]; then
        print_error "RACK_DIR environment variable is not set."
        echo "Please set it to the path of your VCV Rack source directory:"
        echo "export RACK_DIR=/path/to/Rack"
        echo ""
        echo "Or run this script with:"
        echo "RACK_DIR=/path/to/Rack ./build.sh"
        exit 1
    fi

    # Check if Rack directory exists
    if [ ! -d "$RACK_DIR" ]; then
        print_error "Rack directory not found: $RACK_DIR"
        exit 1
    fi

    # Check if Rack's plugin.mk exists
    if [ ! -f "$RACK_DIR/plugin.mk" ]; then
        print_error "Rack's plugin.mk not found. Make sure you have the VCV Rack source code."
        exit 1
    fi
}

# Build type
BUILD_TYPE=${1:-dev}
case $BUILD_TYPE in
    dev|development)
        print_status "Building in development mode (debug symbols)"
        check_rack_dir
        make dev
        ;;
    release)
        print_status "Building in release mode (optimized)"
        check_rack_dir
        make release
        ;;
    core)
        print_status "Building core DSP library only (platform-agnostic)"
        make core
        ;;
    clean)
        print_status "Cleaning build artifacts"
        make clean
        exit 0
        ;;
    install)
        print_status "Building and installing to Rack plugins directory"
        check_rack_dir
        make install
        exit 0
        ;;
    *)
        print_error "Unknown build type: $BUILD_TYPE"
        echo "Available options: dev, release, core, clean, install"
        echo ""
        echo "  dev      - Development build for VCV Rack (debug symbols)"
        echo "  release  - Release build for VCV Rack (optimized)"
        echo "  core     - Build core DSP library only (for other platforms)"
        echo "  clean    - Clean build artifacts"
        echo "  install  - Build and install to VCV Rack"
        exit 1
        ;;
esac

if [ $? -eq 0 ]; then
    print_status "Build completed successfully!"
    if [ "$BUILD_TYPE" != "core" ] && [ "$BUILD_TYPE" != "clean" ]; then
        print_status "You can now load the module in VCV Rack"
    elif [ "$BUILD_TYPE" = "core" ]; then
        print_status "Core library built successfully for platform porting"
    fi
else
    print_error "Build failed!"
    exit 1
fi 