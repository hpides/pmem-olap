#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <string>
#include <sys/time.h>
#include <sys/mman.h>
#include <linux/memfd.h>
#include <numa.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctime>
#include <iostream>

#include <libpmem.h>
#include <libpmemobj.h>

#include "ex_finger.h"

#include "common.hpp"

//#define DRAM_BM
//#define PMEM_BM
//#define SSD_BM

#ifdef DRAM_BM
#include "dram_ht.hpp"
#endif

#ifdef PMEM_BM
#include "pmem_ht.hpp"
#endif

#ifdef SSD_BM
#include "ssd_ht.hpp"
#endif

double avg_run_time{0};
std::atomic<uint64_t> total_run_time{0};
std::atomic<uint64_t> total_scan_time{0};
std::atomic<uint64_t> total_join_time{0};
std::atomic<uint64_t> total_aggregate_time{0};

//socket 2:

#define QF1 1
#define QF2 2
#define QF3 3
#define QF4 4

#define Q1 1
#define Q2 2
#define Q3 3
#define Q4 4

#define PRINT false

struct timeval tv0, tv1, tv2, tv3, tv4;

int ACCESS_SIZE;
int THREAD_COUNT;
int PINNING;
int MULTISOCKET;
int NUMA;
int WORKERS;

#define TUPLE_SIZE 128


#define LO_ORDERKEY_IND 0
#define LO_LINENUMBER_IND 10
#define LO_CUSTKEY_IND 12
#define LO_PARTKEY_IND 20
#define LO_SUPKEY_IND 28
#define LO_ORDERDATE_IND 35
#define LO_ORDERPRIO_IND 44
#define LO_SHIPPRIO_IND 60
#define LO_QUANT_IND 62
#define LO_EXTENDEDPRICE_IND 65
#define LO_ORDERTOTPRICE_IND 74
#define LO_DISC_IND 83
#define LO_REVENUE_IND 86
#define LO_SUPPCOST_IND 95
#define LO_TAX_IND 102
#define LO_COMMITDATE_IND 104
#define LO_SHIPMODE_IND 113

#define PART_KEY_LEN 8
#define PART_KEY_IND 0
#define PART_NAME_IND 9
#define PART_MFGR_IND 32
#define PART_CATEGORY_IND 40
#define PART_BRAND_IND 49
#define PART_COLOR_IND 61
#define PART_TYPE_IND 72
#define PART_SIZE_IND 99
#define PART_CONTAINER_IND 103

#define DAT_DATEKEY_LEN 8
#define DAT_DATEKEY_IND 0
#define DAT_YEAR_IND 48
#define DAT_YEARMONTHNUM_IND 53
#define DAT_YEARMONTH_IND 60
#define DAT_WEEKNUMINYEAR_IND 80

#define SUP_KEY_LEN 6
#define SUP_KEY_IND 0
#define SUP_NAME_IND 8
#define SUP_ADDR_IND 28
#define SUP_CITY_IND 54
#define SUP_NATION_IND 66
#define SUP_REGION_IND 82
#define SUP_PHONE_IND 95

#define CUST_KEY_LEN 7
#define CUST_KEY_IND 0
#define CUST_NAME_IND 9
#define CUST_ADDR_IND 29
#define CUST_CITY_IND 55
#define CUST_NATION_IND 67
#define CUST_REGION_IND 83
#define CUST_PHONE_IND 96

inline int fast_atoi( const char * str )
{
    int val = 0;
    while( *str >= '0' && *str <= '9' ) {
        val = val*10 + (*str++ - '0');
    }
    return val;
}

void set_NUMA(int socket) {
	if (NUMA) {
		if (socket == 1) {
			struct bitmask *bm = numa_parse_nodestring("0,1");

        	numa_run_on_node_mask(bm);
        	numa_set_membind(bm);
			numa_set_interleave_mask(bm);
			bm = numa_get_interleave_mask();

		} else if (socket == 2) {
			struct bitmask *bm = numa_parse_nodestring("2,3");
        	numa_run_on_node_mask(bm);
        	numa_set_membind(bm);
			numa_set_interleave_mask(bm);
			bm = numa_get_interleave_mask();
		}

	}
}

inline uint64_t time_diff_ms(struct timeval *start_time, struct timeval *end_time) {
	return (end_time->tv_sec * 1000) - (start_time->tv_sec * 1000) +
			(end_time->tv_usec / 1000) - (start_time->tv_usec / 1000);
}

inline uint64_t time_diff_us(struct timeval *start_time, struct timeval *end_time) {
	return (end_time->tv_sec * 1e6) - (start_time->tv_sec * 1e6) + end_time->tv_usec - start_time->tv_usec;
}

struct join_tuple {
	char *lineorder_tuple;
	char *part_tuple;
	char *supplier_tuple;
	char *date_tuple;
	char *customer_tuple;
};

struct hash_tables {
	Abstract_HT date_ht;
	Abstract_HT part_ht;
	Abstract_HT supplier_ht;
	Abstract_HT customer_ht;

	char *results;
};

struct mem_blocks {
	Mem_Block date_mem;
	Mem_Block supplier_mem;
	Mem_Block customer_mem;
	Mem_Block part_mem;
	Mem_Block lineorder_mem;
	Mem_Block intermediate_mem;
};


struct thread_info {
	pthread_t thread_id;
	int thread_number;
	int thread_count;
	int core;
	char *memory_start;
	char *memory_end;
	struct hash_tables *hts;
	char *intermediate_buffer_start;
	char *intermediate_buffer_end;

	int query_flight;
	int query;

	uint64_t selected_lines;
	uint64_t stored_tuples;
	uint64_t lo_lines;

	int socket;
};

struct socket_info {
	pthread_t thread_id;
	Mem_Block intermediate_buffer;
	Mem_Block lo_table;
	struct hash_tables hts;
	int socket;
	uint64_t tuples;
	int query_flight;
	int query;
	int worker;
	int worker_count;
	int thread_count;
};

struct worker_info {
	pthread_t thread_id;
	struct socket_info s_info1;
	struct socket_info s_info2;
};

int core_map[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};

int core_map_s1_w1[] = {  0,  1,  2,  5,  6,  9, 10, 14, 15 };
int core_map_s1_w2[] = {  3,  4,  7,  8, 11, 12, 13, 16, 17 };
int core_map_s2_w1[] = { 18, 19, 20, 23, 24, 27, 28, 32, 33 };
int core_map_s2_w2[] = { 21, 22, 25, 26, 29, 30, 31, 34, 35 };

