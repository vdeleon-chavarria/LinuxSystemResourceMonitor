#include <stdio.h>
#include <unistd.h>
#include "cpu.h"

int main(void) {
    cpu_stats_t stats;

    printf("Initializing CPU counters (sampling T0)...\n");
    cpu_init();

    printf("Sampling across 3 intervals (1-second delay each)...\n\n");

    for (int iter = 1; iter <= 3; iter++) {
        sleep(1);
        cpu_get_stats(&stats);

        printf("--- Sample #%d ---\n", iter);
        printf("Total CPU Usage: %6.2f%%\n", stats.total_usage);
        printf("Detected Cores : %d\n", stats.core_count);

        for (int i = 0; i < stats.core_count; i++) {
            printf("  Core %-2d: %6.2f%%\n", i, stats.core_usage[i]);
        }
        printf("\n");
    }

    
    return 0;
}