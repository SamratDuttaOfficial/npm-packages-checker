#include "npm_analyzer.h"
#include <time.h>

static int execute_npm_command(const char *project_path, const char *output_file) {
    if (project_path == NULL || output_file == NULL) {
        return 0;
    }

    printf("Checking if npm is available...\n");
    int npm_check = system("npm --version > /dev/null 2>&1");
    if (npm_check != 0) {
        printf("Error: npm command not found in PATH\n");
        printf("Please ensure Node.js and npm are properly installed\n");
        return 0;
    }

    char package_json_path[MAX_PATH_LEN];
    snprintf(package_json_path, sizeof(package_json_path), "%s/package.json", project_path);
    FILE *package_file = fopen(package_json_path, "r");
    if (package_file == NULL) {
        printf("Error: No package.json found in %s\n", project_path);
        printf("Please select a valid npm project directory\n");
        return 0;
    }
    fclose(package_file);

    char command[MAX_PATH_LEN + 100];

#ifdef _WIN32
    snprintf(command, sizeof(command),
             "cd /d \"%s\" && npm list --json --all > \"%s\" 2>nul",
             project_path, output_file);
#else
    snprintf(command, sizeof(command),
             "cd \"%s\" && npm list --json --all > \"%s\" 2>/dev/null",
             project_path, output_file);
#endif

    printf("Executing: %s\n", command);
    int result = system(command);

    FILE *check_file = fopen(output_file, "r");
    if (check_file == NULL) {
        printf("Error: npm command did not create output file\n");
        return 0;
    }

    fseek(check_file, 0, SEEK_END);
    long file_size = ftell(check_file);
    fclose(check_file);

    printf("npm output file size: %ld bytes\n", file_size);

    if (file_size < 10) {
        printf("Warning: npm output file is very small, may be empty or contain errors\n");
    }

    if (result != 0) {
        printf("Warning: npm list command failed (exit code: %d), trying alternative...\n", result);

#ifdef _WIN32
        snprintf(command, sizeof(command),
                 "cd /d \"%s\" && npm ls --json > \"%s\" 2>nul",
                 project_path, output_file);
#else
        snprintf(command, sizeof(command),
                 "cd \"%s\" && npm ls --json > \"%s\" 2>/dev/null",
                 project_path, output_file);
#endif

        printf("Executing alternative: %s\n", command);
        result = system(command);

        if (result != 0) {
            printf("Error: Both npm commands failed (exit code: %d)\n", result);
            printf("This could be due to:\n");
            printf("- Missing node_modules (try 'npm install' first)\n");
            printf("- Corrupted package-lock.json\n");
            printf("- Network issues\n");
            printf("- Permission problems\n");
        }
    }

    return (result == 0) ? 1 : 0;
}

static int extract_json_string_value(const char *line, const char *key, char *value, size_t value_size) {
    if (line == NULL || key == NULL || value == NULL || value_size == 0) {
        return 0;
    }

    char search_pattern[64];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\":", key);

    char *key_start = strstr(line, search_pattern);
    if (key_start == NULL) {
        return 0;
    }

    char *value_start = strchr(key_start, ':');
    if (value_start == NULL) {
        return 0;
    }

    value_start = strchr(value_start, '"');
    if (value_start == NULL) {
        return 0;
    }
    value_start++;

    char *value_end = strchr(value_start, '"');
    if (value_end == NULL) {
        return 0;
    }

    size_t len = value_end - value_start;
    if (len >= value_size) {
        len = value_size - 1;
    }

    strncpy(value, value_start, len);
    value[len] = '\0';

    return 1;
}

static int extract_package_name_from_line(const char *line, char *package_name, size_t max_len) {
    char *name_start = strchr(line, '"');
    if (name_start == NULL) return 0;
    name_start++;

    char *name_end = strchr(name_start, '"');
    if (name_end == NULL) return 0;

    size_t name_len = name_end - name_start;
    if (name_len >= max_len) name_len = max_len - 1;

    strncpy(package_name, name_start, name_len);
    package_name[name_len] = '\0';

    if (strcmp(package_name, "name") == 0 ||
        strcmp(package_name, "version") == 0 ||
        strcmp(package_name, "lockfileVersion") == 0 ||
        strcmp(package_name, "requires") == 0 ||
        strcmp(package_name, "dependencies") == 0) {
        return 0;
    }

    return 1;
}

