#ifndef NVM_DB_BENCHMARK_INCLUDE_PARSING_H
#define NVM_DB_BENCHMARK_INCLUDE_PARSING_H

#include "common.h"

int parse_input(int argc, char** argv, int* operation_count, struct access_info* p_ac_infos, int operation_limit);

void set_thread_function(struct access_info *ac_info);

#endif