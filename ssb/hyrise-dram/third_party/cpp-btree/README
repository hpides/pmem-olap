C++ B-tree is a template library that implements ordered in-memory containers
based on a B-tree data structure. Based on https://code.google.com/archive/p/cpp-btree/,
but forked from https://github.com/algorithm-ninja/cpp-btree.

A b-tree based implementation can have advantages over the RB-tree
based implementations of std::map. It has fewer pointers leading
to significant memory savings and less memory fragmentation. Due
to better usage of cache lines, large containers can also see
a significant performance increase. See btree_bench.cc for details.

The disadvantage is that all modifications invalidate all pointers
and iterators due to possible merge/split operations.

The dmeister repo adds C++11 integration, e.g. rvalue constructor and assignment,
emplace() and at().

----

This library is a C++ template library and, as such, there is no
library to build and install.  Copy the .h files and use them!

See http://code.google.com/p/cpp-btree/wiki/UsageInstructions for
details.

----

To build and run the provided tests, however, you will need to install
CMake, the Google C++ Test framework, and the Google flags package.

Download and install CMake from http://www.cmake.org

Download and build the GoogleTest framework from
https://github.com/google/googletest

Download and install gflags from https://github.com/gflags/gflags

Set GTEST_ROOT to the directory where GTEST was built.
Set GFLAGS_ROOT to the directory prefix where GFLAGS is installed.

export GTEST_ROOT=/path/for/gtest-x.y
export GFLAGS_ROOT=/opt

For example to build with ninja and clang++:

mkdir build
pushd build
cmake .. -GNinja -Dbuild_tests=ON
popd build
ninja -C build
ninja install
