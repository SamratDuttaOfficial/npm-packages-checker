#include "../test_framework.h"
#include "../../src/csv_parser.h"
#include "../../src/common.h"

int test_parse_versions_single() {
    char versions[MAX_VERSIONS][MAX_VERSION_LEN];
    int count = parse_versions("4.17.20", versions, MAX_VERSIONS);

    TEST_ASSERT_EQ(1, count, "Should parse single version");
    TEST_ASSERT_STR_EQ("4.17.20", versions[0], "Should match expected version");

    return 1;
}

int test_parse_versions_multiple() {
    char versions[MAX_VERSIONS][MAX_VERSION_LEN];
    int count = parse_versions("4.17.19 4.17.20 4.18.0", versions, MAX_VERSIONS);

    TEST_ASSERT_EQ(3, count, "Should parse three versions");
    TEST_ASSERT_STR_EQ("4.17.19", versions[0], "First version should match");
    TEST_ASSERT_STR_EQ("4.17.20", versions[1], "Second version should match");
    TEST_ASSERT_STR_EQ("4.18.0", versions[2], "Third version should match");

    return 1;
}

int test_parse_versions_with_spaces() {
    char versions[MAX_VERSIONS][MAX_VERSION_LEN];
    int count = parse_versions("  4.17.19   4.17.20  ", versions, MAX_VERSIONS);

    TEST_ASSERT_EQ(2, count, "Should parse versions with extra spaces");
    TEST_ASSERT_STR_EQ("4.17.19", versions[0], "First version should be trimmed");
    TEST_ASSERT_STR_EQ("4.17.20", versions[1], "Second version should be trimmed");

    return 1;
}

int test_parse_versions_empty() {
    char versions[MAX_VERSIONS][MAX_VERSION_LEN];
    int count = parse_versions("", versions, MAX_VERSIONS);

    TEST_ASSERT_EQ(0, count, "Should return 0 for empty string");

    return 1;
}

int test_parse_versions_null() {
    char versions[MAX_VERSIONS][MAX_VERSION_LEN];
    int count = parse_versions(NULL, versions, MAX_VERSIONS);

    TEST_ASSERT_EQ(0, count, "Should return 0 for NULL input");

    return 1;
}

int test_load_csv_basic() {
    const char *csv_content =
        "Package,Versions,Severity\n"
        "lodash,4.17.19 4.17.20,High\n"
        "express,4.16.4,Medium\n";

    const char *temp_file = "build/test_basic.csv";
    create_test_file(temp_file, csv_content);

    PackageList *packages = create_package_list();
    TEST_ASSERT(packages != NULL, "Should create package list");

    int result = load_csv_packages(temp_file, 0, 1, packages);

    TEST_ASSERT_EQ(1, result, "Should successfully load CSV");
    TEST_ASSERT_EQ(2, packages->count, "Should parse 2 packages");

    TEST_ASSERT_STR_EQ("lodash", packages->packages[0].name, "First package name should match");
    TEST_ASSERT_EQ(2, packages->packages[0].version_count, "First package should have 2 versions");
    TEST_ASSERT_STR_EQ("4.17.19", packages->packages[0].versions[0], "First version should match");
    TEST_ASSERT_STR_EQ("4.17.20", packages->packages[0].versions[1], "Second version should match");

    TEST_ASSERT_STR_EQ("express", packages->packages[1].name, "Second package name should match");
    TEST_ASSERT_EQ(1, packages->packages[1].version_count, "Second package should have 1 version");
    TEST_ASSERT_STR_EQ("4.16.4", packages->packages[1].versions[0], "Express version should match");

    free_package_list(packages);
    cleanup_test_file(temp_file);
    return 1;
}

int test_load_csv_with_spaces() {
    const char *csv_content =
        "Package Name,Vulnerable Versions,Risk\n"
        "  lodash  , 4.17.19   4.17.20  ,High\n"
        " express ,  4.16.4  ,Medium\n";

    create_test_file("build/test_spaces.csv", csv_content);

    PackageList *packages = create_package_list();
    TEST_ASSERT(packages != NULL, "Should create package list");

    int result = load_csv_packages("build/test_spaces.csv", 0, 1, packages);

    TEST_ASSERT_EQ(1, result, "Should successfully load CSV with spaces");
    TEST_ASSERT_EQ(2, packages->count, "Should parse 2 packages");

    TEST_ASSERT_STR_EQ("lodash", packages->packages[0].name, "Package name should be trimmed");
    TEST_ASSERT_STR_EQ("express", packages->packages[1].name, "Package name should be trimmed");

    free_package_list(packages);
    cleanup_test_file("build/test_spaces.csv");
    return 1;
}

