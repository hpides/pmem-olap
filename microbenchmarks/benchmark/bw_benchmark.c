#define _GNU_SOURCE
#include "bw_benchmark.h"
#include "common.h"
#include <numa.h>
#include <unistd.h>

void process_operations(struct access_info *ac_infos, int operation_count){
    //TODO: MEASURE TIMING FOR ALL OPERATIONS
    int i;
    for(i = 0; i < operation_count; i++) {
        pthread_create(&ac_infos[i].operation_id, 0, process_operation, &ac_infos[i]);
    }
    for(i = 0; i < operation_count; i++) {
        pthread_join(ac_infos[i].operation_id, NULL);
    }
}

void prepare_dimms(struct thread_info *t_info) {

    char *it = t_info->pmem_start;
    while(it <= t_info->pmem_end - t_info->access_distance) {
        if(*it == 0) {
            t_info->time_seed++;
        }
        it += 128;
    }
}

void process_operation(struct access_info *ac_info){
    if (ac_info->numa != NULL) {
        struct bitmask *bm = numa_parse_nodestring(ac_info->numa);
        numa_run_on_node_mask(bm);
        numa_set_membind(bm);
    }
    struct thread_info *t_infos = (struct thread_info *)malloc(ac_info->thread_count * sizeof(struct thread_info));
    pthread_attr_t *t_atts = (pthread_attr_t *)malloc(ac_info->thread_count * sizeof(pthread_attr_t));
    cpu_set_t *t_cpus = (cpu_set_t*)malloc(ac_info->thread_count * sizeof(cpu_set_t));

	pthread_barrier_t bar;

    int i;
    for(i = 0; i < ac_info->thread_count; i++){
        t_infos[i] = (struct thread_info){
            .thread_number = i,
            .thread_count = ac_info->thread_count,
            .core = ac_info->thread_mapping[i],
            .pmem_size = ac_info->pmem_size,
            .pmem_start = ac_info->pmem_start,
            .pmem_end = ac_info->pmem_end,
            .access_distance = ac_info->access_distance,
			.accesses_per_thread = ac_info->access_count / ac_info->thread_count,
			.thread_start_addr = ac_info->thread_start_addr[i],
			.random_ring = ac_info->random_ring + (i * (ac_info->access_count / ac_info->thread_count)),
			.access_pattern = ac_info->access_pattern,
			.bar = &bar
        };

    }

    int success = 0;
    for(i = 0; i < ac_info->thread_count; i++){
        struct thread_info *t_info = &t_infos[i];
        pthread_attr_t *t_att = &t_atts[i];
        pthread_attr_init(t_att);

        if(ac_info->pinned){
            cpu_set_t *cpu_set = &t_cpus[i];

            CPU_ZERO(cpu_set);
            CPU_SET(t_info->core, cpu_set);
            if(verboseFlag)
                printf("Setting thread %d to core %d\n", i, t_info->core);
            success = pthread_attr_setaffinity_np(t_att, sizeof(cpu_set_t), cpu_set);
            if(success != 0) {
                exit(EXIT_FAILURE);
            }
        }
    }
    /*
    pthread_create(&t_infos[0].thread_id, &t_atts[0], prepare_dimms, &t_infos[0]);
    pthread_join(t_infos[0].thread_id, NULL);
    */
    int result = pthread_barrier_init(&bar, NULL, ac_info->thread_count+1);
    int a = 0;
    for(i = 0; i < ac_info->thread_count; i++){
        pthread_create(&t_infos[i].thread_id, &t_atts[i], ac_info->pthread_function, &t_infos[i]);
    }
	pthread_barrier_wait(&bar);
	gettimeofday(&ac_info->time_begin, NULL);
    for(i = 0; i < ac_info->thread_count; i++){
        pthread_join(t_infos[i].thread_id, NULL);
    }

    gettimeofday(&ac_info->time_end, NULL);
	//pthread_barrier_destroy(&bar);

    free(t_infos);
    free(t_atts);
    free(t_cpus);

    return;
}

void calculate_bandwidths(struct access_info *ac_infos, int operation_count){
    int i;

    for(i = 0; i < operation_count; i++){
        int group_members = 0;
        long earliest_start = 0;
        long latest_end = 0;
        double byte_sum = 0;

        int access_distance = 0;
        int thread_count = 0;
        int j;
        for(j = 0; j < operation_count; j++){
            if(ac_infos[j].measure_group == i){
                group_members++;

                //TODO: Is it a good idea to use different access_distances in a single measure_group?
                access_distance = ac_infos[j].access_distance;
                thread_count += ac_infos[j].thread_count;

                long start = (long)ac_infos[j].time_begin.tv_sec * 1000 + (long)ac_infos[j].time_begin.tv_usec / 1000;
                if(earliest_start == 0){
                    earliest_start = start;
                }
                earliest_start = MIN(earliest_start, start);

                long end = (long)ac_infos[j].time_end.tv_sec * 1000 + (long)ac_infos[j].time_end.tv_usec / 1000;
                if(latest_end == 0){
                    latest_end = end;
                }
                latest_end = MAX(latest_end, end);

                byte_sum += ac_infos[j].pmem_size;
                //double rate = (ac_infos[j].pmem_size / (end - start)) / 1000.0f;
                //printf("%d,%d,%d,%ld,%.3f\n", i, ac_infos[j].thread_count, access_distance,  end - start, rate);
            }
        }
        if(group_members > 0){
                double rate = (byte_sum / (latest_end - earliest_start)) / 1000.0f;
                printf("%d,%d,%d,%ld,%.3f\n", i, thread_count, access_distance,  latest_end - earliest_start, rate);
        }
    }
}