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
include CMakeFiles/Client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Client.dir/flags.make

CMakeFiles/Client.dir/tests/Client.cc.o: CMakeFiles/Client.dir/flags.make
CMakeFiles/Client.dir/tests/Client.cc.o: ../tests/Client.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Client.dir/tests/Client.cc.o"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Client.dir/tests/Client.cc.o -c /Code/c++/my_code_project/base_pro/tool/Tool/Filets/tests/Client.cc

CMakeFiles/Client.dir/tests/Client.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Client.dir/tests/Client.cc.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Code/c++/my_code_project/base_pro/tool/Tool/Filets/tests/Client.cc > CMakeFiles/Client.dir/tests/Client.cc.i

CMakeFiles/Client.dir/tests/Client.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Client.dir/tests/Client.cc.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Code/c++/my_code_project/base_pro/tool/Tool/Filets/tests/Client.cc -o CMakeFiles/Client.dir/tests/Client.cc.s

# Object files for target Client
Client_OBJECTS = \
"CMakeFiles/Client.dir/tests/Client.cc.o"

# External object files for target Client
Client_EXTERNAL_OBJECTS =

../bin/Client: CMakeFiles/Client.dir/tests/Client.cc.o
../bin/Client: CMakeFiles/Client.dir/build.make
../bin/Client: ../lib/libNFT.a
../bin/Client: CMakeFiles/Client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/Client"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Client.dir/build: ../bin/Client

.PHONY : CMakeFiles/Client.dir/build

CMakeFiles/Client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Client.dir/clean

CMakeFiles/Client.dir/depend:
	cd /Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Code/c++/my_code_project/base_pro/tool/Tool/Filets /Code/c++/my_code_project/base_pro/tool/Tool/Filets /Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug /Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug /Code/c++/my_code_project/base_pro/tool/Tool/Filets/cmake-build-debug/CMakeFiles/Client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Client.dir/depend