// Iterative JSON parser for npm dependency tree
static int parse_all_dependencies(FILE *file, DependencyTree *tree) {
    char line[MAX_LINE_LEN];
    char package_stack[10][MAX_PACKAGE_NAME_LEN];
    char version_stack[10][MAX_VERSION_LEN];
    int depth = 0;
    int max_depth = 9;

    while (fgets(line, sizeof(line), file) != NULL) {
        int braces_opened = 0, braces_closed = 0;
        for (char *p = line; *p; p++) {
            if (*p == '{') braces_opened++;
            else if (*p == '}') braces_closed++;
        }

        if (strchr(line, '"') != NULL && strchr(line, ':') != NULL && strchr(line, '{') != NULL) {
            char package_name[MAX_PACKAGE_NAME_LEN];
            if (extract_package_name_from_line(line, package_name, sizeof(package_name))) {
                if (depth <= max_depth) {
                    safe_string_copy(package_stack[depth], package_name, sizeof(package_stack[depth]));
                    version_stack[depth][0] = '\0';
                }
                depth += braces_opened - braces_closed;
                continue;
            }
        }

        if (strstr(line, "\"version\":") != NULL) {
            char version[MAX_VERSION_LEN];
            if (extract_json_string_value(line, "version", version, sizeof(version))) {
                int pkg_depth = depth;
                while (pkg_depth >= 0 && (pkg_depth > max_depth || strlen(package_stack[pkg_depth]) == 0)) {
                    pkg_depth--;
                }

                if (pkg_depth >= 0 && pkg_depth <= max_depth && strlen(package_stack[pkg_depth]) > 0) {
                    safe_string_copy(version_stack[pkg_depth], version, sizeof(version_stack[pkg_depth]));

                    DependencyNode node = {0};
                    safe_string_copy(node.name, package_stack[pkg_depth], sizeof(node.name));
                    safe_string_copy(node.version, version, sizeof(node.version));

                    node.is_direct = (pkg_depth == 2);

                    if (pkg_depth == 2) {
                        snprintf(node.path, sizeof(node.path), "%s@%s",
                                package_stack[pkg_depth], version);
                    } else {
                        char full_path[MAX_DEPENDENCY_PATH] = "";
                        for (int i = 2; i <= pkg_depth; i++) {
                            if (strlen(version_stack[i]) > 0) {
                                if (strlen(full_path) > 0) {
                                    strncat(full_path, " -> ", sizeof(full_path) - strlen(full_path) - 1);
                                }
                                char segment[256];
                                snprintf(segment, sizeof(segment), "%s@%s", package_stack[i], version_stack[i]);
                                strncat(full_path, segment, sizeof(full_path) - strlen(full_path) - 1);
                            }
                        }
                        strncpy(node.path, full_path, sizeof(node.path) - 1);
                        node.path[sizeof(node.path) - 1] = '\0';
                    }

                    if (!add_dependency(tree, &node)) {
                        printf("Error: Failed to add dependency '%s' - out of memory\n", package_stack[pkg_depth]);
                        return 0;
                    }
                }
            }
        }

        depth += braces_opened - braces_closed;
        if (depth < 0) depth = 0;

        if (braces_closed > 0) {
            for (int i = depth; i <= max_depth; i++) {
                package_stack[i][0] = '\0';
                version_stack[i][0] = '\0';
            }
        }
    }

    return 1;
}

int parse_npm_json_output(const char *json_file, DependencyTree *tree) {
    if (json_file == NULL || tree == NULL) {
        return 0;
    }

    FILE *file = fopen(json_file, "r");
    if (file == NULL) {
        printf("Error: Cannot read npm output file: %s\n", json_file);
        return 0;
    }

    int lines_read = 0;

    rewind(file);
    parse_all_dependencies(file, tree);

    fclose(file);

    printf("=== Parsing Summary ===\n");
    printf("Total lines read: %d\n", lines_read);
    printf("Successfully parsed %d dependencies from npm output\n", tree->count);

    if (tree->count == 0) {
        printf("Warning: No dependencies found. This could mean:\n");
        printf("- The npm project has no dependencies\n");
        printf("- The JSON format is different than expected\n");
        printf("- npm list command returned error data instead of dependency list\n");
    }

    return 1;
}

int get_npm_dependencies(const char *project_path, const char *output_dir, DependencyTree *tree) {
    if (project_path == NULL || output_dir == NULL || tree == NULL) {
        printf("Error: Invalid parameters for npm dependency analysis\n");
        return 0;
    }

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", tm_info);

    char output_file[MAX_PATH_LEN];
    snprintf(output_file, sizeof(output_file), "%s%snpm_deps_%s.json", output_dir, PATH_SEPARATOR, timestamp);

    if (!execute_npm_command(project_path, output_file)) {
        printf("Error: Failed to execute npm command\n");
        return 0;
    }

    int result = parse_npm_json_output(output_file, tree);

    printf("npm output saved to: %s (for debugging)\n", output_file);
    printf("You can examine this file to see the raw npm output\n");


    return result;
}