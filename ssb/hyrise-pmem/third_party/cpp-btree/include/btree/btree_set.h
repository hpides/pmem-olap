// Copyright 2013 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// A btree_set<> implements the STL unique sorted associative container
// interface (a.k.a set<>) using a btree. A btree_multiset<> implements the STL
// multiple sorted associative container interface (a.k.a multiset<>) using a
// btree. See btree.h for details of the btree implementation and caveats.

#pragma once

#include <functional>
#include <memory>
#include <string>

#include "btree.h"
#include "btree_container.h"

namespace btree {

// The btree_set class is needed mainly for its constructors.
template <typename Key, typename Compare = std::less<Key>,
          typename Alloc = std::allocator<Key>, int TargetNodeSize = 256>
class btree_set
    : public btree_unique_container<
          btree<btree_set_params<Key, Compare, Alloc, TargetNodeSize> > > {
  using self_type = btree_set<Key, Compare, Alloc, TargetNodeSize>;
  using params_type = btree_set_params<Key, Compare, Alloc, TargetNodeSize>;
  using btree_type = btree<params_type>;
  using super_type = btree_unique_container<btree_type>;

 public:
  using key_compare = typename btree_type::key_compare;
  using allocator_type = typename btree_type::allocator_type;

 public:
  // Default constructor.
  btree_set(const key_compare &comp = key_compare(),
            const allocator_type &alloc = allocator_type())
      : super_type(comp, alloc) {}

  // Copy constructor.
  btree_set(const self_type &x) = default;

  // Move constructor.
  btree_set(self_type &&x) = default;

  // Range constructor.
  template <class InputIterator>
  btree_set(InputIterator b, InputIterator e,
            const key_compare &comp = key_compare(),
            const allocator_type &alloc = allocator_type())
      : super_type(b, e, comp, alloc) {}

  btree_set(std::initializer_list<typename btree_type::value_type> l,
            const key_compare &comp = key_compare(),
            const allocator_type &alloc = allocator_type())
      : super_type(l, comp, alloc) {}

  self_type &operator=(const self_type &x) = default;
  self_type &operator=(self_type &&x) = default;
};

template <typename K, typename C, typename A, int N>
inline void swap(btree_set<K, C, A, N> &x, btree_set<K, C, A, N> &y) {
  x.swap(y);
}

// The btree_multiset class is needed mainly for its constructors.
template <typename Key, typename Compare = std::less<Key>,
          typename Alloc = std::allocator<Key>, int TargetNodeSize = 256>
class btree_multiset
    : public btree_multi_container<
          btree<btree_set_params<Key, Compare, Alloc, TargetNodeSize> > > {
  using self_type = btree_multiset<Key, Compare, Alloc, TargetNodeSize>;
  using params_type = btree_set_params<Key, Compare, Alloc, TargetNodeSize>;
  using btree_type = btree<params_type>;
  using super_type = btree_multi_container<btree_type>;

 public:
  using key_compare = typename btree_type::key_compare;
  using allocator_type = typename btree_type::allocator_type;

 public:
  // Default constructor.
  btree_multiset(const key_compare &comp = key_compare(),
                 const allocator_type &alloc = allocator_type())
      : super_type(comp, alloc) {}

  // Copy constructor.
  btree_multiset(const self_type &x) = default;

  // Move constuctor
  btree_multiset(self_type &&x) = default;

  // Range constructor.
  template <class InputIterator>
  btree_multiset(InputIterator b, InputIterator e,
                 const key_compare &comp = key_compare(),
                 const allocator_type &alloc = allocator_type())
      : super_type(b, e, comp, alloc) {}

  btree_multiset(std::initializer_list<typename btree_type::value_type> l,
                 const key_compare &comp = key_compare(),
                 const allocator_type &alloc = allocator_type())
      : super_type(l, comp, alloc) {}

  self_type &operator=(const self_type &x) = default;

  self_type &operator=(self_type &&x) = default;
};

template <typename K, typename C, typename A, int N>
inline void swap(btree_multiset<K, C, A, N> &x, btree_multiset<K, C, A, N> &y) {
  x.swap(y);
}

}  // namespace btree
