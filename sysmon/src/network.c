#include "network.h"
#include <stdio.h>
#include <string.h>

static uint64_t prev_rx_bytes = 0;
static uint64_t prev_tx_bytes = 0;

// read /proc/net/dev and parse interface name, rx bytes, and tx bytes
static void read_total_bytes(uint64_t *rx, uint64_t *tx) {
    FILE *fp = fopen("/proc/net/dev", "r");
    if (!fp) return;

    char line[256];
    *rx = 0;
    *tx = 0;

    // Skip headers
    if (fgets(line, sizeof(line), fp)) {}
    if (fgets(line, sizeof(line), fp)) {}

    while (fgets(line, sizeof(line), fp)) {
        char iface[32];
        uint64_t r_bytes, t_bytes;
        uint64_t dummy;

        char *colon = strchr(line, ':');
        if (!colon) continue;

        *colon = ' ';
        sscanf(line, "%31s %lu %lu %lu %lu %lu %lu %lu %lu %lu",
               iface, &r_bytes, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &t_bytes);

        if (strcmp(iface, "lo") != 0) {
            *rx += r_bytes;
            *tx += t_bytes;
        }
    }
    fclose(fp);
}

// initialize
void net_init(void) {
    read_total_bytes(&prev_rx_bytes, &prev_tx_bytes);
}

// convert to kilobyte and get real-time transfer speed, update for next tick
void net_get_stats(net_stats_t *stats, double elapsed_seconds) {
    uint64_t curr_rx, curr_tx;
    read_total_bytes(&curr_rx, &curr_tx);

    if (elapsed_seconds > 0.0) {
        stats->rx_kb_per_sec = (double)(curr_rx - prev_rx_bytes) / 1024.0 / elapsed_seconds;
        stats->tx_kb_per_sec = (double)(curr_tx - prev_tx_bytes) / 1024.0 / elapsed_seconds;
    } else {
        stats->rx_kb_per_sec = 0.0;
        stats->tx_kb_per_sec = 0.0;
    }

    prev_rx_bytes = curr_rx;
    prev_tx_bytes = curr_tx;
}