struct qf1_intermediate {
	uint64_t lines;
	uint64_t sum;
};

struct qf2_intermediate {
	char brand[9];
	int year;
	uint64_t revenue;
};

struct qf3_intermediate {
	uint64_t year;
	uint64_t revenue;
	char cust_nation[15];
	char sup_nation[15];
	char cust_city[10];
	char sup_city[10];
};

struct qf4_intermediate {
	uint64_t year;
	uint64_t lo_revenue;
	uint64_t lo_supplycost;
	char cust_nation[15];
	char sup_nation[15];
	char sup_city[10];
	char part_brand[9];
	char part_category[7];
	uint64_t result;
};
inline size_t im_size(int query_flight) {
	switch (query_flight) {
	case QF1:
		return sizeof(struct qf1_intermediate);
	case QF2:
		return sizeof(struct qf2_intermediate);
	case QF3:
		return sizeof(struct qf3_intermediate);
	case QF4:
		return sizeof(struct qf4_intermediate);
	}

	throw std::runtime_error("should never happen!");
	return 0;
}


bool inline lineorder_filter(char *tuple, int query_flight, int query) {
	int discount;
	int quantity;
	switch(query_flight) {

	case QF1:
		switch(query) {

		case Q1:
		discount = fast_atoi(&tuple[LO_DISC_IND]);

			if (!(discount >= 1 && discount <= 3))
				return false;
			quantity = fast_atoi(&tuple[LO_QUANT_IND]);

			if (!(quantity < 25))
				return false;
			break;

		case Q2:
			discount = fast_atoi(&tuple[LO_DISC_IND]);
			if(!(discount >= 4 && discount <= 6))
				return false;
			quantity = fast_atoi(&tuple[LO_QUANT_IND]);
			if (!(quantity >= 26 && quantity <= 35))
				return false;
			break;

		case Q3:
			discount = fast_atoi(&tuple[LO_DISC_IND]);
			if(!(discount >= 5 && discount <= 7))
				return false;
			quantity = fast_atoi(&tuple[LO_QUANT_IND]);
			if (!(quantity >= 36 && quantity <= 40))
				return false;
			break;

		default:
			break;
		};
	break;
	default:
		break;
	};
	return true;
}

char inline *part_retrieve(uint64_t key, Abstract_HT *part_ht, int query_flight, int query) {
	char *part_tuple = part_ht->get_tuple(key);
	if (part_tuple == nullptr) {
		return nullptr;
	}

	char *category;
	int brand;
	switch (query_flight) {

	case QF2:
		switch (query) {

		case Q1:
			if (strncmp(&part_tuple[PART_CATEGORY_IND], "MFGR#12", 7) != 0)
				return nullptr;
			break;

		case Q2:
			brand = fast_atoi(&part_tuple[PART_BRAND_IND+5]);
			if (!(brand >= 2221 && brand <= 2228))
				return nullptr;
			break;

		case Q3:
			brand = fast_atoi(&part_tuple[PART_BRAND_IND+5]);
			if (!(brand == 2339))
				return nullptr;
			break;

		default:
			break;
		};
	break;

	case QF4:
		switch (query) {
		case QF1:
		case QF2:
			if ((strncmp(&part_tuple[PART_MFGR_IND], "MFGR#1", 6) != 0) && (strncmp(&part_tuple[PART_MFGR_IND], "MFGR#2", 6) != 0))
				return nullptr;
			break;
		case QF3:
			if (strncmp(&part_tuple[PART_CATEGORY_IND], "MFGR#14", 7) != 0)
				return nullptr;
			break;

		default:
			break;
		};
	break;
	default:
		break;
	};

	return part_tuple;
}

char inline *supplier_retrieve(uint64_t key, Abstract_HT *supplier_ht, int query_flight, int query) {
	char *supplier_tuple = supplier_ht->get_tuple(key);

	if (supplier_tuple == nullptr) {
		return nullptr;
	}

	char *category;
	switch (query_flight) {
	case QF2:
		switch (query) {

		case Q1:
			if(strncmp(&supplier_tuple[SUP_REGION_IND], "AMERICA", 7) != 0)
					return nullptr;
			break;
		case Q2:
			if(strncmp(&supplier_tuple[SUP_REGION_IND], "ASIA", 4) != 0)
					return nullptr;
			break;
		case Q3:
			if(strncmp(&supplier_tuple[SUP_REGION_IND], "EUROPE", 6) != 0)
					return nullptr;
			break;
		default:
			break;
		};
	break;
	case QF3:
		switch (query) {

		case Q1:
			if(strncmp(&supplier_tuple[SUP_REGION_IND], "ASIA", 4) != 0)
				return nullptr;
			break;

		case Q2:
			if(strncmp(&supplier_tuple[SUP_NATION_IND], "UNITED STATES", 13) != 0)
				return nullptr;
			break;
		case Q3:
		case Q4:
			if((strncmp(&supplier_tuple[SUP_CITY_IND], "UNITED KI1", 10) != 0) && (strncmp(&supplier_tuple[SUP_CITY_IND], "UNITED KI5", 10) != 0))
				return nullptr;
			break;
		default:
			break;
		}
		break;
	case QF4:
		switch (query) {
		case Q1:
		case Q2:
			if(strncmp(&supplier_tuple[SUP_REGION_IND], "AMERICA", 7) != 0)
				return nullptr;
			break;

		case Q3:
			if(strncmp(&supplier_tuple[SUP_NATION_IND], "UNITED STATES", 13) != 0)
				return nullptr;
			break;

		default:
			break;
		}
	break;
	default:
		break;
	};
	return supplier_tuple;
}

char inline *customer_retrieve(uint64_t key, Abstract_HT *customer_ht, int query_flight, int query) {

	char *customer_tuple = customer_ht->get_tuple(key);
	if (customer_tuple == nullptr) {
		return nullptr;
	}
	switch (query_flight) {
	case QF3:
		switch (query) {
		case Q1:
			if(strncmp(&customer_tuple[CUST_REGION_IND], "ASIA", 4) != 0)
				return nullptr;
			break;
		case Q2:
			if(strncmp(&customer_tuple[CUST_NATION_IND], "UNITED STATES", 13) != 0)
				return nullptr;
			break;
		case Q3:
		case Q4:
			if((strncmp(&customer_tuple[CUST_CITY_IND], "UNITED KI1", 10) != 0) && (strncmp(&customer_tuple[CUST_CITY_IND], "UNITED KI5", 10) != 0))
				return nullptr;
			break;
		default:
			break;
		};
	break;
	case QF4:
		if((strncmp(&customer_tuple[CUST_REGION_IND], "AMERICA", 7) != 0))
			return nullptr;
		break;

	default:
		break;
	};
	return customer_tuple;
}

