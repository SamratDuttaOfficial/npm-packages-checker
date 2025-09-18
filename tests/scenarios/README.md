# Test Scenarios

This directory contains organized test scenarios for the NPM Vulnerability Checker.

## Test Scenarios

### Scenario 1: Mixed Vulnerabilities (STATUS: VULNERABLE)
- **CSV**: `fixtures/csv/scenario_mixed_vulnerabilities.csv`
- **NPM**: `fixtures/npm/scenario_mixed_dependencies.json`
- **Expected**: `fixtures/expected_reports/scenario_mixed_status_vulnerable.txt`
- **Description**: Contains both vulnerabilities and safe packages

### Scenario 2: Safe Packages (STATUS: OK)
- **CSV**: `fixtures/csv/scenario_safe_packages.csv`
- **NPM**: `fixtures/npm/scenario_safe_dependencies.json`
- **Expected**: `fixtures/expected_reports/scenario_safe_status_ok.txt`
- **Description**: All packages are safe versions

### Scenario 3: Partial Matches (STATUS: WARNING)
- **CSV**: `fixtures/csv/scenario_partial_matches.csv`
- **NPM**: `fixtures/npm/scenario_partial_dependencies.json`
- **Expected**: `fixtures/expected_reports/scenario_partial_status_warning.txt`
- **Description**: Contains only partial/similar package matches

## Edge Cases

### Edge Case Files
- `fixtures/csv/edge_case_complex_format.csv` - Complex CSV with extra columns
- `fixtures/csv/edge_case_malformed_data.csv` - Malformed/empty data
- `fixtures/csv/edge_case_empty_file.csv` - Empty CSV file

## File Naming Convention

- **scenario_** prefix for main test scenarios
- **edge_case_** prefix for edge case testing
- Descriptive names indicating the purpose/content