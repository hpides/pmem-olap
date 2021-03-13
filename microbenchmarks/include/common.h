#ifndef NVM_DB_BENCHMARK_INCLUDE_COMMON_H
#define NVM_DB_BENCHMARK_INCLUDE_COMMON_H

#define MIN(a,b) (((a)<(b))?(a):(b))

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <libpmem.h>
#include <libpmem2.h>

#include <sys/sysinfo.h>
#include <pthread.h>
#include <sys/time.h>
#include <pthread.h>

#define READ_MODE 0
#define WRITE_MODE 1

#define MAX_THREADS 72
//avx512 makes it necessary to align offsets.
#define ADDRESS_ALIGNMENT 64
#define AVX_ALIGN(x) (x / ADDRESS_ALIGNMENT) * ADDRESS_ALIGNMENT
#define DEVDAX_FILE_SIZE 70ULL * 1024ULL * 1024ULL * 1024ULL

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define ACCESS_LOGFILE 0
#define ACCESS_DISJOINT 1
#define THREAD_ALIGNMENT 64

bool verboseFlag;
bool alignedFlag;
long long time_seed;

struct thread_info{
    pthread_t thread_id;
    int thread_number;
    int thread_count;
    int core;

    long long pmem_size;
    char *pmem_start;
    char *pmem_end;

    int access_distance;
	uint64_t accesses_per_thread;
	int access_pattern;
	char *thread_start_addr;
	char **random_ring;
	pthread_barrier_t *bar;

    long long time_seed;
};

struct access_info{

    int dram_mode;

    char *pmem_path;
    uint64_t pmem_size;
    char *pmem_start;
    char *pmem_end;

    int thread_count;
    char *mapping_path;
    int thread_mapping[MAX_THREADS];
	char **random_ring;
	char *thread_start_addr[MAX_THREADS];
	uint64_t accesses_per_thread;
	int access_pattern;

    int operation_mode;
    char *operation_name;
    pthread_t operation_id;

    int measure_group;
    struct timeval time_begin;
    struct timeval time_end;
    uint64_t access_distance;
	uint64_t access_count;

    bool random;
	bool reuse_random;
    int pinned;
    char *numa;

    #ifdef LIB_PMEM2
    struct pmem2_config *cfg;
    struct pmem2_map *map;
    struct pmem2_source *src;
    #endif

    void (*pthread_function) (struct thread_info*);
};

struct min_access_info{
    char *pmem_start;
    char *pmem_end;
    size_t stride;
};

#endif