/* date_retrieve:
 * Get a tuple by key from the date_ht and apply
 * filters according to query
*/
char *date_retrieve(uint64_t key, Abstract_HT *date_ht, int query_flight, int query) {
	//Get tuple
	char *dat_tuple = date_ht->get_tuple(key);
	if (dat_tuple == nullptr) {
		return nullptr;
	}
	//QUERY FILTER:
	int year;
	int yearmonthnum;
	int weeknum;
	switch (query_flight) {

	case QF1:
		switch (query) {

		case Q1:
			year = fast_atoi(&dat_tuple[DAT_YEAR_IND]);
			if (!(year == 1993))
				return nullptr;
			break;

		case Q2:
			yearmonthnum = fast_atoi(&dat_tuple[DAT_YEARMONTHNUM_IND]);
			if (!(yearmonthnum == 199401))
				return nullptr;
			break;

		case Q3:
			year = fast_atoi(&dat_tuple[DAT_YEAR_IND]);
			if (!(year == 1994))
				return nullptr;
			weeknum = fast_atoi(&dat_tuple[DAT_WEEKNUMINYEAR_IND]);
			if (!(weeknum == 6))
				return nullptr;
			break;

		default:
			break;
		};
	break;
	case QF3:
		switch (query) {

		case Q1:
		case Q2:
		case Q3:
			year = fast_atoi(&dat_tuple[DAT_YEAR_IND]);
			if (!(year >= 1992 && year <= 1997))
				return nullptr;
			break;
		case Q4:
			if (strncmp(&dat_tuple[DAT_YEARMONTH_IND], "Dec1997", 7) != 0)
				return nullptr;
			break;

		default:
			break;
		}
	break;
	case QF4:
		switch (query) {

		case Q2:
		case Q3:
			year = fast_atoi(&dat_tuple[DAT_YEAR_IND]);
			if (!(year == 1997 || year == 1998))
				return nullptr;
			break;

		default:
			break;
		}
	break;
	default:
		break;
	};
	return dat_tuple;
}

uint64_t inline getKey(char* tuple, size_t offset, size_t length) {
	uint64_t *pKey = (uint64_t *)(tuple + offset);
	uint64_t key = *pKey;
	for(int i = 8; i > length; i--)
		((char *)(&key))[i-1] = 0;
	return key;
}
/* join_and_filter performs all join operations and filters of a query and,
*	if there is a match, sets all relevant pointers in the jointuple.
*	TODO: Determine joinorder by selectivity of queries.
*
*/
bool join_and_filter(char *lineorder_tuple, struct hash_tables *hts, struct join_tuple *jt, int query_flight, int query) {
	char *date_tuple, *supplier_tuple, *part_tuple, *customer_tuple;
	uint64_t date_key, sup_key, part_key, cust_key;
	if(!(lineorder_filter(lineorder_tuple, query_flight, query)))
		return false;

	switch (query_flight) {

	case QF1:
		date_key = getKey(lineorder_tuple, LO_ORDERDATE_IND, DAT_DATEKEY_LEN);
		if(!(date_tuple = date_retrieve(date_key, &hts->date_ht, query_flight, query)))
			return false;
		break;
	break;
	case QF2:
		date_key = getKey(lineorder_tuple, LO_ORDERDATE_IND, DAT_DATEKEY_LEN);
		if(!(date_tuple = date_retrieve(date_key, &hts->date_ht, query_flight, query)))
			return false;
		sup_key = getKey(lineorder_tuple, LO_SUPKEY_IND, SUP_KEY_LEN);
		if(!(supplier_tuple = supplier_retrieve(sup_key, &hts->supplier_ht, query_flight, query)))
			return false;

		part_key = getKey(lineorder_tuple, LO_PARTKEY_IND, PART_KEY_LEN);
		if(!(part_tuple = part_retrieve(part_key, &hts->part_ht, query_flight, query)))
			return false;
		break;
	break;
	case QF3:

		date_key = getKey(lineorder_tuple, LO_ORDERDATE_IND, DAT_DATEKEY_LEN);
		if(!(date_tuple = date_retrieve(date_key, &hts->date_ht, query_flight, query)))
			return false;

		sup_key = getKey(lineorder_tuple, LO_SUPKEY_IND, SUP_KEY_LEN);
		if(!(supplier_tuple = supplier_retrieve(sup_key, &hts->supplier_ht, query_flight, query)))
			return false;

		cust_key = getKey(lineorder_tuple, LO_CUSTKEY_IND, CUST_KEY_LEN);
		if(!(customer_tuple = customer_retrieve(cust_key, &hts->customer_ht, query_flight, query)))
			return false;
		break;
	break;
	case QF4:

		date_key = getKey(lineorder_tuple, LO_ORDERDATE_IND, DAT_DATEKEY_LEN);
		if(!(date_tuple = date_retrieve(date_key, &hts->date_ht, query_flight, query)))
			return false;

		sup_key = getKey(lineorder_tuple, LO_SUPKEY_IND, SUP_KEY_LEN);
		if(!(supplier_tuple = supplier_retrieve(sup_key, &hts->supplier_ht, query_flight, query)))
			return false;

		cust_key = getKey(lineorder_tuple, LO_CUSTKEY_IND, CUST_KEY_LEN);
		if(!(customer_tuple = customer_retrieve(cust_key, &hts->customer_ht, query_flight, query)))
			return false;

		part_key = getKey(lineorder_tuple, LO_PARTKEY_IND, PART_KEY_LEN);
		if(!(part_tuple = part_retrieve(part_key, &hts->part_ht, query_flight, query)))
			return false;
		break;
	break;
	default:
		break;
	}

	jt->lineorder_tuple = lineorder_tuple;
	jt->date_tuple = date_tuple;
	jt->supplier_tuple = supplier_tuple;
	jt->part_tuple = part_tuple;
	jt->customer_tuple = customer_tuple;

	return true;
}

int year_brand_comp(struct qf2_intermediate *payload1, struct qf2_intermediate *payload2) {

	int year_comp, brand_comp;

	if (payload1->year < payload2->year)
		return -1;
	if (payload1->year > payload2->year)
		return 1;
	return strncmp(payload1->brand, payload2->brand, 9);
}

