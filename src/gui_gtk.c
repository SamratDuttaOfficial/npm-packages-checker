#ifndef _WIN32

#include "gui_gtk.h"
#include "csv_parser.h"
#include "npm_analyzer.h"
#include "vulnerability_checker.h"
#include "report_generator.h"
#include <time.h>

static GTKGUIElements gui_elements = {0};
static AppState app_state = {0};

void on_npm_folder_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select NPM Project Folder",
        GTK_WINDOW(data), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Select", GTK_RESPONSE_ACCEPT, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folder = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (folder != NULL) {
            gtk_entry_set_text(GTK_ENTRY(gui_elements.npm_path_entry), folder);
            strncpy(app_state.npm_project_path, folder, sizeof(app_state.npm_project_path) - 1);
            app_state.npm_project_path[sizeof(app_state.npm_project_path) - 1] = '\0';
            g_free(folder);
        }
    }

    gtk_widget_destroy(dialog);
}

void on_csv_file_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select CSV File",
        GTK_WINDOW(data), GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT, NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "CSV files");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename != NULL) {
            gtk_entry_set_text(GTK_ENTRY(gui_elements.csv_path_entry), filename);
            strncpy(app_state.csv_file_path, filename, sizeof(app_state.csv_file_path) - 1);
            app_state.csv_file_path[sizeof(app_state.csv_file_path) - 1] = '\0';
            g_free(filename);
        }
    }

    gtk_widget_destroy(dialog);
}

void on_output_dir_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Output Directory",
        GTK_WINDOW(data), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Select", GTK_RESPONSE_ACCEPT, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folder = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (folder != NULL) {
            gtk_entry_set_text(GTK_ENTRY(gui_elements.report_path_entry), folder);
            strncpy(app_state.output_directory, folder, sizeof(app_state.output_directory) - 1);
            app_state.output_directory[sizeof(app_state.output_directory) - 1] = '\0';
            g_free(folder);
        }
    }

    gtk_widget_destroy(dialog);
}

void on_scan_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;

    gtk_label_set_text(GTK_LABEL(gui_elements.status_label), "Processing... Please wait...");

    while (gtk_events_pending()) {
        gtk_main_iteration();
    }

    const char *name_col_str = gtk_entry_get_text(GTK_ENTRY(gui_elements.name_col_entry));
    const char *version_col_str = gtk_entry_get_text(GTK_ENTRY(gui_elements.version_col_entry));

    app_state.package_name_column = atoi(name_col_str) - 1;
    app_state.version_column = atoi(version_col_str) - 1;

    if (strlen(app_state.npm_project_path) == 0 ||
        strlen(app_state.csv_file_path) == 0 ||
        strlen(app_state.output_directory) == 0) {
        gtk_label_set_text(GTK_LABEL(gui_elements.status_label),
                          "Error: Please select all required files and directories");
        return;
    }

    if (app_state.package_name_column < 0 || app_state.version_column < 0) {
        gtk_label_set_text(GTK_LABEL(gui_elements.status_label),
                          "Error: Column numbers must be positive");
        return;
    }

    PackageList *vulnerable_packages = create_package_list();
    DependencyTree *dependencies = create_dependency_tree();
    VulnerabilityResults results = {0};
    char report[100000] = {0};

    if (vulnerable_packages == NULL || dependencies == NULL) {
        gtk_label_set_text(GTK_LABEL(gui_elements.status_label), "Error: Out of memory");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return;
    }

    if (!load_csv_packages(app_state.csv_file_path,
                         app_state.package_name_column,
                         app_state.version_column,
                         vulnerable_packages)) {
        gtk_label_set_text(GTK_LABEL(gui_elements.status_label), "Error: Failed to load CSV file");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return;
    }

    if (!get_npm_dependencies(app_state.npm_project_path, app_state.output_directory, dependencies)) {
        gtk_label_set_text(GTK_LABEL(gui_elements.status_label), "Error: Failed to analyze NPM project");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return;
    }

    if (!check_vulnerabilities(vulnerable_packages, dependencies, &results)) {
        gtk_label_set_text(GTK_LABEL(gui_elements.status_label), "Error: Failed to check vulnerabilities");
        free_package_list(vulnerable_packages);
        free_dependency_tree(dependencies);
        return;
    }

    if (!generate_vulnerability_report(&results, vulnerable_packages, dependencies,
                                     report, sizeof(report))) {
        gtk_label_set_text(GTK_LABEL(gui_elements.status_label), "Error: Failed to generate report");
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
    snprintf(report_path, sizeof(report_path), "%s%svuln_report_%s.txt",
             app_state.output_directory, PATH_SEPARATOR, timestamp);

    if (save_report_to_file(report_path, report)) {
        char status_msg[300];
        snprintf(status_msg, sizeof(status_msg),
                "Scan complete! Found %d vulnerabilities. Files saved to %s",
                results.count, app_state.output_directory);
        gtk_label_set_text(GTK_LABEL(gui_elements.status_label), status_msg);
    } else {
        gtk_label_set_text(GTK_LABEL(gui_elements.status_label), "Error: Failed to save report");
    }

    free_package_list(vulnerable_packages);
    free_dependency_tree(dependencies);
}

