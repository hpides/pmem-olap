#include "pmem_mapping.h"
#include "common.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <malloc.h>
#include <numa.h>
#include <time.h>

#ifndef LIB_PMEM2
void map_operation_pmem(struct access_info *ac_info){
    int is_pmem;
    size_t actual_size;
    char* pmem_addr;
    /* Create a pmem file and memory map it */
    if ((pmem_addr = (char *)pmem_map_file(ac_info->pmem_path, 0, 0,
                                  0666, &actual_size, &is_pmem)) == NULL) {
        perror("Mapping the pmem file failed.");
        exit(EXIT_FAILURE);
    }

	if (strncmp(ac_info->pmem_path, "/dev/", 5) == 0)
        actual_size = DEVDAX_FILE_SIZE;
    /* Here we become the real fuckers */

    // char* pmem_addr2;

    // size_t size2;
    // if ((pmem_addr2 = (char *)pmem_map_file("/dev/dax0.0", 0, 0 ,
    //                               0666, &size2, &is_pmem)) == NULL) {
    //     perror("Mapping the pmem file failed.");
    //     exit(EXIT_FAILURE);
    // }
    //
    //memcpy(pmem_addr2, pmem_addr, actual_size);
    //pmem_addr = pmem_addr2;
    //printf("%.*s\n", 15, pmem_addr);
    /* Boil out if no pmem file */
    if (!is_pmem) {
        pmem_unmap(pmem_addr, actual_size);
        perror("Not a pmem file");
        exit(EXIT_FAILURE);
    }

    /* Something went wrong if the specified mapped size and the actual mapped size differ*/
    if (!actual_size) {
        perror("Actually mapped size is incorrect.");
        exit(EXIT_FAILURE);
    }

    ac_info->pmem_size = actual_size;
    ac_info->pmem_start = pmem_addr;
    ac_info->pmem_end = pmem_addr + actual_size;
    return;
}
#endif
#ifdef LIB_PMEM2
map_operation_pmem(struct access_info *ac_info)
{
    int fd;
    int success = 0;
    ac_info->pmem_path;
    bool new_file = false;
    if ((fd = open(ac_info->pmem_path, O_RDWR)) < 0)
    {
        perror("Open PMEM file");
        return -1;
    }
    if (pmem2_config_new(&ac_info->cfg))
    {
        pmem2_perror("pmem2_config_new");
        close(fd);
        return -1;
    }
    // we need to make sure that the mapped memory is aligned

    if (pmem2_source_from_fd(&ac_info->src, fd))
    {
        pmem2_perror("pmem2_source_from_fd");
        close(fd);
        return -1;
    }

    size_t alignment = 0;
    if (pmem2_source_alignment(ac_info->src, &alignment) != 0)
    {
        pmem2_perror("pmemF2_source_from_alignment");
        close(fd);
        return -1;
    }
    // size_t min_size = sizeof(T) * elements;
    // size_t num_pages = ((min_size - 1)  / alignment) + 1;
    // size_t size_in_bytes = num_pages * alignment;
    // size_t min_size_for_mapping = std::max((size_t)4096, size_in_bytes);
    size_t file_size = 0;
    pmem2_source_size(ac_info->src, &file_size);
    /*
    *  For devdax, we do not want to benchmark the whole file, but only 40GiB atm.
    */
    if (strncmp(ac_info->pmem_path, "/dev/", 5) == 0)
        file_size = DEVDAX_FILE_SIZE;
    ac_info->pmem_size = file_size;

    pmem2_config_set_length(ac_info->cfg, ac_info->pmem_size);

    if (pmem2_config_set_required_store_granularity(ac_info->cfg, PMEM2_GRANULARITY_CACHE_LINE))
    {
        pmem2_perror("pmem2_config_set_required_store_granularity");
        close(fd);
        return -1;
    }

    if (pmem2_map_new(&ac_info->map, ac_info->cfg, ac_info->src))
    {
        pmem2_perror("pmem2_map");
        close(fd);
        return -1;
    }
    ac_info->pmem_start = (char *)(pmem2_map_get_address(ac_info->map));
    ac_info->pmem_end = (char *)(ac_info->pmem_start + ac_info->pmem_size);
    return 0;
}
#endif

