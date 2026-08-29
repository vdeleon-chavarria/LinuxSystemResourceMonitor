#include <stdio.h>
#include <unistd.h>
#include "network.h"

int main(void) {
    net_stats_t stats;

    printf("Initializing network counters (sampling T0)...\n");
    net_init();

    printf("Monitoring network throughput across 5 intervals (1-second delay)...\n");
    printf("Tip: Generate traffic in another terminal (e.g., curl or speedtest) to see rates spike!\n\n");

    for (int iter = 1; iter <= 5; iter++) {
        sleep(1);
        net_get_stats(&stats, 1.0);

        printf("--- Sample #%d ---\n", iter);
        printf("  Download (RX) : %8.2f KB/s (%6.2f MB/s)\n", 
               stats.rx_kb_per_sec, stats.rx_kb_per_sec / 1024.0);
        printf("  Upload   (TX) : %8.2f KB/s (%6.2f MB/s)\n\n", 
               stats.tx_kb_per_sec, stats.tx_kb_per_sec / 1024.0);
    }

    return 0;
}