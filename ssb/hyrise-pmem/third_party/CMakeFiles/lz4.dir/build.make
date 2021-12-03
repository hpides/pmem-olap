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
include third_party/CMakeFiles/lz4.dir/depend.make

# Include the progress variables for this target.
include third_party/CMakeFiles/lz4.dir/progress.make

# Include the compile flags for this target's objects.
include third_party/CMakeFiles/lz4.dir/flags.make

third_party/CMakeFiles/lz4.dir/lz4/lib/lz4.c.o: third_party/CMakeFiles/lz4.dir/flags.make
third_party/CMakeFiles/lz4.dir/lz4/lib/lz4.c.o: third_party/lz4/lib/lz4.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/hpi/fs00/home/lars.bollmeier/hyrise/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object third_party/CMakeFiles/lz4.dir/lz4/lib/lz4.c.o"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/lz4.dir/lz4/lib/lz4.c.o -c /hpi/fs00/home/lars.bollmeier/hyrise/third_party/lz4/lib/lz4.c

third_party/CMakeFiles/lz4.dir/lz4/lib/lz4.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lz4.dir/lz4/lib/lz4.c.i"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /hpi/fs00/home/lars.bollmeier/hyrise/third_party/lz4/lib/lz4.c > CMakeFiles/lz4.dir/lz4/lib/lz4.c.i

third_party/CMakeFiles/lz4.dir/lz4/lib/lz4.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lz4.dir/lz4/lib/lz4.c.s"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /hpi/fs00/home/lars.bollmeier/hyrise/third_party/lz4/lib/lz4.c -o CMakeFiles/lz4.dir/lz4/lib/lz4.c.s

third_party/CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.o: third_party/CMakeFiles/lz4.dir/flags.make
third_party/CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.o: third_party/lz4/lib/lz4hc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/hpi/fs00/home/lars.bollmeier/hyrise/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object third_party/CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.o"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.o -c /hpi/fs00/home/lars.bollmeier/hyrise/third_party/lz4/lib/lz4hc.c

third_party/CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.i"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /hpi/fs00/home/lars.bollmeier/hyrise/third_party/lz4/lib/lz4hc.c > CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.i

third_party/CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.s"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /hpi/fs00/home/lars.bollmeier/hyrise/third_party/lz4/lib/lz4hc.c -o CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.s

# Object files for target lz4
lz4_OBJECTS = \
"CMakeFiles/lz4.dir/lz4/lib/lz4.c.o" \
"CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.o"

# External object files for target lz4
lz4_EXTERNAL_OBJECTS =

third_party/liblz4.a: third_party/CMakeFiles/lz4.dir/lz4/lib/lz4.c.o
third_party/liblz4.a: third_party/CMakeFiles/lz4.dir/lz4/lib/lz4hc.c.o
third_party/liblz4.a: third_party/CMakeFiles/lz4.dir/build.make
third_party/liblz4.a: third_party/CMakeFiles/lz4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/hpi/fs00/home/lars.bollmeier/hyrise/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library liblz4.a"
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party && $(CMAKE_COMMAND) -P CMakeFiles/lz4.dir/cmake_clean_target.cmake
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lz4.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
third_party/CMakeFiles/lz4.dir/build: third_party/liblz4.a

.PHONY : third_party/CMakeFiles/lz4.dir/build

third_party/CMakeFiles/lz4.dir/clean:
	cd /hpi/fs00/home/lars.bollmeier/hyrise/third_party && $(CMAKE_COMMAND) -P CMakeFiles/lz4.dir/cmake_clean.cmake
.PHONY : third_party/CMakeFiles/lz4.dir/clean

third_party/CMakeFiles/lz4.dir/depend:
	cd /hpi/fs00/home/lars.bollmeier/hyrise && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /hpi/fs00/home/lars.bollmeier/hyrise /hpi/fs00/home/lars.bollmeier/hyrise/third_party /hpi/fs00/home/lars.bollmeier/hyrise /hpi/fs00/home/lars.bollmeier/hyrise/third_party /hpi/fs00/home/lars.bollmeier/hyrise/third_party/CMakeFiles/lz4.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third_party/CMakeFiles/lz4.dir/depend
