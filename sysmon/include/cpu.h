#ifndef CPU_H
#define CPU_H

#include <stdint.h>

// /proc/stat
typedef struct {
    uint64_t user;
    uint64_t nice;
    uint64_t system;
    uint64_t idle;
    uint64_t iowait;
    uint64_t irq;
    uint64_t softirq;
    uint64_t steal;
} cpu_raw_t;

// stats for the UI layer
typedef struct {
    double total_usage;
    int core_count;
    double core_usage[128];
} cpu_stats_t;

// functions
void cpu_init(void);
void cpu_get_stats(cpu_stats_t *stats);


#endif