void map_operation_dram(struct access_info *ac_info) {
    if (verboseFlag) {
        printf("Trying to map DRAM...\n");
    }
	long long file_size;
	char *dram_address;

	static char* dram1 = NULL;
	static char* dram2 = NULL;
	bool is_dram1 = false;
	bool is_dram2 = false;

	if (strncmp(ac_info->pmem_path, "/dev/dax", 8) == 0) {
        file_size = DEVDAX_FILE_SIZE;
		if(strncmp(ac_info->pmem_path, "/dev/dax0", 9) == 0) {
			struct bitmask *bm = numa_parse_nodestring("0,1");
        	numa_set_membind(bm);
			is_dram1 = true;
		} else if(strncmp(ac_info->pmem_path, "/dev/dax1", 9) == 0) {
			struct bitmask *bm = numa_parse_nodestring("2,3");
        	numa_set_membind(bm);
			is_dram2 = true;
		} else {
			printf("Unknown data path: %s!\n", ac_info->pmem_path);
			exit(1);
		}

		if (is_dram1 && dram1 != NULL) {
			dram_address = dram1;
		} else if (is_dram2 && dram2 != NULL) {
			dram_address = dram2;
		} else {
			dram_address = mmap(0, file_size, PROT_READ|PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_LOCKED, 0, 0);
			if (dram_address == NULL || dram_address == MAP_FAILED) {
				exit(EXIT_FAILURE);
			}
			mlock(dram_address, file_size);
			//fill with garbage, if necessary
			memset(dram_address, 'a', file_size);

			if (is_dram1) dram1 = dram_address;
			if (is_dram2) dram2 = dram_address;
		}
	} else {
		int fd = open(ac_info->pmem_path, O_RDWR);
		if (fd < 0) {
			printf("Error: Could not open DRAM path. Exiting.\n");
			exit(EXIT_FAILURE);
		}
		struct stat file_status;
		if (fstat(fd, &file_status) != 0) {
			printf("Error: Could not determine the size of the file. Exiting.\n");
			close(fd);
			exit(EXIT_FAILURE);
		}
		file_size = file_status.st_size;
		dram_address = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
		if(mlock(dram_address, file_size) != 0){
			perror("mlock failure");
			exit(EXIT_FAILURE);
		}
    	close(fd);
		if (dram_address == MAP_FAILED) {
			printf("Error: Could not map shm-file into address space. Exiting.\n");
			perror("Reason: ");
			exit(EXIT_FAILURE);
   		}

	}
    ac_info->pmem_start = dram_address;
    ac_info->pmem_end = dram_address + file_size;
    ac_info->pmem_size = file_size;
}

// Taken from: https://stackoverflow.com/questions/33010010/how-to-generate-random-64-bit-unsigned-integer-in-c
#define IMAX_BITS(m) ((m)/((m)%255+1) / 255%255*8 + 7-86/((m)%255+12))
#define RAND_MAX_WIDTH IMAX_BITS(RAND_MAX)
_Static_assert((RAND_MAX & (RAND_MAX + 1u)) == 0, "RAND_MAX not a Mersenne number");

uint64_t rand64(void) {
  uint64_t r = 0;
  for (int i = 0; i < 64; i += RAND_MAX_WIDTH) {
    r <<= RAND_MAX_WIDTH;
    r ^= (unsigned) rand();
  }
  return r;
}

struct list_node {
	struct list_node *next;
	struct list_node *prev;
	char *jump_address;
};

struct list_node *linked_list_create(uint64_t size) {
	struct list_node *list = (struct list_node *) malloc(size * sizeof(struct list_node));
	for (uint64_t i = 1; i < size - 1; i++) {
		list[i].next = &list[i+1];
		list[i].prev = &list[i-1];
	}
	list[0].next = &list[1];
	list[0].prev = &list[size - 1];
	list[size - 1].next = list;
	list[size - 1].prev = &list[size-2];
	return list;
}

void linked_list_init_jumps(struct access_info *ac_info, struct list_node *list) {
	for (uint64_t i = 0; i < ac_info->access_count; i++) {
		list->jump_address = (ac_info->pmem_start + (ac_info->access_distance * (i + 1)));
		list = list->next;
	}
	//finish jump from last node:
	list->prev->jump_address = ac_info->pmem_start;
}

void linked_list_swap_nodes(struct list_node *a, struct list_node *b) {
	//catch edge cases
	if (a == b)
		return;
	//b shall be always right of a
	if (b->next == a) {
		struct list_node *tmp = a;
		a = b;
		b = tmp;
	}
	//special: b follows a directly
	if (a->next == b) {
		a->prev->next = b;
		b->next->prev = a;

		a->next = b->next;
		b->prev = a->prev;

		b->next = a;
		a->prev = b;
		return;
	}
	//update neighbours
	a->next->prev = b;
	a->prev->next = b;
	b->next->prev = a;
	b->prev->next = a;

	//update nodes
	struct list_node *tmp_prev, *tmp_next;
	tmp_prev = a->prev;
	tmp_next = a->next;
	a->prev = b->prev;
	a->next = b->next;
	b->prev = tmp_prev;
	b->next = tmp_next;
}

struct list_node *linked_list_next_nth(struct list_node *node, uint64_t n) {
	for(uint64_t i = 0; i < n; i++) {
		node = node->next;
	}
	return node;
}

bool linked_list_is_ring(struct list_node *list, uint64_t size) {
	struct list_node *current_node = list;
	for(uint64_t i = 0; i < size - 1; i++) {
		current_node = current_node->next;
		if (current_node == list)
			return false;
	}
	current_node = current_node->next;
	if (current_node != list)
		return false;
	return true;
}

