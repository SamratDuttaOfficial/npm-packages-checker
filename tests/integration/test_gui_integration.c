#include "../test_framework.h"
#include "../../src/common.h"

// Mock GUI functions for testing
int mock_gui_initialized = 0;
int mock_gui_scan_called = 0;

// Mock GUI initialization
int mock_init_gui() {
    mock_gui_initialized = 1;
    return 1;
}

// Mock GUI scan function
int mock_gui_scan(const char *csv_path, const char *project_path,
                  const char *output_path, int name_col, int version_col) {
    mock_gui_scan_called = 1;

    // Validate parameters
    if (!csv_path || !project_path || !output_path) {
        return 0;
    }

    if (name_col < 1 || version_col < 1) {
        return 0;
    }

    return 1;
}

int test_gui_initialization() {
    mock_gui_initialized = 0;

    int result = mock_init_gui();

    TEST_ASSERT_EQ(1, result, "GUI should initialize successfully");
    TEST_ASSERT_EQ(1, mock_gui_initialized, "GUI initialized flag should be set");

    return 1;
}

int test_gui_scan_parameters() {
    mock_gui_scan_called = 0;

    int result = mock_gui_scan("test.csv", "./project", "report.txt", 1, 2);

    TEST_ASSERT_EQ(1, result, "GUI scan should succeed with valid parameters");
    TEST_ASSERT_EQ(1, mock_gui_scan_called, "GUI scan should be called");

    return 1;
}

int test_gui_scan_invalid_parameters() {
    // Test null parameters
    int result1 = mock_gui_scan(NULL, "./project", "report.txt", 1, 2);
    TEST_ASSERT_EQ(0, result1, "Should fail with null CSV path");

    int result2 = mock_gui_scan("test.csv", NULL, "report.txt", 1, 2);
    TEST_ASSERT_EQ(0, result2, "Should fail with null project path");

    int result3 = mock_gui_scan("test.csv", "./project", NULL, 1, 2);
    TEST_ASSERT_EQ(0, result3, "Should fail with null output path");

    // Test invalid column numbers
    int result4 = mock_gui_scan("test.csv", "./project", "report.txt", 0, 2);
    TEST_ASSERT_EQ(0, result4, "Should fail with invalid name column");

    int result5 = mock_gui_scan("test.csv", "./project", "report.txt", 1, 0);
    TEST_ASSERT_EQ(0, result5, "Should fail with invalid version column");

    return 1;
}

int test_gui_data_structures() {
    // Test that GUI-related data structures are properly initialized
    PackageList *packages = create_package_list();
    TEST_ASSERT(packages != NULL, "Should create package list for GUI");

    DependencyTree *deps = create_dependency_tree();
    TEST_ASSERT(deps != NULL, "Should create dependency tree for GUI");

    // Test data structure operations that GUI would use
    Package test_pkg = {0};
    strcpy(test_pkg.name, "test-package");
    strcpy(test_pkg.versions[0], "1.0.0");
    test_pkg.version_count = 1;

    int add_result = add_package(packages, &test_pkg);
    TEST_ASSERT_EQ(1, add_result, "Should add package successfully");
    TEST_ASSERT_EQ(1, packages->count, "Package count should be 1");

    free_package_list(packages);
    free_dependency_tree(deps);

    return 1;
}

int test_gui_file_operations() {
    // Test file operations that GUI would perform
    const char *test_content = "Package,Version\ntest,1.0.0\n";
    const char *test_file = "gui_test_file.csv";

    create_test_file(test_file, test_content);
    TEST_ASSERT(file_exists(test_file), "Test file should exist");

    // Clean up
    remove(test_file);

    return 1;
}

void run_gui_integration_tests(void) {
    printf("\n=== GUI Integration Tests (Mock) ===\n");
    RUN_TEST(test_gui_initialization);
    RUN_TEST(test_gui_scan_parameters);
    RUN_TEST(test_gui_scan_invalid_parameters);
    RUN_TEST(test_gui_data_structures);
    RUN_TEST(test_gui_file_operations);
}