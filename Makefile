# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/eason/doc_file/MPC硬件

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/eason/doc_file/MPC硬件

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/eason/doc_file/MPC硬件/CMakeFiles /home/eason/doc_file/MPC硬件/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/eason/doc_file/MPC硬件/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named test_ex

# Build rule for target.
test_ex: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_ex
.PHONY : test_ex

# fast build rule for target.
test_ex/fast:
	$(MAKE) -f CMakeFiles/test_ex.dir/build.make CMakeFiles/test_ex.dir/build
.PHONY : test_ex/fast

#=============================================================================
# Target rules for targets named file_deal

# Build rule for target.
file_deal: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 file_deal
.PHONY : file_deal

# fast build rule for target.
file_deal/fast:
	$(MAKE) -f CMakeFiles/file_deal.dir/build.make CMakeFiles/file_deal.dir/build
.PHONY : file_deal/fast

#=============================================================================
# Target rules for targets named hardware_lib

# Build rule for target.
hardware_lib: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 hardware_lib
.PHONY : hardware_lib

# fast build rule for target.
hardware_lib/fast:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/build
.PHONY : hardware_lib/fast

#=============================================================================
# Target rules for targets named main

# Build rule for target.
main: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 main
.PHONY : main

# fast build rule for target.
main/fast:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/build
.PHONY : main/fast

#=============================================================================
# Target rules for targets named hardware

# Build rule for target.
hardware: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 hardware
.PHONY : hardware

# fast build rule for target.
hardware/fast:
	$(MAKE) -f HW/CMakeFiles/hardware.dir/build.make HW/CMakeFiles/hardware.dir/build
.PHONY : hardware/fast

src/Merkle_Hash.o: src/Merkle_Hash.cpp.o

.PHONY : src/Merkle_Hash.o

# target to build an object file
src/Merkle_Hash.cpp.o:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/Merkle_Hash.cpp.o
.PHONY : src/Merkle_Hash.cpp.o

src/Merkle_Hash.i: src/Merkle_Hash.cpp.i

.PHONY : src/Merkle_Hash.i

# target to preprocess a source file
src/Merkle_Hash.cpp.i:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/Merkle_Hash.cpp.i
.PHONY : src/Merkle_Hash.cpp.i

src/Merkle_Hash.s: src/Merkle_Hash.cpp.s

.PHONY : src/Merkle_Hash.s

# target to generate assembly for a file
src/Merkle_Hash.cpp.s:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/Merkle_Hash.cpp.s
.PHONY : src/Merkle_Hash.cpp.s

src/_file_deal.o: src/_file_deal.cpp.o

.PHONY : src/_file_deal.o

# target to build an object file
src/_file_deal.cpp.o:
	$(MAKE) -f CMakeFiles/file_deal.dir/build.make CMakeFiles/file_deal.dir/src/_file_deal.cpp.o
.PHONY : src/_file_deal.cpp.o

src/_file_deal.i: src/_file_deal.cpp.i

.PHONY : src/_file_deal.i

# target to preprocess a source file
src/_file_deal.cpp.i:
	$(MAKE) -f CMakeFiles/file_deal.dir/build.make CMakeFiles/file_deal.dir/src/_file_deal.cpp.i
.PHONY : src/_file_deal.cpp.i

src/_file_deal.s: src/_file_deal.cpp.s

.PHONY : src/_file_deal.s

# target to generate assembly for a file
src/_file_deal.cpp.s:
	$(MAKE) -f CMakeFiles/file_deal.dir/build.make CMakeFiles/file_deal.dir/src/_file_deal.cpp.s
.PHONY : src/_file_deal.cpp.s

src/main.o: src/main.cpp.o

.PHONY : src/main.o

# target to build an object file
src/main.cpp.o:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/main.cpp.o
.PHONY : src/main.cpp.o

src/main.i: src/main.cpp.i

.PHONY : src/main.i

