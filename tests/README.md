# NPM Vulnerability Checker - Test Suite

## Directory Structure

```
tests/
├── unit/                           # Unit tests
│   ├── test_csv_parser.c
│   ├── test_report_generator.c
│   └── test_vulnerability_checker.c
├── integration/                    # Integration tests
│   ├── test_full_workflow.c
│   └── test_npm_analyzer.c
├── e2e/                           # End-to-end tests
│   └── test_end_to_end.c
├── fixtures/                      # Test data fixtures
│   ├── csv/                       # CSV vulnerability databases
│   │   ├── scenario_mixed_vulnerabilities.csv
│   │   ├── scenario_safe_packages.csv
│   │   ├── scenario_partial_matches.csv
│   │   ├── edge_case_complex_format.csv
│   │   ├── edge_case_malformed_data.csv
│   │   └── edge_case_empty_file.csv
│   ├── npm/                       # NPM dependency outputs
│   │   ├── scenario_mixed_dependencies.json
│   │   ├── scenario_safe_dependencies.json
│   │   └── scenario_partial_dependencies.json
│   └── expected_reports/          # Expected report outputs
│       ├── scenario_mixed_status_vulnerable.txt
│       ├── scenario_safe_status_ok.txt
│       └── scenario_partial_status_warning.txt
├── scenarios/                     # Test scenario documentation
│   └── README.md
├── mock_data/                     # Mock data helpers
│   ├── npm_mock.c
│   └── npm_mock.h
├── test_framework.c               # Test framework implementation
├── test_framework.h               # Test framework headers
└── run_all_tests.c               # Test runner
```

## Test Scenarios

### Main Scenarios
1. **Mixed Vulnerabilities** (STATUS: VULNERABLE)
   - Has both vulnerable packages and safe packages
   - CSV: `scenario_mixed_vulnerabilities.csv`
   - NPM: `scenario_mixed_dependencies.json`
   - Expected: `scenario_mixed_status_vulnerable.txt`

2. **Safe Packages** (STATUS: OK)
   - All packages are safe versions
   - CSV: `scenario_safe_packages.csv`
   - NPM: `scenario_safe_dependencies.json`
   - Expected: `scenario_safe_status_ok.txt`

3. **Partial Matches** (STATUS: WARNING)
   - Only partial/similar package name matches
   - CSV: `scenario_partial_matches.csv`
   - NPM: `scenario_partial_dependencies.json`
   - Expected: `scenario_partial_status_warning.txt`

### Edge Cases
- **Complex Format**: CSV with extra columns and complex formatting
- **Malformed Data**: CSV with empty/invalid entries
- **Empty File**: Empty CSV file

## Running Tests

```bash
# Build and run all tests
./build_tests.sh

# Run specific test categories
./build/tests/run_all_tests --unit-only
./build/tests/run_all_tests --integration-only
./build/tests/run_all_tests --e2e-only
```

## File Naming Convention

- **scenario_** prefix: Main test scenarios
- **edge_case_** prefix: Edge case testing files
- Descriptive names indicating purpose/content
- Consistent naming across CSV, NPM, and expected report files