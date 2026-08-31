#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include "ui.h"
#include "cpu.h"
#include "memory.h"
#include "network.h"
#include "process.h"

int main(void) {
    ui_init();
    
    timeout(100); 

    cpu_stats_t mock_cpu = {
        .total_usage = 42.5,
        .core_count = 4,
        .core_usage = {30.0, 55.0, 78.0, 12.0}
    };

    mem_stats_t mock_mem = {
        .total_kb = 8192 * 1024,
        .available_kb = 3072 * 1024,
        .mem_usage_pct = 62.5
    };

    net_stats_t mock_net = {
        .rx_kb_per_sec = 1250.40,
        .tx_kb_per_sec = 340.80
    };

    proc_list_t mock_procs;
    mock_procs.count = 5;
    mock_procs.capacity = 5;
    mock_procs.items = malloc(5 * sizeof(proc_info_t));

    mock_procs.items[0] = (proc_info_t){ .pid = 1,    .name = "systemd",     .user = "root",     .state = 'S', .cpu_usage = 1.2,  .mem_rss_kb = 12288 };
    mock_procs.items[1] = (proc_info_t){ .pid = 542,  .name = "mysqld",      .user = "mysql",    .state = 'S', .cpu_usage = 14.5, .mem_rss_kb = 256000 };
    mock_procs.items[2] = (proc_info_t){ .pid = 1024, .name = "code-server", .user = "valerie",  .state = 'R', .cpu_usage = 28.3, .mem_rss_kb = 184320 };
    mock_procs.items[3] = (proc_info_t){ .pid = 1420, .name = "nginx",        .user = "www-data", .state = 'S', .cpu_usage = 3.1,  .mem_rss_kb = 32768 };
    mock_procs.items[4] = (proc_info_t){ .pid = 2048, .name = "bash",         .user = "valerie",  .state = 'S', .cpu_usage = 0.4,  .mem_rss_kb = 8192 };

    proc_sort_mode_t sort_mode = SORT_BY_CPU;
    proc_sort(&mock_procs, sort_mode);
    int selected_index = 0;
    int last_key = 0;

    while (1) {
        ui_render(&mock_cpu, &mock_mem, &mock_net, &mock_procs, sort_mode, selected_index);

        mvprintw(LINES - 1, 0, "[DEBUG] Last Key: %d ('%c') | Sort Mode: %s ", 
                 last_key, (last_key >= 32 && last_key <= 126) ? last_key : '?',
                 sort_mode == SORT_BY_CPU ? "CPU" : (sort_mode == SORT_BY_MEM ? "MEM" : "PID"));
        refresh();

        int ch = getch();
        if (ch != ERR) {
            last_key = ch;
            if (ch == 'q' || ch == 'Q') {
                break;
            } else if (ch == KEY_DOWN) {
                if (selected_index < (int)mock_procs.count - 1) selected_index++;
            } else if (ch == KEY_UP) {
                if (selected_index > 0) selected_index--;
            } else if (ch == 'p' || ch == 'P') {
                sort_mode = SORT_BY_CPU;
                proc_sort(&mock_procs, sort_mode);
                selected_index = 0;
            } else if (ch == 'm' || ch == 'M') {
                sort_mode = SORT_BY_MEM;
                proc_sort(&mock_procs, sort_mode);
                selected_index = 0;
            } else if (ch == 'i' || ch == 'I') {
                sort_mode = SORT_BY_PID;
                proc_sort(&mock_procs, sort_mode);
                selected_index = 0;
            }
        }
    }

    free(mock_procs.items);
    ui_cleanup();

    return 0;
}