int cnation_snation_year_comp(struct qf3_intermediate *im1, struct qf3_intermediate *im2) {
	int res = 0;
	res = strncmp(im1->cust_nation, im2->cust_nation, 15);
	if(res != 0)
		return res;
	res = strncmp(im1->sup_nation, im2->sup_nation, 10);
	if(res != 0)
		return res;
	return im1->year-im2->year;

}

int ccity_scity_year_comp(struct qf3_intermediate *im1, struct qf3_intermediate *im2) {
	int res = 0;
	res = strncmp(im1->cust_city, im2->cust_city, 10);
	if (res != 0)
		return res;
	res = strncmp(im1->sup_city, im2->sup_city, 10);
	if (res != 0)
		return res;
	return im1->year-im2->year;
}

int yearasc_revdesc_comp(struct qf3_intermediate *im1, struct qf3_intermediate *im2) {
	uint64_t res = im1->year - im2->year;
	if (res != 0)
		return res;
	res = im1->revenue - im2->revenue;
	return -res;
}

int dyear_cnation_comp(struct qf4_intermediate *im1, struct qf4_intermediate *im2) {
	int res = 0;
	res = im1->year - im2->year;
	if(res != 0)
		return res;
	return strncmp(im1->cust_nation, im2->cust_nation, 15);
}

int dyear_snation_pcategory_comp(struct qf4_intermediate *im1, struct qf4_intermediate *im2) {
	int res = 0;
	res = im1->year - im2->year;
	if(res != 0)
		return res;
	res = strncmp(im1->sup_nation, im2->sup_nation, 15);
	if (res != 0)
		return res;
	return strncmp(im1->part_category, im2->part_category, 7);
}

int dyear_scity_pbrand_comp(struct qf4_intermediate *im1, struct qf4_intermediate *im2) {
	int res = 0;
	res = im1->year - im2->year;
	if (res != 0)
		return res;
	res = strncmp(im1->sup_city, im2->sup_city, 10);
	if (res != 0)
		return res;
	return strncmp(im1->part_brand, im2->part_brand, 9);
}

/*
	Groups (tuple_count) many payload tuples at (memory) inplace,
	returning the number of groups
*/

uint64_t group(char *memory_start, size_t tuple_count, int query_flight, int query) {
	uint64_t rep_index;
	uint64_t iterator;
	struct qf2_intermediate *qf2_mem;
	struct qf3_intermediate *qf3_mem;
	struct qf4_intermediate *qf4_mem;
	switch (query_flight) {
	case QF2:
		qf2_mem = (struct qf2_intermediate *) memory_start;
		qsort(qf2_mem, tuple_count, sizeof(struct qf2_intermediate), (__compar_fn_t) year_brand_comp);

		rep_index = 0;
		iterator = 1;
		for (iterator = 1; iterator < tuple_count; iterator++) {
			if (year_brand_comp(&qf2_mem[rep_index], &qf2_mem[iterator]) == 0) {
				qf2_mem[rep_index].revenue += qf2_mem[iterator].revenue;
			} else {
				rep_index++;
				memcpy(&qf2_mem[rep_index], &qf2_mem[iterator], sizeof(struct qf2_intermediate));
			}
		}
		return rep_index + 1;
	case QF3:
		qf3_mem = (struct qf3_intermediate *) memory_start;

		switch (query) {
		case Q1:
			qsort(qf3_mem, tuple_count, sizeof(struct qf3_intermediate), (__compar_fn_t) cnation_snation_year_comp);
			rep_index = 0;
			iterator = 1;
			for (iterator = 1; iterator < tuple_count; iterator++) {
				if (cnation_snation_year_comp(&qf3_mem[rep_index], &qf3_mem[iterator]) == 0) {
					qf3_mem[rep_index].revenue += qf3_mem[iterator].revenue;
				} else {
					rep_index++;
					memcpy(&qf3_mem[rep_index], &qf3_mem[iterator], sizeof(struct qf3_intermediate));
				}
			}
			return rep_index + 1;
			break;
		case Q2:
		case Q3:
		case Q4:
			qsort(qf3_mem, tuple_count, sizeof(struct qf3_intermediate), (__compar_fn_t) ccity_scity_year_comp);
			rep_index = 0;
			iterator = 1;
			for (iterator = 1; iterator < tuple_count; iterator++) {
				if (ccity_scity_year_comp(&qf3_mem[rep_index], &qf3_mem[iterator]) == 0) {
					qf3_mem[rep_index].revenue += qf3_mem[iterator].revenue;
				} else {
					rep_index++;
					memcpy(&qf3_mem[rep_index], &qf3_mem[iterator], sizeof(struct qf3_intermediate));
				}
			}
			return rep_index + 1;
			break;
		break;

		default:
			break;
		}
	case Q4:
		qf4_mem = (struct qf4_intermediate *) memory_start;
		switch (query) {
		case Q1:
			qsort(qf4_mem, tuple_count, sizeof(struct qf4_intermediate), (__compar_fn_t) dyear_cnation_comp);
			rep_index = 0;
			iterator = 1;
			for (iterator = 1; iterator < tuple_count; iterator++) {
				if (dyear_cnation_comp(&qf4_mem[rep_index], &qf4_mem[iterator]) == 0) {
					qf4_mem[rep_index].result += qf4_mem[iterator].result;
				} else {
					rep_index++;
					memcpy(&qf4_mem[rep_index], &qf4_mem[iterator], sizeof(struct qf4_intermediate));
				}
			}
			return rep_index + 1;

		case Q2:
			qsort(qf4_mem, tuple_count, sizeof(struct qf4_intermediate), (__compar_fn_t) dyear_snation_pcategory_comp);
			rep_index = 0;
			iterator = 1;
			for (iterator = 1; iterator < tuple_count; iterator++) {
				if (dyear_snation_pcategory_comp(&qf4_mem[rep_index], &qf4_mem[iterator]) == 0) {
					qf4_mem[rep_index].lo_revenue += qf4_mem[iterator].lo_revenue;
					qf4_mem[rep_index].lo_revenue -= qf4_mem[iterator].lo_supplycost;
				} else {
					rep_index++;
					memcpy(&qf4_mem[rep_index], &qf4_mem[iterator], sizeof(struct qf4_intermediate));
					qf4_mem[rep_index].lo_revenue -= qf4_mem[rep_index].lo_supplycost;
				}
			}
			return rep_index + 1;

		case Q3:
			qsort(qf4_mem, tuple_count, sizeof(struct qf4_intermediate), (__compar_fn_t) dyear_scity_pbrand_comp);
			rep_index = 0;
			iterator = 1;
			for (iterator = 1; iterator < tuple_count; iterator++) {
				if (dyear_scity_pbrand_comp(&qf4_mem[rep_index], &qf4_mem[iterator]) == 0) {
					qf4_mem[rep_index].lo_revenue += qf4_mem[iterator].lo_revenue;
					qf4_mem[rep_index].lo_revenue -= qf4_mem[iterator].lo_supplycost;
				} else {
					rep_index++;
					memcpy(&qf4_mem[rep_index], &qf4_mem[iterator], sizeof(struct qf4_intermediate));
					qf4_mem[rep_index].lo_revenue -= qf4_mem[rep_index].lo_supplycost;
				}
			}
			return rep_index + 1;

		default:
			break;
		};

	default:
		break;
	};

	throw std::runtime_error("should not happen!");
	return 0;
}

