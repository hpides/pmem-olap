# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /hpi/fs00/home/lars.bollmeier/hyrise

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /hpi/fs00/home/lars.bollmeier/hyrise

# Include any dependencies generated for this target.
include third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/depend.make

# Include the progress variables for this target.
include third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/progress.make

# Include the compile flags for this target's objects.
include third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/flags.make

third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.o: third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/flags.make
third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.o: third_party/nlohmann_json/test/src/unit-reference_access.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/hpi/fs00/home/lars.bollmeier/hyrise/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.o"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.o -c /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/src/unit-reference_access.cpp

third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.i"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/src/unit-reference_access.cpp > CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.i

third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.s"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/src/unit-reference_access.cpp -o CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.s

# Object files for target test-reference_access
test__reference_access_OBJECTS = \
"CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.o"

# External object files for target test-reference_access
test__reference_access_EXTERNAL_OBJECTS = \
"/hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/CMakeFiles/doctest_main.dir/src/unit.cpp.o"

test-reference_access: third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/src/unit-reference_access.cpp.o
test-reference_access: third_party/nlohmann_json/test/CMakeFiles/doctest_main.dir/src/unit.cpp.o
test-reference_access: third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/build.make
test-reference_access: third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/hpi/fs00/home/lars.bollmeier/hyrise/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../test-reference_access"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-reference_access.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/build: test-reference_access

.PHONY : third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/build

third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/clean:
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test && $(CMAKE_COMMAND) -P CMakeFiles/test-reference_access.dir/cmake_clean.cmake
.PHONY : third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/clean

third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/depend:
	cd /hpi/fs00/home/lars.bollmeier/hyrise && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /hpi/fs00/home/lars.bollmeier/hyrise /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test /hpi/fs00/home/lars.bollmeier/hyrise /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test /hpi/fs00/home/lars.bollmeier/hyrise/third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third_party/nlohmann_json/test/CMakeFiles/test-reference_access.dir/depend
