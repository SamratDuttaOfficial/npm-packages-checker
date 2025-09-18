#include "../test_framework.h"
#include "../../src/csv_parser.h"
#include "../../src/npm_analyzer.h"
#include "../../src/vulnerability_checker.h"
#include "../../src/report_generator.h"
#include "../mock_data/npm_mock.h"

int test_e2e_realistic_scenario() {
    printf("\n--- Running realistic end-to-end scenario ---\n");

    const char *project_path = "e2e_test_project";
    const char *csv_file = "tests/fixtures/csv/scenario_mixed_vulnerabilities.csv";
    const char *npm_output = "e2e_npm_output.json";
    const char *report_file = "e2e_vulnerability_report.txt";

    create_mock_npm_project(project_path);
    create_test_file(npm_output, MOCK_NPM_VULNERABLE_OUTPUT);

    printf("Step 1: Loading vulnerability database from CSV...\n");
    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 0, 1, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should load CSV vulnerability database");
    printf("  Loaded %d vulnerable packages\n", vulnerable_packages->count);

    printf("Step 2: Parsing NPM dependency tree...\n");
    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    int npm_result = parse_npm_json_output(npm_output, dependencies);
    TEST_ASSERT_EQ(1, npm_result, "Should parse NPM dependencies");
    printf("  Found %d dependencies\n", dependencies->count);

    printf("Step 3: Checking for vulnerabilities...\n");
    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should check vulnerabilities successfully");
    printf("  Found %d vulnerabilities\n", results.count);

    printf("Step 4: Generating vulnerability report...\n");
    char report[50000] = {0};
    int report_result = generate_vulnerability_report(&results, vulnerable_packages,
                                                    dependencies, report, sizeof(report));
    TEST_ASSERT_EQ(1, report_result, "Should generate report");

    printf("Step 5: Saving report to file...\n");
    int save_result = save_report_to_file(report_file, report);
    TEST_ASSERT_EQ(1, save_result, "Should save report to file");

    printf("Step 6: Verifying report content...\n");
    TEST_ASSERT(file_exists(report_file), "Report file should exist");

    FILE *report_check = fopen(report_file, "r");
    TEST_ASSERT(report_check != NULL, "Should be able to read report file");

    char report_content[50000];
    size_t read_size = fread(report_content, 1, sizeof(report_content) - 1, report_check);
    report_content[read_size] = '\0';
    fclose(report_check);

    TEST_ASSERT(strstr(report_content, "NPM VULNERABILITY REPORT") != NULL, "Report should have header");
    TEST_ASSERT(strstr(report_content, "Generated:") != NULL, "Report should have timestamp");

    if (results.count > 0) {
        TEST_ASSERT(strstr(report_content, "STATUS: VULNERABLE") != NULL, "Should show vulnerable status");
        TEST_ASSERT(strstr(report_content, "VULNERABILITY DETAILS:") != NULL, "Should have details section");
    } else {
        TEST_ASSERT(strstr(report_content, "STATUS: OK") != NULL, "Should show OK status");
    }

    printf("End-to-end test completed successfully!\n");
    printf("Report saved to: %s\n", report_file);

    cleanup_test_file(npm_output);
    cleanup_mock_npm_project(project_path);
    cleanup_test_file(report_file);

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);

    return 1;
}

int test_e2e_no_vulnerabilities_scenario() {
    printf("\n--- Testing clean project scenario ---\n");

    const char *csv_content =
        "Package,Versions,Severity\n"
        "fake-vulnerable-package,1.0.0,High\n"
        "another-fake-package,2.0.0,Critical\n";

    const char *csv_file = "e2e_clean_csv.csv";
    const char *npm_output = "e2e_clean_npm.json";
    const char *report_file = "e2e_clean_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_output, MOCK_NPM_SIMPLE_OUTPUT);

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    load_csv_packages(csv_file, 0, 1, vulnerable_packages);

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    parse_npm_json_output(npm_output, dependencies);

    VulnerabilityResults results = {0};
    check_vulnerabilities(vulnerable_packages, dependencies, &results);

    char report[10000] = {0};
    generate_vulnerability_report(&results, vulnerable_packages, dependencies,
                                report, sizeof(report));

    save_report_to_file(report_file, report);

    TEST_ASSERT_EQ(0, results.count, "Should find no vulnerabilities");
    TEST_ASSERT(strstr(report, "STATUS: OK") != NULL, "Report should show OK status");

    printf("Clean project test completed - no vulnerabilities found (as expected)\n");

    cleanup_test_file(csv_file);
    cleanup_test_file(npm_output);
    cleanup_test_file(report_file);

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);

    return 1;
}

