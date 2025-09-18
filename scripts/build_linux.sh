#!/bin/bash

# NPM Vulnerability Checker - Linux Build Script
# This script compiles the application for Linux

echo "Building NPM Vulnerability Checker for Linux..."

# Change to project root directory
cd "$(dirname "$0")/.."

# Check if GTK development libraries are installed
if ! pkg-config --exists gtk+-3.0; then
    echo "Error: GTK+3 development libraries not found."
    echo "Please install them using:"
    echo "  Ubuntu/Debian: sudo apt-get install libgtk-3-dev"
    echo "  CentOS/RHEL: sudo yum install gtk3-devel"
    echo "  Fedora: sudo dnf install gtk3-devel"
    echo "  Arch: sudo pacman -S gtk3"
    exit 1
fi

# Create build directory
mkdir -p build/linux

# Source files
SOURCES="src/main.c src/common.c src/csv_parser.c src/npm_analyzer.c src/vulnerability_checker.c src/report_generator.c src/gui_gtk.c src/cli.c"

# Compile the application
echo "Compiling..."
gcc -o build/linux/npm-vuln-checker $SOURCES \
    $(pkg-config --cflags --libs gtk+-3.0) \
    -Isrc \
    -Wall -Wextra -Wno-unused-parameter -O2 -std=c99

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Executable created: build/linux/npm-vuln-checker"

    # Make it executable
    chmod +x build/linux/npm-vuln-checker

    echo ""
    echo "To run the application:"
    echo "  ./build/linux/npm-vuln-checker"
else
    echo "Build failed!"
    exit 1
fi