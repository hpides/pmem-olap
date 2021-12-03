#include <sys/types.h>
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <unistd.h>
#include <string>
#include <libpmem.h>
#include <libpmemobj.h>

#include "common.hpp"
#include "ex_finger.h"

#define INTERMEDIATE_BUFFER_SIZE (1024ULL*1024ULL*1024ULL*1ULL)

#define INTERMEDIATE_BUF_PATH_SOCKET_1 SOCKET_1_PATH "/buffers/"
#define INTERMEDIATE_BUF_PATH_SOCKET_2 SOCKET_2_PATH "/buffers/"

#define PMEM_HASH_SOCKET_1 SOCKET_1_PATH "ssb_hash.tmp"
#define PMEM_HASH_SOCKET_2 SOCKET_2_PATH "ssb_hash.tmp"

extern int MULTISOCKET;

void pmem_clean_up() {
    std::filesystem::remove_all(INTERMEDIATE_BUF_PATH_SOCKET_1);
    std::filesystem::remove_all(INTERMEDIATE_BUF_PATH_SOCKET_2);
    std::filesystem::remove(PMEM_HASH_SOCKET_1);
    std::filesystem::remove(PMEM_HASH_SOCKET_2);
}

class Abstract_HT {
public:
    void init_date_table(int socket) {
        if(socket == 1) {
            init_table(PMEM_HASH_SOCKET_1, &ht);
        } else if (socket == 2) {
            init_table(PMEM_HASH_SOCKET_2, &ht);
        }
    }
    void init_part_table(int socket) {
        if(socket == 1) {
            init_table(PMEM_HASH_SOCKET_1, &ht);
        } else if (socket == 2) {
            init_table(PMEM_HASH_SOCKET_2, &ht);
        }
    }
    void init_customer_table(int socket) {
        if(socket == 1) {
            init_table(PMEM_HASH_SOCKET_1, &ht);
        } else if (socket == 2) {
            init_table(PMEM_HASH_SOCKET_2, &ht);
        }
    }
    void init_supplier_table(int socket) {
        if(socket == 1) {
            init_table(PMEM_HASH_SOCKET_1, &ht);
        } else if (socket == 2) {
            init_table(PMEM_HASH_SOCKET_2, &ht);
        }
    }

    inline void insert(uint64_t key, char *tuple){
        ht->Insert(key, tuple);
    }

    inline char *get_tuple(uint64_t key) {
        return (char *) ht->Get(key);
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
        if (socket == 1) {
            p_create(INTERMEDIATE_BUF_PATH_SOCKET_1);
        } else if (socket == 2) {
            p_create(INTERMEDIATE_BUF_PATH_SOCKET_2);
        }
    }

    void destroy_intermediate_buffer () {
        munmap(mem_start, mem_size);
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
        mem_start = pmem_addr;
        mem_end = pmem_addr + actual_size;
        mem_size = actual_size;
    }

    void p_create(char const *path) {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directory(path);
        }

        //buffers should have a random name
        std::string path_s = path;
        path_s += "buf" + std::to_string(rand() % 1000000);
        path_s += ".tmp";

        int is_pmem;
        size_t actual_size;
        char* pmem_addr;
		pmem_addr = (char *)pmem_map_file(path_s.c_str(), INTERMEDIATE_BUFFER_SIZE, PMEM_FILE_CREATE, S_IRWXU, &actual_size, &is_pmem);
		if (pmem_addr == NULL) {
			perror("Creating pmem_file failed");
        	exit(EXIT_FAILURE);
		}
        if (!actual_size) {
            perror("Actually mapped size is incorrect.");
            exit(EXIT_FAILURE);
        }

        mem_start = pmem_addr;
        mem_end = pmem_addr + actual_size;
        mem_size = actual_size;
	}
};