#ifdef _WIN32

#include "gui_windows.h"
#include "csv_parser.h"
#include "npm_analyzer.h"
#include "vulnerability_checker.h"
#include "report_generator.h"
#include <time.h>

static WindowsGUIElements gui_elements = {0};
static AppState app_state = {0};

char* select_folder_dialog(void) {
    static char folder_path[MAX_PATH_LEN];
    BROWSEINFO bi = {0};
    LPITEMIDLIST pidl;

    bi.lpszTitle = "Select NPM Project Folder";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    pidl = SHBrowseForFolder(&bi);
    if (pidl != NULL) {
        if (SHGetPathFromIDList(pidl, folder_path)) {
            CoTaskMemFree(pidl);
            return folder_path;
        }
        CoTaskMemFree(pidl);
    }
    return NULL;
}

char* select_csv_file_dialog(void) {
    static char file_path[MAX_PATH_LEN];
    OPENFILENAME ofn = {0};

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = file_path;
    ofn.nMaxFile = MAX_PATH_LEN;
    ofn.lpstrFilter = "CSV Files\0*.csv\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = "Select CSV File";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    file_path[0] = '\0';

    if (GetOpenFileName(&ofn)) {
        return file_path;
    }
    return NULL;
}

char* select_output_directory_dialog(void) {
    static char folder_path[MAX_PATH_LEN];
    BROWSEINFO bi = {0};
    LPITEMIDLIST pidl;

    bi.lpszTitle = "Select Output Directory";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    pidl = SHBrowseForFolder(&bi);
    if (pidl != NULL) {
        if (SHGetPathFromIDList(pidl, folder_path)) {
            CoTaskMemFree(pidl);
            return folder_path;
        }
        CoTaskMemFree(pidl);
    }
    return NULL;
}

