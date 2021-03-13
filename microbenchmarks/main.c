#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <string.h>
#include <time.h>

#include "common.h"
#include "parsing.h"
#include "pmem_mapping.h"
#include "thread_mapping.h"
#include "bw_benchmark.h"

#define MAX_OPERATIONS 8

int main(int argc, char *argv[])
{
    srand(time(0));
    struct access_info* ac_infos = calloc(sizeof(struct access_info) *MAX_OPERATIONS, 1);
    int operation_count;
    parse_input(argc, argv, &operation_count, ac_infos, MAX_OPERATIONS);
    int i;

    for (i = 0; i < operation_count; i++) {
        map_operation(&ac_infos[i]);
        set_thread_function(&ac_infos[i]);
        create_thread_mapping(&ac_infos[i]);
    }

    struct timeval timecheck;
    gettimeofday(&timecheck, NULL);
    
    time_seed = timecheck.tv_usec;

    process_operations(ac_infos, operation_count);
    calculate_bandwidths(ac_infos, operation_count);
    
    for(i = 0; i < operation_count; i++){
        unmap_operation(&ac_infos[i]);
        free(ac_infos[i].mapping_path);
        free(ac_infos[i].operation_name);
        free(ac_infos[i].pmem_path);
    }
    free(ac_infos);
}