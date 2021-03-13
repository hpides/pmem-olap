#include <libpmem.h>
#include <libpmemobj.h>
#include <stdlib.h>
#include "numa.h"
#include <filesystem>

#define POOL_SIZE (10737418240 / 16)
#define INTERMEDIATE_BUFFER_SIZE (1024ULL*1024ULL*1024ULL*4ULL)

#define DRAM_PATH "/dev/shm/"
#define DRAM_HASH_PATH DRAM_PATH "ssb_hash.tmp"

#include "common.hpp"
#include "ex_finger.h"

extern int MULTISOCKET;

void dram_clean_up() {
    std::filesystem::remove(DRAM_HASH_PATH);
}

class Abstract_HT {
    using HashTableT = extendible::Finger_EH<uint64_t>;

    public:

    void init_date_table(int socket) {
        init_table(DRAM_HASH_PATH, &ht);
    }
    void init_part_table(int socket) {
        init_table(DRAM_HASH_PATH, &ht);
    }
    void init_customer_table(int socket) {
        init_table(DRAM_HASH_PATH, &ht);
    }
    void init_supplier_table(int socket) {
        init_table(DRAM_HASH_PATH, &ht);
    }

    inline void insert(uint64_t key, char *tuple){
        ht->Insert(key, tuple);
    }

    inline char *get_tuple(uint64_t key) {
        return (char *)ht->Get(key);
    }
    private:
        HashTableT *ht;
};

class Mem_Block{
public:
    char *mem_start;
    char *mem_end;
    uint64_t mem_size;

    void open_date_table (int socket) {
        if (socket == 1) {
            p_open(DAT_TAB_PATH_SOCKET_1);
        } else if (socket == 2) {
            p_open(DAT_TAB_PATH_SOCKET_2);
        }
    }
    void open_part_table (int socket) {
        if (socket == 1) {
            p_open(PART_TAB_PATH_SOCKET_1);
        } else if (socket == 2) {
            p_open(PART_TAB_PATH_SOCKET_2);
        }
    }
    void open_supplier_table (int socket) {
        if (socket == 1) {
            p_open(SUP_TAB_PATH_SOCKET_1);
        } else if (socket == 2) {
            p_open(SUP_TAB_PATH_SOCKET_2);
        }
    }
    void open_customer_table (int socket) {
        if (socket == 1) {
            p_open(CUST_TAB_PATH_SOCKET_1);
        } else if (socket == 2) {
            p_open(CUST_TAB_PATH_SOCKET_2);
        }
    }
    void open_lineorder_table (int socket) {
        if (socket == 1) {
            if (MULTISOCKET) {
                p_open(LO_TAB_PATH_SOCKET_1);
            } else {
                p_open(LO_TAB_PATH_FULL);
            }
        } else if (socket == 2) {
            p_open(LO_TAB_PATH_SOCKET_2);
        }
    }

    void create_intermediate_buffer (int socket) {
        p_create();
    }

    private:
    void p_open(char const *path) {
        int is_pmem;
        size_t actual_size;
        char* pmem_addr;
        /* Create a pmem file and memory map it */
        pmem_addr = (char *)pmem_map_file(path, 0, 0, 0666, &actual_size, &is_pmem);
        if (pmem_addr == NULL) {
            printf("%s\n", path);
            perror("Openinig pmem_file failed");
            exit(EXIT_FAILURE);
        }
        if (!actual_size) {
        perror("Actually mapped size is incorrect.");
            exit(EXIT_FAILURE);
        }

        mem_start = (char *)malloc(actual_size);
        mlock(mem_start, actual_size);
        memcpy(mem_start, pmem_addr, actual_size);

        //mlock(mem_start, actual_size);

        pmem_unmap(pmem_addr, actual_size);
        mem_end = mem_start + actual_size;
        mem_size = actual_size;
    }

    void p_create() {
        mem_start = (char *) malloc(INTERMEDIATE_BUFFER_SIZE);
        mlock(mem_start, INTERMEDIATE_BUFFER_SIZE);
        mem_end = mem_start + INTERMEDIATE_BUFFER_SIZE;
        mem_size = INTERMEDIATE_BUFFER_SIZE;

	}

};