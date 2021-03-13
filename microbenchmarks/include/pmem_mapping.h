#ifndef NVM_DB_BENCHMARK_INCLUDE_PMEM_MAPPING_H
#define NVM_DB_BENCHMARK_INCLUDE_PMEM_MAPPING_H
#include "common.h"

void map_operation(struct access_info *ac_info);
void unmap_operation(struct access_info *ac_info);

#endif