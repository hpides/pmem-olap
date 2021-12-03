#pragma once

#include <boost/container/pmr/memory_resource.hpp>

#include "utils/singleton.hpp"

namespace opossum {

enum class AllocationPurpose {
  AggregateHashGrouping,
  AggregateHashAggregation,
  PosList,
  JoinHashMaterialization,
  JoinHashHashTable,
};

class KindMemoryManager : public Singleton<KindMemoryManager> {
 public:
  boost::container::pmr::memory_resource& get_resource(const AllocationPurpose purpose) const;

  // TODO potentially not thread-safe
  void set_resource(const AllocationPurpose purpose, boost::container::pmr::memory_resource& resource);

 protected:
  KindMemoryManager();
  friend class Singleton;

  std::vector<std::reference_wrapper<boost::container::pmr::memory_resource>> _mapping;
};

}  // namespace opossum
