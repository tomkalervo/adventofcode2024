#include "../include/day19.h"
#include <cstdint>
#include <iostream>
#include <list>
#include <stack>
#include <string>
#include <utility>
#include <vector>

const bool DEBUG = false;

static void insertionSort(std::vector<std::string> &patterns) {
  for (int i = 1; i < patterns.size(); i++) {
    for (int j = 0; j < i; j++) {
      if (patterns[i] < patterns[j]) {
        std::string tmp = patterns[i];
        patterns[i] = patterns[j];
        patterns[j] = tmp;
        continue;
      }
    }
  }
}

static bool validPart(const std::string &p, const std::vector<std::string> &d) {
  int32_t low = 0;
  int32_t high = d.size();
  while (low <= high) {
    int32_t mid = (low + high) / 2;
    if (p == d[mid]) {
      return true;
    } else if (p < d[mid]) {
      high = mid - 1;
    } else {
      low = mid + 1;
    }
  }
  return false;
}
/* A DFS using memoization to keep track of already checked substrings.
 * The parts list keep track of the index to the start of substrings included in
 * a successful combination.
 */
static int64_t validDesign(const std::string &design,
                           const std::vector<std::string> &patterns) {
  std::list<int32_t> parts;
  std::vector<int64_t> combo_map(design.size(), 0);
  std::vector<bool> checked(design.size(), false);
  std::stack<std::pair<int32_t, int32_t>> interval;
  interval.push({0, design.size()});
  parts.push_back(0);
  while (interval.size()) {
    auto [low, high] = interval.top();
    interval.pop();
    parts.pop_back();
    // base cases
    if (low == design.size()) {
      // when low has reached max size a successful combination of substrings
      // have been found
      for (int32_t i : parts) {
        combo_map[i]++;
      }
      continue;
    }
    if (high == low) {
      // we cannot progress further at the current index.
      checked[low] = true;
      continue;
    }
    if (checked[low]) {
      // we have already examined all substrings leading from this index (low)
      for (int32_t i : parts) {
        combo_map[i] += combo_map[low];
      }
      continue;
    }
    // recursive cases
    std::string part = design.substr(low, high - low);
    parts.push_back(low);
    if (validPart(part, patterns)) {
      interval.push({low, high - 1});
      interval.push({high, design.size()});
      parts.push_back(high);
    } else {
      interval.push({low, high - 1});
    }
  }

  return combo_map.at(0);
}

static void analyzeTowels(const std::vector<std::string> &patterns,
                          const std::list<std::string> &designs,
                          std::list<std::string> &valid,
                          std::list<std::string> &invalid) {
  for (const std::string design : designs) {
    int64_t combos = validDesign(design, patterns);
    // std::cerr << "Validated Design with combo of " << combos << std::endl;
    if (combos > 0) {
      valid.push_back(design);
    } else {
      invalid.push_back(design);
    }
  }
  return;
}
void Day19::run(std::ifstream &file) {
  std::string line;
  std::list<std::string> designs;
  std::vector<std::string> patterns;
  // Fetch all valid patterns
  while (getline(file, line)) {
    if (line == "") {
      break;
    }
    std::string pattern;
    for (char c : line) {
      if (c == ',') {
        patterns.push_back(pattern);
        continue;
      }
      if (c == ' ') {
        pattern = "";
        continue;
      }
      pattern.push_back(c);
    }
    patterns.push_back(pattern);
  }
  // Fetch all wanted designs
  while (getline(file, line)) {
    designs.push_back(line);
  }
  // Sort the patterns (no need for a fancy sort)
  insertionSort(patterns);

  // Analyze the designs
  std::cout << "Executing Part 1" << std::endl;
  std::list<std::string> valid;
  std::list<std::string> invalid;
  analyzeTowels(patterns, designs, valid, invalid);
  std::cout << "Amount of valid designs: " << valid.size() << std::endl;

  // Second part, we redo the calculations of the valid designs
  std::cout << "Executing Part 2" << std::endl;
  uint64_t sum = 0;
  for (auto design : valid) {
    sum += validDesign(design, patterns);
  }
  std::cout << "Amount of valid combinations: " << sum << std::endl;
}
