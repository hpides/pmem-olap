#ifndef NVM_DB_BENCHMARK_WRITE_BW_BENCHMARK_H
#define NVM_DB_BENCHMARK_WRITE_BW_BENCHMARK_H

#include "common.h"

//#define SFENCE_AFTER_STORE 1

#ifdef SFENCE_AFTER_STORE
	#define OPT_FENCE "sfence \n"
#endif
#ifndef SFENCE_AFTER_STORE
	#define OPT_FENCE ""
#endif

void write_nt(struct thread_info* t_info);
void write_nt_random(struct thread_info * t_info);

void write_clwb(struct thread_info* t_info);
void write_clwb_random(struct thread_info* t_info);

void write_store(struct thread_info* t_info);
void write_store_random(struct thread_info* t_info);

void write_nt_stride(struct thread_info* t_info);

#endif