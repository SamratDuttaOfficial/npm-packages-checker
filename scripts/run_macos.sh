#!/bin/bash

# NPM Vulnerability Checker - macOS Runtime Script
# This script runs the pre-compiled application on macOS

echo "Starting NPM Vulnerability Checker..."

# Change to project root directory
cd "$(dirname "$0")/.."

# Check if Xcode command line tools are installed
if ! command -v gcc &> /dev/null && ! command -v clang &> /dev/null; then
    echo "Error: No C compiler found."
    echo "Please install Xcode command line tools:"
    echo "  xcode-select --install"
    exit 1
fi

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "Error: Homebrew not found."
    echo "Please install Homebrew first: https://brew.sh"
    exit 1
fi

# Check if pkg-config is available
if ! command -v pkg-config &> /dev/null; then
    echo "Error: pkg-config not found."
    echo "Please install it using: brew install pkg-config"
    exit 1
fi

# Check if GTK is installed
if ! pkg-config --exists gtk+-3.0; then
    echo "Error: GTK+3 not found."
    echo "Please install it using: brew install gtk+3"
    exit 1
fi

# Check if the executable exists
if [ ! -f "build/macos/npm-vuln-checker" ]; then
    echo "Error: npm-vuln-checker executable not found in build/macos/ directory."
    echo "Please build the project first using: scripts/build_macos.sh"
    exit 1
fi

# Make sure it's executable
chmod +x build/macos/npm-vuln-checker

# Check if GTK runtime libraries are available
if ! otool -L build/macos/npm-vuln-checker | grep -q "gtk"; then
    echo "Warning: GTK libraries may not be available."
    echo "If the application fails to start, install GTK runtime:"
    echo "  brew install gtk+3"
    echo ""
fi

# Check if npm is available
if ! command -v npm &> /dev/null; then
    echo "Warning: npm is not installed or not in PATH."
    echo "The application requires npm to analyze projects."
    echo "Please install Node.js and npm from: https://nodejs.org/"
    echo "Or using Homebrew: brew install node"
    echo ""
fi

# Run the application
echo "Launching application..."
./build/macos/npm-vuln-checker

if [ $? -ne 0 ]; then
    echo "Application exited with an error."
    echo "Please check the above messages for details."
fi