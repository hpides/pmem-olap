#define _GNU_SOURCE
#include "read_bw_benchmark.h"
#include "bw_benchmark.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpmem.h>

#include <sys/sysinfo.h>
#include <pthread.h>
#include <sys/time.h>

#include "common.h"
#include "unrolled_instructions.h"

#define ROUND_ROBIN 1
#define ACCESS_LOGFILE 1
#define ACCESS_DISJUNCT 1
#define ALIGNMENT 4096



void read_nt_random_64(struct thread_info* t_info){
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_64_ASM
			READ_NEXT_ADDR
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}

void read_nt_random_128(struct thread_info* t_info){
	uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_128_ASM
			READ_NEXT_ADDR
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}


void read_nt_random_256(struct thread_info* t_info){
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_256_ASM
			READ_NEXT_ADDR
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}

void read_nt_random_512(struct thread_info* t_info){
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_512_ASM
			READ_NEXT_ADDR
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}

void read_nt_random_1024(struct thread_info* t_info){
	uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_1024_ASM
			READ_NEXT_ADDR
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}

void read_nt_random_2048(struct thread_info* t_info){
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_2048_ASM
			READ_NEXT_ADDR
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}

void read_nt_random_4096(struct thread_info* t_info){
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_4096_ASM
			READ_NEXT_ADDR
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}

void read_nt_random_8192(struct thread_info* t_info){
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_8192_ASM
			READ_NEXT_ADDR
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}

void read_nt_random_16384(struct thread_info* t_info){
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_8192_ASM
			READ_NEXT_ADDR
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}

void read_nt_random_32768(struct thread_info* t_info){
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_8192_ASM
			READ_NEXT_ADDR
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}


void read_nt_random_65536(struct thread_info* t_info){
    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		UNROLL_OPT(
        asm volatile(
			READ_NT_8192_ASM
			READ_NEXT_ADDR
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
			READ_ADDR_ADD_8192
			READ_NT_8192_ASM
            : [next_addr] "=r" (next_addr), [addr] "+r" (mem_addr)
            :
            : "%zmm0", "%xmm0"
        );
		mem_addr = next_addr;
		)
	}
}

void read_nt_random(struct thread_info* t_info){
	pthread_barrier_wait(t_info->bar);

    switch (t_info->access_distance) {
        case 64:
            read_nt_random_64(t_info);
            return;
        case 128:
            read_nt_random_128(t_info);
            return;
        case 256:
            read_nt_random_256(t_info);
            return;
        case 512:
            read_nt_random_512(t_info);
            return;
        case 1024:
            read_nt_random_1024(t_info);
            return;
        case 2048:
            read_nt_random_2048(t_info);
            return;
        case 4096:
            read_nt_random_4096(t_info);
            return;
        case 8192:
            read_nt_random_8192(t_info);
            return;
        case 16384:
            read_nt_random_16384(t_info);
            return;
        case 32768:
            read_nt_random_32768(t_info);
            return;
        case 65536:
            read_nt_random_65536(t_info);
            return;
    };

    uint64_t ac = t_info->pmem_size / t_info->access_distance;
	uint64_t ad = t_info->access_distance;
	char *start_addr = t_info->thread_start_addr;
	char *mem_addr = start_addr;
	for (uint64_t i = 0; i < t_info->accesses_per_thread; i+=ACCESSES_PER_LOOP) {
		char *next_addr;
		uint64_t loops_per_access = ad / 65536;
		UNROLL_OPT(
		asm volatile(
			READ_NT_8192_ASM
			READ_NEXT_ADDR
			READ_NT_8192_ASM
			READ_NT_8192_ASM
			READ_NT_8192_ASM
			READ_NT_8192_ASM
			READ_NT_8192_ASM
			READ_NT_8192_ASM
			READ_NT_8192_ASM
			: [next_addr] "=r" (next_addr)
			: [addr] "r" (mem_addr)
			: "%zmm0", "%xmm0"
		);
		mem_addr = next_addr;
		for(int i = 1; i < loops_per_access; i++) {
			asm volatile(
				READ_NT_8192_ASM
				READ_NT_8192_ASM
				READ_NT_8192_ASM
				READ_NT_8192_ASM
				READ_NT_8192_ASM
				READ_NT_8192_ASM
				READ_NT_8192_ASM
				READ_NT_8192_ASM
				: [next_addr] "=r" (next_addr)
				: [addr] "r" (mem_addr)
				: "%zmm0", "%xmm0"
			);
		}
		)
	}
}


