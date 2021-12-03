#pragma once

#include <algorithm>
#include <functional>
#include <iosfwd>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Some changes into the std namespace needed for the tests
namespace std {

template <typename T, typename U>
ostream &operator<<(ostream &os, const std::pair<T, U> &p) {
  os << "(" << p.first << "," << p.second << ")";
  return os;
}

// Provide pair equality testing that works as long as x.first is comparable to
// y.first and x.second is comparable to y.second. Needed in the test for
// comparing std::pair<T, U> to std::pair<const T, U>.
template <typename T, typename U, typename V, typename W>
bool operator==(const std::pair<T, U> &x, const std::pair<V, W> &y) {
  return x.first == y.first && x.second == y.second;
}

// Partial specialization of remove_const that propagates the removal through
// std::pair.
template <typename T, typename U>
struct remove_const<pair<T, U> > {
  using type =
      pair<typename remove_const<T>::type, typename remove_const<U>::type>;
};

}  // namespace std