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
#pragma once

#include <iosfwd>
#include <utility>

#include "btree.h"

namespace btree {

// A common base class for btree_set, btree_map, btree_multiset and
// btree_multimap.
template <typename Tree>
class btree_container {
  using self_type = btree_container<Tree>;

 public:
  using params_type = typename Tree::params_type;
  using key_type = typename Tree::key_type;
  using value_type = typename Tree::value_type;
  using key_compare = typename Tree::key_compare;
  using allocator_type = typename Tree::allocator_type;
  using pointer = typename Tree::pointer;
  using const_pointer = typename Tree::const_pointer;
  using reference = typename Tree::reference;
  using const_reference = typename Tree::const_reference;
  using size_type = typename Tree::size_type;
  using difference_type = typename Tree::difference_type;
  using iterator = typename Tree::iterator;
  using const_iterator = typename Tree::const_iterator;
  using reverse_iterator = typename Tree::reverse_iterator;
  using const_reverse_iterator = typename Tree::const_reverse_iterator;

 public:
  // Default constructor.
  btree_container(const key_compare &comp, const allocator_type &alloc)
      : tree_(comp, alloc) {}

  btree_container(const allocator_type &alloc) : tree_(alloc) {}

  // Copy constructor.
  btree_container(const self_type &x) = default;

  // Move constructor
  btree_container(self_type &&x) = default;

  self_type &operator=(const self_type &x) = default;
  self_type &operator=(self_type &&x) = default;

  // Iterator routines.
  iterator begin() { return tree_.begin(); }
  const_iterator begin() const { return tree_.begin(); }
  iterator end() { return tree_.end(); }
  const_iterator end() const { return tree_.end(); }
  reverse_iterator rbegin() { return tree_.rbegin(); }
  const_reverse_iterator rbegin() const { return tree_.rbegin(); }
  reverse_iterator rend() { return tree_.rend(); }
  const_reverse_iterator rend() const { return tree_.rend(); }

  // Lookup routines.
  iterator lower_bound(const key_type &key) { return tree_.lower_bound(key); }
  const_iterator lower_bound(const key_type &key) const {
    return tree_.lower_bound(key);
  }
  iterator upper_bound(const key_type &key) { return tree_.upper_bound(key); }
  const_iterator upper_bound(const key_type &key) const {
    return tree_.upper_bound(key);
  }
  std::pair<iterator, iterator> equal_range(const key_type &key) {
    return tree_.equal_range(key);
  }
  std::pair<const_iterator, const_iterator> equal_range(
      const key_type &key) const {
    return tree_.equal_range(key);
  }

  // Utility routines.
  void clear() { tree_.clear(); }
  void swap(self_type &x) { tree_.swap(x.tree_); }
  void dump(std::ostream &os) const { tree_.dump(os); }
  void verify() const { tree_.verify(); }

  // Size routines.
  size_type size() const { return tree_.size(); }
  size_type max_size() const { return tree_.max_size(); }
  bool empty() const { return tree_.empty(); }
  size_type height() const { return tree_.height(); }
  size_type internal_nodes() const { return tree_.internal_nodes(); }
  size_type leaf_nodes() const { return tree_.leaf_nodes(); }
  size_type nodes() const { return tree_.nodes(); }
  size_type bytes_used() const { return tree_.bytes_used(); }
  static double average_bytes_per_value() {
    return Tree::average_bytes_per_value();
  }
  double fullness() const { return tree_.fullness(); }
  double overhead() const { return tree_.overhead(); }

  bool operator==(const self_type &x) const {
    if (size() != x.size()) {
      return false;
    }
    for (const_iterator i = begin(), xi = x.begin(); i != end(); ++i, ++xi) {
      if (*i != *xi) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const self_type &other) const { return !operator==(other); }

 protected:
  Tree tree_;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const btree_container<T> &b) {
  b.dump(os);
  return os;
}

// A common base class for btree_set and safe_btree_set.
template <typename Tree>
class btree_unique_container : public btree_container<Tree> {
  using self_type = btree_unique_container<Tree>;
  using super_type = btree_container<Tree>;

