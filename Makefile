# NPM Vulnerability Checker - Enhanced Makefile with Testing

# Detect operating system
UNAME_S := $(shell uname -s)

# Default target
all: build

# Build for current platform
build:
ifeq ($(OS),Windows_NT)
	@echo "Building for Windows..."
	@cmd /c scripts\windows\build.bat
else ifeq ($(UNAME_S),Linux)
	@echo "Building for Linux..."
	@chmod +x scripts/build_linux.sh
	@./scripts/build_linux.sh
else ifeq ($(UNAME_S),Darwin)
	@echo "Building for macOS..."
	@chmod +x scripts/build_macos.sh
	@./scripts/build_macos.sh
else
	@echo "Unsupported platform: $(UNAME_S)"
	@exit 1
endif

# Build tests
test-build:
ifeq ($(OS),Windows_NT)
	@cmd /c scripts\windows\test.bat
else
	@chmod +x scripts/test.sh
	@./scripts/test.sh
endif

# Run tests (build if necessary)
test: test-build

# Run only unit tests
test-unit:
ifeq ($(OS),Windows_NT)
	@if exist "build\tests\run_all_tests.exe" ( \
		build\tests\run_all_tests.exe --unit-only \
	) else ( \
		cmd /c "build_tests.bat --unit-only" \
	)
else
	@if [ -f "build/tests/run_all_tests" ]; then \
		./build/tests/run_all_tests --unit-only; \
	else \
		chmod +x scripts/test.sh && ./scripts/test.sh --unit-only; \
	fi
endif

# Run only integration tests
test-integration:
ifeq ($(OS),Windows_NT)
	@if exist "build\tests\run_all_tests.exe" ( \
		build\tests\run_all_tests.exe --integration-only \
	) else ( \
		cmd /c "build_tests.bat --integration-only" \
	)
else
	@if [ -f "build/tests/run_all_tests" ]; then \
		./build/tests/run_all_tests --integration-only; \
	else \
		chmod +x scripts/test.sh && ./scripts/test.sh --integration-only; \
	fi
endif

# Run only end-to-end tests
test-e2e:
ifeq ($(OS),Windows_NT)
	@if exist "build\tests\run_all_tests.exe" ( \
		build\tests\run_all_tests.exe --e2e-only \
	) else ( \
		cmd /c "build_tests.bat --e2e-only" \
	)
else
	@if [ -f "build/tests/run_all_tests" ]; then \
		./build/tests/run_all_tests --e2e-only; \
	else \
		chmod +x scripts/test.sh && ./scripts/test.sh --e2e-only; \
	fi
endif

# Build for specific platforms
linux:
	@chmod +x scripts/build_linux.sh
	@./scripts/build_linux.sh

windows:
ifeq ($(OS),Windows_NT)
	@cmd /c scripts\windows\build.bat
else
	@echo "Cross-compilation to Windows not supported from $(UNAME_S)"
	@echo "Please build on a Windows machine or use MinGW cross-compiler"
endif

macos:
ifeq ($(UNAME_S),Darwin)
	@chmod +x scripts/build_macos.sh
	@./scripts/build_macos.sh
else
	@echo "Cross-compilation to macOS not supported from $(UNAME_S)"
	@echo "Please build on a macOS machine"
endif

# Clean build artifacts
clean:
	@echo "Cleaning build directories..."
	@rm -rf build/
	@echo "Clean complete."

# Create distribution packages
dist: build
	@echo "Creating distribution packages..."
	@mkdir -p dist
ifeq ($(OS),Windows_NT)
	@mkdir -p dist/windows
	@cp build/windows/npm-vuln-checker.exe dist/windows/
	@cp scripts/windows/run.bat dist/windows/run_windows.bat
	@cp README.md dist/windows/
	@echo "Windows distribution created in dist/windows/"
else ifeq ($(UNAME_S),Linux)
	@mkdir -p dist/linux
	@cp build/linux/npm-vuln-checker dist/linux/
	@cp scripts/run_linux.sh dist/linux/
	@cp README.md dist/linux/
	@chmod +x dist/linux/run_linux.sh
	@chmod +x dist/linux/npm-vuln-checker
	@echo "Linux distribution created in dist/linux/"
else ifeq ($(UNAME_S),Darwin)
	@mkdir -p dist/macos
	@cp build/macos/npm-vuln-checker dist/macos/
	@cp scripts/run_macos.sh dist/macos/
	@cp README.md dist/macos/
	@chmod +x dist/macos/run_macos.sh
	@chmod +x dist/macos/npm-vuln-checker
	@echo "macOS distribution created in dist/macos/"
endif

# Install system dependencies
deps:
ifeq ($(OS),Windows_NT)
	@echo "Please install manually:"
	@echo "  - MinGW-w64 or Visual Studio Build Tools"
	@echo "  - Node.js and npm"
else ifeq ($(UNAME_S),Linux)
	@echo "Installing Linux dependencies..."
	@if command -v apt-get >/dev/null 2>&1; then \
		sudo apt-get update && sudo apt-get install -y libgtk-3-dev build-essential; \
	elif command -v yum >/dev/null 2>&1; then \
		sudo yum install -y gtk3-devel gcc; \
	elif command -v dnf >/dev/null 2>&1; then \
		sudo dnf install -y gtk3-devel gcc; \
	elif command -v pacman >/dev/null 2>&1; then \
		sudo pacman -S gtk3 gcc; \
	else \
		echo "Unknown package manager. Please install GTK+3 development libraries manually."; \
	fi
else ifeq ($(UNAME_S),Darwin)
	@echo "Installing macOS dependencies..."
	@if command -v brew >/dev/null 2>&1; then \
		brew install gtk+3; \
	else \
		echo "Please install Homebrew first: https://brew.sh"; \
		echo "Then run: brew install gtk+3"; \
	fi
endif

# Run the application
run: build
ifeq ($(OS),Windows_NT)
	@build\windows\npm-vuln-checker.exe
else ifeq ($(UNAME_S),Linux)
	@./build/linux/npm-vuln-checker
else ifeq ($(UNAME_S),Darwin)
	@./build/macos/npm-vuln-checker
endif

# Comprehensive test and build
ci: clean deps build test
	@echo "Continuous integration pipeline completed successfully!"

# Development workflow
dev: build test
	@echo "Development build and test completed!"

# Help target
help:
	@echo "NPM Vulnerability Checker - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build for current platform (default)"
	@echo "  build         - Build for current platform"
	@echo "  linux         - Build for Linux"
	@echo "  windows       - Build for Windows"
	@echo "  macos         - Build for macOS"
	@echo ""
	@echo "Testing targets:"
	@echo "  test          - Build and run all tests"
	@echo "  test-build    - Build tests only"
	@echo "  test-unit     - Run unit tests only"
	@echo "  test-integration - Run integration tests only"
	@echo "  test-e2e      - Run end-to-end tests only"
	@echo ""
	@echo "Utility targets:"
	@echo "  clean         - Remove build artifacts"
	@echo "  dist          - Create distribution package"
	@echo "  deps          - Install system dependencies"
	@echo "  run           - Build and run the application"
	@echo "  ci            - Full CI pipeline (clean + deps + build + test)"
	@echo "  dev           - Development workflow (build + test)"
	@echo "  help          - Show this help message"

.PHONY: all build linux windows macos clean dist deps run test test-build test-unit test-integration test-e2e ci dev help