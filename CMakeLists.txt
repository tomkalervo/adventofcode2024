cmake_minimum_required(VERSION 3.10)
project(AoC_2024)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS Off)
set(CMAKE_BUILD_TYPE Debug) 
# Set stack size to 16 MB (or any desired size in bytes)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-stack_size,0x1000000")

# Add include directories
include_directories(src/include)

# Add all source files
file(GLOB_RECURSE SRC_FILES
  src/main.cpp
  src/challenges/*.cpp
  src/utils/*.cpp
)

# Create the executable
add_executable(aoc ${SRC_FILES})