 public:
  using key_type = typename Tree::key_type;
  using value_type = typename Tree::value_type;
  using size_type = typename Tree::size_type;
  using key_compare = typename Tree::key_compare;
  using allocator_type = typename Tree::allocator_type;
  using iterator = typename Tree::iterator;
  using const_iterator = typename Tree::const_iterator;

 public:
  // Default constructor.
  btree_unique_container(const key_compare &comp = key_compare(),
                         const allocator_type &alloc = allocator_type())
      : super_type(comp, alloc) {}

  btree_unique_container(const allocator_type &alloc = allocator_type())
      : super_type(alloc) {}

  // Copy constructor.
  btree_unique_container(const self_type &x) = default;

  // Move constructor.
  btree_unique_container(self_type &&x) = default;

  // Range constructor.
  template <class InputIterator>
  btree_unique_container(InputIterator b, InputIterator e,
                         const key_compare &comp = key_compare(),
                         const allocator_type &alloc = allocator_type())
      : super_type(comp, alloc) {
    insert(b, e);
  }

  btree_unique_container(std::initializer_list<value_type> l,
                         const key_compare &comp = key_compare(),
                         const allocator_type &alloc = allocator_type())
      : super_type(comp, alloc) {
    insert(l.begin(), l.end());
  }

  self_type &operator=(const self_type &x) = default;
  self_type &operator=(self_type &&x) = default;

  // Lookup routines.
  iterator find(const key_type &key) { return this->tree_.find_unique(key); }
  const_iterator find(const key_type &key) const {
    return this->tree_.find_unique(key);
  }
  size_type count(const key_type &key) const {
    return this->tree_.count_unique(key);
  }

  // Insertion routines.
  std::pair<iterator, bool> insert(const value_type &x) {
    return this->tree_.insert_unique(x);
  }
  iterator insert(iterator position, const value_type &x) {
    return this->tree_.insert_unique(position, x);
  }
  template <typename InputIterator>
  void insert(InputIterator b, InputIterator e) {
    this->tree_.insert_unique(b, e);
  }

  template <class... Args>
  std::pair<iterator, bool> emplace(Args &&... args) {
    return this->tree_.emplace_unique(std::forward<Args>(args)...);
  }
  template <class... Args>
  iterator emplace_hint(iterator hint, Args &&... args) {
    return this->tree_.emplace_unique_hint(hint, std::forward<Args>(args)...);
  }

  // Deletion routines.
  int erase(const key_type &key) { return this->tree_.erase_unique(key); }
  // Erase the specified iterator from the btree. The iterator must be valid
  // (i.e. not equal to end()).  Return an iterator pointing to the node after
  // the one that was erased (or end() if none exists).
  iterator erase(const iterator &iter) { return this->tree_.erase(iter); }
  void erase(const iterator &first, const iterator &last) {
    this->tree_.erase(first, last);
  }
};

// A common base class for btree_map and safe_btree_map.
template <typename Tree>
class btree_map_container : public btree_unique_container<Tree> {
  using self_type = btree_map_container<Tree>;
  using super_type = btree_unique_container<Tree>;

 public:
  using key_type = typename Tree::key_type;
  using data_type = typename Tree::data_type;
  using value_type = typename Tree::value_type;
  using mapped_type = typename Tree::mapped_type;
  using key_compare = typename Tree::key_compare;
  using allocator_type = typename Tree::allocator_type;

 private:
  // A pointer-like object which only generates its value when
  // dereferenced. Used by operator[] to avoid constructing an empty data_type
  // if the key already exists in the map.
  struct generate_value {
    generate_value(const key_type &k) : key(k) {}
    value_type operator*() const { return std::make_pair(key, data_type()); }
    const key_type &key;
  };

 public:
  // Default constructor.
  btree_map_container(const key_compare &comp = key_compare(),
                      const allocator_type &alloc = allocator_type())
      : super_type(comp, alloc) {}

  btree_map_container(const allocator_type &alloc = allocator_type())
      : super_type(alloc) {}

  // Copy constructor.
  btree_map_container(const self_type &x) = default;

