#include "report_generator.h"
#include <time.h>
#include <stdarg.h>

static void format_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

static void append_to_report(char *report, size_t report_size, const char *format, ...) {
    size_t current_len = strlen(report);
    size_t remaining = report_size - current_len - 1;

    if (remaining <= 0) {
        return;
    }

    va_list args;
    va_start(args, format);
    vsnprintf(report + current_len, remaining, format, args);
    va_end(args);
}

static void generate_report_header(char *report, size_t report_size) {
    char timestamp[64];
    format_timestamp(timestamp, sizeof(timestamp));

    append_to_report(report, report_size,
        "NPM VULNERABILITY REPORT\n"
        "========================\n"
        "Generated: %s\n\n",
        timestamp);
}

static void generate_status_section(char *report, size_t report_size, int vulnerability_count, int warning_count) {
    if (vulnerability_count > 0) {
        append_to_report(report, report_size,
            "STATUS: VULNERABLE\n"
            "Found %d vulnerable package(s) in your project\n\n",
            vulnerability_count);
    } else if (warning_count > 0) {
        append_to_report(report, report_size,
            "STATUS: WARNING\n"
            "Found %d potential package match(es) requiring attention\n\n",
            warning_count);
    } else {
        append_to_report(report, report_size,
            "STATUS: OK\n"
            "No vulnerable packages found in your project.\n\n");
    }
}

static void generate_vulnerability_details(char *report, size_t report_size,
                                         const VulnerabilityResults *results,
                                         const PackageList *vulnerable_packages,
                                         const DependencyTree *dependencies) {
    if (results->count == 0) {
        return;
    }

    append_to_report(report, report_size,
        "VULNERABILITY DETAILS:\n"
        "=====================\n");

    for (int i = 0; i < results->count; i++) {
        const VulnerabilityMatch *match = &results->matches[i];

        append_to_report(report, report_size,
            "\n[%d] VULNERABLE PACKAGE FOUND:\n"
            "    Package Name: %s %s (Reported Vulnerable Package: %s %s)\n"
            "    Dependency Type: %s\n"
            "    Dependency Levels: %s\n",
            i + 1,
            match->package_name,
            match->used_version,
            match->vulnerable_package_name,
            match->vulnerable_version,
            match->is_direct_dependency ? "Direct" : "Indirect",
            match->dependency_path);

        append_to_report(report, report_size, "    All Vulnerable Versions in Database: ");
        int found_versions = 0;
        if (vulnerable_packages->count > 0 && vulnerable_packages->packages != NULL) {
            for (int j = 0; j < vulnerable_packages->count; j++) {
                if (strcmp(vulnerable_packages->packages[j].name, match->package_name) == 0) {
                    for (int k = 0; k < vulnerable_packages->packages[j].version_count && k < MAX_VERSIONS; k++) {
                        if (found_versions > 0) {
                            append_to_report(report, report_size, ", ");
                        }
                        append_to_report(report, report_size, "%s",
                                       vulnerable_packages->packages[j].versions[k]);
                        found_versions++;
                    }
                    break;
                }
            }
        }
        if (found_versions == 0) {
            append_to_report(report, report_size, "Unknown");
        }

        append_to_report(report, report_size, "\n    All Instances in the Project: ");
        int found_instances = 0;
        if (dependencies->count > 0 && dependencies->nodes != NULL) {
            for (int j = 0; j < dependencies->count; j++) {
                if (strcmp(dependencies->nodes[j].name, match->package_name) == 0) {
                    if (found_instances > 0) {
                        append_to_report(report, report_size, ", ");
                    }
                    append_to_report(report, report_size, "%s", dependencies->nodes[j].version);
                    found_instances++;
                }
            }
        }
        if (found_instances == 0) {
            append_to_report(report, report_size, "Not found");
        }
        append_to_report(report, report_size, "\n");
    }

    append_to_report(report, report_size, "\n");
}

