#include "cli.h"
#include "csv_parser.h"
#include "npm_analyzer.h"
#include "vulnerability_checker.h"
#include "report_generator.h"
#include <string.h>

void print_cli_help(const char *program_name) {
    printf("NPM Vulnerability Checker - CLI Mode\n");
    printf("Author: Samrat Dutta (https://github.com/SamratDuttaOfficial)\n\n");

    printf("DESCRIPTION:\n");
    printf("  Analyzes NPM projects for vulnerable packages based on a CSV database.\n\n");

    printf("USAGE:\n");
    print_cli_usage(program_name);

    printf("\nREQUIRED ARGUMENTS:\n");
    printf("  --csv <file>      Path to CSV file containing vulnerability data\n");
    printf("  --project <dir>   Path to NPM project directory (containing package.json)\n");
    printf("  --output <file>   Path where the vulnerability report will be saved\n");

    printf("\nOPTIONAL ARGUMENTS:\n");
    printf("  --name-col <num>    Column number for package names in CSV (default: 1)\n");
    printf("  --version-col <num> Column number for versions in CSV (default: 2)\n");
    printf("  --help              Show this help message\n");

    printf("\nEXAMPLES:\n");
    printf("  # Basic usage\n");
    printf("  %s --cli --csv vulns.csv --project ./my-app --output report.txt\n\n", program_name);
    printf("  # With custom CSV columns\n");
    printf("  %s --cli --csv vulns.csv --project ./my-app --output report.txt --name-col 2 --version-col 3\n\n", program_name);

    printf("CSV FORMAT:\n");
    printf("  CSV file should have package names and versions. Example:\n");
    printf("  \n");
    printf("  Package,Versions,Severity\n");
    printf("  lodash,4.17.20 4.17.19,High\n");
    printf("  express,4.17.0 4.16.4,Medium\n");
    printf("  \n");
    printf("  Notes:\n");
    printf("  - Multiple versions separated by spaces\n");
    printf("  - Column numbers start from 1\n");
    printf("  - Package names are cleaned automatically\n\n");

    printf("REQUIREMENTS:\n");
    printf("  - npm must be installed and available in PATH\n");
    printf("  - Target project must have a valid package.json file\n");
    printf("  - Write permissions for the output file location\n\n");
}

void print_cli_usage(const char *program_name) {
    printf("  %s --cli --csv <csv_file> --project <project_path> --output <output_file> [options]\n", program_name);
}

int parse_cli_arguments(int argc, char *argv[], CLIArgs *args) {
    memset(args, 0, sizeof(CLIArgs));
    args->name_column = 1;
    args->version_column = 2;

    // Look for CLI flag first
    int has_cli_flag = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--cli") == 0) {
            has_cli_flag = 1;
            break;
        }
    }

    if (!has_cli_flag) {
        return 0;
    }

    // Process command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--cli") == 0) {
            continue;
        } else if (strcmp(argv[i], "--help") == 0) {
            args->show_help = 1;
            return 1;
        } else if (strcmp(argv[i], "--csv") == 0) {
            if (i + 1 >= argc) {
                printf("Error: --csv requires a file path\n");
                return -1;
            }
            safe_string_copy(args->csv_file_path, argv[++i], sizeof(args->csv_file_path));
        } else if (strcmp(argv[i], "--project") == 0) {
            if (i + 1 >= argc) {
                printf("Error: --project requires a directory path\n");
                return -1;
            }
            safe_string_copy(args->project_path, argv[++i], sizeof(args->project_path));
        } else if (strcmp(argv[i], "--output") == 0) {
            if (i + 1 >= argc) {
                printf("Error: --output requires a file path\n");
                return -1;
            }
            safe_string_copy(args->output_file_path, argv[++i], sizeof(args->output_file_path));
        } else if (strcmp(argv[i], "--name-col") == 0) {
            if (i + 1 >= argc) {
                printf("Error: --name-col requires a column number\n");
                return -1;
            }
            args->name_column = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--version-col") == 0) {
            if (i + 1 >= argc) {
                printf("Error: --version-col requires a column number\n");
                return -1;
            }
            args->version_column = atoi(argv[++i]);
        } else {
            printf("Error: Unknown argument '%s'\n", argv[i]);
            printf("Use --help for usage information\n");
            return -1;
        }
    }

    return 1; // CLI mode detected
}

