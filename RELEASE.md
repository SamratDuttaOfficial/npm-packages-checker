# Release Process for NPM Vulnerability Checker

This document outlines how to create and distribute releases of the NPM Vulnerability Checker.

## Building Distribution Packages

### Prerequisites
- Ensure you have the development environment set up for each target platform
- All platforms require npm to be installed and available in PATH

### Building on Windows

```cmd
# Build Windows distribution
scripts\windows\build_dist.bat
```

This creates:
- `dist/windows/` - Standalone Windows package
- `dist/npm-vuln-checker-windows.tar.gz` - Archive for release

### Building on Linux/macOS

```bash
# Make script executable (first time only)
chmod +x scripts/build_dist.sh

# Build distributions
./scripts/build_dist.sh
```

This creates:
- `dist/linux/` - Standalone Linux package (on Linux)
- `dist/macos/` - Standalone macOS package (on macOS)
- `dist/npm-vuln-checker-linux-<arch>.tar.gz` - Linux archive
- `dist/npm-vuln-checker-macos-<arch>.tar.gz` - macOS archive
- `dist/DISTRIBUTION_INFO.md` - Information about the packages

### Cross-Platform Build Strategy

Since cross-compilation can be complex, the recommended approach is:

1. **Build on Windows**: Run `scripts\windows\build_dist.bat` to create Windows packages
2. **Build on Linux**: Run `./scripts/build_dist.sh` to create Linux packages
3. **Build on macOS**: Run `./scripts/build_dist.sh` to create macOS packages

## Creating a GitHub Release

### 1. Prepare Release Assets

After building on all platforms, you should have:
- `npm-vuln-checker-windows.tar.gz`
- `npm-vuln-checker-linux-x86_64.tar.gz` (or similar)
- `npm-vuln-checker-macos-arm64.tar.gz` (or similar)

### 2. Create the Release

#### Option A: Using GitHub Web Interface

1. Go to your repository on GitHub
2. Click "Releases" in the right sidebar
3. Click "Create a new release"
4. Fill in the release details:
   - **Tag version**: `v1.0.0` (follow semantic versioning)
   - **Release title**: `NPM Vulnerability Checker v1.0.0`
   - **Description**: Include changelog and installation instructions
5. Upload the `.tar.gz` files as release assets
6. Click "Publish release"

#### Option B: Using GitHub CLI (gh)

```bash
# Create a new release
gh release create v1.0.0 \
    dist/npm-vuln-checker-windows.tar.gz \
    dist/npm-vuln-checker-linux-*.tar.gz \
    dist/npm-vuln-checker-macos-*.tar.gz \
    --title "NPM Vulnerability Checker v1.0.0" \
    --notes-file CHANGELOG.md

# Or create with auto-generated notes
gh release create v1.0.0 \
    dist/*.tar.gz \
    --title "NPM Vulnerability Checker v1.0.0" \
    --generate-notes
```

### 3. Release Description Template

```markdown
# NPM Vulnerability Checker v1.0.0

## What's New
- [Feature] Description of new features
- [Fix] Description of bug fixes
- [Change] Description of changes

## Installation

Download the appropriate package for your platform:

### Windows
1. Download `npm-vuln-checker-windows.tar.gz`
2. Extract: `tar -xzf npm-vuln-checker-windows.tar.gz`
3. Run: `npm-vuln-checker.exe` or `run_windows.bat`

### Linux
1. Download `npm-vuln-checker-linux-x86_64.tar.gz`
2. Extract: `tar -xzf npm-vuln-checker-linux-x86_64.tar.gz`
3. Make executable: `chmod +x npm-vuln-checker`
4. Run: `./npm-vuln-checker` or `./run_linux.sh`

### macOS
1. Download `npm-vuln-checker-macos-arm64.tar.gz` (or x86_64)
2. Extract: `tar -xzf npm-vuln-checker-macos-arm64.tar.gz`
3. Make executable: `chmod +x npm-vuln-checker`
4. Run: `./npm-vuln-checker` or `./run_macos.sh`

## Requirements
- Node.js and npm must be installed on the target system
- Linux/macOS: GTK+3 runtime libraries (usually pre-installed)

## Checksums
[Include SHA256 checksums of release files]
```

## Automated Release Workflow (Optional)

### GitHub Actions Workflow

Create `.github/workflows/release.yml`:

```yaml
name: Create Release

on:
  push:
    tags:
      - 'v*'

jobs:
  build-windows:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4
      - name: Build Windows
        run: build_dist.bat
      - name: Upload Windows artifact
        uses: actions/upload-artifact@v3
        with:
          name: windows-dist
          path: dist/npm-vuln-checker-windows.tar.gz

  build-linux:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Install dependencies
        run: sudo apt-get update && sudo apt-get install -y libgtk-3-dev
      - name: Build Linux
        run: ./build_dist.sh
      - name: Upload Linux artifact
        uses: actions/upload-artifact@v3
        with:
          name: linux-dist
          path: dist/npm-vuln-checker-linux-*.tar.gz

  build-macos:
    runs-on: macos-latest
    steps:
      - uses: actions/checkout@v4
      - name: Install dependencies
        run: brew install gtk+3
      - name: Build macOS
        run: ./build_dist.sh
      - name: Upload macOS artifact
        uses: actions/upload-artifact@v3
        with:
          name: macos-dist
          path: dist/npm-vuln-checker-macos-*.tar.gz

  release:
    needs: [build-windows, build-linux, build-macos]
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Download all artifacts
        uses: actions/download-artifact@v3
      - name: Create Release
        uses: softprops/action-gh-release@v1
        with:
          files: |
            windows-dist/*.tar.gz
            linux-dist/*.tar.gz
            macos-dist/*.tar.gz
          generate_release_notes: true
```

## Version Management

### Semantic Versioning
- **MAJOR** (v1.0.0 -> v2.0.0): Breaking changes
- **MINOR** (v1.0.0 -> v1.1.0): New features, backward compatible
- **PATCH** (v1.0.0 -> v1.0.1): Bug fixes, backward compatible

### Release Schedule
- **Patch releases**: As needed for critical bugs
- **Minor releases**: Monthly or when significant features are ready
- **Major releases**: When breaking changes are necessary

## Testing Before Release

1. Build all platform distributions
2. Test on clean systems without development tools
3. Verify all dependencies are bundled or documented
4. Test with various NPM projects and CSV files
5. Run automated tests: `make test`

## Post-Release Tasks

1. Update documentation if needed
2. Announce release on relevant channels
3. Monitor for issues and feedback
4. Update any package managers or distribution channels