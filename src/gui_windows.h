#ifndef GUI_WINDOWS_H
#define GUI_WINDOWS_H

#ifdef _WIN32

#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include "common.h"

typedef struct {
    HWND npm_path_edit;
    HWND csv_path_edit;
    HWND name_col_edit;
    HWND version_col_edit;
    HWND report_path_edit;
    HWND status_label;
} WindowsGUIElements;

int run_windows_gui(void);
char* select_folder_dialog(void);
char* select_csv_file_dialog(void);
char* save_report_dialog(void);
void process_scan_request(WindowsGUIElements *elements);

#endif

#endif