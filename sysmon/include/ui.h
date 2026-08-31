#ifndef UI_H
#define UI_H

#include "cpu.h"
#include "memory.h"
#include "network.h"
#include "process.h"

// rendering
void ui_init(void);
void ui_render(const cpu_stats_t *cpu, const mem_stats_t *mem, const net_stats_t *net, const proc_list_t *procs, proc_sort_mode_t sort_mode, int selected_index);
void ui_cleanup(void);

#endif