#ifndef GUI_GTK_H
#define GUI_GTK_H

#ifndef _WIN32

#include <gtk/gtk.h>
#include "common.h"

typedef struct {
    GtkWidget *npm_path_entry;
    GtkWidget *csv_path_entry;
    GtkWidget *name_col_entry;
    GtkWidget *version_col_entry;
    GtkWidget *report_path_entry;
    GtkWidget *status_label;
    GtkWidget *window;
} GTKGUIElements;

int run_gtk_gui(int argc, char *argv[]);
void on_npm_folder_clicked(GtkWidget *widget, gpointer data);
void on_csv_file_clicked(GtkWidget *widget, gpointer data);
void on_report_path_clicked(GtkWidget *widget, gpointer data);
void on_scan_clicked(GtkWidget *widget, gpointer data);

#endif

#endif