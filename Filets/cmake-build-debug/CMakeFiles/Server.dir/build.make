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
CMAKE_COMMAND = /opt/clion/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Code/c++/my_code_project/base_pro/tool/Tool/Filets

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Server.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Server.dir/flags.make

CMakeFiles/Server.dir/tests/Server.cc.o: CMakeFiles/Server.dir/flags.make
CMakeFiles/Server.dir/tests/Server.cc.o: ../tests/Server.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Server.dir/tests/Server.cc.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Server.dir/tests/Server.cc.o -c /Code/c++/my_code_project/base_pro/tool/Tool/Filets/tests/Server.cc

CMakeFiles/Server.dir/tests/Server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Server.dir/tests/Server.cc.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Code/c++/my_code_project/base_pro/tool/Tool/Filets/tests/Server.cc > CMakeFiles/Server.dir/tests/Server.cc.i

CMakeFiles/Server.dir/tests/Server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Server.dir/tests/Server.cc.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Code/c++/my_code_project/base_pro/tool/Tool/Filets/tests/Server.cc -o CMakeFiles/Server.dir/tests/Server.cc.s

# Object files for target Server
Server_OBJECTS = \
"CMakeFiles/Server.dir/tests/Server.cc.o"

# External object files for target Server
Server_EXTERNAL_OBJECTS =

../bin/Server: CMakeFiles/Server.dir/tests/Server.cc.o
../bin/Server: CMakeFiles/Server.dir/build.make
../bin/Server: ../lib/libNFT.a
../bin/Server: CMakeFiles/Server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/Server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Server.dir/build: ../bin/Server

.PHONY : CMakeFiles/Server.dir/build

CMakeFiles/Server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Server.dir/clean

CMakeFiles/Server.dir/depend:
	cd /Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Code/c++/my_code_project/base_pro/tool/Tool/Filets /Code/c++/my_code_project/base_pro/tool/Tool/Filets /Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug /Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug /Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug/CMakeFiles/Server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Server.dir/depend

