#!/bin/bash

# Data Bender JUCE Template Setup Script
# This script helps you create a default template for AudioPluginHost

set -e

# Colors for output
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

print_instruction() {
    echo -e "${BLUE}[INSTRUCTION]${NC} $1"
}

print_status "Setting up AudioPluginHost template for Data Bender development..."

# Check if AudioPluginHost is installed
if [ ! -d "/Applications/JUCEAudioPluginHost.app" ]; then
    print_error "JUCEAudioPluginHost not found in /Applications/"
    print_status "Please install AudioPluginHost first"
    exit 1
fi

print_instruction "Step 1: Launch AudioPluginHost"
print_status "Opening AudioPluginHost..."
open -a JUCEAudioPluginHost

print_instruction "Step 2: Set up your ideal session"
echo ""
echo "In AudioPluginHost, please set up your preferred testing environment:"
echo "1. Add your '_Data Bender JUCE' plugin to a track"
echo "2. Set up any audio sources you want (test tone, file player, etc.)"
echo "3. Configure any MIDI routing, audio routing, or other plugins"
echo "4. Set the window size and layout you prefer"
echo "5. Make sure everything is working as you want it"
echo ""
echo "Step 3: Save the template"
echo "1. Go to File > Save Session As..."
echo "2. Navigate to: $(pwd)"
echo "3. Save as: DataBender_Template.aph"
echo "4. Close AudioPluginHost"
echo ""

read -p "Press Enter when you've completed these steps..."

# Check if template was created
if [ -f "DataBender_Template.aph" ]; then
    print_status "✅ Template created successfully!"
    print_status "The auto-rebuild script will now automatically open AudioPluginHost with this template after each build."
    echo ""
    print_status "To test the workflow:"
    echo "1. Run: ./auto-rebuild.sh"
    echo "2. Make a change to any source file"
    echo "3. Watch AudioPluginHost automatically open with your template!"
else
    print_warning "Template file not found. Please make sure you saved it as 'DataBender_Template.aph' in this directory."
    print_status "You can run this script again once you've created the template."
fi 