# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /classes/OS/connor/connor.4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /classes/OS/connor/connor.4/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Project_4.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Project_4.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Project_4.dir/flags.make

CMakeFiles/Project_4.dir/src/oss.cpp.o: CMakeFiles/Project_4.dir/flags.make
CMakeFiles/Project_4.dir/src/oss.cpp.o: ../src/oss.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /classes/OS/connor/connor.4/cmake-build-debug/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Project_4.dir/src/oss.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Project_4.dir/src/oss.cpp.o -c /classes/OS/connor/connor.4/src/oss.cpp

CMakeFiles/Project_4.dir/src/oss.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Project_4.dir/src/oss.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /classes/OS/connor/connor.4/src/oss.cpp > CMakeFiles/Project_4.dir/src/oss.cpp.i

CMakeFiles/Project_4.dir/src/oss.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Project_4.dir/src/oss.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /classes/OS/connor/connor.4/src/oss.cpp -o CMakeFiles/Project_4.dir/src/oss.cpp.s

CMakeFiles/Project_4.dir/src/oss.cpp.o.requires:
.PHONY : CMakeFiles/Project_4.dir/src/oss.cpp.o.requires

CMakeFiles/Project_4.dir/src/oss.cpp.o.provides: CMakeFiles/Project_4.dir/src/oss.cpp.o.requires
	$(MAKE) -f CMakeFiles/Project_4.dir/build.make CMakeFiles/Project_4.dir/src/oss.cpp.o.provides.build
.PHONY : CMakeFiles/Project_4.dir/src/oss.cpp.o.provides

CMakeFiles/Project_4.dir/src/oss.cpp.o.provides.build: CMakeFiles/Project_4.dir/src/oss.cpp.o

CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o: CMakeFiles/Project_4.dir/flags.make
CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o: ../src/sharedmemory.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /classes/OS/connor/connor.4/cmake-build-debug/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o -c /classes/OS/connor/connor.4/src/sharedmemory.cpp

CMakeFiles/Project_4.dir/src/sharedmemory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Project_4.dir/src/sharedmemory.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /classes/OS/connor/connor.4/src/sharedmemory.cpp > CMakeFiles/Project_4.dir/src/sharedmemory.cpp.i

CMakeFiles/Project_4.dir/src/sharedmemory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Project_4.dir/src/sharedmemory.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /classes/OS/connor/connor.4/src/sharedmemory.cpp -o CMakeFiles/Project_4.dir/src/sharedmemory.cpp.s

CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o.requires:
.PHONY : CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o.requires

CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o.provides: CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o.requires
	$(MAKE) -f CMakeFiles/Project_4.dir/build.make CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o.provides.build
.PHONY : CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o.provides

CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o.provides.build: CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o

CMakeFiles/Project_4.dir/src/childhandler.cpp.o: CMakeFiles/Project_4.dir/flags.make
CMakeFiles/Project_4.dir/src/childhandler.cpp.o: ../src/childhandler.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /classes/OS/connor/connor.4/cmake-build-debug/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Project_4.dir/src/childhandler.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Project_4.dir/src/childhandler.cpp.o -c /classes/OS/connor/connor.4/src/childhandler.cpp

CMakeFiles/Project_4.dir/src/childhandler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Project_4.dir/src/childhandler.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /classes/OS/connor/connor.4/src/childhandler.cpp > CMakeFiles/Project_4.dir/src/childhandler.cpp.i

CMakeFiles/Project_4.dir/src/childhandler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Project_4.dir/src/childhandler.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /classes/OS/connor/connor.4/src/childhandler.cpp -o CMakeFiles/Project_4.dir/src/childhandler.cpp.s

CMakeFiles/Project_4.dir/src/childhandler.cpp.o.requires:
.PHONY : CMakeFiles/Project_4.dir/src/childhandler.cpp.o.requires

CMakeFiles/Project_4.dir/src/childhandler.cpp.o.provides: CMakeFiles/Project_4.dir/src/childhandler.cpp.o.requires
	$(MAKE) -f CMakeFiles/Project_4.dir/build.make CMakeFiles/Project_4.dir/src/childhandler.cpp.o.provides.build
.PHONY : CMakeFiles/Project_4.dir/src/childhandler.cpp.o.provides

CMakeFiles/Project_4.dir/src/childhandler.cpp.o.provides.build: CMakeFiles/Project_4.dir/src/childhandler.cpp.o

CMakeFiles/Project_4.dir/src/errors.cpp.o: CMakeFiles/Project_4.dir/flags.make
CMakeFiles/Project_4.dir/src/errors.cpp.o: ../src/errors.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /classes/OS/connor/connor.4/cmake-build-debug/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Project_4.dir/src/errors.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Project_4.dir/src/errors.cpp.o -c /classes/OS/connor/connor.4/src/errors.cpp

