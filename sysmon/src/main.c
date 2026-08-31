#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <ncurses.h>
#include "cpu.h"
#include "memory.h"
#include "network.h"
#include "process.h"
#include "ui.h"

static volatile bool running = true;

static void handle_sigint(int sig) {
    (void)sig;
    running = false;
}

int main(void) {
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    ui_init();
    timeout(1000);

    cpu_init();
    net_init();

    proc_list_t procs;
    proc_list_init(&procs);

    cpu_stats_t cpu_stats;
    mem_stats_t mem_stats;
    net_stats_t net_stats;

    proc_sort_mode_t sort_mode = SORT_BY_CPU;
    int selected_index = 0;

    // Initial data capture
    cpu_get_stats(&cpu_stats);
    mem_get_stats(&mem_stats);
    net_get_stats(&net_stats, 1.0);
    proc_scan_all(&procs);
    proc_sort(&procs, sort_mode);

    while (running) {
        ui_render(&cpu_stats, &mem_stats, &net_stats, &procs, sort_mode, selected_index);

        int ch = getch();

        if (ch == 'q' || ch == 'Q') {
            break;
        } else if (ch == 'p' || ch == 'P') {
            sort_mode = SORT_BY_CPU;
            proc_sort(&procs, sort_mode);
            selected_index = 0;
        } else if (ch == 'm' || ch == 'M') {
            sort_mode = SORT_BY_MEM;
            proc_sort(&procs, sort_mode);
            selected_index = 0;
        } else if (ch == 'i' || ch == 'I') {
            sort_mode = SORT_BY_PID;
            proc_sort(&procs, sort_mode);
            selected_index = 0;
        } else if (ch == KEY_DOWN) {
            if (selected_index < (int)procs.count - 1) {
                selected_index++;
            }
        } else if (ch == KEY_UP) {
            if (selected_index > 0) {
                selected_index--;
            }
        }

        cpu_get_stats(&cpu_stats);
        mem_get_stats(&mem_stats);
        net_get_stats(&net_stats, 1.0);

        proc_scan_all(&procs);
        proc_sort(&procs, sort_mode);

        if (selected_index >= (int)procs.count && procs.count > 0) {
            selected_index = (int)procs.count - 1;
        }
    }

    proc_list_free(&procs);
    ui_cleanup();
    return 0;
}