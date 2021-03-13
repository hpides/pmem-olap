#ifndef NVM_DB_BENCHMARK_READ_BW_BENCHMARK_H
#define NVM_DB_BENCHMARK_READ_BW_BENCHMARK_H

#include "common.h"

void read_nt_random(struct thread_info* t_info);
void read_nt(struct thread_info* t_info);
void read_simd(struct thread_info* t_info);
void read_memcpy(struct thread_info* t_info);
void read_pointerinc(struct thread_info* t_info);
void read_mov(struct thread_info* t_info);

#endif