uint64_t aggregate(char *mem_start, size_t tuple_count, int query_flight, int query) {
	struct timeval start_time, end_time;
	// gettimeofday(&start_time, NULL);

	struct qf1_intermediate * qf1_im = (struct qf1_intermediate *)mem_start;
	switch (query_flight) {
	case QF1:
		for (int i = 1; i < tuple_count; i++) {
			qf1_im->lines += qf1_im[i].lines;
		}
		return 1;
		break;
	case QF2:
	case QF3:
	case QF4:
		return group(mem_start, tuple_count, query_flight, query);
		break;
	default:
		break;
	}
	// gettimeofday(&end_time, NULL);
	// total_aggregate_time.fetch_add(time_diff_us(&start_time, &end_time));
	return 0;
}

uint64_t collect_socket_intermediates (char *intermediate_buffer, struct thread_info *t_infos, int query_flight) {
	char *mem_it = intermediate_buffer;
	size_t tuple_size = 0;
	tuple_size = im_size(query_flight);

	uint64_t tuple_count = 0;
	for (int i = 0; i < THREAD_COUNT; i++) {
		size_t bufsize = t_infos[i].stored_tuples * tuple_size;
		memcpy(mem_it, t_infos[i].intermediate_buffer_start, bufsize);
		mem_it += bufsize;
		tuple_count++;
	}
	return tuple_count;
}

void order_by(char *aggregate_buf, uint64_t tuple_count, int query_flight) {
	switch (query_flight) {
	case QF2:
		//should be already grouped, do nothing
		break;
	case QF3:
		qsort(aggregate_buf, tuple_count, sizeof(struct qf3_intermediate), (__compar_fn_t) yearasc_revdesc_comp);
		break;
	case QF4:
		//group should do the sorting
		break;
	default:
		break;
	};
}

uint64_t filtered_lines = 0;


void result_print(char *aggregate_buffer, uint64_t tuple_count, int query_flight, int query) {
	struct qf1_intermediate *qf1_im;
	struct qf2_intermediate *qf2_im;
	struct qf3_intermediate *qf3_im;
	struct qf4_intermediate *qf4_im;

	switch (query_flight) {
	case QF1:
		printf("Lines: %lld\n", ((struct qf1_intermediate *)aggregate_buffer)->lines);
		break;
	case QF2:
		for (int i = 0; i < tuple_count; i++) {
			qf2_im = (struct qf2_intermediate *)aggregate_buffer + i;
			printf("%d \t%.*s\t %d\n", qf2_im->year, 9, qf2_im->brand, qf2_im->revenue);
		}
	break;
	case QF3:
		switch (query) {
		case Q1:
			for (int i = 0; i < tuple_count; i++) {
				qf3_im = (struct qf3_intermediate *)aggregate_buffer + i;
				printf("%d \t%.*s\t %.*s\t %lld\n", qf3_im->year, 15, qf3_im->cust_nation, 15, qf3_im->sup_nation, qf3_im->revenue);
			}
			break;
		case Q2:
		case Q3:
		case Q4:
			for (int i = 0; i < tuple_count; i++) {
				qf3_im = (struct qf3_intermediate *)aggregate_buffer + i;
				printf("%d \t%.*s\t %.*s\t %lld\n", qf3_im->year, 10, qf3_im->cust_city, 10, qf3_im->sup_city, qf3_im->revenue);
			}
				break;
		};
	break;
	case QF4:
		switch (query) {
		case Q1:
			for (int i = 0; i < tuple_count; i++) {
				qf4_im = (struct qf4_intermediate *)aggregate_buffer + i;
				printf("%d \t%.*s\t %lld\n", qf4_im->year, 15, qf4_im->cust_nation, qf4_im->result);
			}
			break;
		case Q2:
			for (int i = 0; i < tuple_count; i++) {
				qf4_im = (struct qf4_intermediate *)aggregate_buffer + i;
				printf("%d \t%.*s\t %.*s\t %lld\n", qf4_im->year, 15, qf4_im->sup_nation, 7, qf4_im->part_category, qf4_im->lo_revenue);
			}
			break;

		case Q3:
			for (int i = 0; i < tuple_count; i++) {
				qf4_im = (struct qf4_intermediate *)aggregate_buffer + i;
				printf("%d \t%.*s\t %.*s\t %lld\n", qf4_im->year, 10, qf4_im->sup_city, 9, qf4_im->part_brand, qf4_im->lo_revenue);
			}
			break;

		default:
			break;
		};
	break;
	default:
		break;
	};

	printf("FILTERLINES: %lld\n", filtered_lines);

}

