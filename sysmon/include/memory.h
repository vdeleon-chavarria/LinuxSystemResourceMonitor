#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct {
    uint64_t total_kb;
    uint64_t free_kb;
    uint64_t available_kb;
    uint64_t buffers_kb;
    uint64_t cached_kb;
    uint64_t swap_total_kb;
    uint64_t swap_free_kb;
    double mem_usage_pct;
    double swap_usage_pct;
} mem_stats_t;

void mem_get_stats(mem_stats_t *stats);


#endif