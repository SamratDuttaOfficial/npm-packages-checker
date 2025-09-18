#ifndef CLI_H
#define CLI_H

#include "common.h"

typedef struct {
    char csv_file_path[MAX_PATH_LEN];
    char project_path[MAX_PATH_LEN];
    char output_file_path[MAX_PATH_LEN];
    int name_column;
    int version_column;
    int show_help;
    int is_valid;
} CLIArgs;

// Function declarations
int run_cli_mode(int argc, char *argv[]);
int parse_cli_arguments(int argc, char *argv[], CLIArgs *args);
void print_cli_help(const char *program_name);
void print_cli_usage(const char *program_name);
int validate_cli_args(const CLIArgs *args);

#endif // CLI_H