char* store_intermediate(char* memory_it, struct join_tuple *jt, int query_flight) {
	struct qf1_intermediate *qf1_im;
	struct qf2_intermediate *qf2_im;
	struct qf3_intermediate *qf3_im;
	struct qf4_intermediate *qf4_im;
	uint64_t result;

	switch(query_flight) {
	case QF1:
		qf1_im = (struct qf1_intermediate *) memory_it;
		qf1_im->lines = 1;
		result = fast_atoi(&jt->lineorder_tuple[LO_EXTENDEDPRICE_IND]) * fast_atoi(&jt->lineorder_tuple[LO_DISC_IND]);
		qf1_im->sum = result;
		return memory_it + sizeof(struct qf1_intermediate);
	break;
	case QF2:
		qf2_im = (struct qf2_intermediate *) memory_it;
		memcpy(qf2_im->brand, &jt->part_tuple[PART_BRAND_IND], 9);
		qf2_im->year = fast_atoi(&jt->date_tuple[DAT_YEAR_IND]);
		qf2_im->revenue = fast_atoi(&jt->lineorder_tuple[LO_REVENUE_IND]);
		return memory_it + sizeof(struct qf2_intermediate);
		break;
	case QF3:
		qf3_im = (struct qf3_intermediate *) memory_it;
		memcpy(qf3_im->cust_city, &jt->customer_tuple[CUST_CITY_IND], 10);
		memcpy(qf3_im->cust_nation, &jt->customer_tuple[CUST_NATION_IND], 15);
		memcpy(qf3_im->sup_city, &jt->supplier_tuple[SUP_CITY_IND], 10);
		memcpy(qf3_im->sup_nation, &jt->supplier_tuple[SUP_NATION_IND], 15);
		qf3_im->revenue = fast_atoi(&jt->lineorder_tuple[LO_REVENUE_IND]);
		qf3_im->year = fast_atoi(&jt->date_tuple[DAT_YEAR_IND]);
		return memory_it + sizeof(struct qf3_intermediate);
	case QF4:
		qf4_im = (struct qf4_intermediate *) memory_it;
		memcpy(qf4_im->cust_nation, &jt->customer_tuple[CUST_NATION_IND], 15);
		memcpy(qf4_im->part_brand, &jt->part_tuple[PART_BRAND_IND], 9);
		memcpy(qf4_im->part_category, &jt->part_tuple[PART_CATEGORY_IND], 7);
		memcpy(qf4_im->sup_nation, &jt->supplier_tuple[SUP_NATION_IND], 15);
		memcpy(qf4_im->sup_city, &jt->supplier_tuple[SUP_CITY_IND], 10);
		qf4_im->year = fast_atoi(&jt->date_tuple[DAT_YEAR_IND]);
		qf4_im->lo_revenue = fast_atoi(&jt->lineorder_tuple[LO_REVENUE_IND]);
		qf4_im->lo_supplycost = fast_atoi(&jt->lineorder_tuple[LO_SUPPCOST_IND]);
		qf4_im->result = qf4_im->lo_revenue - qf4_im->lo_supplycost;

		return memory_it + sizeof(struct qf4_intermediate);

		break;
	default:
		break;
	};

	throw std::runtime_error("should never happen!");
	return 0;
}


int thread_function(struct thread_info *t_info) {
	struct timeval st, et;
	gettimeofday(&st, NULL);
	const size_t total_memory = t_info->memory_end - t_info->memory_start;
	const size_t total_num_tuples = total_memory / TUPLE_SIZE;
	const size_t num_tuples_per_block = total_num_tuples / t_info->thread_count;
	const size_t memory_size_per_block = num_tuples_per_block * TUPLE_SIZE;

	char *pblock = t_info->memory_start + (t_info->thread_number * memory_size_per_block);
	const char* start_block = pblock;
	char *end_pblock = pblock + memory_size_per_block;
	char dblock[ACCESS_SIZE];
	char *lineorder_tuple;
	struct join_tuple jt;

	char *mem_it = t_info->intermediate_buffer_start;
	uint64_t matching_tuples = 0;

	// struct timeval start_time, end_time;
	// gettimeofday(&start_time, NULL);
	for (pblock; pblock < end_pblock; pblock += ACCESS_SIZE) {
		const long as = ACCESS_SIZE;
		size_t block_size = std::min(as, (end_pblock - pblock));

		for (lineorder_tuple = pblock; lineorder_tuple < pblock + block_size; lineorder_tuple+=TUPLE_SIZE) {
			t_info->lo_lines++;
			if(!join_and_filter(lineorder_tuple, t_info->hts, &jt, t_info->query_flight, t_info->query))
				continue;
			t_info->selected_lines++;
			t_info->stored_tuples++;
			mem_it = store_intermediate(mem_it, &jt, t_info->query_flight);
		}
	}
	// gettimeofday(&end_time, NULL);
	// total_scan_time.fetch_add(time_diff_us(&start_time, &end_time));

	t_info->intermediate_buffer_end = mem_it;
	t_info->stored_tuples = aggregate(t_info->intermediate_buffer_start, t_info->stored_tuples, t_info->query_flight, t_info->query);
	filtered_lines += t_info->selected_lines;

	// gettimeofday(&et, NULL);
	// printf("THREAD %d: %d ms, lines: %d, selected: %d, hts: %p, qf:%d.%d, start_block: %p, mem_size_per_block: %d\n",
	// 	t_info->thread_number, time_diff_ms(&st, &et), t_info->lo_lines, t_info->selected_lines, t_info->hts,
	// 	t_info->query_flight, t_info->query, start_block, memory_size_per_block);

	return 0;
}


