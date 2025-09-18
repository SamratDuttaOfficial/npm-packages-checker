#!/bin/bash
# NPM Vulnerability Checker - Distribution Builder for Unix/Linux/macOS
# Author: Samrat Dutta

set -e  # Exit on any error

echo "============================================"
echo "NPM Vulnerability Checker - Distribution Builder"
echo "============================================"

# Change to project root directory
cd "$(dirname "$0")/.."

# Detect platform
PLATFORM=$(uname -s)
ARCH=$(uname -m)

# Clean previous distributions
echo "Cleaning previous distribution files..."
rm -rf dist
mkdir -p dist/{linux,macos,windows}

# Function to build for Linux
build_linux() {
    echo ""
    echo "[1/3] Building Linux executable..."
    if [ -f "scripts/build_linux.sh" ]; then
        chmod +x scripts/build_linux.sh
        ./scripts/build_linux.sh
    else
        echo "ERROR: scripts/build_linux.sh not found!"
        return 1
    fi

    echo "Packaging Linux distribution..."
    cp build/linux/npm-vuln-checker dist/linux/
    cp README.md dist/linux/

    # Create or copy run script
    if [ -f "scripts/run_linux.sh" ]; then
        cp scripts/run_linux.sh dist/linux/
    else
        cat > dist/linux/run_linux.sh << 'EOF'
#!/bin/bash
echo "Starting NPM Vulnerability Checker..."
./npm-vuln-checker
read -p "Press Enter to continue..."
EOF
    fi

    chmod +x dist/linux/run_linux.sh
    chmod +x dist/linux/npm-vuln-checker

    # Create archive
    echo "Creating Linux archive..."
    cd dist/linux
    tar -czf ../npm-vuln-checker-linux-${ARCH}.tar.gz *
    cd ../..
}

# Function to build for macOS
build_macos() {
    echo ""
    echo "[2/3] Building macOS executable..."
    if [ -f "scripts/build_macos.sh" ]; then
        chmod +x scripts/build_macos.sh
        ./scripts/build_macos.sh
    else
        echo "ERROR: scripts/build_macos.sh not found!"
        return 1
    fi

    echo "Packaging macOS distribution..."
    cp build/macos/npm-vuln-checker dist/macos/
    cp README.md dist/macos/

    # Create or copy run script
    if [ -f "scripts/run_macos.sh" ]; then
        cp scripts/run_macos.sh dist/macos/
    else
        cat > dist/macos/run_macos.sh << 'EOF'
#!/bin/bash
echo "Starting NPM Vulnerability Checker..."
./npm-vuln-checker
read -p "Press Enter to continue..."
EOF
    fi

    chmod +x dist/macos/run_macos.sh
    chmod +x dist/macos/npm-vuln-checker

    # Create archive
    echo "Creating macOS archive..."
    cd dist/macos
    tar -czf ../npm-vuln-checker-macos-${ARCH}.tar.gz *
    cd ../..
}

# Function to create distribution info
create_info() {
    cat > dist/DISTRIBUTION_INFO.md << EOF
# NPM Vulnerability Checker - Distribution Packages

Generated on: $(date)
Platform: ${PLATFORM}
Architecture: ${ARCH}

## Available Packages

### Linux
- \`npm-vuln-checker-linux-${ARCH}.tar.gz\`
- Contains: executable, run script, README
- Requirements: npm installed on target system

### macOS
- \`npm-vuln-checker-macos-${ARCH}.tar.gz\`
- Contains: executable, run script, README
- Requirements: npm installed on target system

### Windows
- \`npm-vuln-checker-windows.tar.gz\` (build on Windows)
- Contains: executable (.exe), run script (.bat), README
- Requirements: npm installed on target system

## Installation Instructions

1. Download the appropriate package for your platform
2. Extract the archive: \`tar -xzf npm-vuln-checker-<platform>.tar.gz\`
3. Make executable (Linux/macOS): \`chmod +x npm-vuln-checker\`
4. Run: \`./npm-vuln-checker\` or use the provided run script

## Notes

- All distributions are standalone but require npm to be available in PATH
- GUI interface uses native Windows API on Windows, GTK+3 on Linux/macOS
- For GTK+3 systems, ensure GTK+3 runtime libraries are installed
EOF
}

# Build based on current platform
case "$PLATFORM" in
    "Linux")
        build_linux
        echo ""
        echo "[INFO] Running on Linux - only Linux distribution built"
        echo "[INFO] To build for other platforms, run this script on those systems"
        ;;
    "Darwin")
        build_macos
        # Try to build Linux if cross-compilation tools are available
        if command -v gcc >/dev/null 2>&1; then
            echo ""
            echo "[OPTIONAL] Attempting Linux build (may fail without proper toolchain)..."
            build_linux || echo "Linux build failed - this is expected without cross-compilation setup"
        fi
        ;;
    *)
        echo "ERROR: Unsupported platform: $PLATFORM"
        echo "This script supports Linux and macOS. For Windows, use scripts/windows/build_dist.bat"
        exit 1
        ;;
esac

# Create distribution info
create_info

echo ""
echo "============================================"
echo "Distribution build completed!"
echo "============================================"
echo ""
echo "Generated files in dist/:"
ls -la dist/*.tar.gz 2>/dev/null || echo "No archives created"
echo ""
echo "Generated directories:"
ls -ld dist/*/ 2>/dev/null || echo "No directories created"
echo ""
echo "To distribute:"
echo "  1. Upload .tar.gz files to GitHub releases"
echo "  2. Or share individual platform directories"
echo ""
echo "See dist/DISTRIBUTION_INFO.md for details"