int test_e2e_mixed_results_scenario() {
    printf("\n--- Testing mixed vulnerability results ---\n");

    const char *csv_content =
        "Package,Versions,Severity\n"
        "lodash,4.17.19,High\n"
        "express,4.16.4,Medium\n"
        "moment,2.24.0,Low\n"
        "axios,0.18.0,Critical\n"
        "nonexistent-package,1.0.0,High\n";

    const char *csv_file = "e2e_mixed_csv.csv";
    const char *npm_output = "e2e_mixed_npm.json";
    const char *report_file = "e2e_mixed_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_output, MOCK_NPM_VULNERABLE_OUTPUT);

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    load_csv_packages(csv_file, 0, 1, vulnerable_packages);

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    parse_npm_json_output(npm_output, dependencies);

    VulnerabilityResults results = {0};
    check_vulnerabilities(vulnerable_packages, dependencies, &results);

    char report[15000] = {0};
    generate_vulnerability_report(&results, vulnerable_packages, dependencies,
                                report, sizeof(report));

    save_report_to_file(report_file, report);

    TEST_ASSERT(results.count > 0, "Should find some vulnerabilities");
    TEST_ASSERT(results.count < vulnerable_packages->count, "Should not match all vulnerable packages");

    printf("Mixed results test completed - found %d/%d potential vulnerabilities\n",
           results.count, vulnerable_packages->count);

    cleanup_test_file(csv_file);
    cleanup_test_file(npm_output);
    cleanup_test_file(report_file);

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);

    return 1;
}

int test_e2e_complex_csv_columns() {
    printf("\n--- Testing complex CSV column configuration ---\n");

    const char *csv_content =
        "ID,CVE,Description,Package Name,Risk,Vulnerable Versions,Date\n"
        "1,CVE-2021-23337,Prototype pollution,  lodash  ,High, 4.17.19   4.17.20  ,2021-03-15\n"
        "2,CVE-2022-24999,HTTP smuggling, express ,Medium,  4.16.4  ,2022-01-20\n"
        "3,CVE-2022-24785,ReDoS attack,moment,Low,2.24.0,2022-02-10\n";

    const char *csv_file = "e2e_complex_csv.csv";
    const char *npm_output = "e2e_complex_npm.json";
    const char *report_file = "e2e_complex_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_output, MOCK_NPM_VULNERABLE_OUTPUT);

    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 3, 5, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should handle complex CSV column layout");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    parse_npm_json_output(npm_output, dependencies);

    VulnerabilityResults results = {0};
    check_vulnerabilities(vulnerable_packages, dependencies, &results);

    char report[15000] = {0};
    generate_vulnerability_report(&results, vulnerable_packages, dependencies,
                                report, sizeof(report));

    save_report_to_file(report_file, report);

    TEST_ASSERT(vulnerable_packages->count >= 3, "Should parse packages from complex CSV");

    printf("Complex CSV test completed - handled custom column layout\n");

    cleanup_test_file(csv_file);
    cleanup_test_file(npm_output);
    cleanup_test_file(report_file);

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);

    return 1;
}

