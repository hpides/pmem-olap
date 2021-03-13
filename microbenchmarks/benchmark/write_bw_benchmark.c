#define _GNU_SOURCE
#include "write_bw_benchmark.h"
#include "bw_benchmark.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpmem.h>

#include <sys/sysinfo.h>
#include <pthread.h>

#include "common.h"
#include "unrolled_instructions.h"

void write_nt_64(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        WRITE_NT_64_ASM
		OPT_FENCE
        :
        : [addr] "r" (memaddr), [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt_128(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        WRITE_NT_128_ASM
		OPT_FENCE
        :
        : [addr] "r" (memaddr), [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt_256(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        WRITE_NT_256_ASM
		OPT_FENCE
        :
        : [addr] "r" (memaddr), [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt_512(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        WRITE_NT_512_ASM
		OPT_FENCE
        :
        : [addr] "r" (memaddr), [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt_1024(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        WRITE_NT_1024_ASM
		OPT_FENCE
        :
        : [addr] "r" (memaddr), [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt_2048(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        WRITE_NT_2048_ASM
		OPT_FENCE
        :
        : [addr] "r" (memaddr), [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt_4096(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        WRITE_NT_4096_ASM
		OPT_FENCE
        :
        : [addr] "r" (memaddr), [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt_8192(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        WRITE_NT_8192_ASM
		OPT_FENCE
        :
        : [addr] "r" (memaddr), [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt_16384(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        WRITE_NT_8192_ASM
		"add $8192, %[addr]\n"
		WRITE_NT_8192_ASM
		OPT_FENCE
        : [addr] "+r" (memaddr)
        : [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt_32768(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        EXEC_4_TIMES(
        WRITE_NT_8192_ASM
        "add $8192, %[addr]\n"
        )
		OPT_FENCE
        : [addr] "+r" (memaddr)
        :  [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt_65536(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        "vmovntdqa  (%[data]), %%zmm0 \n"
        EXEC_8_TIMES(
        WRITE_NT_8192_ASM
        "add $8192, %[addr]\n"
        )
		OPT_FENCE
        : [addr] "+r" (memaddr)
        :  [data] "r" (memdata)
        : "%zmm0"
        );
    }
}

void write_nt(struct thread_info* t_info){
	pthread_barrier_wait(t_info->bar);

    switch (t_info->access_distance) {
        case 64:
            write_nt_64(t_info);
            return;
        case 128:
            write_nt_128(t_info);
            return;
        case 256:
            write_nt_256(t_info);
            return;
        case 512:
            write_nt_512(t_info);
            return;
        case 1024:
            write_nt_1024(t_info);
            return;
        case 2048:
            write_nt_2048(t_info);
            return;
        case 4096:
            write_nt_4096(t_info);
            return;
        case 8192:
            write_nt_8192(t_info);
            return;
        case 16384:
            write_nt_16384(t_info);
            return;
        case 32768:
            write_nt_32768(t_info);
            return;
        case 65536:
            write_nt_65536(t_info);
            return;
    };
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);

    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
	uint64_t loops = t_info->access_distance / 131072;
	uint64_t count = 0;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;

        for (uint64_t i = 0; i < loops; i++) {
            // write 512 Bit / 64 Byte
			if (memaddr+131072 > mai.pmem_end)
				break;
			asm volatile(
            "vmovntdqa  (%[data]), %%zmm0 \n"
            EXEC_16_TIMES(
            WRITE_NT_8192_ASM
            "add $8192, %[addr]\n"
            )
            : [addr] "+r" (memaddr)
            : [data] "r" (memdata)
            : "%zmm0"
            );
			memaddr+=131072;
			count++;
        }
		asm volatile(
			OPT_FENCE
		: : :
		);
    }
}

void write_nt_stride(struct thread_info* t_info){
    char *pmem_start = t_info->pmem_start + (4096 * t_info->thread_number);
    char *pmem_end = t_info->pmem_end;

    const char memdata[64] __attribute__((aligned(64))) = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    if(t_info->access_distance > 4096) {
        printf("Stride not allowed for size bigger than 4096\n");
        return;
    }
    int loops = 4096 / t_info->access_distance;
    char *t;
    int i = 0;
    for(i = 0; i < loops; i++) {
        for (t = pmem_start + 256 * i; t < pmem_end; t += (t_info->thread_count * 4096)) {
            char *memaddr;
            for (memaddr = t; memaddr < t + t_info->access_distance && memaddr < pmem_end - 64; memaddr += 64) {
                // write 512 Bit / 64 Byte
                asm volatile(
                "vmovntdqa  (%[data]), %%zmm0 \n"
                "vmovntdq %%zmm0, (%[addr]) \n"
                :
                : [addr] "r" (memaddr), [data] "r" (memdata)
                : "%zmm0"
                );

            }
            asm volatile("sfence \n");
        }
    }
}

// random taken from https://github.com/lemire/testingRNG/blob/master/source/lehmer64.h
static inline uint64_t splitmix64_stateless(uint64_t index) {
  uint64_t z = (index + UINT64_C(0x9E3779B97F4A7C15));
  z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
  z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
  return z ^ (z >> 31);
}

static inline __uint128_t lehmer_seed(uint64_t seed) {
	return (((__uint128_t)splitmix64_stateless(seed)) << 64) +
                     splitmix64_stateless(seed + 1);
}
static inline __uint128_t lehmer_random(__uint128_t state) {
	return state *= UINT64_C(0xda942042e4dd58b5);
}

#define LEHMER_RANDOM(x) x * UINT64_C(0xda942042e4dd58b5);

uint64_t get_access_mask(uint64_t ad) {
	uint64_t mask = 0;
	uint64_t fs = DEVDAX_FILE_SIZE;

	while (fs >>= 1) {
		mask <<= 1;
		mask |= 1;
	}
	int bit_count = 0;
	uint64_t rand_alignment = ad;
	if (!alignedFlag) {
		rand_alignment = 64;
	}
	while(rand_alignment >>= 1) {
		bit_count++;
	}
	mask >>= bit_count;
	mask <<= bit_count;
	return mask;
}




void write_nt_random_64(struct thread_info* t_info){
	const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=1) {
		//EXEC_64_TIMES(
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_64_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
		//)
	}
}

void write_nt_random_64_old(struct thread_info* t_info){

    long long rand_seed = time_seed + rand();
    unsigned long long access_mask = 0xFFFFFFFFFFFFFFC0ULL;
	const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    unsigned long long max_offset = (t_info->pmem_end - t_info->pmem_start) - t_info->access_distance;
    long long bytes_to_write = (t_info->pmem_end - t_info->pmem_start) / t_info->thread_count;
    while(bytes_to_write >= 0)    {
        unsigned long long rand_val = 0;
        GET_NEXT_RANDOM_VALUE
        unsigned long long offset = ((rand_val % max_offset)) & access_mask ;
        offset = (offset / t_info->access_distance) * t_info->access_distance;
        char* t = t_info->pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
            WRITE_NT_64_ASM
            :
            : [addr] "r" (t), [data] "r" (memdata)
            : "%zmm0"
        );

        bytes_to_write -= t_info->access_distance;
    }
}


void write_nt_random_128(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_128_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
	}
}

void write_nt_random_256(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_256_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
	}
}

void write_nt_random_512(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_512_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
	}
}

void write_nt_random_1024(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_1024_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
	}
}

void write_nt_random_2048(struct thread_info* t_info){
   const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_2048_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
	}
}

void write_nt_random_4096(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_4096_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
	}
}

void write_nt_random_8192(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_8192_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
	}
}

void write_nt_random_16384(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
	}
}

void write_nt_random_32768(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
	}
}


void write_nt_random_65536(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
	char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;
        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			OPT_FENCE
            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
	}
}


void write_nt_random(struct thread_info* t_info){
	pthread_barrier_wait(t_info->bar);

    switch (t_info->access_distance) {
        case 64:
            write_nt_random_64(t_info);
            return;
        case 128:
            write_nt_random_128(t_info);
            return;
        case 256:
            write_nt_random_256(t_info);
            return;
        case 512:
            write_nt_random_512(t_info);
            return;
        case 1024:
            write_nt_random_1024(t_info);
            return;
        case 2048:
            write_nt_random_2048(t_info);
            return;
        case 4096:
            write_nt_random_4096(t_info);
            return;
        case 8192:
            write_nt_random_8192(t_info);
            return;
        case 16384:
            write_nt_random_16384(t_info);
            return;
        case 32768:
            write_nt_random_32768(t_info);
            return;
        case 65536:
            write_nt_random_65536(t_info);
            return;
    };
    const char memdata[64] __attribute__((aligned(64))) = "!!HansGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *mem_addr;
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	uint64_t mem_size = t_info->pmem_size;
	char *pmem_start = t_info->pmem_start;
	uint64_t offset;
	uint64_t mask = get_access_mask(ad);

	__uint128_t rstate = lehmer_seed(rand());

	for (uint64_t i = 0; i < t_info->accesses_per_thread; i++) {
		uint64_t loops_per_access = ad / 65536;
		offset = rstate >> 64;
		offset = (offset & mask);
		rstate = LEHMER_RANDOM(rstate);
		mem_addr = pmem_start + offset;

		for(int i = 0; i < loops_per_access; i++) {

        asm volatile(
			"vmovntdqa  (%[data]), %%zmm0 \n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM
			"add $8192, %[addr]\n"
			WRITE_NT_8192_ASM

            : [addr] "+r" (mem_addr)
            : [data] "r" (memdata)
            : "%zmm0"
        );
		asm volatile(OPT_FENCE : : :);

		}

	}
}


void write_clwb_random(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "RANDOMHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
    long long rand_seed = time_seed + t_info->thread_number; //this should be randomized enough
    unsigned long long access_mask =  ((1ULL << 63ULL)-1ULL) ^ 63ULL; //align to 64 bit;
    unsigned long long max_offset = t_info->pmem_end - t_info->pmem_start;
    long long bytes_to_write = (t_info->pmem_end - t_info->pmem_start) / t_info->thread_count;
    while(bytes_to_write >= 0)    {
        unsigned long long rand_val = 0;
        asm volatile(
            "movabs $0xd800000000000000, %%rcx \n"	// rcx: bitmask used in LFSR NOT for the result
            "xor %%r8, %%r8 \n"						// r8: access offset
            "xor %%r11, %%r11 \n"
            "mov    %[random], %%r9 \n"
            "mov    %%r9, %%r12 \n"
            "shr    %%r9 \n"
            "and    $0x1, %%r12d \n"        //sets r12 to one or zero (not only r12d)
            "neg    %%r12 \n"               //sets r12 either to 0 (if zero) or is filled with ones
            "and    %%rcx, %%r12 \n"        //apply mask only if last bit is one
            "xor    %%r9, %%r12 \n"
            "mov    %%r12, %[random] \n"
            "mov    %%r12, %%r8 \n"
            //"and    %[accessmask], %%r8 \n" // not needed atm, because
            "mov    %%r8, %[rand_val] \n"
            :[random] "=r" (rand_seed), [rand_val] "=r" (rand_val)
            : "0"(rand_seed), [accessmask] "r" (access_mask)
            : "%rcx", "%r12", "%r11", "%r10", "%r9", "%r8"
        );
        unsigned long long offset = ((rand_val % max_offset)) & access_mask ;//^ 1LL;
        char* t = t_info->pmem_start + offset;
        char* memaddr;
        for (memaddr = t; memaddr < t + t_info->access_distance && memaddr < t_info->pmem_end - 64; memaddr += 64) {
            // Read 128 Bit / 16 Byte
            asm volatile(
            "vmovntdqa  (%[data]), %%zmm0 \n"
            "vmovdqa64 %%zmm0, (%[addr]) \n"
            "clwb (%%rsi) \n"
            :
            : [addr] "r" (memaddr), [data] "r" (memdata)
            : "%zmm0"
            );

        }

        asm volatile("sfence \n");

        bytes_to_write -= t_info->access_distance;
    }
}
void write_clwb(struct thread_info* t_info){
    char *pmem_start = t_info->pmem_start + (t_info->access_distance * t_info->thread_number);
    char *pmem_end = t_info->pmem_end;

    const char memdata[64] __attribute__((aligned(64))) = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = pmem_start; t < pmem_end; t += (t_info->thread_count * t_info->access_distance)) {
        char *memaddr;
        for (memaddr = t; memaddr < t + t_info->access_distance && memaddr < pmem_end - 64; memaddr += 64) {
            // Read 128 Bit / 16 Byte
            asm volatile(
            "vmovntdqa  (%[data]), %%zmm0 \n"
            "vmovdqa64 %%zmm0, (%[addr]) \n"
            "clwb (%[addr]) \n"
            :
            : [addr] "r" (memaddr), [data] "r" (memdata)
            : "%zmm0"
            );

        }
        asm volatile("sfence \n");
    }
}

void write_store_random(struct thread_info* t_info){
    const char memdata[64] __attribute__((aligned(64))) = "RANDOMHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
    long long rand_seed = time_seed + t_info->thread_number; //this should be randomized enough
    unsigned long long access_mask =  ((1ULL << 63ULL)-1ULL) ^ 63ULL; //align to 64 bit;
    unsigned long long max_offset = t_info->pmem_end - t_info->pmem_start;
    long long bytes_to_write = (t_info->pmem_end - t_info->pmem_start) / t_info->thread_count;
    while(bytes_to_write >= 0)    {
        unsigned long long rand_val = 0;
        asm volatile(
            "movabs $0xd800000000000000, %%rcx \n"	// rcx: bitmask used in LFSR NOT for the result
            "xor %%r8, %%r8 \n"						// r8: access offset
            "xor %%r11, %%r11 \n"
            "mov    %[random], %%r9 \n"
            "mov    %%r9, %%r12 \n"
            "shr    %%r9 \n"
            "and    $0x1, %%r12d \n"        //sets r12 to one or zero (not only r12d)
            "neg    %%r12 \n"               //sets r12 either to 0 (if zero) or is filled with ones
            "and    %%rcx, %%r12 \n"        //apply mask only if last bit is one
            "xor    %%r9, %%r12 \n"
            "mov    %%r12, %[random] \n"
            "mov    %%r12, %%r8 \n"
            //"and    %[accessmask], %%r8 \n" // not needed atm, because
            "mov    %%r8, %[rand_val] \n"
            :[random] "=r" (rand_seed), [rand_val] "=r" (rand_val)
            : "0"(rand_seed), [accessmask] "r" (access_mask)
            : "%rcx", "%r12", "%r11", "%r10", "%r9", "%r8"
        );
        unsigned long long offset = ((rand_val % max_offset)) & access_mask ;//^ 1LL;
        char* t = t_info->pmem_start + offset;
        char *memaddr;
        for (memaddr = t; memaddr < t + t_info->access_distance && memaddr < t_info->pmem_end - 64; memaddr += 64) {
            // Read 128 Bit / 16 Byte
            asm volatile(
            "vmovntdqa  (%[data]), %%zmm0 \n"
            "vmovdqa64 %%zmm0, (%[addr]) \n"
            :
            : [addr] "r" (memaddr), [data] "r" (memdata)
            : "%zmm0"
            );
        }
        asm volatile("sfence \n");
        bytes_to_write -= t_info->access_distance;
    }
}

void write_store(struct thread_info* t_info){
    char *pmem_start = t_info->pmem_start + (t_info->access_distance * t_info->thread_number);
    char *pmem_end = t_info->pmem_end;

    const char memdata[64] __attribute__((aligned(64))) = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

    char *t;
    for (t = pmem_start; t < pmem_end; t += (t_info->thread_count * t_info->access_distance)) {
        char *memaddr;
        for (memaddr = t; memaddr < t + t_info->access_distance && memaddr < pmem_end - 64; memaddr += 64) {
            // Read 128 Bit / 16 Byte
            asm volatile(
            "vmovntdqa  (%[data]), %%zmm0 \n"
            "vmovdqa64 %%zmm0, (%[addr]) \n"
            :
            : [addr] "r" (memaddr), [data] "r" (memdata)
            : "%zmm0"
            );
        }
        asm volatile("sfence \n");
    }
}