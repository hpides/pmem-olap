.PHONY: all clean

CXX := g++
CFLAGS := -std=c++17 -I./ -I./dash/src/ -I./dash/VeryPM/ -Wno-format -march=native -DPMEM
OPT_FLAGS := -O3 -funroll-loops
DBG_FLAGS := -g -fvar-tracking -fvar-tracking-assignments -O0 -DDEBUG

LINK_LIBS := -lrt -lpmemobj -lpmem -lpthread -lnuma

all: DRAM PMEM SSD

DEBUG: ssb.cpp CCEH-PMDK CCEH_MSB common.hpp
	$(CXX) $(CFLAGS) $(DBG_FLAGS) -o ssb_dram.out ssb.cpp $(LINK_LIBS) -DDRAM_BM
	$(CXX) $(CFLAGS) $(DBG_FLAGS) -o ssb_pmem.out ssb.cpp $(LINK_LIBS) -DPMEM_BM

DRAM: ssb.cpp common.hpp
	$(CXX) $(CFLAGS) $(OPT_FLAGS) -o ssb_dram.out       ssb.cpp $(LINK_LIBS) -DDRAM_BM
	$(CXX) $(CFLAGS) $(DBG_FLAGS) -o ssb_dram_debug.out ssb.cpp $(LINK_LIBS) -DDRAM_BM

PMEM: ssb.cpp common.hpp
	$(CXX) $(CFLAGS) $(OPT_FLAGS) -o ssb_pmem.out       ssb.cpp $(LINK_LIBS) -DPMEM_BM
	$(CXX) $(CFLAGS) $(DBG_FLAGS) -o ssb_pmem_debug.out ssb.cpp $(LINK_LIBS) -DPMEM_BM

SSD: ssb.cpp common.hpp
	$(CXX) $(CFLAGS) $(OPT_FLAGS) -o ssb_ssd.out       ssb.cpp $(LINK_LIBS) -DSSD_BM
	$(CXX) $(CFLAGS) $(DBG_FLAGS) -o ssb_ssd_debug.out ssb.cpp $(LINK_LIBS) -DSSD_BM

clean:
	rm -rf **/*.o ssb*.out
