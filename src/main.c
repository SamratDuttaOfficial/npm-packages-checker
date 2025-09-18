#include "common.h"

#ifdef _WIN32
    #include "gui_windows.h"
#else
    #include "gui_gtk.h"
#endif

int main(int argc, char *argv[]) {
    printf("Starting NPM Vulnerability Checker...\n");

#ifdef _WIN32
    (void)argc;
    (void)argv;
    return run_windows_gui();
#else
    return run_gtk_gui(argc, argv);
#endif
}