#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include "parsing.h"
#include "common.h"
#include "read_bw_benchmark.h"
#include "write_bw_benchmark.h"

int parse_input(int argc, char** argv, int* operation_count, struct access_info* ac_infos, int operation_limit){

    int operation_index = -1;
	char read_pattern_buf[128] = {0};
	alignedFlag=false;

    /*
        Parsing loop for options
        Syntax:
        nvm_db_benchmark --read [pointerinc, memcpy, nt] [--threads THREADCOUNT] [--ad ACCESSDISTANCE]
        nvm_db_benchmark --write [] [--threads THREADCOUNT] [--ad ACCESSDISTANCE]
    */
    while(1){
        struct option long_options[] ={
            {"read", required_argument, 0, 0},
            {"write", required_argument, 0, 1},
            {"threads", required_argument, 0, 2},
            {"ad", required_argument, 0, 3},
            {"mapping_file", required_argument, 0, 4},
            {"package", required_argument, 0, 5},
            {"verbose", no_argument, 0, 6},
            {"random", no_argument, 0, 7},
            {"measure_group", required_argument, 0, 8},
            {"dram", no_argument, 0, 9},
            {"numa", required_argument, 0, 10},
            {"reuse_random", no_argument, 0, 11},
            {"access_pattern", required_argument, 0, 12},
            {"align", no_argument, 0, 13},
            {0,0,0,0}
        };
        /*
            long_index is set to the index of the long_option called;
            can be useful if you want to use the same return value for two different options
        */
        int long_index = 0;
        int index = getopt_long(argc, argv, "", long_options, &long_index);
        if(index == '?'){
            printf("Error: Invalid option or missing parameters.\n");
            exit(EXIT_FAILURE);
        }
        if(index == -1)
            break;
		struct access_info *ac_info = NULL;
            if(operation_index >=0){
                ac_info = &ac_infos[operation_index];
            }
        switch(index){

            /* read option parsing, nontemporal read is standard */
        case 0:
            operation_index++;
            if(operation_index >= operation_limit){
                printf("Warning: Exceeding limit of operations. Following operations are ignored.\n");
                return 0;
            }
            ac_info = &ac_infos[operation_index];
            ac_info->operation_mode = READ_MODE;

            ac_info->operation_name = malloc(128);
            ac_info->measure_group = operation_index;
            ac_info->pinned = false;
            strcpy(ac_info->operation_name, optarg);
            break;
            /* write option parsing */
        case 1:
            operation_index++;
            if(operation_index >= operation_limit){
                printf("Warning: Exceeding limit of operations. Following operations are ignored.\n");
                return 0;
            }
            ac_info = &ac_infos[operation_index];
            ac_info->operation_mode = WRITE_MODE;
            ac_info->operation_name = malloc(128);
            ac_info->measure_group = operation_index;
            ac_info->pinned = false;
            strcpy(ac_info->operation_name, optarg);
            break;
            /* multithreading parsing */
        case 2:
            ac_info->thread_count = atoi(optarg);
            break;
        case 3:
            /* interleavedistance parsing */
            ac_info->access_distance = atoi(optarg);
            break;
        case 4:
            ac_info->mapping_path = malloc(128);
            strcpy(ac_info->mapping_path, optarg);
            ac_info->pinned = true;
            break;
        case 5:
            ac_info->pmem_path = malloc(128);
            strcpy(ac_info->pmem_path, optarg);
            break;
        case 6:
            verboseFlag = true;
            break;
        case 7:
            ac_info->random = true;
            break;
        case 8:
            ac_info->measure_group = atoi(optarg);
            break;
        case 9:
            ac_info->dram_mode = true;
            break;
        case 10:
            ac_info->numa = malloc(128);
            strcpy(ac_info->numa, optarg);
            break;
		case 11:
			ac_info->reuse_random = true;
			break;
		case 12:
			strcpy(read_pattern_buf, optarg);
			if (strcmp("log", read_pattern_buf) == 0){
				ac_info->access_pattern = ACCESS_LOGFILE;
			} else if (strcmp("disjoint", read_pattern_buf) == 0) {
				ac_info->access_pattern = ACCESS_DISJOINT;
			} else {
				printf("Warning: Could not read access pattern");
				ac_info->access_pattern = ACCESS_LOGFILE;
			}
            break;
		case 13:
			alignedFlag = true;
			break;
        default:
            break;
        }
    }
    *operation_count = operation_index + 1;
}

void set_thread_function(struct access_info *ac_info){
    char *operation_name = ac_info->operation_name;
    if(ac_info->operation_mode == READ_MODE){
        if(strcmp("nt", operation_name) == 0) {
            if(ac_info->random){
                ac_info->pthread_function = read_nt_random;
            } else {
                ac_info->pthread_function = read_nt;
            }
        } else if(strcmp("mov", operation_name) == 0) {
            ac_info->pthread_function = read_mov;
        }  else if(strcmp("simd", operation_name) == 0) {
            ac_info->pthread_function = read_simd;
        }
    } else if (ac_info->operation_mode == WRITE_MODE) {
        if (strcmp("nt", operation_name) == 0){
            if(ac_info->random){
                ac_info->pthread_function = write_nt_random;
            } else {
                ac_info->pthread_function = write_nt;
            }
        } else if(strcmp("clwb", operation_name) == 0) {
            if(ac_info->random){
                ac_info->pthread_function = write_clwb_random;
            } else {
                ac_info->pthread_function = write_clwb;
            }
        } else if(strcmp("store", operation_name) == 0) {
            if(ac_info->random){
                ac_info->pthread_function = write_store_random;
            } else {
                ac_info->pthread_function = write_store;
            }
        } else if(strcmp("stride", operation_name) == 0) {
            ac_info->pthread_function = write_nt_stride;
        }
    }
}
