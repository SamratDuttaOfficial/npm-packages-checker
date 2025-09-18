#!/bin/bash

# NPM Vulnerability Checker - macOS Build Script
# This script compiles the application for macOS

echo "Building NPM Vulnerability Checker for macOS..."

# Change to project root directory
cd "$(dirname "$0")/.."

# Check if Xcode command line tools are installed
if ! command -v gcc &> /dev/null && ! command -v clang &> /dev/null; then
    echo "Error: No C compiler found."
    echo "Please install Xcode command line tools:"
    echo "  xcode-select --install"
    exit 1
fi

# Check if Homebrew is installed for GTK
if ! command -v brew &> /dev/null; then
    echo "Error: Homebrew not found."
    echo "Please install Homebrew first: https://brew.sh"
    echo "Then install GTK+3: brew install gtk+3"
    exit 1
fi

# Check if GTK is installed
if ! pkg-config --exists gtk+-3.0; then
    echo "Error: GTK+3 not found."
    echo "Please install it using:"
    echo "  brew install gtk+3"
    exit 1
fi

# Create build directory
mkdir -p build/macos

# Determine the best compiler
if command -v clang &> /dev/null; then
    COMPILER="clang"
elif command -v gcc &> /dev/null; then
    COMPILER="gcc"
else
    echo "Error: No suitable compiler found"
    exit 1
fi

echo "Using $COMPILER compiler..."

# Source files
SOURCES="src/main.c src/common.c src/csv_parser.c src/npm_analyzer.c src/vulnerability_checker.c src/report_generator.c src/gui_gtk.c src/cli.c"

# Compile the application
echo "Compiling..."
$COMPILER -o build/macos/npm-vuln-checker $SOURCES \
    $(pkg-config --cflags --libs gtk+-3.0) \
    -Isrc \
    -Wall -Wextra -Wno-unused-parameter -O2 -std=c99

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Executable created: build/macos/npm-vuln-checker"

    # Make it executable
    chmod +x build/macos/npm-vuln-checker

    echo ""
    echo "To run the application:"
    echo "  ./build/macos/npm-vuln-checker"
    echo ""
    echo "Note: You may need to install GTK runtime if not already present:"
    echo "  brew install gtk+3"
else
    echo "Build failed!"
    exit 1
fi