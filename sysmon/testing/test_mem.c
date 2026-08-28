#include <stdio.h>
#include "memory.h"

int main(void) {
    mem_stats_t stats;
    mem_get_stats(&stats);

    printf("--- Memory Test ---\n");
    printf("Total RAM: %lu MB\n", stats.total_kb / 1024);
    printf("Available: %lu MB\n", stats.available_kb / 1024);
    printf("Free RAM: %lu MB\n", stats.free_kb / 1024);
    printf("Used RAM: %lu MB\n", (stats.total_kb - stats.available_kb) / 1024);
    printf("Usage: %.2f%%\n", stats.mem_usage_pct);
    printf("Swap Total: %lu MB\n", stats.swap_total_kb / 1024);
    printf("Swap Used: %.2f%%\n", stats.swap_usage_pct);

    
    return 0;
}