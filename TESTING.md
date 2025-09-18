# Testing

## Quick Start

### Windows
```cmd
run_tests.bat
```

### Linux/macOS
```bash
./run_tests.sh
```

The test scripts automatically build and run all tests.

## Prerequisites

**All platforms**: C compiler (GCC/MinGW-w64/Visual Studio Build Tools)

## Test Structure

```
tests/
├── unit/          # Component tests
├── integration/   # Workflow tests
├── e2e/          # End-to-end tests
└── fixtures/     # Test data
```

## Specific Test Categories

After building once, run specific categories:

**Windows:**
```cmd
build\tests\run_all_tests.exe --unit-only
build\tests\run_all_tests.exe --integration-only
build\tests\run_all_tests.exe --e2e-only
```

**Linux/macOS:**
```bash
./build/tests/run_all_tests --unit-only
./build/tests/run_all_tests --integration-only
./build/tests/run_all_tests --e2e-only
```