void process_scan_request(WindowsGUIElements *elements) {
    if (elements == NULL) {
        return;
    }

    SetWindowText(elements->status_label, "Processing... Please wait...");

    char name_col_str[10], version_col_str[10];
    GetWindowText(elements->name_col_edit, name_col_str, sizeof(name_col_str));
    GetWindowText(elements->version_col_edit, version_col_str, sizeof(version_col_str));

    app_state.package_name_column = atoi(name_col_str) - 1;
    app_state.version_column = atoi(version_col_str) - 1;

    if (strlen(app_state.npm_project_path) == 0 ||
        strlen(app_state.csv_file_path) == 0 ||
        strlen(app_state.output_directory) == 0) {
        SetWindowText(elements->status_label, "Error: Please select all required files and directories");
        return;
    }

    if (app_state.package_name_column < 0 || app_state.version_column < 0) {
        SetWindowText(elements->status_label, "Error: Column numbers must be positive");
        return;
    }

    PackageList *vulnerable_packages = create_package_list();
    DependencyTree *dependencies = create_dependency_tree();
    VulnerabilityResults results = {0};
    char report[100000] = {0};

    if (vulnerable_packages == NULL || dependencies == NULL) {
        SetWindowText(elements->status_label, "Error: Out of memory");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return;
    }

    if (!load_csv_packages(app_state.csv_file_path,
                         app_state.package_name_column,
                         app_state.version_column,
                         vulnerable_packages)) {
        SetWindowText(elements->status_label, "Error: Failed to load CSV file");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return;
    }

    if (!get_npm_dependencies(app_state.npm_project_path, app_state.output_directory, dependencies)) {
        SetWindowText(elements->status_label, "Error: Failed to analyze NPM project");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return;
    }

    if (!check_vulnerabilities(vulnerable_packages, dependencies, &results)) {
        SetWindowText(elements->status_label, "Error: Failed to check vulnerabilities");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return;
    }

    if (!generate_vulnerability_report(&results, vulnerable_packages, dependencies,
                                     report, sizeof(report))) {
        SetWindowText(elements->status_label, "Error: Failed to generate report");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return;
    }

    // Auto-generate filenames with timestamp
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", tm_info);

    char report_path[MAX_PATH_LEN];
    snprintf(report_path, sizeof(report_path), "%s\\vuln_report_%s.txt",
             app_state.output_directory, timestamp);

    if (save_report_to_file(report_path, report)) {
        char status_msg[300];
        snprintf(status_msg, sizeof(status_msg),
                "Scan complete! Found %d vulnerabilities. Files saved to %s",
                results.count, app_state.output_directory);
        SetWindowText(elements->status_label, status_msg);
    } else {
        SetWindowText(elements->status_label, "Error: Failed to save report");
    }

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1001: {
                    char *path = select_folder_dialog();
                    if (path != NULL) {
                        SetWindowText(gui_elements.npm_path_edit, path);
                        strcpy(app_state.npm_project_path, path);
                    }
                    break;
                }
                case 1002: {
                    char *path = select_csv_file_dialog();
                    if (path != NULL) {
                        SetWindowText(gui_elements.csv_path_edit, path);
                        strcpy(app_state.csv_file_path, path);
                    }
                    break;
                }
                case 1003: {
                    char *path = select_output_directory_dialog();
                    if (path != NULL) {
                        SetWindowText(gui_elements.report_path_edit, path);
                        strcpy(app_state.output_directory, path);
                    }
                    break;
                }
                case 1004: {
                    process_scan_request(&gui_elements);
                    break;
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int run_windows_gui(void) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char CLASS_NAME[] = "NPMVulnChecker";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        printf("Error: Failed to register window class\n");
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "NPM Vulnerability Checker",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 500,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        printf("Error: Failed to create window\n");
        return 0;
    }

    CreateWindow("STATIC", "NPM Project Folder:", WS_VISIBLE | WS_CHILD,
                 10, 10, 150, 20, hwnd, NULL, hInstance, NULL);
    gui_elements.npm_path_edit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                             10, 35, 400, 25, hwnd, NULL, hInstance, NULL);
    CreateWindow("BUTTON", "Browse", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                 420, 35, 80, 25, hwnd, (HMENU)1001, hInstance, NULL);

    CreateWindow("STATIC", "CSV File:", WS_VISIBLE | WS_CHILD,
                 10, 70, 150, 20, hwnd, NULL, hInstance, NULL);
    gui_elements.csv_path_edit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                             10, 95, 400, 25, hwnd, NULL, hInstance, NULL);
    CreateWindow("BUTTON", "Browse", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                 420, 95, 80, 25, hwnd, (HMENU)1002, hInstance, NULL);

    CreateWindow("STATIC", "Package Name Column (1-based):", WS_VISIBLE | WS_CHILD,
                 10, 130, 200, 20, hwnd, NULL, hInstance, NULL);
    gui_elements.name_col_edit = CreateWindow("EDIT", "1", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                             220, 130, 50, 25, hwnd, NULL, hInstance, NULL);

    CreateWindow("STATIC", "Version Column (1-based):", WS_VISIBLE | WS_CHILD,
                 10, 165, 200, 20, hwnd, NULL, hInstance, NULL);
    gui_elements.version_col_edit = CreateWindow("EDIT", "2", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                                 220, 165, 50, 25, hwnd, NULL, hInstance, NULL);

    CreateWindow("STATIC", "Save Report To:", WS_VISIBLE | WS_CHILD,
                 10, 200, 150, 20, hwnd, NULL, hInstance, NULL);
    gui_elements.report_path_edit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                                10, 225, 400, 25, hwnd, NULL, hInstance, NULL);
    CreateWindow("BUTTON", "Browse", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                 420, 225, 80, 25, hwnd, (HMENU)1003, hInstance, NULL);

    CreateWindow("BUTTON", "Scan for Vulnerabilities", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                 10, 270, 200, 35, hwnd, (HMENU)1004, hInstance, NULL);

    gui_elements.status_label = CreateWindow("STATIC", "Ready to scan", WS_VISIBLE | WS_CHILD,
                                           10, 320, 500, 60, hwnd, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 1;
}

#endif