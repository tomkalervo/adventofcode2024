/*
 * The challenge of Day 7 was solved using recursion. Even though we reach
 * complexity of 3^(n-1) for part 2 it solves the input quite fast. An important
 * thing to watch out for is keeping the integers within the range of its data
 * type.
 */

#include "../include/day07.h"
#include <cassert>
#include <climits>
#include <cstdint>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

static uint64_t calibrate(uint64_t delta, const uint64_t &result,
                          std::vector<uint64_t> V, int idx,
                          const bool &concatenation) {
  // Overflow control
  if (delta > UINT64_MAX / 10) {
    std::cout << "WARNING for high value: " << delta << std::endl;
  }
  // Base-case: reached a too high delta result
  if (delta > result) {
    return 0;
  }
  // Base-case: all integers in vector V has been processed
  if (idx == V.size()) {
    if (delta == result) {
      return result;
    } else {
      return 0;
    }
  }
  // Recursive-case: we have integers left in vector V to process
  // First explore the addition path
  if (calibrate((uint64_t)(delta + V[idx]), result, V, idx + 1,
                concatenation)) {
    return result;
  }

  // Second explore the multiply path
  if (calibrate((uint64_t)(delta * V[idx]), result, V, idx + 1,
                concatenation)) {
    return result;
  }

  // A third path to explore that only is used in Part 2 of the problem
  if (concatenation) {
    // We need to concatinate integers: 34, 907 becomes 34907
    uint64_t multiplyer = 1;
    uint64_t tmp = V[idx];
    while (0 < tmp) {
      multiplyer *= 10;
      tmp /= 10;
    }
    // Check if Overflow
    if (delta > UINT64_MAX / multiplyer) {
      std::cout << "Overflow in concatenation of " << delta << " || " << V[idx]
                << ", when result is " << result << std::endl;
      return 0;
    }
    if (calibrate((delta * multiplyer) + V[idx], result, V, idx + 1,
                  concatenation)) {
      return result;
    }
  }

  return 0;
}

static uint64_t calibrate(const uint64_t &result,
                          const std::list<uint64_t> &parts,
                          const bool &concatenation) {
  if (parts.empty()) {
    return 0;
  }
  // Lets get a vector V to iterate over in an easier fashion
  std::vector<uint64_t> V(parts.begin(), parts.end());
  return calibrate(V[0], result, V, 1, concatenation);
}

void Day07::run(std::ifstream &file) {
  std::cout << "Executing Part 1 & Part 2" << std::endl;
  uint64_t sum = 0;
  uint64_t sum_part2 = 0;
  std::string line;
  while (getline(file, line)) {
    std::istringstream iss(line);
    uint64_t res;
    char dlm;
    if (!(iss >> res >> dlm)) {
      std::cerr << "Invalid format of line: " << line << std::endl;
      return;
    }
    if (':' != dlm) {
      std::cerr << "Invalid format of delimiter: " << dlm << std::endl;
      return;
    }
    std::list<uint64_t> parts;
    uint64_t p;
    while (iss >> p) {
      parts.push_back(p);
    }
    uint64_t delta = calibrate(res, parts, false);
    sum += delta;
    sum_part2 += delta;
    if (!delta) {
      // Part 2 of the problem
      delta = calibrate(res, parts, true);
      sum_part2 += delta;
    }
  }
  std::cout << "Part 1 total sum: " << sum << std::endl;
  std::cout << "Part 2 total sum: " << sum_part2 << std::endl;
}
