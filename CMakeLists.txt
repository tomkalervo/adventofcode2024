cmake_minimum_required(VERSION 3.10)
project(AoC_2024)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS Off)

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
