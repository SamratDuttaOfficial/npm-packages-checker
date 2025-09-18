#include "../test_framework.h"
#include "../../src/csv_parser.h"
#include "../../src/npm_analyzer.h"
#include "../../src/vulnerability_checker.h"
#include "../../src/report_generator.h"
#include "../mock_data/npm_mock.h"
#include "test_data_paths.h"
#include "integration_test_setup.h"

int test_full_workflow_no_vulnerabilities() {
    const char *csv_content =
        "Package,Versions,Severity\n"
        "nonexistent-package,1.0.0,High\n"
        "another-fake-package,2.0.0,Medium\n";

    const char *csv_file = TEST_OUTPUT_DIR "/test_no_vuln.csv";
    const char *npm_file = TEST_OUTPUT_DIR "/test_no_vuln_npm.json";
    const char *report_file = TEST_OUTPUT_DIR "/test_no_vuln_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_file, MOCK_NPM_SIMPLE_OUTPUT);

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 0, 1, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should load CSV successfully");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    int npm_result = parse_npm_json_output(npm_file, dependencies);
    TEST_ASSERT_EQ(1, npm_result, "Should parse npm output successfully");

    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should check vulnerabilities successfully");
    TEST_ASSERT_EQ(0, results.count, "Should find no vulnerabilities");

    char report[10000] = {0};
    int report_result = generate_vulnerability_report(&results, vulnerable_packages,
                                                    dependencies, report, sizeof(report));
    TEST_ASSERT_EQ(1, report_result, "Should generate report successfully");

    int save_result = save_report_to_file(report_file, report);
    TEST_ASSERT_EQ(1, save_result, "Should save report successfully");

    TEST_ASSERT(file_exists(report_file), "Report file should exist");
    TEST_ASSERT(strstr(report, "STATUS: OK") != NULL, "Report should show OK status");

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
    cleanup_test_file(csv_file);
    cleanup_test_file(npm_file);
    cleanup_test_file(report_file);
    return 1;
}

int test_full_workflow_with_vulnerabilities() {
    const char *csv_content =
        "Package,Versions,Severity\n"
        "lodash,4.17.20,High\n"
        "express,4.17.1,Medium\n"
        "moment,2.24.0,Low\n";

    const char *csv_file = TEST_CSV_WITH_VULN;
    const char *npm_file = TEST_NPM_WITH_VULN;
    const char *report_file = TEST_OUTPUT_DIR "/test_with_vuln_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_file, MOCK_NPM_SIMPLE_OUTPUT);

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 0, 1, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should load CSV successfully");
    TEST_ASSERT_EQ(3, vulnerable_packages->count, "Should load 3 vulnerable packages");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    int npm_result = parse_npm_json_output(npm_file, dependencies);
    TEST_ASSERT_EQ(1, npm_result, "Should parse npm output successfully");
    TEST_ASSERT_EQ(3, dependencies->count, "Should parse 3 dependencies");

    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should check vulnerabilities successfully");
    TEST_ASSERT_EQ(3, results.count, "Should find 3 vulnerabilities");

    char report[10000] = {0};
    int report_result = generate_vulnerability_report(&results, vulnerable_packages,
                                                    dependencies, report, sizeof(report));
    TEST_ASSERT_EQ(1, report_result, "Should generate report successfully");

    int save_result = save_report_to_file(report_file, report);
    TEST_ASSERT_EQ(1, save_result, "Should save report successfully");

    TEST_ASSERT(file_exists(report_file), "Report file should exist");
    TEST_ASSERT(strstr(report, "STATUS: VULNERABLE") != NULL, "Report should show VULNERABLE status");
    TEST_ASSERT(strstr(report, "Found 3 vulnerable package(s)") != NULL, "Report should show correct count");

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
    cleanup_test_file(csv_file);
    cleanup_test_file(npm_file);
    cleanup_test_file(report_file);
    return 1;
}