static void generate_summary_section(char *report, size_t report_size,
                                   const VulnerabilityResults *results,
                                   const PackageList *vulnerable_packages,
                                   const DependencyTree *dependencies) {
    int direct_count = 0;
    int indirect_count = 0;

    for (int i = 0; i < results->count; i++) {
        if (results->matches[i].is_direct_dependency) {
            direct_count++;
        } else {
            indirect_count++;
        }
    }

    append_to_report(report, report_size,
        "SCAN SUMMARY:\n"
        "=============\n"
        "Total dependencies scanned: %d\n"
        "Vulnerable packages in database: %d\n"
        "Vulnerabilities found: %d\n"
        "Direct dependency vulnerabilities: %d\n"
        "Indirect dependency vulnerabilities: %d\n\n",
        dependencies->count,
        vulnerable_packages->count,
        results->count,
        direct_count,
        indirect_count);

    // Add warnings section
    if (results->warning_count > 0) {
        append_to_report(report, report_size,
            "POTENTIAL PACKAGE MATCHES:\n"
            "=========================\n");

        for (int i = 0; i < results->warning_count; i++) {
            const VulnerabilityMatch *warning = &results->warnings[i];

            char vuln_versions[512] = "";
            int found_vuln_pkg = 0;
            if (vulnerable_packages->count > 0 && vulnerable_packages->packages != NULL) {
                for (int j = 0; j < vulnerable_packages->count; j++) {
                    if (strcmp(vulnerable_packages->packages[j].name, warning->package_name) == 0) {
                        found_vuln_pkg = 1;
                        for (int k = 0; k < vulnerable_packages->packages[j].version_count && k < MAX_VERSIONS; k++) {
                            if (k > 0) strcat(vuln_versions, ", ");
                            strcat(vuln_versions, vulnerable_packages->packages[j].versions[k]);
                        }
                        break;
                    }
                }
            }

            append_to_report(report, report_size,
                "[%d] %s@%s (vulnerable: %s %s)\n"
                "    Dependency Levels: %s\n",
                i + 1,
                warning->package_name,
                warning->used_version,
                warning->vulnerable_package_name,
                found_vuln_pkg ? vuln_versions : "unknown",
                warning->dependency_path);
        }
        append_to_report(report, report_size, "\n");
    }

    // Show package matching details
    append_to_report(report, report_size,
        "PACKAGE MATCHING ANALYSIS:\n"
        "=========================\n");

    int total_matches = 0;

    // Only process if we have dependencies
    if (dependencies->count > 0 && dependencies->nodes != NULL) {
        for (int i = 0; i < dependencies->count; i++) {
            const char *dep_name = dependencies->nodes[i].name;

            if (vulnerable_packages->count > 0 && vulnerable_packages->packages != NULL) {
                for (int j = 0; j < vulnerable_packages->count; j++) {
                    if (strcmp(vulnerable_packages->packages[j].name, dep_name) == 0) {
                        total_matches++;

                        int is_vulnerable = 0;
                        if (results->count > 0) {
                            for (int k = 0; k < results->count; k++) {
                                if (strcmp(results->matches[k].package_name, dep_name) == 0) {
                                    is_vulnerable = 1;
                                    break;
                                }
                            }
                        }

                        if (is_vulnerable) {
                            append_to_report(report, report_size,
                                "  %s@%s -> VULNERABLE\n"
                                "    Dependency Levels: %s\n",
                                dep_name, dependencies->nodes[i].version, dependencies->nodes[i].path);
                        } else {
                            // Show unsafe versions for safe packages
                            append_to_report(report, report_size,
                                "  %s@%s -> Safe version (unsafe: ",
                                dep_name, dependencies->nodes[i].version);

                            int first_unsafe = 1;
                            for (int k = 0; k < vulnerable_packages->packages[j].version_count && k < MAX_VERSIONS; k++) {
                                if (!first_unsafe) append_to_report(report, report_size, ", ");
                                append_to_report(report, report_size, "%s",
                                               vulnerable_packages->packages[j].versions[k]);
                                first_unsafe = 0;
                            }
                            append_to_report(report, report_size, ")\n"
                                "    Dependency Levels: %s\n", dependencies->nodes[i].path);
                        }
                        break;
                    }
                }
            }
        }
    }

    append_to_report(report, report_size,
        "\nPackages found in vulnerability database: %d\n"
        "Packages not in database (unknown): %d\n"
        "Warnings (partial matches): %d\n",
        total_matches,
        dependencies->count - total_matches,
        results->warning_count);
}

int generate_vulnerability_report(const VulnerabilityResults *results,
                                 const PackageList *vulnerable_packages,
                                 const DependencyTree *dependencies,
                                 char *report,
                                 size_t report_size) {
    if (results == NULL || vulnerable_packages == NULL ||
        dependencies == NULL || report == NULL || report_size == 0) {
        return 0;
    }

    report[0] = '\0';

    generate_report_header(report, report_size);
    generate_status_section(report, report_size, results->count, results->warning_count);
    generate_vulnerability_details(report, report_size, results, vulnerable_packages, dependencies);
    generate_summary_section(report, report_size, results, vulnerable_packages, dependencies);

    return 1;
}

int save_report_to_file(const char *report_path, const char *report_content) {
    if (report_path == NULL || report_content == NULL) {
        printf("Error: Invalid parameters for saving report\n");
        return 0;
    }

    FILE *file = fopen(report_path, "w");
    if (file == NULL) {
        printf("Error: Cannot create report file: %s\n", report_path);
        return 0;
    }

    if (fprintf(file, "%s", report_content) < 0) {
        printf("Error: Failed to write report content\n");
        fclose(file);
        return 0;
    }

    fclose(file);
    printf("Report successfully saved to: %s\n", report_path);
    return 1;
}