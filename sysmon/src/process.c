#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>

// vector management
void proc_list_init(proc_list_t *list) {
    list->count = 0;
    list->capacity = 128;
    list->items = malloc(list->capacity * sizeof(proc_info_t));
}

void proc_list_free(proc_list_t *list) {
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

// parsing & resolving UID
static void parse_proc_stat(pid_t pid, proc_info_t *proc) {
    char path[128];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *fp = fopen(path, "r");
    if (!fp) return;

    proc->pid = pid;
    fscanf(fp, "%*d (%63[^)]) %c", proc->name, &proc->state);

    unsigned long utime = 0, stime = 0;
    long rss = 0;
    // Fast forward to utime (field 14), stime (field 15), and rss (field 24)
    for (int i = 4; i <= 13; i++) {
        unsigned long dummy;
        if (fscanf(fp, "%lu", &dummy) != 1) break;
    }
    if (fscanf(fp, "%lu %lu", &utime, &stime) == 2) {
        proc->cpu_usage = (double)(utime + stime) / (double)sysconf(_SC_CLK_TCK);
    }
    for (int i = 16; i <= 23; i++) {
        long dummy;
        if (fscanf(fp, "%ld", &dummy) != 1) break;
    }
    if (fscanf(fp, "%ld", &rss) == 1) {
        proc->mem_rss_kb = rss * (sysconf(_SC_PAGESIZE) / 1024);
    }
    fclose(fp);

    // Resolve owner
    struct stat st;
    snprintf(path, sizeof(path), "/proc/%d", pid);
    if (stat(path, &st) == 0) {
        struct passwd *pw = getpwuid(st.st_uid);
        if (pw) strncpy(proc->user, pw->pw_name, sizeof(proc->user) - 1);
        else snprintf(proc->user, sizeof(proc->user), "%d", st.st_uid);
    }
}

// Scan directory
void proc_scan_all(proc_list_t *list) {
    list->count = 0;
    DIR *dir = opendir("/proc");
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            pid_t pid = (pid_t)atoi(entry->d_name);
            if (list->count >= list->capacity) {
                list->capacity *= 2;
                list->items = realloc(list->items, list->capacity * sizeof(proc_info_t));
            }
            parse_proc_stat(pid, &list->items[list->count]);
            list->count++;
        }
    }
    closedir(dir);
}

// sorting 
static int cmp_cpu(const void *a, const void *b) {
    const proc_info_t *pa = (const proc_info_t *)a;
    const proc_info_t *pb = (const proc_info_t *)b;
    return (pb->cpu_usage > pa->cpu_usage) - (pb->cpu_usage < pa->cpu_usage);
}

static int cmp_mem(const void *a, const void *b) {
    const proc_info_t *pa = (const proc_info_t *)a;
    const proc_info_t *pb = (const proc_info_t *)b;
    return (pb->mem_rss_kb - pa->mem_rss_kb);
}

static int cmp_pid(const void *a, const void *b) {
    const proc_info_t *pa = (const proc_info_t *)a;
    const proc_info_t *pb = (const proc_info_t *)b;
    return (pa->pid - pb->pid);
}

void proc_sort(proc_list_t *list, proc_sort_mode_t mode) {
    if (mode == SORT_BY_CPU) qsort(list->items, list->count, sizeof(proc_info_t), cmp_cpu);
    else if (mode == SORT_BY_MEM) qsort(list->items, list->count, sizeof(proc_info_t), cmp_mem);
    else qsort(list->items, list->count, sizeof(proc_info_t), cmp_pid);
}