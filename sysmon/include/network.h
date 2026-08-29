#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>

// UI layer struct
typedef struct {
    double rx_kb_per_sec;
    double tx_kb_per_sec;
} net_stats_t;

// lifecycle
void net_init(void);
void net_get_stats(net_stats_t *stats, double elapsed_seconds);

#endif