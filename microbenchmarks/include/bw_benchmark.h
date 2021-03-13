#ifndef NVM_DB_BENCHMARK_INCLUDE_BW_BENCHMARK_H
#define NVM_DB_BENCHMARK_INCLUDE_BW_BENCHMARK_H

#include "common.h"

void process_operations(struct access_info *ac_infos, int operation_count);
void process_operation(struct access_info *ac_info);

inline static void set_access_pattern(struct min_access_info *mai, struct thread_info *t_info) {
    if (t_info->access_pattern == ACCESS_LOGFILE) {
        mai->pmem_start = t_info->pmem_start + t_info->thread_number * t_info->access_distance;
        mai->pmem_end = t_info->pmem_end;
        mai->stride = t_info->access_distance * t_info->thread_count;
    } else if (t_info->access_pattern == ACCESS_DISJOINT) {
		size_t offset_per_thread = (t_info->pmem_size / t_info->thread_count) / THREAD_ALIGNMENT * THREAD_ALIGNMENT;
		mai->pmem_start = t_info->pmem_start + t_info->thread_number * offset_per_thread;
		mai->pmem_end = MIN(mai->pmem_start + offset_per_thread, t_info->pmem_end);
		mai->stride = t_info->access_distance;
    }
}

#endif