# target to preprocess a source file
src/main.cpp.i:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/main.cpp.i
.PHONY : src/main.cpp.i

src/main.s: src/main.cpp.s

.PHONY : src/main.s

# target to generate assembly for a file
src/main.cpp.s:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/main.cpp.s
.PHONY : src/main.cpp.s

src/network.o: src/network.cpp.o

.PHONY : src/network.o

# target to build an object file
src/network.cpp.o:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/network.cpp.o
.PHONY : src/network.cpp.o

src/network.i: src/network.cpp.i

.PHONY : src/network.i

# target to preprocess a source file
src/network.cpp.i:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/network.cpp.i
.PHONY : src/network.cpp.i

src/network.s: src/network.cpp.s

.PHONY : src/network.s

# target to generate assembly for a file
src/network.cpp.s:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/network.cpp.s
.PHONY : src/network.cpp.s

src/protocol.o: src/protocol.cpp.o

.PHONY : src/protocol.o

# target to build an object file
src/protocol.cpp.o:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/protocol.cpp.o
.PHONY : src/protocol.cpp.o

src/protocol.i: src/protocol.cpp.i

.PHONY : src/protocol.i

# target to preprocess a source file
src/protocol.cpp.i:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/protocol.cpp.i
.PHONY : src/protocol.cpp.i

src/protocol.s: src/protocol.cpp.s

.PHONY : src/protocol.s

# target to generate assembly for a file
src/protocol.cpp.s:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/protocol.cpp.s
.PHONY : src/protocol.cpp.s

src/tool.o: src/tool.cpp.o

.PHONY : src/tool.o

# target to build an object file
src/tool.cpp.o:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/tool.cpp.o
.PHONY : src/tool.cpp.o

src/tool.i: src/tool.cpp.i

.PHONY : src/tool.i

# target to preprocess a source file
src/tool.cpp.i:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/tool.cpp.i
.PHONY : src/tool.cpp.i

src/tool.s: src/tool.cpp.s

.PHONY : src/tool.s

# target to generate assembly for a file
src/tool.cpp.s:
	$(MAKE) -f CMakeFiles/hardware_lib.dir/build.make CMakeFiles/hardware_lib.dir/src/tool.cpp.s
.PHONY : src/tool.cpp.s

test/test.o: test/test.cpp.o

.PHONY : test/test.o

# target to build an object file
test/test.cpp.o:
	$(MAKE) -f CMakeFiles/test_ex.dir/build.make CMakeFiles/test_ex.dir/test/test.cpp.o
.PHONY : test/test.cpp.o

test/test.i: test/test.cpp.i

.PHONY : test/test.i

# target to preprocess a source file
test/test.cpp.i:
	$(MAKE) -f CMakeFiles/test_ex.dir/build.make CMakeFiles/test_ex.dir/test/test.cpp.i
.PHONY : test/test.cpp.i

test/test.s: test/test.cpp.s

.PHONY : test/test.s

# target to generate assembly for a file
test/test.cpp.s:
	$(MAKE) -f CMakeFiles/test_ex.dir/build.make CMakeFiles/test_ex.dir/test/test.cpp.s
.PHONY : test/test.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... test_ex"
	@echo "... file_deal"
	@echo "... hardware_lib"
	@echo "... edit_cache"
	@echo "... main"
	@echo "... hardware"
	@echo "... src/Merkle_Hash.o"
	@echo "... src/Merkle_Hash.i"
	@echo "... src/Merkle_Hash.s"
	@echo "... src/_file_deal.o"
	@echo "... src/_file_deal.i"
	@echo "... src/_file_deal.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/network.o"
	@echo "... src/network.i"
	@echo "... src/network.s"
	@echo "... src/protocol.o"
	@echo "... src/protocol.i"
	@echo "... src/protocol.s"
	@echo "... src/tool.o"
	@echo "... src/tool.i"
	@echo "... src/tool.s"
	@echo "... test/test.o"
	@echo "... test/test.i"
	@echo "... test/test.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

