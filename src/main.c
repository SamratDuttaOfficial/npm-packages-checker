#include "common.h"
#include "cli.h"

#ifdef _WIN32
    #include "gui_windows.h"
#else
    #include "gui_gtk.h"
#endif

int main(int argc, char *argv[]) {
    // Check if CLI mode is requested
    int cli_result = run_cli_mode(argc, argv);
    if (cli_result >= 0) {
        // CLI mode was used (either successfully or with error)
        return cli_result;
    }

    // Fall back to GUI mode
    printf("Starting NPM Vulnerability Checker...\n");

#ifdef _WIN32
    (void)argc;
    (void)argv;
    return run_windows_gui();
#else
    return run_gtk_gui(argc, argv);
#endif
}