int test_load_csv_different_columns() {
    const char *csv_content =
        "ID,Severity,Package,Versions,Description\n"
        "1,High,lodash,4.17.19 4.17.20,Vulnerability description\n"
        "2,Medium,express,4.16.4,Another vulnerability\n";

    create_test_file("build/test_columns.csv", csv_content);

    PackageList *packages = create_package_list();
    TEST_ASSERT(packages != NULL, "Should create package list");

    int result = load_csv_packages("build/test_columns.csv", 2, 3, packages);

    TEST_ASSERT_EQ(1, result, "Should successfully load CSV with different column order");
    TEST_ASSERT_EQ(2, packages->count, "Should parse 2 packages");

    TEST_ASSERT_STR_EQ("lodash", packages->packages[0].name, "Package name should match from column 2");
    TEST_ASSERT_STR_EQ("express", packages->packages[1].name, "Package name should match from column 2");

    free_package_list(packages);

    cleanup_test_file("build/test_columns.csv");
    return 1;
}

int test_load_csv_empty_file() {
    const char *csv_content = "Package,Versions,Severity\n";

    create_test_file("build/test_empty.csv", csv_content);

    PackageList *packages = create_package_list();
    TEST_ASSERT(packages != NULL, "Should create package list");

    int result = load_csv_packages("build/test_empty.csv", 0, 1, packages);

    TEST_ASSERT_EQ(1, result, "Should successfully load empty CSV");
    TEST_ASSERT_EQ(0, packages->count, "Should parse 0 packages");

    free_package_list(packages);

    cleanup_test_file("build/test_empty.csv");
    return 1;
}

int test_load_csv_nonexistent_file() {
    PackageList *packages = create_package_list();
    TEST_ASSERT(packages != NULL, "Should create package list");

    int result = load_csv_packages("nonexistent.csv", 0, 1, packages);

    TEST_ASSERT_EQ(0, result, "Should fail to load nonexistent file");
    TEST_ASSERT_EQ(0, packages->count, "Should have 0 packages");

    free_package_list(packages);

    return 1;
}

int test_load_csv_invalid_columns() {
    const char *csv_content =
        "Package,Versions\n"
        "lodash,4.17.19\n";

    create_test_file("build/test_invalid.csv", csv_content);

    PackageList *packages = create_package_list();
    TEST_ASSERT(packages != NULL, "Should create package list");

    int result = load_csv_packages("build/test_invalid.csv", 0, 5, packages);

    TEST_ASSERT_EQ(1, result, "Should load file even with invalid column index");
    TEST_ASSERT_EQ(0, packages->count, "Should parse 0 packages due to invalid column");

    free_package_list(packages);

    cleanup_test_file("build/test_invalid.csv");
    return 1;
}

int test_clean_package_name() {
    char name1[] = "  lodash  ";
    clean_package_name(name1);
    TEST_ASSERT_STR_EQ("lodash", name1, "Should remove leading/trailing spaces");

    char name2[] = "package with spaces";
    clean_package_name(name2);
    TEST_ASSERT_STR_EQ("packagewithspaces", name2, "Should remove all spaces");

    char name3[] = "  package  with  multiple  spaces  ";
    clean_package_name(name3);
    TEST_ASSERT_STR_EQ("packagewithmultiplespaces", name3, "Should remove all spaces");

    return 1;
}

int test_trim_whitespace() {
    char str1[] = "  hello  ";
    trim_whitespace(str1);
    TEST_ASSERT_STR_EQ("hello", str1, "Should trim leading and trailing spaces");

    char str2[] = "\t\nhello world\n\t";
    trim_whitespace(str2);
    TEST_ASSERT_STR_EQ("hello world", str2, "Should trim tabs and newlines");

    char str3[] = "nospaces";
    trim_whitespace(str3);
    TEST_ASSERT_STR_EQ("nospaces", str3, "Should not change string without spaces");

    char str4[] = "   ";
    trim_whitespace(str4);
    TEST_ASSERT_STR_EQ("", str4, "Should result in empty string for whitespace-only");

    return 1;
}

void run_csv_parser_tests() {
    printf("\n=== CSV Parser Unit Tests ===\n");

    RUN_TEST(test_parse_versions_single);
    RUN_TEST(test_parse_versions_multiple);
    RUN_TEST(test_parse_versions_with_spaces);
    RUN_TEST(test_parse_versions_empty);
    RUN_TEST(test_parse_versions_null);

    RUN_TEST(test_load_csv_basic);
    RUN_TEST(test_load_csv_with_spaces);
    RUN_TEST(test_load_csv_different_columns);
    RUN_TEST(test_load_csv_empty_file);
    RUN_TEST(test_load_csv_nonexistent_file);
    RUN_TEST(test_load_csv_invalid_columns);

    RUN_TEST(test_clean_package_name);
    RUN_TEST(test_trim_whitespace);
}