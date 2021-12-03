#pragma once
#define HYRISE_WITH_SEQPMEM 1

#if HYRISE_WITH_MEMKIND

#include <memkind.h>
#endif

#if HYRISE_WITH_MEMKIND || HYRISE_WITH_SEQPMEM
#include <boost/container/pmr/memory_resource.hpp>
#include <utils/singleton.hpp>
#endif

namespace opossum {

#if HYRISE_WITH_MEMKIND

class NVMMemoryResource : public boost::container::pmr::memory_resource, public Singleton<NVMMemoryResource> {
 public:
  virtual void* do_allocate(std::size_t bytes, std::size_t alignment);

  virtual void do_deallocate(void* p, std::size_t bytes, std::size_t alignment);

  virtual bool do_is_equal(const memory_resource& other) const noexcept;

  memkind_t kind() const;

  size_t memory_usage() const;

 protected:
  NVMMemoryResource();
  ~NVMMemoryResource();

  friend class Singleton;

  memkind_t _dram_kind;
  memkind_t _nvm_kind;
};

#elif HYRISE_WITH_SEQPMEM

#include <libpmem2.h>

class NVMMemoryResource : public boost::container::pmr::memory_resource, public Singleton<NVMMemoryResource> {
 public:
  virtual void* do_allocate(std::size_t bytes, std::size_t alignment);

  virtual void do_deallocate(void* p, std::size_t bytes, std::size_t alignment);

	[[nodiscard]] bool do_is_equal(const memory_resource& other) const BOOST_NOEXCEPT override { return &other == this; }
  size_t memory_usage() const;

 protected:
  NVMMemoryResource();
  ~NVMMemoryResource();

  friend class Singleton;
  private:
  struct pmem2_source *_pmem_source;
  struct pmem2_map *_pmem_map;
  struct pmem2_config *_pmem_config;
  size_t _mapping_size;
  char *_pmem_start;
  char *_pmem_end;
  size_t _pmem_offset;
};

#else

class NVMMemoryResource : public boost::container::pmr::memory_resource, public Singleton<NVMMemoryResource> {
 public:
  void* do_allocate(std::size_t bytes, std::size_t alignment) override { return std::malloc(bytes); }  // NOLINT

  void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override { std::free(p); }  // NOLINT

  [[nodiscard]] bool do_is_equal(const memory_resource& other) const BOOST_NOEXCEPT override { return &other == this; }

  size_t memory_usage() const { return 0; }
};

#endif

}  // namespace opossum
