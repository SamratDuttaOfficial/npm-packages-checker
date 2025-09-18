#include "../test_framework.h"
#include "../../src/npm_analyzer.h"
#include "../mock_data/npm_mock.h"
#include "test_data_paths.h"
#include "integration_test_setup.h"

int test_parse_npm_json_simple() {
    const char *json_file = TEST_OUTPUT_DIR "/test_npm_simple.json";
    create_test_file(json_file, MOCK_NPM_SIMPLE_OUTPUT);

    DependencyTree *tree = create_dependency_tree();
    TEST_ASSERT(tree != NULL, "Should create dependency tree");

    int result = parse_npm_json_output(json_file, tree);

    TEST_ASSERT_EQ(1, result, "Should successfully parse simple npm output");
    TEST_ASSERT_EQ(3, tree->count, "Should parse 3 dependencies");

    int found_lodash = 0, found_express = 0, found_moment = 0;
    for (int i = 0; i < tree->count; i++) {
        if (strcmp(tree->nodes[i].name, "lodash") == 0) {
            found_lodash = 1;
            TEST_ASSERT_STR_EQ("4.17.20", tree->nodes[i].version, "Lodash version should match");
        } else if (strcmp(tree->nodes[i].name, "express") == 0) {
            found_express = 1;
            TEST_ASSERT_STR_EQ("4.17.1", tree->nodes[i].version, "Express version should match");
        } else if (strcmp(tree->nodes[i].name, "moment") == 0) {
            found_moment = 1;
            TEST_ASSERT_STR_EQ("2.24.0", tree->nodes[i].version, "Moment version should match");
        }
    }

    TEST_ASSERT(found_lodash, "Should find lodash dependency");
    TEST_ASSERT(found_express, "Should find express dependency");
    TEST_ASSERT(found_moment, "Should find moment dependency");

    free_dependency_tree(tree);
    cleanup_test_file(json_file);
    return 1;
}

int test_parse_npm_json_complex() {
    const char *json_file = TEST_OUTPUT_DIR "/test_npm_complex.json";
    create_test_file(json_file, MOCK_NPM_COMPLEX_OUTPUT);

    DependencyTree *tree = create_dependency_tree();
    TEST_ASSERT(tree != NULL, "Should create dependency tree");

    int result = parse_npm_json_output(json_file, tree);

    TEST_ASSERT_EQ(1, result, "Should successfully parse complex npm output");
    TEST_ASSERT(tree->count >= 4, "Should parse at least 4 dependencies");

    int found_lodash = 0, found_express = 0, found_debug = 0;
    for (int i = 0; i < tree->count; i++) {
        if (strcmp(tree->nodes[i].name, "lodash") == 0) {
            found_lodash = 1;
            TEST_ASSERT_STR_EQ("4.17.20", tree->nodes[i].version, "Lodash version should match");
        } else if (strcmp(tree->nodes[i].name, "express") == 0) {
            found_express = 1;
            TEST_ASSERT_STR_EQ("4.17.1", tree->nodes[i].version, "Express version should match");
        } else if (strcmp(tree->nodes[i].name, "debug") == 0) {
            found_debug = 1;
            TEST_ASSERT_STR_EQ("2.6.9", tree->nodes[i].version, "Debug version should match");
        }
    }

    TEST_ASSERT(found_lodash, "Should find lodash dependency");
    TEST_ASSERT(found_express, "Should find express dependency");
    TEST_ASSERT(found_debug, "Should find debug dependency");

    free_dependency_tree(tree);
    cleanup_test_file(json_file);
    return 1;
}

int test_parse_npm_json_empty() {
    const char *json_file = TEST_OUTPUT_DIR "/test_npm_empty.json";
    create_test_file(json_file, MOCK_NPM_EMPTY_OUTPUT);

    DependencyTree *tree = create_dependency_tree();
    TEST_ASSERT(tree != NULL, "Should create dependency tree");
    int result = parse_npm_json_output(json_file, tree);

    TEST_ASSERT_EQ(1, result, "Should successfully parse empty npm output");
    TEST_ASSERT_EQ(0, tree->count, "Should parse 0 dependencies");

    free_dependency_tree(tree);
    cleanup_test_file(json_file);
    return 1;
}

int test_parse_npm_json_nonexistent() {
    DependencyTree *tree = create_dependency_tree();
    TEST_ASSERT(tree != NULL, "Should create dependency tree");

    int result = parse_npm_json_output("nonexistent.json", tree);

    TEST_ASSERT_EQ(0, result, "Should fail to parse nonexistent file");
    TEST_ASSERT_EQ(0, tree->count, "Should have 0 dependencies");

    free_dependency_tree(tree);
    return 1;
}

