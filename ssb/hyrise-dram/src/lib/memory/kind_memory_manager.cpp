#include "kind_memory_manager.hpp"

#include <magic_enum.hpp>

#include "nvm_memory_resource.hpp"

namespace opossum {

KindMemoryManager::KindMemoryManager() {
  for (auto purpose_idx = size_t{0}; purpose_idx < magic_enum::enum_count<AllocationPurpose>(); ++purpose_idx) {
    _mapping.emplace_back(*boost::container::pmr::get_default_resource());
  }
}

boost::container::pmr::memory_resource& KindMemoryManager::get_resource(const AllocationPurpose purpose) const {
  return _mapping.at(magic_enum::enum_integer(purpose));
}

void KindMemoryManager::set_resource(const AllocationPurpose purpose, boost::container::pmr::memory_resource& resource) {
  _mapping[magic_enum::enum_integer(purpose)] = resource;
}

}  // namespace opossum
