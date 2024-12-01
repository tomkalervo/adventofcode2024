# adventofcode2024
Coding challanges from https://adventofcode.com/

## The build and run setup
I have a CMake script that build the aoc application.

1. Create a build directory, mkdir build/
2. Init Cmake w/ build directory, cmake -S . -B build/
3. Build the app w/ CMakeList, cmake --build build/
4. If all is well you can run the application, ./build/aoc --Day 1 --file files/day_01.txt

```console
cd adventofcode2024
mkdir build/
cmake -S . -B build/
cmake --build build/
./build/aoc --Day 1 --File files/day_01.txt
```