void read_nt_64(struct thread_info* t_info){
    /* Make sure we touch each memory address at least once */
	int a = 0;
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_64_ASM
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
    }
}


void read_nt_128(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_128_ASM
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
    }
}

void read_nt_256(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_256_ASM
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
    }
}


void read_nt_512(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_512_ASM
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
    }
}

void read_nt_1024(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_1024_ASM
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
    }
}

void read_nt_2048(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_2048_ASM
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
    }
}

void read_nt_4096(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_4096_ASM
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
    }
}

void read_nt_8192(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_8192_ASM
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
	}
}

void read_nt_16384(struct thread_info* t_info){
    /* Make sure we touch each memory address at least once */
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_8192_ASM
        "add $8192, %[addr]\n"
        READ_NT_8192_ASM
		"add $8192, %[addr]\n"
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
    }
}

void read_nt_32768(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_8192_ASM
        "add $8192, %[addr]\n"
		READ_NT_8192_ASM
        "add $8192, %[addr]\n"
		READ_NT_8192_ASM
        "add $8192, %[addr]\n"
		READ_NT_8192_ASM
        "add $8192, %[addr]\n"
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
    }
}

void read_nt_65536(struct thread_info* t_info){
    struct min_access_info mai;
    set_access_pattern(&mai, t_info);
    char *t;
    for (t = mai.pmem_start; t < mai.pmem_end && t + t_info->access_distance <= mai.pmem_end; t += mai.stride) {
        char *memaddr = t;
        asm volatile(
        READ_NT_8192_ASM
        "add $8192, %[addr]\n"
		READ_NT_8192_ASM
        "add $8192, %[addr]\n"
		READ_NT_8192_ASM
        "add $8192, %[addr]\n"
		READ_NT_8192_ASM
        "add $8192, %[addr]\n"
		READ_NT_8192_ASM
        "add $8192, %[addr]\n"
		READ_NT_8192_ASM
        "add $8192, %[addr]\n"
		READ_NT_8192_ASM
        "add $8192, %[addr]\n"
		READ_NT_8192_ASM
        "add $8192, %[addr]\n"
        : [addr] "+r" (memaddr)
        :
        : "%zmm0"
        );
    }
}




void read_nt(struct thread_info* t_info){
	pthread_barrier_wait(t_info->bar);
    switch (t_info->access_distance) {
        case 64:
            read_nt_64(t_info);
            return;
        case 128:
            read_nt_128(t_info);
            return;
        case 256:
            read_nt_256(t_info);
            return;
        case 512:
            read_nt_512(t_info);
            return;
        case 1024:
            read_nt_1024(t_info);
            return;
        case 2048:
            read_nt_2048(t_info);
            return;
        case 4096:
            read_nt_4096(t_info);
            return;
        case 8192:
            read_nt_8192(t_info);
            return;
        case 16384:
            read_nt_16384(t_info);
            return;
        case 32768:
            read_nt_32768(t_info);
            return;
        case 65536:
            read_nt_65536(t_info);
            return;
    };

    /* Make sure we touch each memory address at least once */
    char *pmem_start = t_info->pmem_start + (t_info->access_distance * t_info->thread_number);
    char *pmem_end = t_info->pmem_end;

    char *t;
    for (t = pmem_start; t < pmem_end; t += (t_info->thread_count * t_info->access_distance)) {
        char *memaddr = t;
        for (memaddr = t; memaddr < t + t_info->access_distance && memaddr < pmem_end - 8192; memaddr += 8192) {
            asm volatile(
            READ_NT_8192_ASM
            :
            : [addr] "r" (memaddr)
            : "%zmm0"
            );
        }
        //asm volatile("mfence \n");

    }
}





