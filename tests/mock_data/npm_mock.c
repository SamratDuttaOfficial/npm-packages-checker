#include "npm_mock.h"
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
#endif

void create_mock_npm_project(const char *project_path) {
    mkdir(project_path, 0755);
    create_mock_package_json(project_path);
}

void create_mock_package_json(const char *project_path) {
    char package_json_path[MAX_PATH_LEN];
    snprintf(package_json_path, sizeof(package_json_path), "%s%spackage.json", project_path, PATH_SEPARATOR);

    const char *package_json_content =
        "{\n"
        "  \"name\": \"test-npm-project\",\n"
        "  \"version\": \"1.0.0\",\n"
        "  \"description\": \"Test project for vulnerability scanning\",\n"
        "  \"main\": \"index.js\",\n"
        "  \"dependencies\": {\n"
        "    \"lodash\": \"^4.17.20\",\n"
        "    \"express\": \"^4.17.1\",\n"
        "    \"moment\": \"^2.24.0\",\n"
        "    \"axios\": \"^0.21.1\",\n"
        "    \"underscore\": \"^1.9.1\"\n"
        "  },\n"
        "  \"devDependencies\": {\n"
        "    \"jest\": \"^26.6.3\"\n"
        "  }\n"
        "}";

    create_test_file(package_json_path, package_json_content);
}

void create_mock_npm_dependencies_output(const char *output_file) {
    create_test_file(output_file, MOCK_NPM_SIMPLE_OUTPUT);
}

void cleanup_mock_npm_project(const char *project_path) {
    char package_json_path[MAX_PATH_LEN];
    snprintf(package_json_path, sizeof(package_json_path), "%s%spackage.json", project_path, PATH_SEPARATOR);
    cleanup_test_file(package_json_path);

#ifdef _WIN32
    _rmdir(project_path);
#else
    rmdir(project_path);
#endif
}