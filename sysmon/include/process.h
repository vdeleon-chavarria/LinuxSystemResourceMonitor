#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

// current sorting criteria
typedef enum {
    SORT_BY_CPU,
    SORT_BY_MEM,
    SORT_BY_PID
} proc_sort_mode_t;

// individual process
typedef struct {
    pid_t pid;
    char name[64];
    char user[32];
    char state;
    double cpu_usage;
    long mem_rss_kb;
} proc_info_t;

// running processes
typedef struct {
    proc_info_t *items;
    size_t count;
    size_t capacity;
} proc_list_t;

// function
void proc_list_init(proc_list_t *list);
void proc_scan_all(proc_list_t *list);
void proc_sort(proc_list_t *list, proc_sort_mode_t mode);
void proc_list_free(proc_list_t *list);

#endif