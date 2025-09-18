# NPM Vulnerability Checker

Author: [Samrat Dutta](https://github.com/SamratDuttaOfficial)

A cross-platform C application that analyzes NPM projects for vulnerable packages based on a CSV database of known vulnerabilities.

## Features

- **Dual Interface**: Both GUI and CLI modes in the same executable
- **Cross-platform GUI**: Native Windows interface and GTK-based interface for Linux/macOS
- **Command-line Interface**: Full CLI support for automation and scripting
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

The NPM Vulnerability Checker supports both GUI and CLI modes. You can run the same executable with or without command-line arguments.

### GUI Mode (Default)

Run the executable without any arguments to launch the graphical interface:

```bash
# GUI mode (default)
./npm-vuln-checker                    # Linux/macOS
npm-vuln-checker.exe                  # Windows
```

**GUI Steps:**
1. **Select NPM Project Folder**: Click "Browse" to select your NPM project directory
2. **Select CSV File**: Choose your vulnerability database CSV file
3. **Configure Columns**:
   - Set the package name column number (1-based indexing)
   - Set the version column number (1-based indexing)
4. **Choose Report Location**: Select where to save the vulnerability report
5. **Scan**: Click "Scan for Vulnerabilities" to start the analysis

### CLI Mode

Use the `--cli` flag to run in command-line mode for automation and scripting:

```bash
# Basic CLI usage
npm-vuln-checker --cli --csv vulns.csv --project ./my-app --output report.txt

# With custom CSV columns
npm-vuln-checker --cli --csv vulns.csv --project ./my-app --output report.txt --name-col 2 --version-col 3

# Show help
npm-vuln-checker --cli --help
```

**CLI Arguments:**
- `--cli` - Enable CLI mode
- `--csv <file>` - Path to CSV vulnerability database file
- `--project <dir>` - Path to NPM project directory
- `--output <file>` - Path for the output report
- `--name-col <num>` - CSV column for package names (default: 1)
- `--version-col <num>` - CSV column for versions (default: 2)
- `--help` - Show detailed help information

**CLI Examples:**

```bash
# Scan a React project
npm-vuln-checker --cli --csv security-db.csv --project ./my-react-app --output security-report.txt

# Scan with custom CSV format (package in column 3, version in column 4)
npm-vuln-checker --cli --csv custom-vulns.csv --project . --output report.txt --name-col 3 --version-col 4

# Automated scanning in CI/CD
#!/bin/bash
if npm-vuln-checker --cli --csv vulns.csv --project . --output scan-result.txt; then
    echo "No vulnerabilities found"
else
    echo "Vulnerabilities detected! Check scan-result.txt"
    exit 1
fi
```

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

Both GUI and CLI modes generate the same comprehensive plain text report containing:
- **Status**: OK or VULNERABLE
- **Vulnerability Details**: Lists each vulnerable package found
- **Dependency Information**: Shows if packages are direct or indirect dependencies
- **Scan Summary**: Overview of the analysis results
- **Timestamps**: When the scan was performed

**Example Report:**
```
=== NPM Vulnerability Scan Report ===
Generated: 2024-01-15 14:30:22
Project: ./my-react-app
Total Dependencies: 847
Vulnerabilities Found: 2

STATUS: VULNERABLE

=== VULNERABILITIES ===

1. Package: lodash
   Used Version: 4.17.19
   Vulnerable Versions: 4.17.20 4.17.19
   Dependency Type: Indirect
   Path: react-scripts@5.0.1 -> webpack@5.64.4 -> lodash@4.17.19

2. Package: express
   Used Version: 4.17.0
   Vulnerable Versions: 4.17.0 4.16.4
   Dependency Type: Direct
   Path: express@4.17.0

=== SCAN SUMMARY ===
Total packages scanned: 847
Vulnerable packages found: 2
Direct dependencies affected: 1
Indirect dependencies affected: 1
```

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

## Practical Usage Examples

### Quick Start

1. **Download/extract** a release package for your platform
2. **Create a vulnerability CSV file** (or download from a security database):
   ```csv
   Package,Vulnerable_Versions,Severity
   lodash,4.17.20 4.17.19,High
   express,4.17.0 4.16.4,Medium
   moment,2.29.1 2.29.0,Low
   ```
3. **Run a scan**:
   ```bash
   # GUI mode - just double-click the executable

   # CLI mode
   npm-vuln-checker --cli --csv vulns.csv --project ./my-project --output security-report.txt
   ```

### Integration with CI/CD

**GitHub Actions Example:**
```yaml
- name: Security Scan
  run: |
    curl -O https://example.com/security-db.csv
    ./npm-vuln-checker --cli --csv security-db.csv --project . --output security-report.txt
    if [ $? -ne 0 ]; then
      echo \"Security vulnerabilities found!\"
      cat security-report.txt
      exit 1
    fi
```

**Jenkins Pipeline:**
```groovy
stage('Security Scan') {
    steps {
        sh './npm-vuln-checker --cli --csv security-db.csv --project . --output security-report.txt'
        archiveArtifacts 'security-report.txt'
        script {
            if (sh(returnStatus: true, script: 'test $? -ne 0') == 0) {
                error('Security vulnerabilities detected!')
            }
        }
    }
}
```

### Working with Different CSV Formats

If your vulnerability database has a different format:

```csv
ID,Category,PackageName,AffectedVersions,CVSS
CVE-2021-1234,XSS,react,16.0.0 16.1.0,7.5
CVE-2021-5678,Injection,mysql,2.18.0 2.17.1,9.1
```

Use custom column mapping:
```bash
npm-vuln-checker --cli --csv custom-db.csv --project . --output report.txt --name-col 3 --version-col 4
```

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

**CLI-Specific Issues:**
- **Missing arguments**: Use `--help` to see required parameters
- **Permission denied**: Ensure write permissions for output file location
- **CSV not found**: Use absolute paths or check current working directory
- **Project not found**: Ensure the project directory contains a `package.json` file
- **npm command fails**: Make sure npm is in PATH and the project has valid dependencies

**GUI Issues:**
- **GTK errors on Linux/macOS**: Install GTK+3 runtime libraries
- **Window doesn't appear**: Check for permission issues or missing display environment

**Permission Issues:**
- Make shell scripts executable: `chmod +x *.sh`
- Run with appropriate permissions for file access
- On Windows, ensure the executable isn't blocked by antivirus

**Exit Codes (CLI Mode):**
- `0`: Success, no vulnerabilities found
- `1`: Vulnerabilities found or execution error
- Use exit codes in scripts for automated decision making