bool memory_is_ring(char *start_address, uint64_t jumps) {
	char *current_address = start_address;
	for(uint64_t i = 0; i < jumps - 1; i++) {
		memcpy(&current_address, current_address, sizeof(char *));
		if (current_address == start_address)
			return false;
	}
	memcpy(&current_address, current_address, sizeof(char *));
	if (current_address != start_address)
		return false;
	return true;
}

void set_thread_start_addresses(struct access_info *ac_info) {
	if (!ac_info->random)
		return;
	uint64_t accesses_per_thread = ac_info->accesses_per_thread;
	char *current_address = ac_info->pmem_start;

	if (ac_info->operation_mode == READ_MODE) {
		for (int i = 0; i < ac_info->thread_count; i++) {
			ac_info->thread_start_addr[i] = current_address;
			for (uint64_t i = 0; i < accesses_per_thread; i++) {
				memcpy(&current_address, current_address, sizeof(char *));
			}
		}
	}
}

void shuffle(uint64_t *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          uint64_t t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

//fill every 64bytes with an 64byte offset inside of the file
void fill_with_random_offsets(struct access_info *ac_info) {
	srand(time(NULL));

	if(ac_info->operation_mode == WRITE_MODE) {
		return;
	}
	if (verboseFlag) {
		printf("Starting to randomize read data...\n");
		#ifdef DO_NOT_VERIFY_RANDOM
		printf("Warning: Macro DO_NOT_VERIFY_RANDOM disables consistency checks, which can lead to errors.\n");
		#endif
	}
	char *pmem_start = ac_info->pmem_start;
	char *pmem_end = ac_info->pmem_end;
	uint64_t ac = ac_info->access_count;
	// if(ac_info->dram_mode) {
	// 	ac = ac / 4;
	// }
	uint64_t ad = ac_info->access_distance;

	//create list with jump addresses
	struct list_node *list = linked_list_create(ac);
	linked_list_init_jumps(ac_info, list);

	//shuffle list, but keep its ring structure
	for (uint64_t i = 0; i < ac; i++) {
		uint64_t swap_node_pos = rand64() % ac;
		struct list_node *a = &list[i];
		struct list_node *b = &list[swap_node_pos];

		struct list_node a_bak = *a;
		struct list_node b_bak = *b;
		struct list_node *pa = a;
		struct list_node *pb = b;

		linked_list_swap_nodes(a,b);
	}
	#ifndef DO_NOT_VERIFY_RANDOM
	if (!linked_list_is_ring(list, ac)) {
		printf("Random addresses were not generated properly. Exiting.\n");
		free(list);
		exit(EXIT_FAILURE);
	}
	#endif
	struct list_node *current_node = list;
	if (verboseFlag) {
		printf("Random data generated, but has to be written to memory\n");
	}

	//fill pmem with jump address and garbage
		for (uint64_t i = 0; i < ac; i++) {
			char *current_node_address = current_node->prev->jump_address;
			memcpy(current_node_address, &current_node->jump_address, sizeof(char *));
			memset(current_node_address + sizeof(char *), 'a', ad-sizeof(char *));
			current_node = current_node->next;
		}

	#ifndef DO_NOT_VERIFY_RANDOM
	if (!memory_is_ring(pmem_start, ac)) {
		printf("Jump addresses were not set properly. Exiting.\n");
		free(list);
		exit(EXIT_FAILURE);
	}
	#endif

	free(list);

	if (verboseFlag)
		printf("Finished to randomize read data...\n");
}

void map_operation(struct access_info *ac_info) {
    if(ac_info->numa != NULL) {
        struct bitmask *bm = numa_parse_nodestring(ac_info->numa);
        numa_set_membind(bm);
    }
    if (ac_info->dram_mode) {
        map_operation_dram(ac_info);
    } else {
        map_operation_pmem(ac_info);
    }
	if(ac_info->numa != NULL) {
        struct bitmask *bm = numa_parse_nodestring(ac_info->numa);
        numa_set_membind(bm);
    }
	ac_info->access_count = ac_info->pmem_size / ac_info->access_distance;
	ac_info->accesses_per_thread = ac_info->access_count / ac_info->thread_count;
	if (ac_info->random && !ac_info->reuse_random) {
		fill_with_random_offsets(ac_info);
	} else if (ac_info->random && ac_info->reuse_random) {
		if (verboseFlag) {
			printf("Reusing old random. Warning: You NEED to have the same access distance as before\n");
		}
	}
	set_thread_start_addresses(ac_info);
}

void unmap_operation_pmem(struct access_info *ac_info){
    pmem_unmap(ac_info->pmem_start, ac_info->pmem_size);
}

void unmap_operation_dram(struct access_info *ac_info){
    munmap(ac_info->pmem_start, ac_info->pmem_size);
}

void unmap_operation(struct access_info *ac_info) {
    if (ac_info->dram_mode) {
        unmap_operation_dram(ac_info);
    } else {
        unmap_operation_pmem(ac_info);
    }
}