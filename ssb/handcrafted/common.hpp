#pragma once

#include <filesystem>
#include "ex_finger.h"
#include "allocator.h"
#include "libpmem.h"

#define SOCKET_1_PATH "/mnt/pmem1/ssb/"
#define SOCKET_2_PATH "/mnt/pmem2/ssb/"

#define LO_TAB_PATH   "lineorder_sf100_full_random.tbl"
#define LO_TAB_A_PATH "lineorder_sf100_a.tbl"
#define LO_TAB_B_PATH "lineorder_sf100_b.tbl"
#define DAT_TAB_PATH  "date_sf100.tbl"
#define SUP_TAB_PATH  "supplier_sf100.tbl"
#define PART_TAB_PATH "part_sf100.tbl"
#define CUST_TAB_PATH "customer_sf100.tbl"

#define LO_TAB_PATH_SOCKET_1   SOCKET_1_PATH LO_TAB_A_PATH
#define DAT_TAB_PATH_SOCKET_1  SOCKET_1_PATH DAT_TAB_PATH
#define SUP_TAB_PATH_SOCKET_1  SOCKET_1_PATH SUP_TAB_PATH
#define PART_TAB_PATH_SOCKET_1 SOCKET_1_PATH PART_TAB_PATH
#define CUST_TAB_PATH_SOCKET_1 SOCKET_1_PATH CUST_TAB_PATH

#define LO_TAB_PATH_SOCKET_2   SOCKET_2_PATH LO_TAB_B_PATH
#define DAT_TAB_PATH_SOCKET_2  SOCKET_2_PATH DAT_TAB_PATH
#define SUP_TAB_PATH_SOCKET_2  SOCKET_2_PATH SUP_TAB_PATH
#define PART_TAB_PATH_SOCKET_2 SOCKET_2_PATH PART_TAB_PATH
#define CUST_TAB_PATH_SOCKET_2 SOCKET_2_PATH CUST_TAB_PATH

#define LO_TAB_PATH_FULL SOCKET_1_PATH LO_TAB_PATH

#ifdef DEBUG
#define DEBUG_LOG(msg) (std::cout << msg << std::endl)
#else
#define DEBUG_LOG(msg) do {} while(0)
#endif

constexpr size_t ONE_GB = 1024ul * 1024 * 1024;

using HashTableT = extendible::Finger_EH<uint64_t>;

void init_table(const char *path, HashTableT** ht) {
    if (std::filesystem::exists(path) && Allocator::Get() == nullptr) {
        std::cout << "Trying to remove existing tmp files at " << path << std::endl;
        if (!std::filesystem::remove(path)) {
            std::cerr << "Failed to remove old files at " << path << std::endl;
            exit(1);
        }
    }

    if (!std::filesystem::exists(path)) {
        DEBUG_LOG("init hash tables at " << path);
        int sds_write_value = 0;
        pmemobj_ctl_set(NULL, "sds.at_create", &sds_write_value);
        Allocator::Initialize(path, 10 * ONE_GB);
        Allocator::GetRoot(sizeof(HashTableT));
    }

    const size_t segment_number = 64;
    PMEMoid p_hash_table = OID_NULL;
    auto dummy_constructor = [](PMEMobjpool* pool, void* ptr, void* arg) { return 0; };
    Allocator::Allocate(&p_hash_table, kCacheLineSize, sizeof(HashTableT), dummy_constructor, nullptr);
    *ht = reinterpret_cast<HashTableT*>(pmemobj_direct(p_hash_table));
    new (*ht) HashTableT(segment_number, Allocator::Get()->pm_pool_);
}