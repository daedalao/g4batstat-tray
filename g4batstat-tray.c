include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function to read battery status from /proc/pmu/battery_0
void get_battery_status(int *charge, int *max_charge, int *current, int *voltage, int *time_rem) {
    FILE *fp = fopen("/proc/pmu/battery_0", "r");
    if (fp == NULL) {
        perror("Failed to open battery file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "charge     : %d", charge) == 1) continue;
        if (sscanf(line, "max_charge : %d", max_charge) == 1) continue;
        if (sscanf(line, "current    : %d", current) == 1) continue;
        if (sscanf(line, "voltage    : %d", voltage) == 1) continue;
        if (sscanf(line, "time rem.  : %d", time_rem) == 1) continue;
    }

    fclose(fp);
}

// Function to update the tray icon and tooltip
void update_tray_icon(GtkStatusIcon *status_icon) {
    int charge = 0, max_charge = 0, current = 0, voltage = 0, time_rem = 0;
    get_battery_status(&charge, &max_charge, &current, &voltage, &time_rem);

    // Calculate charge percentage
    int charge_percentage = max_charge > 0 ? (charge * 100) / max_charge : 0;

    // Update the tooltip with battery information
    char tooltip[256];
    snprintf(tooltip, sizeof(tooltip), "Charge: %d mAh\nMax Charge: %d mAh\nCurrent Draw: %d mA\nVoltage: %d V\nTi>
             charge, max_charge, current, voltage / 1000, time_rem, charge_percentage);
    gtk_status_icon_set_tooltip_text(status_icon, tooltip);

    // Update icon based on battery level
    if (charge_percentage >= 75) {
        gtk_status_icon_set_from_icon_name(status_icon, "battery-full");
    } else if (charge_percentage >= 30) {
        gtk_status_icon_set_from_icon_name(status_icon, "battery-medium");
    } else {
        gtk_status_icon_set_from_icon_name(status_icon, "battery-empty");
    }
}

// Main function
int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    GtkStatusIcon *status_icon = gtk_status_icon_new();
    gtk_status_icon_set_visible(status_icon, TRUE);
    update_tray_icon(status_icon);

    // Update every 60 seconds
    g_timeout_add_seconds(60, (GSourceFunc) update_tray_icon, status_icon);

    // Create a quit menu item
    GtkWidget *menu = gtk_menu_new();
    GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(gtk_main_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit_item);
    gtk_widget_show_all(menu);

    // Right-click to show menu
    g_signal_connect(status_icon, "popup-menu", G_CALLBACK(gtk_menu_popup), menu);

    gtk_main();

    return 0;
}
