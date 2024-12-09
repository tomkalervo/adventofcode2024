#include "../include/day09.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

static uint64_t fileCompact(const std::vector<int> &file_blocks,
                            const std::vector<int> &space_blocks) {
  uint64_t sum = 0;
  int pos = 0;
  int fi = 0;
  int fn = file_blocks.size() - 1;
  int fnb = 0;
  int si = 0;
  int sb = 0;
  while (fi <= fn) {
    // Process file block from left
    int fb = file_blocks[fi];
    while (fb) {
      sum += fi * pos++;
      // std::cout << "sum: " << sum << ", fi: " << fi << std::endl;
      fb--;
    }
    fi++;
    // Process file block from right (into empty space block)
    if (!sb) {
      sb = space_blocks[si++];
    }
    while (sb) {
      if (!fnb) {
        if (fn < fi) {
          break;
        }
        fnb = file_blocks[fn];
        fn--;
      }
      sum += (fn + 1) * pos++;
      // std::cout << "sum: " << sum << ", fn: " << (fn + 1) << std::endl;
      sb--;
      fnb--;
    }
  }
  while (fnb) {
    sum += (fn + 1) * pos++;
    // std::cout << "sum: " << sum << ", fn: " << (fn + 1) << std::endl;
    fnb--;
  }
  return sum;
}

/*
 * This function uses a vector to map put the file system. Working backwards it
 * moves file blocks into lowest possible index that contains empty blocks that
 * are large enough to fit all file blocks. A bit less sophisticated approach,
 * but works and runs relativly quick.
 */
static uint64_t fileMove(const std::vector<int> &file_blocks,
                         const std::vector<int> &space_blocks) {
  uint64_t sum = 0;
  // Map out a system vector, representing the layout if file- and empty blocks
  int system_size = 0;
  for (int e : file_blocks) {
    system_size += e;
  }
  for (int e : space_blocks) {
    system_size += e;
  }
  std::vector<int> system;
  system.reserve(system_size);
  int i = 0;
  while (i + 1 < file_blocks.size()) {
    for (int j = 0; j < file_blocks[i]; j++) {
      system.push_back(i);
    }
    for (int j = 0; j < space_blocks[i]; j++) {
      system.push_back(-1);
    }
    i++;
  }
  for (int j = 0; j < file_blocks[i]; j++) {
    system.push_back(i);
  }

  // std::cout << "System layout:";
  // for (int e : system) {
  //   std::cout << " " << e;
  // }
  // std::cout << std::endl;

  // Start move of files operation
  int pos = system.size();
  while (pos > 0) {
    // Find the next file to move
    pos--;
    int value = system[pos];
    if (value < 0) {
      continue;
    }
    int blocks = 1;
    while (pos > 0 && value == system[pos - 1]) {
      blocks++;
      pos--;
    }
    // Locate enough free space
    int idx = 0;
    while (idx < pos) {
      while (idx < system.size() && system[idx] >= 0) {
        idx++;
      }
      if (idx >= pos) {
        continue;
      }
      int free_blocks = 0;
      while (idx < system.size() && system[idx] < 0) {
        free_blocks++;
        idx++;
      }
      if (free_blocks >= blocks) {
        idx -= free_blocks;
        break;
      }
    }
    // Check if a slot exists
    if (idx < pos) {
      // Move the file into free slot
      while (blocks--) {
        system[idx++] = value;
        system[pos + blocks] = -1;
      }
    }
  }

  // std::cout << "System layout:";
  // for (int e : system) {
  //   std::cout << " " << e;
  // }
  // std::cout << std::endl;

  // Calculate checksum
  pos = 0;
  for (int fv : system) {
    if (fv >= 0) {
      sum += fv * pos;
    }
    pos++;
  }

  return sum;
}

void Day09::run(std::ifstream &file) {
  std::string line;
  std::vector<int> file_blocks;
  std::vector<int> free_blocks;
  if (!getline(file, line)) {
    std::cerr << "Could not get the string from file." << std::endl;
    return;
  }
  file_blocks.reserve((line.size() / 2) + 1);
  free_blocks.reserve((line.size() / 2) + 1);
  int i = 0;
  for (char c : line) {
    if (i++ % 2) {
      free_blocks.push_back(atoi(&c));
    } else {
      file_blocks.push_back(atoi(&c));
    }
  }

  std::cout << "Executing Part 1" << std::endl;
  uint64_t checksum = fileCompact(file_blocks, free_blocks);
  std::cout << "Checksum: " << checksum << std::endl;

  std::cout << "Executing Part 2" << std::endl;
  checksum = fileMove(file_blocks, free_blocks);
  std::cout << "Checksum: " << checksum << std::endl;
}
