#include "../test_framework.h"
#include "../../src/report_generator.h"
#include "../../src/vulnerability_checker.h"
#include "../../src/common.h"

int test_generate_report_no_vulnerabilities() {
    VulnerabilityResults results = {0};
    PackageList vuln_packages = {0};
    DependencyTree dependencies = {0};

    vuln_packages.count = 5;
    dependencies.count = 10;

    char report[10000] = {0};
    int success = generate_vulnerability_report(&results, &vuln_packages, &dependencies,
                                              report, sizeof(report));

    TEST_ASSERT_EQ(1, success, "Should successfully generate report");
    TEST_ASSERT(strstr(report, "NPM VULNERABILITY REPORT") != NULL, "Should contain report header");
    TEST_ASSERT(strstr(report, "STATUS: OK") != NULL, "Should show OK status");
    TEST_ASSERT(strstr(report, "No vulnerable packages found") != NULL, "Should indicate no vulnerabilities");
    TEST_ASSERT(strstr(report, "Total dependencies scanned: 10") != NULL, "Should show dependency count");
    TEST_ASSERT(strstr(report, "Vulnerable packages in database: 5") != NULL, "Should show vuln package count");

    return 1;
}

int test_generate_report_with_vulnerabilities() {
    VulnerabilityResults results = {0};

    strcpy(results.matches[0].package_name, "lodash");
    strcpy(results.matches[0].vulnerable_version, "4.17.19");
    strcpy(results.matches[0].used_version, "4.17.19");
    strcpy(results.matches[0].dependency_path, "lodash@4.17.19");
    results.matches[0].is_direct_dependency = 1;

    strcpy(results.matches[1].package_name, "express");
    strcpy(results.matches[1].vulnerable_version, "4.16.4");
    strcpy(results.matches[1].used_version, "4.16.4");
    strcpy(results.matches[1].dependency_path, "express@4.16.4");
    results.matches[1].is_direct_dependency = 0;

    results.count = 2;

    PackageList vuln_packages = {0};
    DependencyTree dependencies = {0};
    vuln_packages.count = 5;
    dependencies.count = 10;

    char report[10000] = {0};
    int success = generate_vulnerability_report(&results, &vuln_packages, &dependencies,
                                              report, sizeof(report));

    TEST_ASSERT_EQ(1, success, "Should successfully generate report");
    TEST_ASSERT(strstr(report, "STATUS: VULNERABLE") != NULL, "Should show VULNERABLE status");
    TEST_ASSERT(strstr(report, "Found 2 vulnerable package(s)") != NULL, "Should show vulnerability count");
    TEST_ASSERT(strstr(report, "VULNERABILITY DETAILS:") != NULL, "Should contain details section");
    TEST_ASSERT(strstr(report, "lodash") != NULL, "Should mention lodash vulnerability");
    TEST_ASSERT(strstr(report, "express") != NULL, "Should mention express vulnerability");
    TEST_ASSERT(strstr(report, "Direct") != NULL, "Should show direct dependency");
    TEST_ASSERT(strstr(report, "Indirect") != NULL, "Should show indirect dependency");

    return 1;
}

int test_generate_report_multiple_vulnerabilities() {
    VulnerabilityResults results = {0};

    for (int i = 0; i < 5; i++) {
        snprintf(results.matches[i].package_name, sizeof(results.matches[i].package_name),
                "package%d", i);
        snprintf(results.matches[i].vulnerable_version, sizeof(results.matches[i].vulnerable_version),
                "1.0.%d", i);
        snprintf(results.matches[i].used_version, sizeof(results.matches[i].used_version),
                "1.0.%d", i);
        snprintf(results.matches[i].dependency_path, sizeof(results.matches[i].dependency_path),
                "package%d@1.0.%d", i, i);
        results.matches[i].is_direct_dependency = (i % 2 == 0) ? 1 : 0;
    }
    results.count = 5;

    PackageList vuln_packages = {0};
    DependencyTree dependencies = {0};
    vuln_packages.count = 10;
    dependencies.count = 20;

    char report[10000] = {0};
    int success = generate_vulnerability_report(&results, &vuln_packages, &dependencies,
                                              report, sizeof(report));

    TEST_ASSERT_EQ(1, success, "Should successfully generate report");
    TEST_ASSERT(strstr(report, "Found 5 vulnerable package(s)") != NULL, "Should show correct count");
    TEST_ASSERT(strstr(report, "[1] VULNERABLE PACKAGE FOUND:") != NULL, "Should number vulnerabilities");
    TEST_ASSERT(strstr(report, "[5] VULNERABLE PACKAGE FOUND:") != NULL, "Should show all vulnerabilities");
    TEST_ASSERT(strstr(report, "Direct dependency vulnerabilities: 3") != NULL, "Should count direct deps");
    TEST_ASSERT(strstr(report, "Indirect dependency vulnerabilities: 2") != NULL, "Should count indirect deps");

    return 1;
}

