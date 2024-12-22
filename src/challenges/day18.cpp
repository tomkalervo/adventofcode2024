/*
 * The problem of day is solved using BFS. Part 1 finds the shortest path from
 * start to end. Part 2 uses the shortest path algorithm from part 1. The goal
 * is to find the corrupted byte (in order from first to last) that prohibits
 * the creation of a path from start to end. By using binary search we are able
 * to narrow down the selection of corrupted bytes until we find the one byte
 * that stops a path from reaching the end.
 */
#include "../include/day18.h"
#include <cstdint>
#include <iostream>
#include <list>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

const bool DEBUG = false;

static void printMemory(std::vector<std::vector<char>> &memory) {
  for (auto row : memory) {
    for (char c : row) {
      std::cout << c;
    }
    std::cout << std::endl;
  }
}

static std::vector<std::vector<char>>
corruptMemory(const int32_t &grid_size, const int32_t &bytes,
              const std::list<std::pair<uint32_t, uint32_t>> &corrupt_bytes) {

  std::vector<std::vector<char>> memory(grid_size,
                                        std::vector<char>(grid_size, '.'));

  auto corrupt = corrupt_bytes.begin();
  for (int i = 0; i < bytes; i++) {
    memory[corrupt->first][corrupt->second] = '#';
    corrupt++;
    if (corrupt == corrupt_bytes.end()) {
      std::cerr << "Invalid length of corrupted bytes." << std::endl;
      exit(1);
    }
  }

  return memory;
}

static int32_t shortestPath(const std::pair<std::uint32_t, uint32_t> start,
                            const std::pair<uint32_t, uint32_t> end,
                            std::vector<std::vector<char>> &mem) {
  auto memory = mem;
  std::vector<std::vector<std::pair<uint32_t, uint32_t>>> path(
      memory.size(),
      std::vector<std::pair<uint32_t, uint32_t>>(memory[0].size(), {0, 0}));

  bool reached_end = false;
  memory[start.first][start.second] = '0';
  std::queue<std::pair<uint32_t, uint32_t>> fifo;
  fifo.push(start);

  while (fifo.size()) {
    auto p = fifo.front();
    fifo.pop();

    if (p == end) {
      if (DEBUG) {
        std::cout << "End reached" << std::endl;
      }
      reached_end = true;
      break;
    }

    // Find all adjecent steps.
    for (int32_t dy : {1, -1}) {
      if (dy + p.first < 0 || dy + p.first >= memory.size()) {
        continue;
      }
      uint32_t y = p.first + dy;
      if (memory[y][p.second] == '.') {
        path[y][p.second] = {p.first, p.second};
        memory[y][p.second] = '0';
        fifo.push({y, p.second});
      }
    }
    for (int32_t dx : {1, -1}) {
      if (dx + p.second < 0 || dx + p.second >= memory[p.first].size()) {
        continue;
      }
      uint32_t x = p.second + dx;
      if (memory[p.first][x] == '.') {
        path[p.first][x] = {p.first, p.second};
        memory[p.first][x] = '0';
        fifo.push({p.first, x});
      }
    }
  }

  int32_t steps = 0;
  if (reached_end) {
    auto step = end;
    mem[step.first][step.second] = '0';
    while (step != start) {
      steps++;
      step = path[step.first][step.second];
      mem[step.first][step.second] = '0';
    }
    if (DEBUG) {
      printMemory(mem);
    }
  }

  return steps;
}

static std::pair<uint32_t, uint32_t>
findStopByte(const std::pair<uint32_t, uint32_t> &start,
             const std::pair<uint32_t, uint32_t> &end,
             const std::list<std::pair<uint32_t, uint32_t>> &corrupt_bytes,
             uint32_t low, uint32_t high, uint32_t grid_size) {

  uint32_t mid = (high + low) / 2;
  while (low < high) {
    if (DEBUG) {
      std::cout << "High: " << high << " Low: " << low << ", Trying " << mid
                << " amount of corrypted bytes." << std::endl;
    }
    auto memory = corruptMemory(grid_size, mid, corrupt_bytes);
    if (shortestPath(start, end, memory)) {
      low = mid + 1;
    } else {
      high = mid;
    }
    mid = (high + low) / 2;
  }
  if (DEBUG) {
    std::cout << "Byte number " << low << " prohibits an exit." << std::endl;
  }
  auto byte = corrupt_bytes.begin();
  int i = 1;
  while (low > i) {
    if (DEBUG) {
      std::cout << "[" << i << "] \t (" << byte->second << "," << byte->first
                << ")" << std::endl;
    }
    byte++;
    i++;
  }
  if (DEBUG) {
    std::cout << "[" << i << "] \t (" << byte->second << "," << byte->first
              << ")" << std::endl;
  }
  return *byte;
}

void Day18::run(std::ifstream &file) {
  std::string line;
  std::list<std::pair<uint32_t, uint32_t>> corrupt_bytes;
  while (getline(file, line)) {
    std::istringstream iss(line);
    uint32_t x, y;
    char dummy;
    if (!(iss >> x >> dummy >> y)) {
      std::cerr << "Could not parse line: " << line << std::endl;
      return;
    }
    corrupt_bytes.push_back({y, x});
  }
  if (DEBUG) {
    std::cout << "Amount of corrupted bytes: " << corrupt_bytes.size()
              << std::endl;
  }

  // Setup. Modify for different inputs.
  uint32_t grid_size = 71;
  uint32_t bytes = 1024;
  std::pair<uint32_t, uint32_t> end = {70, 70};

  // Part 1.
  std::cout << "Executing Part 1" << std::endl;
  auto memory = corruptMemory(grid_size, bytes, corrupt_bytes);
  if (DEBUG) {
    printMemory(memory);
  }
  int64_t steps = shortestPath({0, 0}, end, memory);
  std::cout << "Least amount of steps needed to escape: " << steps << std::endl;

  // Part 2
  std::cout << "Executing Part 2" << std::endl;
  std::pair<uint32_t, uint32_t> stop_byte = findStopByte(
      {0, 0}, end, corrupt_bytes, bytes, corrupt_bytes.size() - 1, grid_size);
  std::cout << "The stop byte is (" << stop_byte.second << ","
            << stop_byte.first << ")" << std::endl;
}