int test_parse_npm_json_malformed() {
    const char *json_file = TEST_OUTPUT_DIR "/test_npm_malformed.json";
    const char *malformed_json = "{ invalid json content without proper structure";

    create_test_file(json_file, malformed_json);

    DependencyTree *tree = create_dependency_tree();
    TEST_ASSERT(tree != NULL, "Should create dependency tree");

    int result = parse_npm_json_output(json_file, tree);

    TEST_ASSERT_EQ(1, result, "Should not fail on malformed JSON (graceful handling)");
    TEST_ASSERT_EQ(0, tree->count, "Should parse 0 dependencies from malformed JSON");

    free_dependency_tree(tree);
    cleanup_test_file(json_file);
    return 1;
}

int test_parse_npm_json_with_vulnerable_versions() {
    const char *json_file = TEST_OUTPUT_DIR "/test_npm_vulnerable.json";
    create_test_file(json_file, MOCK_NPM_VULNERABLE_OUTPUT);

    DependencyTree *tree = create_dependency_tree();
    TEST_ASSERT(tree != NULL, "Should create dependency tree");

    int result = parse_npm_json_output(json_file, tree);

    TEST_ASSERT_EQ(1, result, "Should successfully parse vulnerable versions");
    TEST_ASSERT(tree->count >= 4, "Should parse multiple dependencies");

    int found_vulnerable_lodash = 0, found_vulnerable_express = 0;
    for (int i = 0; i < tree->count; i++) {
        if (strcmp(tree->nodes[i].name, "lodash") == 0) {
            TEST_ASSERT_STR_EQ("4.17.19", tree->nodes[i].version, "Should find vulnerable lodash version");
            found_vulnerable_lodash = 1;
        } else if (strcmp(tree->nodes[i].name, "express") == 0) {
            TEST_ASSERT_STR_EQ("4.16.4", tree->nodes[i].version, "Should find vulnerable express version");
            found_vulnerable_express = 1;
        }
    }

    TEST_ASSERT(found_vulnerable_lodash, "Should find vulnerable lodash");
    TEST_ASSERT(found_vulnerable_express, "Should find vulnerable express");

    free_dependency_tree(tree);
    cleanup_test_file(json_file);
    return 1;
}

int test_get_npm_dependencies_mock_project() {
    const char *project_path = "test_mock_project";
    create_mock_npm_project(project_path);

    const char *mock_npm_output = TEST_OUTPUT_DIR "/test_mock_project_npm_deps.json";
    create_test_file(mock_npm_output, MOCK_NPM_SIMPLE_OUTPUT);

    DependencyTree *tree = create_dependency_tree();
    TEST_ASSERT(tree != NULL, "Should create dependency tree");

    char original_function[256];
    sprintf(original_function, "%s", "get_npm_dependencies");

    int result = parse_npm_json_output(mock_npm_output, tree);

    TEST_ASSERT_EQ(1, result, "Should successfully process mock project");
    TEST_ASSERT(tree->count > 0, "Should find dependencies in mock project");

    free_dependency_tree(tree);
    cleanup_test_file(mock_npm_output);
    cleanup_mock_npm_project(project_path);
    return 1;
}

int test_npm_dependency_path_format() {
    const char *json_file = TEST_OUTPUT_DIR "/test_path_format.json";
    create_test_file(json_file, MOCK_NPM_SIMPLE_OUTPUT);

    DependencyTree *tree = create_dependency_tree();
    TEST_ASSERT(tree != NULL, "Should create dependency tree");

    int result = parse_npm_json_output(json_file, tree);

    TEST_ASSERT_EQ(1, result, "Should successfully parse npm output");

    for (int i = 0; i < tree->count; i++) {
        TEST_ASSERT(strlen(tree->nodes[i].path) > 0, "Dependency path should not be empty");
        TEST_ASSERT(strstr(tree->nodes[i].path, tree->nodes[i].name) != NULL,
                   "Path should contain package name");
        TEST_ASSERT(strstr(tree->nodes[i].path, tree->nodes[i].version) != NULL,
                   "Path should contain version");
        TEST_ASSERT(strstr(tree->nodes[i].path, "@") != NULL,
                   "Path should contain @ separator");
    }

    free_dependency_tree(tree);
    cleanup_test_file(json_file);
    return 1;
}

void run_npm_analyzer_integration_tests() {
    printf("\n=== NPM Analyzer Integration Tests ===\n");

    RUN_TEST(test_parse_npm_json_simple);
    RUN_TEST(test_parse_npm_json_complex);
    RUN_TEST(test_parse_npm_json_empty);
    RUN_TEST(test_parse_npm_json_nonexistent);
    RUN_TEST(test_parse_npm_json_malformed);
    RUN_TEST(test_parse_npm_json_with_vulnerable_versions);
    RUN_TEST(test_get_npm_dependencies_mock_project);
    RUN_TEST(test_npm_dependency_path_format);
}