int test_full_workflow_partial_matches() {
    const char *csv_content =
        "Package,Versions,Severity\n"
        "lodash,4.17.19 4.17.20,High\n"
        "express,4.16.4,Medium\n"
        "nonexistent,1.0.0,Critical\n";

    const char *csv_file = TEST_CSV_PARTIAL;
    const char *npm_file = TEST_NPM_PARTIAL;
    const char *report_file = TEST_OUTPUT_DIR "/test_partial_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_file, MOCK_NPM_SIMPLE_OUTPUT);

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 0, 1, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should load CSV successfully");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    int npm_result = parse_npm_json_output(npm_file, dependencies);
    TEST_ASSERT_EQ(1, npm_result, "Should parse npm output successfully");

    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should check vulnerabilities successfully");

    TEST_ASSERT_EQ(1, results.count, "Should find 1 vulnerability (lodash 4.17.20)");

    char report[10000] = {0};
    int report_result = generate_vulnerability_report(&results, vulnerable_packages, dependencies,
                                report, sizeof(report));
    TEST_ASSERT_EQ(1, report_result, "Should generate report successfully");

    int save_result = save_report_to_file(report_file, report);
    TEST_ASSERT_EQ(1, save_result, "Should save report successfully");

    TEST_ASSERT(strstr(report, "lodash") != NULL, "Report should mention lodash");
    TEST_ASSERT(strstr(report, "express@4.17.1 -> Safe version") != NULL, "Express should be marked as safe version");
    TEST_ASSERT(strstr(report, "express@4.17.1 -> VULNERABLE") == NULL, "Express should not be marked as vulnerable");

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
    cleanup_test_file(csv_file);
    cleanup_test_file(npm_file);
    cleanup_test_file(report_file);
    return 1;
}

int test_full_workflow_complex_csv_format() {
    const char *csv_content =
        "ID,Severity,Package Name,Vulnerable Versions,CVE,CVSS\n"
        "1,High,  lodash  , 4.17.19   4.17.20  ,CVE-2021-23337,8.5\n"
        "2,Medium, express ,  4.16.4  ,CVE-2022-24999,6.1\n"
        "3,Low,moment,2.24.0,CVE-2022-24785,3.7\n";

    const char *csv_file = TEST_OUTPUT_DIR "/test_complex.csv";
    const char *npm_file = TEST_OUTPUT_DIR "/test_complex_npm.json";
    const char *report_file = TEST_OUTPUT_DIR "/test_complex_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_file, MOCK_NPM_SIMPLE_OUTPUT);

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 2, 3, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should handle complex CSV format");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    int npm_result = parse_npm_json_output(npm_file, dependencies);
    TEST_ASSERT_EQ(1, npm_result, "Should parse npm output successfully");

    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should check vulnerabilities successfully");

    char report[10000] = {0};
    int report_result = generate_vulnerability_report(&results, vulnerable_packages, dependencies,
                                report, sizeof(report));
    TEST_ASSERT_EQ(1, report_result, "Should generate report successfully");

    int save_result = save_report_to_file(report_file, report);
    TEST_ASSERT_EQ(1, save_result, "Should save report successfully");

    TEST_ASSERT(results.count > 0, "Should find vulnerabilities despite complex format");

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
    cleanup_test_file(csv_file);
    cleanup_test_file(npm_file);
    cleanup_test_file(report_file);
    return 1;
}

int test_full_workflow_edge_cases() {
    const char *csv_content =
        "Package,Versions,Severity\n"
        ",4.17.19,High\n"
        "lodash,,Medium\n"
        " , ,Low\n"
        "express,4.17.1,Critical\n";

    const char *csv_file = TEST_OUTPUT_DIR "/test_edge.csv";
    const char *npm_file = TEST_OUTPUT_DIR "/test_edge_npm.json";
    const char *report_file = TEST_OUTPUT_DIR "/test_edge_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_file, MOCK_NPM_SIMPLE_OUTPUT);

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 0, 1, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should load CSV successfully");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    int npm_result = parse_npm_json_output(npm_file, dependencies);
    TEST_ASSERT_EQ(1, npm_result, "Should parse npm output successfully");

    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should check vulnerabilities successfully");

    char report[10000] = {0};
    int report_result = generate_vulnerability_report(&results, vulnerable_packages,
                                                    dependencies, report, sizeof(report));
    TEST_ASSERT_EQ(1, report_result, "Should handle edge cases gracefully");

    int save_result = save_report_to_file(report_file, report);
    TEST_ASSERT_EQ(1, save_result, "Should save report successfully");

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
    cleanup_test_file(csv_file);
    cleanup_test_file(npm_file);
    cleanup_test_file(report_file);
    return 1;
}