  // Move constructor
  btree_map_container(self_type &&x) = default;

  // Range constructor.
  template <class InputIterator>
  btree_map_container(InputIterator b, InputIterator e,
                      const key_compare &comp = key_compare(),
                      const allocator_type &alloc = allocator_type())
      : super_type(b, e, comp, alloc) {}

  btree_map_container(std::initializer_list<value_type> l,
                      const key_compare &comp = key_compare(),
                      const allocator_type &alloc = allocator_type())
      : super_type(l.begin(), l.end(), comp, alloc) {}

  self_type &operator=(const self_type &x) = default;
  self_type &operator=(self_type &&x) = default;

  // Insertion routines.
  data_type &operator[](const key_type &key) {
    return this->tree_.insert_unique(key, generate_value(key)).first->second;
  }

  data_type &at(const key_type &key) {
    auto i = this->find(key);
    if (i == this->end()) {
      throw std::out_of_range("at");
    }
    return i->second;
  }

  const data_type &at(const key_type &key) const {
    auto i = this->find(key);
    if (i == this->end()) {
      throw std::out_of_range("at");
    }
    return i->second;
  }
};

// A common base class for btree_multiset and btree_multimap.
template <typename Tree>
class btree_multi_container : public btree_container<Tree> {
  using self_type = btree_multi_container<Tree>;
  using super_type = btree_container<Tree>;

 public:
  using key_type = typename Tree::key_type;
  using value_type = typename Tree::value_type;
  using size_type = typename Tree::size_type;
  using key_compare = typename Tree::key_compare;
  using allocator_type = typename Tree::allocator_type;
  using iterator = typename Tree::iterator;
  using const_iterator = typename Tree::const_iterator;

 public:
  // Default constructor.
  btree_multi_container(const key_compare &comp = key_compare(),
                        const allocator_type &alloc = allocator_type())
      : super_type(comp, alloc) {}

  // Copy constructor.
  btree_multi_container(const self_type &x) = default;

  // Move constructor
  btree_multi_container(self_type &&x) = default;

  // Range constructor.
  template <class InputIterator>
  btree_multi_container(InputIterator b, InputIterator e,
                        const key_compare &comp = key_compare(),
                        const allocator_type &alloc = allocator_type())
      : super_type(comp, alloc) {
    insert(b, e);
  }

  btree_multi_container(std::initializer_list<value_type> l,
                        const key_compare &comp = key_compare(),
                        const allocator_type &alloc = allocator_type())
      : super_type(comp, alloc) {
    insert(l.begin(), l.end());
  }

  self_type &operator=(const self_type &x) = default;
  self_type &operator=(self_type &&x) = default;

  // Lookup routines.
  iterator find(const key_type &key) { return this->tree_.find_multi(key); }
  const_iterator find(const key_type &key) const {
    return this->tree_.find_multi(key);
  }
  size_type count(const key_type &key) const {
    return this->tree_.count_multi(key);
  }

  // Insertion routines.
  iterator insert(const value_type &x) { return this->tree_.insert_multi(x); }
  iterator insert(iterator position, const value_type &x) {
    return this->tree_.insert_multi(position, x);
  }
  template <typename InputIterator>
  void insert(InputIterator b, InputIterator e) {
    this->tree_.insert_multi(b, e);
  }

  template <class... Args>
  iterator emplace(Args &&... args) {
    return this->tree_.emplace_multi(std::forward<Args>(args)...);
  }

  template <class... Args>
  iterator emplace_hint(iterator hint, Args &&... args) {
    return this->tree_.emplace_multi_hint(hint, std::forward<Args>(args)...);
  }

  // Deletion routines.
  int erase(const key_type &key) { return this->tree_.erase_multi(key); }
  // Erase the specified iterator from the btree. The iterator must be valid
  // (i.e. not equal to end()).  Return an iterator pointing to the node after
  // the one that was erased (or end() if none exists).
  iterator erase(const iterator &iter) { return this->tree_.erase(iter); }
  void erase(const iterator &first, const iterator &last) {
    this->tree_.erase(first, last);
  }
};

}  // namespace btree
