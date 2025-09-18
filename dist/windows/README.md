# NPM Vulnerability Checker

Author: Samrat Dutta

A cross-platform C application that analyzes NPM projects for vulnerable packages based on a CSV database of known vulnerabilities.

## Features

- **Cross-platform GUI**: Native Windows interface and GTK-based interface for Linux/macOS
- **CSV vulnerability database**: Load custom vulnerability data with package names and versions
- **NPM project analysis**: Automatically scans dependency trees using `npm list`
- **Detailed reporting**: Generates comprehensive vulnerability reports
- **Version handling**: Supports multiple version formats and spaces in CSV data
- **Dependency tracking**: Shows direct vs indirect package dependencies

## Building the Application

### Prerequisites

**Windows:**
- MinGW-w64 or Visual Studio Build Tools
- Node.js and npm

**Linux:**
- GCC compiler
- GTK+3 development libraries
- Node.js and npm

**macOS:**
- Xcode command line tools
- Homebrew
- GTK+3 (via Homebrew)
- Node.js and npm

### Build Scripts

**Linux:**
```bash
chmod +x scripts/build_linux.sh
./scripts/build_linux.sh
```

**Windows:**
```cmd
scripts\windows\build.bat
```

**macOS:**
```bash
chmod +x scripts/build_macos.sh
./scripts/build_macos.sh
```

## Running the Application

### From Source (after building)

**Linux:**
```bash
./build/linux/npm-vuln-checker
```

**Windows:**
```cmd
build\windows\npm-vuln-checker.exe
```

**macOS:**
```bash
./build/macos/npm-vuln-checker
```

### Standalone Distribution

Copy the executable and run scripts to target machines:

**Linux:**
```bash
chmod +x scripts/run_linux.sh
./scripts/run_linux.sh
```

**Windows:**
```cmd
scripts\windows\run.bat
```

**macOS:**
```bash
chmod +x scripts/run_macos.sh
./scripts/run_macos.sh
```

## Usage

1. **Select NPM Project Folder**: Click "Browse" to select your NPM project directory
2. **Select CSV File**: Choose your vulnerability database CSV file
3. **Configure Columns**:
   - Set the package name column number (1-based indexing)
   - Set the version column number (1-based indexing)
4. **Choose Report Location**: Select where to save the vulnerability report
5. **Scan**: Click "Scan for Vulnerabilities" to start the analysis

### CSV Format

Your CSV file should contain:
- **Package Name Column**: NPM package names (spaces will be automatically removed)
- **Version Column**: Vulnerable versions (supports multiple versions separated by spaces)

Example CSV:
```csv
Package,Vulnerable_Versions,Severity
lodash,4.17.20 4.17.19,High
express,4.17.0 4.16.4,Medium
```

### Report Output

The application generates a plain text report containing:
- **Status**: OK or VULNERABLE
- **Vulnerability Details**: Lists each vulnerable package found
- **Dependency Information**: Shows if packages are direct or indirect dependencies
- **Scan Summary**: Overview of the analysis results

## Dependencies

The application requires npm to be installed and available in the system PATH for analyzing NPM projects.

### Runtime Dependencies

**Linux:**
- GTK+3 runtime libraries
- npm

**Windows:**
- npm (GTK not required - uses native Windows API)

**macOS:**
- GTK+3 runtime libraries (via Homebrew)
- npm

## Testing

See [TESTING.md](TESTING.md) for test setup and execution instructions.

## Troubleshooting

**Build Issues:**
- Ensure all development dependencies are installed
- Check compiler availability with `gcc --version` or `cl`

**Runtime Issues:**
- Verify npm is installed: `npm --version`
- Check GTK availability on Linux/macOS
- Ensure CSV file format matches expected structure

**Permission Issues:**
- Make shell scripts executable: `chmod +x *.sh`
- Run with appropriate permissions for file access