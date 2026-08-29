#include "cpu.h"
#include <stdio.h>
#include <string.h>

static cpu_raw_t prev_total;
static cpu_raw_t prev_cores[128];
static int num_cores = 0;

// sums all active and idle ticks and computes the difference between two snaps
static double calculate_usage(const cpu_raw_t *prev, const cpu_raw_t *curr) {
    uint64_t prev_idle = prev->idle + prev->iowait;
    uint64_t curr_idle = curr->idle + curr->iowait;

    uint64_t prev_non_idle = prev->user + prev->nice + prev->system + prev->irq + prev->softirq + prev->steal;
    uint64_t curr_non_idle = curr->user + curr->nice + curr->system + curr->irq + curr->softirq + curr->steal;

    uint64_t prev_total_time = prev_idle + prev_non_idle;
    uint64_t curr_total_time = curr_idle + curr_non_idle;

    uint64_t total_delta = curr_total_time - prev_total_time;
    uint64_t idle_delta = curr_idle - prev_idle;

    if (total_delta == 0) return 0.0; // divide by 0
    return (double)(total_delta - idle_delta) * 100.0 / (double)total_delta;
}

// read /proc/stat and creates a struct for the core info
static void read_raw(cpu_raw_t *total, cpu_raw_t cores[], int *core_cnt) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return;

    char line[256];
    *core_cnt = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "cpu ", 4) == 0) {
            sscanf(line + 4, "%lu %lu %lu %lu %lu %lu %lu %lu",
                   &total->user, &total->nice, &total->system, &total->idle,
                   &total->iowait, &total->irq, &total->softirq, &total->steal);
        } else if (strncmp(line, "cpu", 3) == 0 && *core_cnt < 128) {
            int core_idx;
            sscanf(line, "cpu%d %lu %lu %lu %lu %lu %lu %lu %lu",
                   &core_idx, &cores[*core_cnt].user, &cores[*core_cnt].nice,
                   &cores[*core_cnt].system, &cores[*core_cnt].idle,
                   &cores[*core_cnt].iowait, &cores[*core_cnt].irq,
                   &cores[*core_cnt].softirq, &cores[*core_cnt].steal);
            (*core_cnt)++;
        }
    }
    fclose(fp);
}

void cpu_init(void) {
    read_raw(&prev_total, prev_cores, &num_cores);
}

// compare stats and arrays between ticks
void cpu_get_stats(cpu_stats_t *stats) {
    cpu_raw_t curr_total;
    cpu_raw_t curr_cores[128];
    int current_core_count = 0;

    read_raw(&curr_total, curr_cores, &current_core_count);
    stats->core_count = current_core_count;
    stats->total_usage = calculate_usage(&prev_total, &curr_total);

    for (int i = 0; i < current_core_count; i++) {
        stats->core_usage[i] = calculate_usage(&prev_cores[i], &curr_cores[i]);
        prev_cores[i] = curr_cores[i];
    }
    prev_total = curr_total;
}