void socket_function(struct socket_info *s_info) {
	//prepare huge shared intermediate buffer
	set_NUMA(s_info->socket); //dont worry, does nothing if numa is off;
	s_info->tuples = 0;
	Mem_Block& intermediate_buffer = s_info->intermediate_buffer;
	Mem_Block& lo_table = s_info->lo_table;
	struct hash_tables *hts = &s_info->hts;

	size_t buffer_size_per_thread = intermediate_buffer.mem_size / s_info->thread_count;

    struct thread_info *t_infos = (struct thread_info *)calloc(s_info->thread_count, sizeof(struct thread_info));
    pthread_attr_t *t_atts = (pthread_attr_t *)calloc(s_info->thread_count, sizeof(pthread_attr_t));
    cpu_set_t *t_cpus = (cpu_set_t*)calloc(s_info->thread_count, sizeof(cpu_set_t));

	int *w_coremap;
	if (WORKERS) {
		if (s_info->socket == 1) {
			if(s_info->worker == 0) {
				w_coremap = core_map_s1_w1;
			} else if(s_info->worker == 1) {
				w_coremap = core_map_s1_w2;
			}
		} else if (s_info->socket == 2) {
			if(s_info->worker == 0) {
				w_coremap = core_map_s2_w1;
			} else if(s_info->worker == 1) {
				w_coremap = core_map_s2_w2;
			}
		}
	} else {
		w_coremap = core_map;
	}

	//prepare threads
    for(int i = 0; i < THREAD_COUNT; i++) {
		t_infos[i].thread_number = i;
		t_infos[i].thread_count = s_info->thread_count;
		if (WORKERS) {
			t_infos[i].core = w_coremap[i];
		} else {
			t_infos[i].core = w_coremap[i + ((s_info->socket-1)*18)];
		}
		t_infos[i].socket = s_info->socket;
		t_infos[i].query_flight = s_info->query_flight;
		t_infos[i].query = s_info->query;
    }

	//connect threads to data
	for(int i = 0; i < THREAD_COUNT; i++) {
		t_infos[i].memory_start = lo_table.mem_start;
		t_infos[i].memory_end = lo_table.mem_end;
		t_infos[i].hts = hts;
		t_infos[i].intermediate_buffer_start = intermediate_buffer.mem_start + (buffer_size_per_thread * i);
		t_infos[i].intermediate_buffer_end = intermediate_buffer.mem_start + (buffer_size_per_thread * (i+1));
	}

    int success = 0;
    for(int i = 0; i < THREAD_COUNT; i++){
            struct thread_info *t_info = &t_infos[i];
            pthread_attr_t *t_att = &t_atts[i];
            pthread_attr_init(t_att);

			if (PINNING) {
				cpu_set_t *cpu_set = &t_cpus[i];
				CPU_ZERO(cpu_set);
				CPU_SET(t_info->core, cpu_set);
				success = pthread_attr_setaffinity_np(t_att, sizeof(cpu_set_t), cpu_set);
				if(success != 0) {
					exit(EXIT_FAILURE);
				}
			}
        }

	struct timeval st, et;
	gettimeofday(&st, NULL);

    for(int i = 0; i < THREAD_COUNT; i++){
		int success = 0;
		success = pthread_create(&t_infos[i].thread_id, &t_atts[i], (void *(*)(void *))thread_function, &t_infos[i]);
		if(success == EINVAL) {
			exit(EXIT_FAILURE);
		}
		if(success == EAGAIN) {
			exit(EXIT_FAILURE);
		}
		if(success == EPERM) {
			exit(EXIT_FAILURE);
		}
    }
	for(int i = 0; i < THREAD_COUNT; i++){
        pthread_join(t_infos[i].thread_id, NULL);
    }
	// gettimeofday(&et, NULL);
	// printf("THREAD FNs: %ld ms\n", time_diff_ms(&st, &et));
	uint64_t stored_tuples = 0;
	// gettimeofday(&st, NULL);
	stored_tuples = collect_socket_intermediates(intermediate_buffer.mem_start, t_infos, s_info->query_flight);
	// gettimeofday(&et, NULL);
	// printf("Collect: %ld ms\n", time_diff_ms(&st, &et));
	// gettimeofday(&st, NULL);
	stored_tuples = aggregate(intermediate_buffer.mem_start, stored_tuples, s_info->query_flight, s_info->query);
	// gettimeofday(&et, NULL);
	// printf("Aggregate: %ld ms\n", time_diff_ms(&st, &et));
	intermediate_buffer.mem_end = intermediate_buffer.mem_start + (im_size(s_info->query_flight) * stored_tuples);
	s_info->tuples = stored_tuples;

}
int query_queue_index = 0;
std::vector<std::pair<int, int>> query_queue;
int queries_left;

void get_worker_query(struct worker_info *w_info) {
	//warning: I stupidly rely on the low probability of multiple workers accessing the queue.
	int query_flight = query_queue[query_queue_index].first;
	int query = query_queue[query_queue_index].second;
	query_queue_index++;
	w_info->s_info1.query_flight = query_flight;
	w_info->s_info1.query = query;
	w_info->s_info2.query_flight = query_flight;
	w_info->s_info2.query = query;
	w_info->s_info1.tuples = 0;
	w_info->s_info2.tuples = 0;

}

void worker_function(struct worker_info *w_info) {

	set_NUMA(1);
	cpu_set_t cpus1, cpus2;
	pthread_attr_t tatt1, tatt2;
	pthread_attr_init(&tatt1);
	pthread_attr_init(&tatt2);

	struct timeval start_time, end_time;

	if(PINNING){

			int success = 0;
            CPU_ZERO(&cpus1);
            CPU_SET(0, &cpus1);

            CPU_ZERO(&cpus2);
            CPU_SET(18, &cpus2);
            success = pthread_attr_setaffinity_np(&tatt1, sizeof(cpu_set_t), &cpus1);
            if(success != 0) {
                exit(EXIT_FAILURE);
            }
			success = pthread_attr_setaffinity_np(&tatt1, sizeof(cpu_set_t), &cpus2);
            if(success != 0) {
                exit(EXIT_FAILURE);
            }
    }
	int success = 0;

	while(queries_left > 0) {
		queries_left--;
		get_worker_query(w_info);

		int query_flight = w_info->s_info1.query_flight;
		int query = w_info->s_info1.query;

		gettimeofday(&start_time, NULL);

		success = pthread_create(&w_info->s_info1.thread_id, &tatt1, (void *(*)(void *))socket_function, (void *)&w_info->s_info1);
		if(success != 0) {
			perror("could not create thread");
			exit(EXIT_FAILURE);
		}
		if(MULTISOCKET) {
			success = pthread_create(&w_info->s_info2.thread_id, &tatt2, (void *(*)(void *))socket_function, (void *)&w_info->s_info2);
			if(success != 0) {
				perror("could not create thread");
				exit(EXIT_FAILURE);
			}
		}

		pthread_join(w_info->s_info1.thread_id, NULL);
		if (MULTISOCKET)
			pthread_join(w_info->s_info2.thread_id, NULL);

		uint64_t tuples1, tuples2;
		tuples1 = w_info->s_info1.tuples;
		tuples2 = w_info->s_info2.tuples;
		if (MULTISOCKET) {
			set_NUMA(2);

			tuples1 += tuples2;
			memcpy(w_info->s_info1.intermediate_buffer.mem_end, w_info->s_info2.intermediate_buffer.mem_start, tuples2 * im_size(query_flight));
			set_NUMA(1);
			tuples1 = aggregate(w_info->s_info1.intermediate_buffer.mem_start, tuples1, query_flight, query);
		}

		order_by(w_info->s_info1.intermediate_buffer.mem_start, tuples1, query_flight);
		gettimeofday(&end_time, NULL);
		double worker_time = time_diff_ms(&start_time, &end_time);
		worker_time /= w_info->s_info1.worker_count;
		avg_run_time += worker_time;

		//printf("%d,%d,%d,%d\n", w_info->s_info1.worker, query_flight, query, time_diff_ms(&start_time, &end_time));
		//fflush(stdout);
		if (PRINT) {
			result_print(w_info->s_info1.intermediate_buffer.mem_start, tuples1, query_flight, query);
		}
	}
}


