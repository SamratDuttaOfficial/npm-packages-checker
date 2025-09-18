#!/bin/bash

# NPM Vulnerability Checker - Linux Runtime Script
# This script runs the pre-compiled application on Linux

echo "Starting NPM Vulnerability Checker..."

# Change to project root directory
cd "$(dirname "$0")/.."

# Check if C compiler is available
if ! command -v gcc &> /dev/null; then
    echo "Error: gcc compiler not found."
    echo "Please install build essentials:"
    echo "  Ubuntu/Debian: sudo apt-get install build-essential"
    echo "  CentOS/RHEL: sudo yum groupinstall 'Development Tools'"
    echo "  Fedora: sudo dnf groupinstall 'Development Tools'"
    echo "  Arch: sudo pacman -S base-devel"
    exit 1
fi

# Check if pkg-config is available
if ! command -v pkg-config &> /dev/null; then
    echo "Error: pkg-config not found."
    echo "Please install pkg-config:"
    echo "  Ubuntu/Debian: sudo apt-get install pkg-config"
    echo "  CentOS/RHEL: sudo yum install pkgconfig"
    echo "  Fedora: sudo dnf install pkg-config"
    echo "  Arch: sudo pacman -S pkg-config"
    exit 1
fi

# Check if GTK development files are available
if ! pkg-config --exists gtk+-3.0; then
    echo "Error: GTK+3 development files not found."
    echo "Please install GTK development packages:"
    echo "  Ubuntu/Debian: sudo apt-get install libgtk-3-dev"
    echo "  CentOS/RHEL: sudo yum install gtk3-devel"
    echo "  Fedora: sudo dnf install gtk3-devel"
    echo "  Arch: sudo pacman -S gtk3"
    exit 1
fi

# Check if the executable exists
if [ ! -f "build/linux/npm-vuln-checker" ]; then
    echo "Error: npm-vuln-checker executable not found in build/linux/ directory."
    echo "Please build the project first using: scripts/build_linux.sh"
    exit 1
fi

# Make sure it's executable
chmod +x build/linux/npm-vuln-checker

# Check if GTK runtime libraries are available
if ! ldd build/linux/npm-vuln-checker | grep -q "gtk"; then
    echo "Warning: GTK libraries may not be available."
    echo "If the application fails to start, install GTK runtime:"
    echo "  Ubuntu/Debian: sudo apt-get install libgtk-3-0"
    echo "  CentOS/RHEL: sudo yum install gtk3"
    echo "  Fedora: sudo dnf install gtk3"
    echo "  Arch: sudo pacman -S gtk3"
    echo ""
fi

# Check if npm is available
if ! command -v npm &> /dev/null; then
    echo "Warning: npm is not installed or not in PATH."
    echo "The application requires npm to analyze projects."
    echo "Please install Node.js and npm."
    echo ""
fi

# Run the application
echo "Launching application..."
./build/linux/npm-vuln-checker

if [ $? -ne 0 ]; then
    echo "Application exited with an error."
    echo "Please check the above messages for details."
fi