int test_generate_report_timestamp() {
    VulnerabilityResults results = {0};
    PackageList vuln_packages = {0};
    DependencyTree dependencies = {0};

    char report[10000] = {0};
    int success = generate_vulnerability_report(&results, &vuln_packages, &dependencies,
                                              report, sizeof(report));

    TEST_ASSERT_EQ(1, success, "Should successfully generate report");
    TEST_ASSERT(strstr(report, "Generated:") != NULL, "Should contain timestamp");

    return 1;
}

int test_generate_report_null_inputs() {
    VulnerabilityResults results = {0};
    PackageList vuln_packages = {0};
    DependencyTree dependencies = {0};
    char report[1000] = {0};

    int result1 = generate_vulnerability_report(NULL, &vuln_packages, &dependencies,
                                               report, sizeof(report));
    TEST_ASSERT_EQ(0, result1, "Should fail with NULL results");

    int result2 = generate_vulnerability_report(&results, NULL, &dependencies,
                                               report, sizeof(report));
    TEST_ASSERT_EQ(0, result2, "Should fail with NULL vulnerable packages");

    int result3 = generate_vulnerability_report(&results, &vuln_packages, NULL,
                                               report, sizeof(report));
    TEST_ASSERT_EQ(0, result3, "Should fail with NULL dependencies");

    int result4 = generate_vulnerability_report(&results, &vuln_packages, &dependencies,
                                               NULL, sizeof(report));
    TEST_ASSERT_EQ(0, result4, "Should fail with NULL report buffer");

    return 1;
}

int test_save_report_to_file() {
    const char *report_content = "This is a test report\nWith multiple lines\n";
    const char *filename = "test_report.txt";

    int success = save_report_to_file(filename, report_content);
    TEST_ASSERT_EQ(1, success, "Should successfully save report");

    TEST_ASSERT(file_exists(filename), "Report file should exist");

    FILE *file = fopen(filename, "r");
    TEST_ASSERT(file != NULL, "Should be able to open saved file");

    char buffer[1000] = {0};
    fread(buffer, 1, sizeof(buffer) - 1, file);
    fclose(file);

    TEST_ASSERT_STR_EQ(report_content, buffer, "File content should match report");

    cleanup_test_file(filename);
    return 1;
}

int test_save_report_invalid_inputs() {
    const char *report_content = "Test report";

    int result1 = save_report_to_file(NULL, report_content);
    TEST_ASSERT_EQ(0, result1, "Should fail with NULL filename");

    int result2 = save_report_to_file("test.txt", NULL);
    TEST_ASSERT_EQ(0, result2, "Should fail with NULL content");

    return 1;
}

int test_save_report_invalid_path() {
    const char *report_content = "Test report";
    const char *invalid_path = "/invalid/path/that/does/not/exist/report.txt";

    int result = save_report_to_file(invalid_path, report_content);
    TEST_ASSERT_EQ(0, result, "Should fail with invalid path");

    return 1;
}

int test_report_content_format() {
    VulnerabilityResults results = {0};
    strcpy(results.matches[0].package_name, "test-package");
    strcpy(results.matches[0].vulnerable_package_name, "test-package");
    strcpy(results.matches[0].vulnerable_version, "1.0.0");
    strcpy(results.matches[0].used_version, "1.0.0");
    strcpy(results.matches[0].dependency_path, "test-package@1.0.0");
    results.matches[0].is_direct_dependency = 1;
    results.count = 1;

    PackageList vuln_packages = {0};
    DependencyTree dependencies = {0};
    vuln_packages.count = 1;
    dependencies.count = 1;

    char report[5000] = {0};
    int success = generate_vulnerability_report(&results, &vuln_packages, &dependencies,
                                              report, sizeof(report));

    TEST_ASSERT_EQ(1, success, "Should successfully generate report");

    TEST_ASSERT(strstr(report, "Package Name: test-package 1.0.0") != NULL, "Should format package name with version");
    TEST_ASSERT(strstr(report, "Reported Vulnerable Package: test-package 1.0.0") != NULL, "Should format vulnerable package with version");
    TEST_ASSERT(strstr(report, "Dependency Type: Direct") != NULL, "Should format dependency type");
    TEST_ASSERT(strstr(report, "Dependency Levels: test-package@1.0.0") != NULL, "Should format dependency levels");
    TEST_ASSERT(strstr(report, "VULNERABLE PACKAGE FOUND") != NULL, "Should include vulnerability header");

    return 1;
}

void run_report_generator_tests() {
    printf("\n=== Report Generator Unit Tests ===\n");

    RUN_TEST(test_generate_report_no_vulnerabilities);
    RUN_TEST(test_generate_report_with_vulnerabilities);
    RUN_TEST(test_generate_report_multiple_vulnerabilities);
    RUN_TEST(test_generate_report_timestamp);
    RUN_TEST(test_generate_report_null_inputs);

    RUN_TEST(test_save_report_to_file);
    RUN_TEST(test_save_report_invalid_inputs);
    RUN_TEST(test_save_report_invalid_path);

    RUN_TEST(test_report_content_format);
}