#include <stdio.h>
#include "process.h"

static void print_top_processes(const proc_list_t *list, int limit, const char *title) {
    printf("=== %s (Top %d of %zu) ===\n", title, limit, list->count);
    printf("%-7s %-12s %-6s %-12s %-10s %-25s\n",
           "PID", "USER", "STATE", "MEM (KB)", "CPU (sec)", "COMMAND");
    printf("----------------------------------------------------------------------\n");

    int display_count = limit < (int)list->count ? limit : (int)list->count;
    for (int i = 0; i < display_count; i++) {
        printf("%-7d %-12s %-6c %-12ld %-10.2f %-25s\n",
               list->items[i].pid,
               list->items[i].user,
               list->items[i].state,
               list->items[i].mem_rss_kb,
               list->items[i].cpu_usage,
               list->items[i].name);
    }
    printf("\n");
}

int main(void) {
    proc_list_t procs;
    proc_list_init(&procs);

    printf("Scanning /proc directory for active processes...\n");
    proc_scan_all(&procs);
    printf("Total processes discovered: %zu\n\n", procs.count);

    if (procs.count == 0) {
        printf("Error: No processes found. Check /proc filesystem permissions.\n");
        proc_list_free(&procs);
        return 1;
    }

    proc_sort(&procs, SORT_BY_MEM);
    print_top_processes(&procs, 5, "Sorted by Memory Usage (RSS)");

    proc_sort(&procs, SORT_BY_CPU);
    print_top_processes(&procs, 5, "Sorted by CPU Consumption");

    proc_sort(&procs, SORT_BY_PID);
    print_top_processes(&procs, 5, "Sorted by Process ID (PID)");

    proc_list_free(&procs);
    printf("Process list memory freed successfully.\n");

    return 0;
}