#include "memory.h"
#include <stdio.h>
#include <string.h>

void mem_get_stats(mem_stats_t *stats) {
    memset(stats, 0, sizeof(mem_stats_t));
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return;

    char key[64];
    uint64_t val;
    char uint[16];

    while (fscanf(fp, "%63s %lu %15s", key, &val, uint) == 3) {
        if (strcmp(key, "MemTotal:") == 0) stats->total_kb = val;
        else if (strcmp(key, "MemFree:") == 0) stats->free_kb = val;
        else if (strcmp(key, "MemAvailable:") == 0) stats->available_kb = val;
        else if (strcmp(key, "Buffers:") == 0) stats->buffers_kb = val;
        else if (strcmp(key, "Cached:") == 0) stats->cached_kb = val;
        else if (strcmp(key, "SwapTotal:") == 0) stats->swap_total_kb = val;
        else if (strcmp(key, "SwapFree:") == 0) stats->swap_free_kb = val;
    }
    fclose(fp);

    if (stats->total_kb > 0) {
        uint64_t used_kb = stats->total_kb - stats->available_kb;
        stats->mem_usage_pct = ((double)used_kb / (double)stats->total_kb) * 100.0;
    }
    if (stats->swap_total_kb > 0) {
        uint64_t swap_used = stats->swap_total_kb - stats->swap_free_kb;
        stats->swap_usage_pct = ((double)swap_used / (double)stats->swap_total_kb) * 100.0;
    }
}