int test_full_workflow_large_dataset() {
    char csv_content[10000];
    strcpy(csv_content, "Package,Versions,Severity\n");

    for (int i = 0; i < 50; i++) {
        char line[200];
        snprintf(line, sizeof(line), "package-%d,1.%d.0 1.%d.1,High\n", i, i, i);
        strcat(csv_content, line);
    }

    strcat(csv_content, "lodash,4.17.20,Critical\n");
    strcat(csv_content, "express,4.17.1,High\n");

    const char *csv_file = TEST_CSV_LARGE;
    const char *npm_file = TEST_NPM_LARGE;
    const char *report_file = TEST_OUTPUT_DIR "/test_large_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_file, MOCK_NPM_SIMPLE_OUTPUT);

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 0, 1, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should load CSV successfully");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    int npm_result = parse_npm_json_output(npm_file, dependencies);
    TEST_ASSERT_EQ(1, npm_result, "Should parse npm output successfully");

    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should check vulnerabilities successfully");

    char report[20000] = {0};
    int report_result = generate_vulnerability_report(&results, vulnerable_packages, dependencies,
                                report, sizeof(report));
    TEST_ASSERT_EQ(1, report_result, "Should generate report successfully");

    int save_result = save_report_to_file(report_file, report);
    TEST_ASSERT_EQ(1, save_result, "Should save report successfully");

    TEST_ASSERT(vulnerable_packages->count > 50, "Should load large number of packages");
    TEST_ASSERT(results.count >= 2, "Should find at least lodash and express");

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
    cleanup_test_file(csv_file);
    cleanup_test_file(npm_file);
    cleanup_test_file(report_file);
    return 1;
}

int test_full_workflow_report_format() {
    const char *csv_content =
        "Package,Versions,Severity\n"
        "lodash,4.17.20,High\n"
        "express,4.17.1,Medium\n"
        "moment,2.24.0,Low\n";

    const char *csv_file = TEST_CSV_FORMAT;
    const char *npm_file = TEST_NPM_FORMAT;
    const char *report_file = TEST_OUTPUT_DIR "/test_format_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_file, MOCK_NPM_SIMPLE_OUTPUT);

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 0, 1, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should load CSV successfully");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    int npm_result = parse_npm_json_output(npm_file, dependencies);
    TEST_ASSERT_EQ(1, npm_result, "Should parse npm output successfully");

    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should check vulnerabilities successfully");

    char report[10000] = {0};
    int report_result = generate_vulnerability_report(&results, vulnerable_packages,
                                                    dependencies, report, sizeof(report));
    TEST_ASSERT_EQ(1, report_result, "Should generate report successfully");

    // Test specific format requirements
    TEST_ASSERT(strstr(report, "Package Name: lodash 4.17.20 (Reported Vulnerable Package: lodash 4.17.20)") != NULL,
                "Should use correct vulnerability format");
    TEST_ASSERT(strstr(report, "All Instances in the Project:") != NULL,
                "Should show 'All Instances in the Project'");
    TEST_ASSERT(strstr(report, "Dependency Levels:") != NULL,
                "Should show dependency levels");

    // Compare key sections with expected format
    TEST_ASSERT(strstr(report, "VULNERABILITY DETAILS:") != NULL, "Should have vulnerability details section");
    TEST_ASSERT(strstr(report, "SCAN SUMMARY:") != NULL, "Should have scan summary section");
    TEST_ASSERT(strstr(report, "PACKAGE MATCHING ANALYSIS:") != NULL, "Should have package matching analysis section");

    // Test that timestamp is filtered out for consistent comparison
    char *timestamp_start = strstr(report, "Generated: ");
    if (timestamp_start) {
        char *line_end = strchr(timestamp_start, '\n');
        if (line_end) {
            // Replace timestamp line with fixed one for comparison
            memmove(timestamp_start + strlen("Generated: 2025-09-18 03:15:42"),
                   line_end, strlen(line_end) + 1);
            memcpy(timestamp_start, "Generated: 2025-09-18 03:15:42",
                   strlen("Generated: 2025-09-18 03:15:42"));
        }
    }

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
    cleanup_test_file(csv_file);
    cleanup_test_file(npm_file);
    cleanup_test_file(report_file);
    return 1;
}