int run_gtk_gui(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    gui_elements.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(gui_elements.window), "NPM Vulnerability Checker");
    gtk_window_set_default_size(GTK_WINDOW(gui_elements.window), 600, 450);
    gtk_container_set_border_width(GTK_CONTAINER(gui_elements.window), 10);
    g_signal_connect(gui_elements.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(gui_elements.window), vbox);

    GtkWidget *npm_label = gtk_label_new("NPM Project Folder:");
    gtk_box_pack_start(GTK_BOX(vbox), npm_label, FALSE, FALSE, 0);

    GtkWidget *npm_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gui_elements.npm_path_entry = gtk_entry_new();
    GtkWidget *npm_button = gtk_button_new_with_label("Browse");
    gtk_box_pack_start(GTK_BOX(npm_hbox), gui_elements.npm_path_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(npm_hbox), npm_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), npm_hbox, FALSE, FALSE, 0);
    g_signal_connect(npm_button, "clicked", G_CALLBACK(on_npm_folder_clicked), gui_elements.window);

    GtkWidget *csv_label = gtk_label_new("CSV File:");
    gtk_box_pack_start(GTK_BOX(vbox), csv_label, FALSE, FALSE, 0);

    GtkWidget *csv_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gui_elements.csv_path_entry = gtk_entry_new();
    GtkWidget *csv_button = gtk_button_new_with_label("Browse");
    gtk_box_pack_start(GTK_BOX(csv_hbox), gui_elements.csv_path_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(csv_hbox), csv_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), csv_hbox, FALSE, FALSE, 0);
    g_signal_connect(csv_button, "clicked", G_CALLBACK(on_csv_file_clicked), gui_elements.window);

    GtkWidget *cols_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    GtkWidget *name_col_label = gtk_label_new("Package Name Column:");
    gui_elements.name_col_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(gui_elements.name_col_entry), "1");
    gtk_entry_set_width_chars(GTK_ENTRY(gui_elements.name_col_entry), 5);

    GtkWidget *version_col_label = gtk_label_new("Version Column:");
    gui_elements.version_col_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(gui_elements.version_col_entry), "2");
    gtk_entry_set_width_chars(GTK_ENTRY(gui_elements.version_col_entry), 5);

    gtk_box_pack_start(GTK_BOX(cols_hbox), name_col_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cols_hbox), gui_elements.name_col_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cols_hbox), version_col_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cols_hbox), gui_elements.version_col_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), cols_hbox, FALSE, FALSE, 0);

    GtkWidget *output_label = gtk_label_new("Output Directory:");
    gtk_box_pack_start(GTK_BOX(vbox), output_label, FALSE, FALSE, 0);

    GtkWidget *output_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gui_elements.report_path_entry = gtk_entry_new();
    GtkWidget *output_button = gtk_button_new_with_label("Browse");
    gtk_box_pack_start(GTK_BOX(output_hbox), gui_elements.report_path_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(output_hbox), output_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), output_hbox, FALSE, FALSE, 0);
    g_signal_connect(output_button, "clicked", G_CALLBACK(on_output_dir_clicked), gui_elements.window);

    GtkWidget *scan_button = gtk_button_new_with_label("Scan for Vulnerabilities");
    gtk_box_pack_start(GTK_BOX(vbox), scan_button, FALSE, FALSE, 10);
    g_signal_connect(scan_button, "clicked", G_CALLBACK(on_scan_clicked), NULL);

    gui_elements.status_label = gtk_label_new("Ready to scan");
    gtk_box_pack_start(GTK_BOX(vbox), gui_elements.status_label, FALSE, FALSE, 0);

    gtk_widget_show_all(gui_elements.window);
    gtk_main();

    return 1;
}

#endif