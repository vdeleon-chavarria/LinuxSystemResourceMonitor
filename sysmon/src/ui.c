#include "ui.h"
#include <ncurses.h>

// initializing
void ui_init(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_CYAN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_BLACK, COLOR_WHITE);
    }
}

// teardown
void ui_cleanup(void) {
    curs_set(1);
    endwin();
}

// progress bar
static void draw_progress_bar(int y, int x, int width, double pct) {
    mvprintw(y, x, "[");
    int fill = (int)((pct / 100.0) * (width - 2));
    if (fill < 0) fill = 0;
    if (fill > width - 2) fill = width - 2;

    attron(COLOR_PAIR(pct > 80 ? 3 : 1));
    for (int i = 0; i < fill; i++) addch('|');
    attroff(COLOR_PAIR(pct > 80 ? 3 : 1));

    for (int i = fill; i < width - 2; i++) addch(' ');
    printw("] %5.1f%%", pct);
}

// dashboard
void ui_render(const cpu_stats_t *cpu, const mem_stats_t *mem, const net_stats_t *net, const proc_list_t *procs, proc_sort_mode_t sort_mode, int selected_index) {
    erase();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // Header Panel
    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(0, 2, "=== SYSTEM RESOURCE MONITOR ===");
    attroff(A_BOLD | COLOR_PAIR(2));

    // CPU Section
    mvprintw(2, 2, "CPU Total: ");
    draw_progress_bar(2, 13, cols / 3, cpu->total_usage);

    // Memory Section
    mvprintw(3, 2, "Memory   : ");
    draw_progress_bar(3, 13, cols / 3, mem->mem_usage_pct);
    mvprintw(3, (cols / 3) + 24, "(%lu MB / %lu MB)", (mem->total_kb - mem->available_kb) / 1024, mem->total_kb / 1024);

    // Network Section
    mvprintw(4, 2, "Net RX   : %8.2f KB/s | Net TX: %8.2f KB/s", net->rx_kb_per_sec, net->tx_kb_per_sec);

    // Process Table Header
    int table_start_y = 6;
    attron(A_STANDOUT);
    mvprintw(table_start_y, 2, " %-6s %-12s %-6s %-10s %-8s %-20s", "PID", "USER", "STATE", "MEM(KB)", "CPU_TICKS", "COMMAND");
    for (int i = 64; i < cols - 2; i++) addch(' ');
    attroff(A_STANDOUT);

    // Render Process Rows
    int max_visible_procs = rows - table_start_y - 3;
    for (int i = 0; i < max_visible_procs && i < (int)procs->count; i++) {
        int row_y = table_start_y + 1 + i;
        if (i == selected_index) attron(COLOR_PAIR(4));
        mvprintw(row_y, 2, " %-6d %-12s %-6c %-10ld %-8.1f %-20s",
                 procs->items[i].pid,
                 procs->items[i].user,
                 procs->items[i].state,
                 procs->items[i].mem_rss_kb,
                 procs->items[i].cpu_usage,
                 procs->items[i].name);
        if (i == selected_index) attroff(COLOR_PAIR(4));
    }

    // Status Footer
    attron(COLOR_PAIR(2));
    mvprintw(rows - 1, 2, "[Q] Quit | [P] Sort CPU | [M] Sort MEM | [I] Sort PID | Active Sort: %s",
             sort_mode == SORT_BY_CPU ? "CPU" : (sort_mode == SORT_BY_MEM ? "MEM" : "PID"));
    attroff(COLOR_PAIR(2));

    refresh();
}