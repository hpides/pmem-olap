#include "nvm_memory_resource.hpp"
#if HYRISE_WITH_MEMKIND || HYRISE_WITH_SEQPMEM

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#endif


#if HYRISE_WITH_MEMKIND

#include <memkind.h>

namespace opossum {

NVMMemoryResource::NVMMemoryResource() {
  std::ifstream config_file("nvm.json");
  if (!config_file.good()) config_file = std::ifstream{"../nvm.json"};
  Assert(config_file.good(), "nvm.json not found");
  const nlohmann::json config = nlohmann::json::parse(config_file);

  const auto pooldir = config["pooldir"].get<std::string>();
  Assert(std::filesystem::is_directory(pooldir), "NVM pooldir is not a directory");

  auto err = memkind_create_pmem(pooldir.c_str(), 0, &_nvm_kind);
  Assert(!err, "memkind_create_pmem failed");
}

NVMMemoryResource::~NVMMemoryResource() {
  //  Triggers kernel failure
  //  memkind_destroy_kind(_nvm_kind);
}

void* NVMMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
  auto p = memkind_malloc(_nvm_kind, bytes);

  return p;
}

void NVMMemoryResource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment) {
  memkind_free(_nvm_kind, p);
}

bool NVMMemoryResource::do_is_equal(const memory_resource& other) const noexcept { return &other == this; }

memkind_t NVMMemoryResource::kind() const { return _nvm_kind; }

size_t NVMMemoryResource::memory_usage() const {
  memkind_update_cached_stats();

  auto allocated = size_t{};

  const auto error = memkind_get_stat(_nvm_kind, MEMKIND_STAT_TYPE_ALLOCATED, &allocated);
  Assert(!error, "Error when retrieving MEMKIND_STAT_TYPE_ALLOCATED");
  return allocated;
}

}  // namespace opossum

#elif HYRISE_WITH_SEQPMEM
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libpmem2.h>
#include <libpmem.h>
#include <filesystem>
#include <stdio.h>
#define DEVDAX_FILE_SIZE 1ULL * 1024ULL * 1024ULL * 1024ULL

namespace opossum {

NVMMemoryResource::NVMMemoryResource() : _mapping_size(0) {
 	std::ifstream config_file("nvm.json");
	if (!config_file.good()) config_file = std::ifstream{"../nvm.json"};
	Assert(config_file.good(), "nvm.json not found");
	const nlohmann::json config = nlohmann::json::parse(config_file);
	int is_pmem = false;
	_pmem_start = (char *)pmem_map_file("/dev/dax1.1", 0, 0,
                                  0666, &_mapping_size, &is_pmem);
	if (_pmem_start == NULL)
		exit(EXIT_FAILURE);
  	// const auto poolfile = config["poolfile"].get<std::string>();
	// Assert(std::filesystem::exists(poolfile), "NVM poolfile does not exist");
	
    // int fd = 0;
    // Assert(fd = open("/dev/dax1.1", O_RDWR) >= 0, "Could not open pmemfile");
    // std::cout << "fd is:" << fd << std::endl;
	// Assert(pmem2_config_new(&_pmem_config)== 0, "Could not create PMEM config");
    
    // // we need to make sure that the mapped memory is aligned
	// int res = pmem2_source_from_fd(&_pmem_source, fd);
	// std::cout << "Err is:" << res << std::endl;
	// printf("Err is %d\n", res);
   	// Assert(res == 0, "Could not create pmem source");
    // size_t alignment = 0;
	// res = pmem2_source_alignment(_pmem_source, &alignment);
	// std::cout << "Err is:" << fd << std::endl;
    // Assert(res == 0, "Could not set alignment of PMEM");
   
    // size_t file_size = 0;
    // pmem2_source_size(_pmem_source, &file_size);
    /*
    *  For devdax, we do not want to benchmark the whole file, but only 40GiB atm. 
    */ 
    // if (strncmp(poolfile.c_str(), "/dev/", 5) == 0)
    //     file_size = DEVDAX_FILE_SIZE;
    // _mapping_size = file_size;
    
    // pmem2_config_set_length(_pmem_config, _mapping_size);

    // Assert(pmem2_config_set_required_store_granularity(_pmem_config, PMEM2_GRANULARITY_CACHE_LINE) == 0, "Could not set PMEM granularity");
    
    // Assert(pmem2_map_new(&_pmem_map, _pmem_config, _pmem_source), "Coud not create PMEM mapping");
    
    //_pmem_start = (char *)(pmem2_map_get_address(_pmem_map));
	printf("PMEMstart: %p\n", _pmem_start);

    _pmem_end = (char *)(_pmem_start + _mapping_size);
	_pmem_offset = 0;
}

NVMMemoryResource::~NVMMemoryResource() {
  //Do nothing, we do not care
}

void* NVMMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
	size_t new_pmem_offset = (_pmem_offset + alignment-1) / alignment * alignment;
	void *p = _pmem_start + new_pmem_offset;
	_pmem_offset = new_pmem_offset + bytes;
    return p;
}

void NVMMemoryResource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment) {
  //in sequential memory, we _do_ not free anything
}

size_t NVMMemoryResource::memory_usage() const {
  return _pmem_offset;
}

}  // namespace opossum
#endif