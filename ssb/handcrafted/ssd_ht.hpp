#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "numa.h"

#include "common.hpp"
#include "ex_finger.h"

#define POOL_SIZE (10737418240 / 16)
#define INTERMEDIATE_BUFFER_SIZE (1024ULL*1024ULL*1024ULL*4ULL)

#define SSD_HASH_PATH "/dev/shm/ssd_ssb_hash.tmp"
#define SSD_PATH "/mnt/nvme2/ssb/"

#define DAT_TAB_PATH_SSD  SSD_PATH DAT_TAB_PATH
#define SUP_TAB_PATH_SSD  SSD_PATH SUP_TAB_PATH
#define PART_TAB_PATH_SSD SSD_PATH PART_TAB_PATH
#define CUST_TAB_PATH_SSD SSD_PATH CUST_TAB_PATH

#define LO_TAB_PATH_SSD_FULL SSD_PATH LO_TAB_PATH_FULL
#define LO_TAB_A_PATH_SSD    SSD_PATH LO_TAB_A_PATH
#define LO_TAB_B_PATH_SSD    SSD_PATH LO_TAB_B_PATH

extern int MULTISOCKET;

void ssd_clean_up() {
    std::filesystem::remove(SSD_HASH_PATH);
}

class Abstract_HT {
    public:

    void init_date_table(int socket) {
        init_table(SSD_HASH_PATH, &ht);
    }
    void init_part_table(int socket) {
        init_table(SSD_HASH_PATH, &ht);
    }
    void init_customer_table(int socket) {
        init_table(SSD_HASH_PATH, &ht);
    }
    void init_supplier_table(int socket) {
        init_table(SSD_HASH_PATH, &ht);
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
        p_open(DAT_TAB_PATH_SSD);
    }
    void open_part_table (int socket) {
        p_open(PART_TAB_PATH_SSD);
    }
    void open_supplier_table (int socket) {
        p_open(SUP_TAB_PATH_SSD);
    }
    void open_customer_table (int socket) {
        p_open(CUST_TAB_PATH_SSD);
    }

    void open_lineorder_table (int socket) {
        if (socket == 1) {
            if (MULTISOCKET) {
                p_open(LO_TAB_A_PATH_SSD);
            } else {
                p_open(LO_TAB_PATH_SSD_FULL);
            }
        } else if (socket == 2) {
            p_open(LO_TAB_B_PATH_SSD);
        }
    }

    void create_intermediate_buffer (int socket) {
        p_create();
    }

    private:
    void p_open(char const *path) {
        int fd = open(path, O_RDONLY);
        struct stat fd_stat;
        fstat(fd, &fd_stat);
        off_t size = fd_stat.st_size;

        mem_start = (char *)mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mem_start == nullptr || mem_start == MAP_FAILED) {
            printf("could not mmap ssd file %s\n", path);
            exit(1);
        }
        mem_end = mem_start + size;
        mem_size = size;
        close(fd);
    }

    void p_create() {
        mem_start = (char *) malloc(INTERMEDIATE_BUFFER_SIZE);
        mlock(mem_start, INTERMEDIATE_BUFFER_SIZE);
        mem_end = mem_start + INTERMEDIATE_BUFFER_SIZE;
        mem_size = INTERMEDIATE_BUFFER_SIZE;

	}

};