// void read_nt_asm(struct thread_info* t_info){
//     /* Make sure we touch each memory address at least once */
//     char *pmem_start = t_info->pmem_start + (t_info->access_distance * t_info->thread_number);
//     char *pmem_end = t_info->pmem_end;
//     size_t stride = t_info->thread_count * t_info->access_distance;
//     char * t;
//     asm volatile(
//         "OUTERLOOP1:\n\t"
//         "INNERLOOP2:\n\t"
//         "add %[stride] %[t]\n\t"
//         //"cmp %[t] %[pmem_end]\n\t"
//         //"jb OUTERLOOP1"
//         :
//         : [t] "=r" (t), [pmem_end] "=r" (pmem_end), [stride] "r" (stride)
//         : "%zmm0"
//     );
//     /*char *t;
//     for (t = pmem_start; t < pmem_end; t += (t_info->thread_count * t_info->access_distance)) {
//         char *memaddr;
//         for (memaddr = t; memaddr < t + t_info->access_distance && memaddr < pmem_end - 64; memaddr += 64) {
//             // Read 512 Bit / 64 Byte
//             asm volatile(
//             "vmovntdqa (%[addr]), %%zmm0 \n"
//             :
//             : [addr] "r" (memaddr)
//             : "%zmm0"
//             );
//         }
//         asm volatile("mfence \n");

//     }*/

// }

void read_simd(struct thread_info* t_info){
    /* Make sure we touch each memory address at least once */
    char *pmem_start = t_info->pmem_start + (t_info->access_distance * t_info->thread_number);
    char *pmem_end = t_info->pmem_end;

    char *t;
    for (t = pmem_start; t < pmem_end; t += (t_info->thread_count * t_info->access_distance)) {
        char *memaddr;
        for (memaddr = t; memaddr < t + t_info->access_distance && memaddr < pmem_end - 64; memaddr += 64) {
            // Read 512 Bit / 64 Byte
            asm volatile(
            "mov       %[addr],     %%rsi  \n"
            "vmovdqa 0*32(%%rsi), %%ymm0 \n"
            "vmovdqa 1*32(%%rsi), %%ymm1 \n"
            :
            : [addr] "r" (memaddr)
            );
        }
        //asm volatile("mfence \n");

    }

}

void read_mov(struct thread_info* t_info){
    /* Make sure we touch each memory address at least once */
    char *pmem_start = t_info->pmem_start + (t_info->access_distance * t_info->thread_number);
    char *pmem_end = t_info->pmem_end;

    char *buf = malloc(t_info->access_distance);

    char *t;
    for (t = pmem_start; t < pmem_end; t += (t_info->thread_count * t_info->access_distance)) {
        char *memaddr;
        memcpy(buf, t, t_info->access_distance);
        /*for (memaddr = t; memaddr < t + t_info->access_distance && memaddr < pmem_end - 64; memaddr += 64) {
            // Read 512 Bit / 64 Byte
            asm volatile(
            "mov       %[addr],     %%rsi  \n"
            "mov    0*8(%%rsi), %%r8 \n"
            "mov    1*8(%%rsi), %%r8 \n"
            "mov    2*8(%%rsi), %%r8 \n"
            "mov    3*8(%%rsi), %%r8 \n"
            "mov    4*8(%%rsi), %%r8 \n"
            "mov    5*8(%%rsi), %%r8 \n"
            "mov    6*8(%%rsi), %%r8 \n"
            "mov    7*8(%%rsi), %%r8 \n"
            :
            : [addr] "r" (memaddr)
            );
        }
        asm volatile("mfence \n");*/
    }
}

void read_memcpy(struct thread_info* t_info){
    /* Make sure we touch each memory address at least once */

    char *pmem_start = t_info->pmem_start + t_info->access_distance * t_info->thread_number;
    char *pmem_end = t_info->pmem_end;
    char *data_chunk = malloc(t_info->access_distance);
    char *t;
    for (t = pmem_start; t < pmem_end; t += t_info->thread_count * t_info->access_distance) {
        memcpy(data_chunk, t, MIN(t_info->access_distance, pmem_end - t));
    }
    free(data_chunk);
}

void read_pointerinc(struct thread_info* t_info){
    char * t;
    for (t = t_info->pmem_start; t < t_info->pmem_end; t++) {
        asm(""); // A no-op which does not get optimized out
    }
}