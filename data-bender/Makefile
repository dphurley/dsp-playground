# Data Bender Makefile
# Platform-agnostic build system for multiple plugin formats

# Build configuration
RACK_DIR ?= ../Rack

# Plugin-specific settings
SLUG = data-bender
VERSION = 1.0.0

# Source files - core and VCV-specific
CORE_SOURCES = $(wildcard core/*.cpp)
VCV_SOURCES = $(wildcard vcv/*.cpp)
SOURCES = $(CORE_SOURCES) $(VCV_SOURCES)

# Header files
HEADERS = $(wildcard core/*.hpp) $(wildcard vcv/*.hpp)

# Include directories
CXXFLAGS += -Icore -Ivcv

# Clean all build artifacts (all platforms)
clean:
	@echo "🧹 Cleaning all build artifacts..."
	rm -rf build dist core/*.o core/libDataBenderCore.a
	rm -rf juce/build
	rm -rf vcv/build
	@echo "All build artifacts cleaned!"

# Clean specific platform build artifacts
clean-juce:
	@echo "🧹 Cleaning JUCE build artifacts..."
	rm -rf juce/build
	@echo "JUCE build artifacts cleaned!"

clean-vcv:
	@echo "🧹 Cleaning VCV Rack build artifacts..."
	rm -rf build dist core/*.o core/libDataBenderCore.a
	@echo "VCV Rack build artifacts cleaned!"

# Fast build targets
fast-juce:
	@echo "🔨 Building JUCE plugin (fast mode)..."
	cd juce && ./build.sh fast

fast-vcv:
	@echo "🔨 Building VCV Rack plugin (fast mode)..."
	$(MAKE) -f Makefile.vcv all

# Watch targets
watch-juce:
	@echo "👀 Starting auto-rebuild with file watching..."
	cd juce && ./auto-rebuild.sh

watch-vcv:
	@echo "👀 Starting VCV Rack auto-rebuild..."
	# TODO: Implement VCV Rack file watching
	@echo "VCV Rack file watching not yet implemented"

# Host targets
host-juce:
	@echo "🎵 Opening AudioPluginHost with preset..."
	@if [ -f "juce/DataBender_Template.aph" ]; then \
		echo "Found preset template, opening AudioPluginHost..."; \
		/Applications/JUCEAudioPluginHost.app/Contents/MacOS/AudioPluginHost juce/DataBender_Template.aph; \
	else \
		echo "No preset template found, running setup..."; \
		$(MAKE) setup-juce; \
	fi

host-vcv:
	@echo "🎵 Opening VCV Rack..."
	# TODO: Implement VCV Rack host opening
	@echo "VCV Rack host opening not yet implemented"

# Setup targets
setup-juce:
	@echo "⚙️  Running JUCE setup script..."
	cd juce && ./setup-template.sh

setup-vcv:
	@echo "⚙️  Running VCV Rack setup..."
	# TODO: Implement VCV Rack setup
	@echo "VCV Rack setup not yet implemented"

# Development workflow targets
dev-juce: fast-juce host-juce

dev-juce-watch: watch-juce host-juce

dev-vcv: fast-vcv host-vcv

dev-vcv-watch: watch-vcv host-vcv

# VCV Rack specific targets (legacy, will be replaced)
vcv-build:
	@echo "🔨 Building VCV Rack plugin..."
	$(MAKE) -f Makefile.vcv all

vcv-dev:
	@echo "🔧 Development build (debug symbols)..."
	$(MAKE) -f Makefile.vcv dev

vcv-release:
	@echo "🚀 Release build (optimized)..."
	$(MAKE) -f Makefile.vcv release

vcv-install:
	@echo "📦 Installing to Rack plugins directory..."
	$(MAKE) -f Makefile.vcv install

vcv-core:
	@echo "🔧 Building core library only..."
	$(MAKE) -f Makefile.vcv core

# Utility targets
status:
	@echo "📊 Git status:"
	git status --short

help:
	@echo "Data Bender - Available commands:"
	@echo ""
	@echo "Fast build targets:"
	@echo "  fast-juce      - Fast build of JUCE plugin (optimized for speed)"
	@echo "  fast-vcv       - Fast build of VCV Rack plugin"
	@echo "  fast-au        - Fast build of AU plugin (when implemented)"
	@echo "  fast-vst       - Fast build of VST plugin (when implemented)"
	@echo ""
	@echo "Watch targets:"
	@echo "  watch-juce     - Auto-rebuild JUCE plugin on file changes"
	@echo "  watch-vcv      - Auto-rebuild VCV Rack plugin on file changes"
	@echo "  watch-au       - Auto-rebuild AU plugin on file changes (when implemented)"
	@echo "  watch-vst      - Auto-rebuild VST plugin on file changes (when implemented)"
	@echo ""
	@echo "Host targets:"
	@echo "  host-juce      - Open AudioPluginHost with JUCE plugin"
	@echo "  host-vcv       - Open VCV Rack with plugin"
	@echo "  host-au        - Open AU host (when implemented)"
	@echo "  host-vst       - Open VST host (when implemented)"
	@echo ""
	@echo "Setup targets:"
	@echo "  setup-juce     - Run JUCE setup script"
	@echo "  setup-vcv      - Run VCV Rack setup"
	@echo "  setup-au       - Run AU setup (when implemented)"
	@echo "  setup-vst      - Run VST setup (when implemented)"
	@echo ""
	@echo "Development workflow targets:"
	@echo "  dev-juce       - Build and open JUCE host (complete workflow)"
	@echo "  dev-juce-watch - Watch and open JUCE host (full development)"
	@echo "  dev-vcv        - Build and open VCV Rack (complete workflow)"
	@echo "  dev-vcv-watch  - Watch and open VCV Rack (full development)"
	@echo ""
	@echo "Legacy VCV Rack targets (will be replaced):"
	@echo "  vcv-build      - Build the VCV Rack plugin"
	@echo "  vcv-dev        - Development build (debug symbols)"
	@echo "  vcv-release    - Release build (optimized)"
	@echo "  vcv-install    - Install to Rack plugins directory"
	@echo "  vcv-core       - Build core library only (no Rack required)"
	@echo ""
	@echo "Utility targets:"
	@echo "  clean          - Clean all build artifacts (all platforms)"
	@echo "  clean-juce     - Clean JUCE build artifacts"
	@echo "  clean-vcv      - Clean VCV Rack build artifacts"
	@echo "  status         - Show git status"
	@echo "  help           - Show this help message"
	@echo ""
	@echo "Environment variables:"
	@echo "  RACK_DIR       - Path to Rack source directory (required for VCV builds)" 