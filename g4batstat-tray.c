#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static GtkStatusIcon *tray_icon;

gboolean update_battery_status(gpointer data) {
    FILE *fp = fopen("/proc/pmu/battery_0", "r");
    if (fp == NULL) {
        gtk_status_icon_set_tooltip_text(tray_icon, "Battery not found");
        return TRUE;
    }

    char line[128];
    int charge = -1, max_charge = -1, current = -1, voltage = -1, time_remaining = -1;
    char status[32] = "Unknown";

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "charge     : %d", &charge) == 1) continue;
        if (sscanf(line, "max_charge : %d", &max_charge) == 1) continue;
        if (sscanf(line, "current    : %d", &current) == 1) continue;
        if (sscanf(line, "voltage    : %d", &voltage) == 1) continue;
        if (sscanf(line, "time rem.  : %d", &time_remaining) == 1) continue;
    }
    fclose(fp);

    if (charge >= 0 && max_charge > 0) {
        int percent = (charge * 100) / max_charge;
        if (time_remaining >= 0) {
            int hours = time_remaining / 60;
            int minutes = time_remaining % 60;
            snprintf(status, sizeof(status), "%d%% - %d:%02d remaining", percent, hours, minutes);
        } else {
            snprintf(status, sizeof(status), "%d%% (No time data)", percent);
        }

        if (current == 0) {
            strncat(status, " (Full)", sizeof(status) - strlen(status) - 1);
        } else {
            strncat(status, " (Discharging)", sizeof(status) - strlen(status) - 1);
        }
    } else {
        strncpy(status, "Battery not available", sizeof(status) - 1);
    }

    gtk_status_icon_set_tooltip_text(tray_icon, status);
    gtk_status_icon_set_from_icon_name(tray_icon, "battery");

    return TRUE;
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    tray_icon = gtk_status_icon_new_from_icon_name("battery");
    gtk_status_icon_set_tooltip_text(tray_icon, "Battery Monitor");

    update_battery_status(NULL);
    g_timeout_add_seconds(5, update_battery_status, NULL);

    gtk_main();

    return 0;
}