CMakeFiles/Project_4.dir/src/errors.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Project_4.dir/src/errors.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /classes/OS/connor/connor.4/src/errors.cpp > CMakeFiles/Project_4.dir/src/errors.cpp.i

CMakeFiles/Project_4.dir/src/errors.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Project_4.dir/src/errors.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /classes/OS/connor/connor.4/src/errors.cpp -o CMakeFiles/Project_4.dir/src/errors.cpp.s

CMakeFiles/Project_4.dir/src/errors.cpp.o.requires:
.PHONY : CMakeFiles/Project_4.dir/src/errors.cpp.o.requires

CMakeFiles/Project_4.dir/src/errors.cpp.o.provides: CMakeFiles/Project_4.dir/src/errors.cpp.o.requires
	$(MAKE) -f CMakeFiles/Project_4.dir/build.make CMakeFiles/Project_4.dir/src/errors.cpp.o.provides.build
.PHONY : CMakeFiles/Project_4.dir/src/errors.cpp.o.provides

CMakeFiles/Project_4.dir/src/errors.cpp.o.provides.build: CMakeFiles/Project_4.dir/src/errors.cpp.o

CMakeFiles/Project_4.dir/src/filehander.cpp.o: CMakeFiles/Project_4.dir/flags.make
CMakeFiles/Project_4.dir/src/filehander.cpp.o: ../src/filehander.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /classes/OS/connor/connor.4/cmake-build-debug/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/Project_4.dir/src/filehander.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Project_4.dir/src/filehander.cpp.o -c /classes/OS/connor/connor.4/src/filehander.cpp

CMakeFiles/Project_4.dir/src/filehander.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Project_4.dir/src/filehander.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /classes/OS/connor/connor.4/src/filehander.cpp > CMakeFiles/Project_4.dir/src/filehander.cpp.i

CMakeFiles/Project_4.dir/src/filehander.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Project_4.dir/src/filehander.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /classes/OS/connor/connor.4/src/filehander.cpp -o CMakeFiles/Project_4.dir/src/filehander.cpp.s

CMakeFiles/Project_4.dir/src/filehander.cpp.o.requires:
.PHONY : CMakeFiles/Project_4.dir/src/filehander.cpp.o.requires

CMakeFiles/Project_4.dir/src/filehander.cpp.o.provides: CMakeFiles/Project_4.dir/src/filehander.cpp.o.requires
	$(MAKE) -f CMakeFiles/Project_4.dir/build.make CMakeFiles/Project_4.dir/src/filehander.cpp.o.provides.build
.PHONY : CMakeFiles/Project_4.dir/src/filehander.cpp.o.provides

CMakeFiles/Project_4.dir/src/filehander.cpp.o.provides.build: CMakeFiles/Project_4.dir/src/filehander.cpp.o

# Object files for target Project_4
Project_4_OBJECTS = \
"CMakeFiles/Project_4.dir/src/oss.cpp.o" \
"CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o" \
"CMakeFiles/Project_4.dir/src/childhandler.cpp.o" \
"CMakeFiles/Project_4.dir/src/errors.cpp.o" \
"CMakeFiles/Project_4.dir/src/filehander.cpp.o"

# External object files for target Project_4
Project_4_EXTERNAL_OBJECTS =

Project_4: CMakeFiles/Project_4.dir/src/oss.cpp.o
Project_4: CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o
Project_4: CMakeFiles/Project_4.dir/src/childhandler.cpp.o
Project_4: CMakeFiles/Project_4.dir/src/errors.cpp.o
Project_4: CMakeFiles/Project_4.dir/src/filehander.cpp.o
Project_4: CMakeFiles/Project_4.dir/build.make
Project_4: CMakeFiles/Project_4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable Project_4"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Project_4.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Project_4.dir/build: Project_4
.PHONY : CMakeFiles/Project_4.dir/build

CMakeFiles/Project_4.dir/requires: CMakeFiles/Project_4.dir/src/oss.cpp.o.requires
CMakeFiles/Project_4.dir/requires: CMakeFiles/Project_4.dir/src/sharedmemory.cpp.o.requires
CMakeFiles/Project_4.dir/requires: CMakeFiles/Project_4.dir/src/childhandler.cpp.o.requires
CMakeFiles/Project_4.dir/requires: CMakeFiles/Project_4.dir/src/errors.cpp.o.requires
CMakeFiles/Project_4.dir/requires: CMakeFiles/Project_4.dir/src/filehander.cpp.o.requires
.PHONY : CMakeFiles/Project_4.dir/requires

CMakeFiles/Project_4.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Project_4.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Project_4.dir/clean

CMakeFiles/Project_4.dir/depend:
	cd /classes/OS/connor/connor.4/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /classes/OS/connor/connor.4 /classes/OS/connor/connor.4 /classes/OS/connor/connor.4/cmake-build-debug /classes/OS/connor/connor.4/cmake-build-debug /classes/OS/connor/connor.4/cmake-build-debug/CMakeFiles/Project_4.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Project_4.dir/depend

