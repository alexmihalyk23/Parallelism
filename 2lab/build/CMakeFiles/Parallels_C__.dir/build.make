# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/alex/Parallelism/2lab

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alex/Parallelism/2lab/build

# Include any dependencies generated for this target.
include CMakeFiles/Parallels_C__.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Parallels_C__.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Parallels_C__.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Parallels_C__.dir/flags.make

CMakeFiles/Parallels_C__.dir/lin.cpp.o: CMakeFiles/Parallels_C__.dir/flags.make
CMakeFiles/Parallels_C__.dir/lin.cpp.o: ../lin.cpp
CMakeFiles/Parallels_C__.dir/lin.cpp.o: CMakeFiles/Parallels_C__.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alex/Parallelism/2lab/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Parallels_C__.dir/lin.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Parallels_C__.dir/lin.cpp.o -MF CMakeFiles/Parallels_C__.dir/lin.cpp.o.d -o CMakeFiles/Parallels_C__.dir/lin.cpp.o -c /home/alex/Parallelism/2lab/lin.cpp

CMakeFiles/Parallels_C__.dir/lin.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Parallels_C__.dir/lin.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/alex/Parallelism/2lab/lin.cpp > CMakeFiles/Parallels_C__.dir/lin.cpp.i

CMakeFiles/Parallels_C__.dir/lin.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Parallels_C__.dir/lin.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/alex/Parallelism/2lab/lin.cpp -o CMakeFiles/Parallels_C__.dir/lin.cpp.s

# Object files for target Parallels_C__
Parallels_C___OBJECTS = \
"CMakeFiles/Parallels_C__.dir/lin.cpp.o"

# External object files for target Parallels_C__
Parallels_C___EXTERNAL_OBJECTS =

Parallels_C__: CMakeFiles/Parallels_C__.dir/lin.cpp.o
Parallels_C__: CMakeFiles/Parallels_C__.dir/build.make
Parallels_C__: CMakeFiles/Parallels_C__.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alex/Parallelism/2lab/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Parallels_C__"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Parallels_C__.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Parallels_C__.dir/build: Parallels_C__
.PHONY : CMakeFiles/Parallels_C__.dir/build

CMakeFiles/Parallels_C__.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Parallels_C__.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Parallels_C__.dir/clean

CMakeFiles/Parallels_C__.dir/depend:
	cd /home/alex/Parallelism/2lab/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alex/Parallelism/2lab /home/alex/Parallelism/2lab /home/alex/Parallelism/2lab/build /home/alex/Parallelism/2lab/build /home/alex/Parallelism/2lab/build/CMakeFiles/Parallels_C__.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Parallels_C__.dir/depend