int test_full_workflow_ok_status() {
    const char *csv_file = "tests/fixtures/csv/scenario_safe_packages.csv";
    const char *npm_file = "tests/fixtures/npm/scenario_safe_dependencies.json";
    const char *report_file = TEST_OUTPUT_DIR "/test_ok_report.txt";

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 0, 1, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should load CSV successfully");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    int npm_result = parse_npm_json_output(npm_file, dependencies);
    TEST_ASSERT_EQ(1, npm_result, "Should parse npm output successfully");

    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should check vulnerabilities successfully");

    char report[10000] = {0};
    int report_result = generate_vulnerability_report(&results, vulnerable_packages, dependencies, report, sizeof(report));
    TEST_ASSERT_EQ(1, report_result, "Should generate report successfully");

    TEST_ASSERT_EQ(0, results.count, "Should find no vulnerabilities");
    TEST_ASSERT_EQ(0, results.warning_count, "Should find no warnings");
    TEST_ASSERT(strstr(report, "STATUS: OK") != NULL, "Should show OK status");
    TEST_ASSERT(strstr(report, "No vulnerable packages found") != NULL, "Should indicate no vulnerabilities");
    TEST_ASSERT(strstr(report, "Safe version") != NULL, "Should show safe versions");

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
    cleanup_test_file(report_file);
    return 1;
}

int test_full_workflow_warning_status() {
    const char *csv_file = "tests/fixtures/csv/scenario_partial_matches.csv";
    const char *npm_file = "tests/fixtures/npm/scenario_partial_dependencies.json";
    const char *report_file = TEST_OUTPUT_DIR "/test_warning_report.txt";

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 0, 1, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should load CSV successfully");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    int npm_result = parse_npm_json_output(npm_file, dependencies);
    TEST_ASSERT_EQ(1, npm_result, "Should parse npm output successfully");

    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should check vulnerabilities successfully");

    char report[10000] = {0};
    int report_result = generate_vulnerability_report(&results, vulnerable_packages, dependencies, report, sizeof(report));
    TEST_ASSERT_EQ(1, report_result, "Should generate report successfully");

    TEST_ASSERT_EQ(0, results.count, "Should find no direct vulnerabilities");
    TEST_ASSERT(results.warning_count > 0, "Should find warnings for partial matches");
    TEST_ASSERT(strstr(report, "STATUS: WARNING") != NULL, "Should show WARNING status");
    TEST_ASSERT(strstr(report, "POTENTIAL PACKAGE MATCHES:") != NULL, "Should show potential matches section");

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
    cleanup_test_file(report_file);
    return 1;
}


void run_full_workflow_integration_tests() {
    printf("\n=== Full Workflow Integration Tests ===\n");

    RUN_TEST(test_full_workflow_no_vulnerabilities);
    RUN_TEST(test_full_workflow_with_vulnerabilities);
    RUN_TEST(test_full_workflow_partial_matches);
    RUN_TEST(test_full_workflow_complex_csv_format);
    RUN_TEST(test_full_workflow_edge_cases);
    RUN_TEST(test_full_workflow_large_dataset);
    RUN_TEST(test_full_workflow_report_format);
    RUN_TEST(test_full_workflow_ok_status);
    RUN_TEST(test_full_workflow_warning_status);
}