int validate_cli_args(const CLIArgs *args) {
    if (args->show_help) {
        return 1; // Help is always valid
    }

    if (strlen(args->csv_file_path) == 0) {
        printf("Error: CSV file path is required (--csv)\n");
        return 0;
    }

    if (strlen(args->project_path) == 0) {
        printf("Error: Project path is required (--project)\n");
        return 0;
    }

    if (strlen(args->output_file_path) == 0) {
        printf("Error: Output file path is required (--output)\n");
        return 0;
    }

    if (args->name_column < 1) {
        printf("Error: Name column must be >= 1 (got %d)\n", args->name_column);
        return 0;
    }

    if (args->version_column < 1) {
        printf("Error: Version column must be >= 1 (got %d)\n", args->version_column);
        return 0;
    }

    return 1;
}

int run_cli_mode(int argc, char *argv[]) {
    CLIArgs args;

    // Parse CLI arguments first to check if we're in CLI mode
    int parse_result = parse_cli_arguments(argc, argv, &args);
    if (parse_result == 0) {
        return -1; // Not CLI mode, fall back to GUI
    } else if (parse_result < 0) {
        return 1; // Parse error
    }

    // Only print CLI header if we're actually in CLI mode
    printf("NPM Vulnerability Checker - CLI Mode\n");
    printf("====================================\n\n");

    // Show help if requested
    if (args.show_help) {
        print_cli_help(argv[0]);
        return 0;
    }

    // Validate arguments
    if (!validate_cli_args(&args)) {
        printf("\nUse --help for usage information\n");
        return 1;
    }

    printf("Configuration:\n");
    printf("  CSV File: %s\n", args.csv_file_path);
    printf("  Project: %s\n", args.project_path);
    printf("  Output: %s\n", args.output_file_path);
    printf("  CSV Columns: name=%d, version=%d\n\n", args.name_column, args.version_column);

    // Create data structures
    PackageList *vulnerable_packages = create_package_list();
    DependencyTree *dependencies = create_dependency_tree();
    VulnerabilityResults results = {0};

    if (!vulnerable_packages || !dependencies) {
        printf("Error: Failed to initialize data structures\n");
        if (vulnerable_packages) free_package_list(vulnerable_packages);
        if (dependencies) free_dependency_tree(dependencies);
        return 1;
    }

    // Load vulnerability database
    printf("[1/5] Loading CSV vulnerability database...\n");
    int csv_result = load_csv_packages(args.csv_file_path,
                                      args.name_column - 1,
                                      args.version_column - 1,
                                      vulnerable_packages);

    if (csv_result != 1) {
        printf("Error: Failed to load CSV file\n");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return 1;
    }

    printf("  Loaded %d packages from database\n\n", vulnerable_packages->count);

    // Analyze project dependencies
    printf("[2/5] Analyzing project dependencies...\n");
    int npm_result = get_npm_dependencies(args.project_path, "build", dependencies);

    if (npm_result != 1) {
        printf("Error: Failed to analyze NPM dependencies\n");
        printf("  Make sure the project path contains a valid package.json\n");
        printf("  Ensure npm is installed and available in PATH\n");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return 1;
    }

    printf("  Found %d dependencies\n\n", dependencies->count);

    // Step 3: Check for vulnerabilities
    printf("[3/5] Checking for vulnerabilities...\n");
    int vuln_result = check_vulnerabilities(vulnerable_packages, dependencies, &results);

    if (vuln_result != 1) {
        printf("Error: Failed to check vulnerabilities\n");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return 1;
    }

    printf("  Found %d vulnerabilities\n", results.count);
    printf("  Found %d warnings\n\n", results.warning_count);

    // Step 4: Generate report
    printf("[4/5] Generating vulnerability report...\n");
    char report[50000]; // Large buffer for report
    int report_result = generate_vulnerability_report(&results, vulnerable_packages,
                                                     dependencies, report, sizeof(report));

    if (report_result != 1) {
        printf("Error: Failed to generate report\n");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return 1;
    }

    // Step 5: Save report to file
    printf("[5/5] Saving report to file...\n");
    int save_result = save_report_to_file(args.output_file_path, report);

    if (save_result != 1) {
        printf("Error: Failed to save report to file\n");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return 1;
    }

    printf("  Report saved to: %s\n\n", args.output_file_path);

    // Summary
    printf("Scan Complete!\n");
    printf("=============\n");
    printf("  Dependencies analyzed: %d\n", dependencies->count);
    printf("  Vulnerabilities found: %d\n", results.count);
    printf("  Warnings: %d\n", results.warning_count);

    if (results.count > 0) {
        printf("  Status: VULNERABLE\n");
        printf("  Report: %s\n", args.output_file_path);
    } else {
        printf("  Status: OK\n");
    }

    // Cleanup
    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);

    return results.count > 0 ? 1 : 0; // Return 1 if vulnerabilities found, 0 if clean
}