int test_e2e_error_handling() {
    printf("\n--- Testing error handling scenarios ---\n");

    const char *valid_csv = "tests/fixtures/csv/scenario_mixed_vulnerabilities.csv";
    const char *invalid_npm = "nonexistent_npm_output.json";
    const char *invalid_csv = "nonexistent_csv.csv";
    const char *valid_npm = "e2e_error_npm.json";

    create_test_file(valid_npm, MOCK_NPM_SIMPLE_OUTPUT);

    printf("Testing invalid CSV file...\n");
    PackageList *vulnerable_packages1 = create_package_list();
    TEST_ASSERT(vulnerable_packages1 != NULL, "Should create package list");
    int csv_result1 = load_csv_packages(invalid_csv, 0, 1, vulnerable_packages1);
    TEST_ASSERT_EQ(0, csv_result1, "Should fail gracefully with invalid CSV");

    printf("Testing invalid NPM output...\n");
    DependencyTree *dependencies1 = create_dependency_tree();
    TEST_ASSERT(dependencies1 != NULL, "Should create dependency tree");
    int npm_result1 = parse_npm_json_output(invalid_npm, dependencies1);
    TEST_ASSERT_EQ(0, npm_result1, "Should fail gracefully with invalid NPM output");

    printf("Testing valid inputs after errors...\n");
    PackageList *vulnerable_packages2 = create_package_list();
    TEST_ASSERT(vulnerable_packages2 != NULL, "Should create package list");
    int csv_result2 = load_csv_packages(valid_csv, 0, 1, vulnerable_packages2);
    TEST_ASSERT_EQ(1, csv_result2, "Should work with valid CSV after error");

    DependencyTree *dependencies2 = create_dependency_tree();
    TEST_ASSERT(dependencies2 != NULL, "Should create dependency tree");
    int npm_result2 = parse_npm_json_output(valid_npm, dependencies2);
    TEST_ASSERT_EQ(1, npm_result2, "Should work with valid NPM output after error");

    printf("Error handling test completed - system remains stable after errors\n");

    cleanup_test_file(valid_npm);

    free_package_list(vulnerable_packages1);
    free_dependency_tree(dependencies1);
    free_package_list(vulnerable_packages2);
    free_dependency_tree(dependencies2);

    return 1;
}

int test_e2e_performance_stress() {
    printf("\n--- Performance stress test ---\n");

    printf("Generating large CSV file...\n");
    char large_csv_content[50000];
    strcpy(large_csv_content, "Package,Versions,Severity\n");

    for (int i = 0; i < 500; i++) {
        char line[100];
        snprintf(line, sizeof(line), "package-%03d,1.%d.0 1.%d.1 1.%d.2,High\n", i, i%10, i%10, i%10);
        strcat(large_csv_content, line);
    }

    strcat(large_csv_content, "lodash,4.17.19 4.17.20,Critical\n");
    strcat(large_csv_content, "express,4.16.4,High\n");

    const char *csv_file = "e2e_stress_csv.csv";
    const char *npm_output = "e2e_stress_npm.json";
    const char *report_file = "e2e_stress_report.txt";

    create_test_file(csv_file, large_csv_content);
    create_test_file(npm_output, MOCK_NPM_VULNERABLE_OUTPUT);

    printf("Processing large dataset...\n");
    PackageList *vulnerable_packages = create_package_list();
    TEST_ASSERT(vulnerable_packages != NULL, "Should create package list");
    int csv_result = load_csv_packages(csv_file, 0, 1, vulnerable_packages);
    TEST_ASSERT_EQ(1, csv_result, "Should handle large CSV file");

    DependencyTree *dependencies = create_dependency_tree();
    TEST_ASSERT(dependencies != NULL, "Should create dependency tree");
    parse_npm_json_output(npm_output, dependencies);

    VulnerabilityResults results = {0};
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);
    TEST_ASSERT_EQ(1, vuln_result, "Should handle large vulnerability checking");

    char report[100000] = {0};
    generate_vulnerability_report(&results, vulnerable_packages, dependencies,
                                report, sizeof(report));

    save_report_to_file(report_file, report);

    TEST_ASSERT(vulnerable_packages->count > 500, "Should load large number of packages");

    printf("Stress test completed - processed %d packages and %d dependencies\n",
           vulnerable_packages->count, dependencies->count);

    cleanup_test_file(csv_file);
    cleanup_test_file(npm_output);
    cleanup_test_file(report_file);

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);

    return 1;
}

void run_end_to_end_tests() {
    printf("\n=== End-to-End Test Scenarios ===\n");

    RUN_TEST(test_e2e_realistic_scenario);
    RUN_TEST(test_e2e_no_vulnerabilities_scenario);
    RUN_TEST(test_e2e_mixed_results_scenario);
    RUN_TEST(test_e2e_complex_csv_columns);
    RUN_TEST(test_e2e_error_handling);
    RUN_TEST(test_e2e_performance_stress);
}