void insert_keys(struct mem_blocks *blocks, struct hash_tables *hts) {
	//#ifndef SSD_BM
	char *tuple = blocks->date_mem.mem_start;
	for (int i = 0; i < blocks->date_mem.mem_size / TUPLE_SIZE; i++) {
		uint64_t  key = getKey(tuple, DAT_DATEKEY_IND, DAT_DATEKEY_LEN);
		hts->date_ht.insert(key, tuple);
		tuple += TUPLE_SIZE;
	}

	tuple = blocks->supplier_mem.mem_start;
	for (int i = 0; i < blocks->supplier_mem.mem_size / TUPLE_SIZE; i++) {
		uint64_t key = getKey(tuple, SUP_KEY_IND, SUP_KEY_LEN);
		hts->supplier_ht.insert(key, tuple);
		tuple += TUPLE_SIZE;
	}

	tuple = blocks->part_mem.mem_start;
	for (int i = 0; i < blocks->part_mem.mem_size / TUPLE_SIZE; i++) {
		uint64_t key = getKey(tuple, PART_KEY_IND, PART_KEY_LEN);
		hts->part_ht.insert(key, tuple);
		tuple += TUPLE_SIZE;
	}

	tuple = blocks->customer_mem.mem_start;
	for (int i = 0; i < blocks->customer_mem.mem_size / TUPLE_SIZE; i++) {
		uint64_t key = getKey(tuple, CUST_KEY_IND, CUST_KEY_LEN);
		hts->customer_ht.insert(key, tuple);
		tuple += TUPLE_SIZE;
	}
}

//call this to reset the intermediate buffers etc.
void init_socket_hashtables(struct socket_info *s_info, int socket) {
	set_NUMA(socket);
	struct mem_blocks blocks;
	struct hash_tables *hts = &s_info->hts;
	hts->date_ht.init_date_table(socket);
	hts->customer_ht.init_customer_table(socket);
	hts->part_ht.init_part_table(socket);
	hts->supplier_ht.init_supplier_table(socket);

	blocks.date_mem.open_date_table(socket);
	blocks.customer_mem.open_customer_table(socket);
	blocks.supplier_mem.open_supplier_table(socket);
	blocks.part_mem.open_part_table(socket);
	//blocks.lineorder_mem.open_lineorder_table(socket);

	insert_keys(&blocks, hts);

	//s_info->lo_table = blocks.lineorder_mem;
	s_info->socket = socket;
	s_info->tuples = 0;
}


void init_hashtables(struct worker_info *w_info) {

	init_socket_hashtables(&w_info->s_info1, 1);
	if (MULTISOCKET) {
		init_socket_hashtables(&w_info->s_info2, 2);
	}

}

void init_socket_intermediate_buffer(struct socket_info *s_info) {
	s_info->intermediate_buffer.create_intermediate_buffer(s_info->socket);
}

void init_intermediate_buffer(struct worker_info *w_info) {
	set_NUMA(1);
	init_socket_intermediate_buffer(&w_info->s_info1);
	if (MULTISOCKET) {
		set_NUMA(2);
		init_socket_intermediate_buffer(&w_info->s_info2);
	}

}

void fill_queue() {
	for(int i = 0; i < 5; i++) {
		for(int qf = 1; qf <= 4; qf++) {
			for(int q = 1; q <= 4; q++) {
				if(qf != 3 && q == 4)
					continue;
				query_queue.push_back({qf,q});
				queries_left++;
			}
		}
	}
	std::random_shuffle(query_queue.begin(), query_queue.end());
}

int main(int argc, const char** argv) {
	if (argc < 8) {
		std::cerr << "Need at least 7 args" << std::endl;
		exit(1);
	}

	std::srand(std::time(0));
	int sds_write_value = 0;
	pmemobj_ctl_set(NULL, "sds.at_create", &sds_write_value);

	int worker_count;

	int query_flight = fast_atoi(argv[1]);
	int query = fast_atoi(argv[2]);
	ACCESS_SIZE = fast_atoi(argv[3]);
	THREAD_COUNT = fast_atoi(argv[4]);
	PINNING = fast_atoi(argv[5]);
	MULTISOCKET = fast_atoi(argv[6]);
	NUMA = fast_atoi(argv[7]);
	if (argc == 9) {
		WORKERS = fast_atoi(argv[8]);
		worker_count = 18 / THREAD_COUNT;
	} else {
		WORKERS = 0;
		worker_count = 1;
	}
	if (WORKERS) {
		fill_queue();
	} else {
		query_queue.push_back({query_flight, query});
		queries_left = 1;
	}

	struct worker_info *w_infos = (struct worker_info *)calloc(sizeof(struct worker_info), worker_count);
	Mem_Block mb1, mb2;
	set_NUMA(1);
	mb1.open_lineorder_table(1);
	if(MULTISOCKET) {
		set_NUMA(2);
		mb2.open_lineorder_table(2);
	}

	DEBUG_LOG("Initializing hash tables..");
	init_hashtables(&w_infos[0]);

	DEBUG_LOG("Initializing " << worker_count << " worker(s)...");
	for (int i = 0; i < worker_count; i++) {
		w_infos[i] = w_infos[0];
		w_infos[i].s_info1.lo_table = mb1;
		w_infos[i].s_info2.lo_table = mb2;
		w_infos[i].s_info1.thread_count = THREAD_COUNT;
		w_infos[i].s_info1.worker = i;
		w_infos[i].s_info1.worker_count = worker_count;
		w_infos[i].s_info2.thread_count = THREAD_COUNT;
		w_infos[i].s_info2.worker = i;
		w_infos[i].s_info2.worker_count = worker_count;
		init_intermediate_buffer(&w_infos[i]);
	}

	DEBUG_LOG("Running SSB...");
	struct timeval start_time, end_time;
	gettimeofday(&start_time, 0);
	for (int i = 0; i < worker_count; i++) {
		pthread_create(&w_infos[i].thread_id, 0, (void *(*)(void *))worker_function, (void*)&w_infos[i]);
	}
	for (int i = 0; i < worker_count; i++) {
		pthread_join(w_infos[i].thread_id, NULL);
	}
	gettimeofday(&end_time, 0);
	//printf("%lld\n", time_diff_ms(&start_time, &end_time));
	std::cout << "qf" << query_flight << "." << query << "," << avg_run_time << std::endl;

#if defined(DRAM_BM)
	dram_clean_up();
#elif defined(PMEM_BM)
	pmem_clean_up();
#elif defined(SSD_BM)
	ssd_clean_up();
#endif
}
