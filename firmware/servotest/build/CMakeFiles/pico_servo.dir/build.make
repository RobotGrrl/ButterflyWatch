# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.24.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.24.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/build

# Include any dependencies generated for this target.
include CMakeFiles/pico_servo.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/pico_servo.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/pico_servo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/pico_servo.dir/flags.make

CMakeFiles/pico_servo.dir/src/servo.c.obj: CMakeFiles/pico_servo.dir/flags.make
CMakeFiles/pico_servo.dir/src/servo.c.obj: /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/src/servo.c
CMakeFiles/pico_servo.dir/src/servo.c.obj: CMakeFiles/pico_servo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/pico_servo.dir/src/servo.c.obj"
	/usr/local/bin/arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/pico_servo.dir/src/servo.c.obj -MF CMakeFiles/pico_servo.dir/src/servo.c.obj.d -o CMakeFiles/pico_servo.dir/src/servo.c.obj -c /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/src/servo.c

CMakeFiles/pico_servo.dir/src/servo.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/pico_servo.dir/src/servo.c.i"
	/usr/local/bin/arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/src/servo.c > CMakeFiles/pico_servo.dir/src/servo.c.i

CMakeFiles/pico_servo.dir/src/servo.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/pico_servo.dir/src/servo.c.s"
	/usr/local/bin/arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/src/servo.c -o CMakeFiles/pico_servo.dir/src/servo.c.s

# Object files for target pico_servo
pico_servo_OBJECTS = \
"CMakeFiles/pico_servo.dir/src/servo.c.obj"

# External object files for target pico_servo
pico_servo_EXTERNAL_OBJECTS =

libpico_servo.a: CMakeFiles/pico_servo.dir/src/servo.c.obj
libpico_servo.a: CMakeFiles/pico_servo.dir/build.make
libpico_servo.a: CMakeFiles/pico_servo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libpico_servo.a"
	$(CMAKE_COMMAND) -P CMakeFiles/pico_servo.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pico_servo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/pico_servo.dir/build: libpico_servo.a
.PHONY : CMakeFiles/pico_servo.dir/build

CMakeFiles/pico_servo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/pico_servo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/pico_servo.dir/clean

CMakeFiles/pico_servo.dir/depend:
	cd /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/build /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/build /Users/frankenteddy/Documents/PlatformIO/ButterflyWatch/servotest/build/CMakeFiles/pico_servo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/pico_servo.dir/depend

