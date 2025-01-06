#include "../include/day25.h"
#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using std::array;
using std::cout;
using std::endl;
using std::sort;
using std::string;
using std::vector;

static uint64_t combinations(const vector<array<uint32_t, 5>> &locks,
                             const vector<array<uint32_t, 5>> &keys) {
  // Perform a sort on locks and keys to increase effeciency for comparisons
  int idx = 0;
  auto arraySort = [&idx](array<uint32_t, 5> a, array<uint32_t, 5> b) {
    return a[idx] < b[idx];
  };

  auto sort_keys = keys;
  auto sort_locks = locks;
  vector<array<uint32_t, 5>> matched;

  uint64_t combos = 0;
  for (auto lock : locks) {
    for (idx = 0; idx < 5; idx++) {
      sort(sort_keys.begin(), sort_keys.end(), arraySort);
      sort(sort_locks.begin(), sort_locks.end(), arraySort);
      for (int j = 0; j < sort_keys.size(); j++) {
        if (lock[idx] + sort_keys[j][idx] > 5) {
          break;
        }
        matched.push_back(sort_keys[j]);
      }
      sort_keys = matched;
      matched.clear();
    }
    combos += sort_keys.size();
    sort_keys = keys;
  }
  return combos;
}

void Day25::run(std::ifstream &file) {
  vector<array<uint32_t, 5>> keys;
  vector<array<uint32_t, 5>> locks;

  // Lambda for constructing generic part (that can either be key or lock) from
  // input filestream
  auto construct = [&file](vector<array<uint32_t, 5>> &parts) {
    int len = 0;
    array<uint32_t, 5> part = {0, 0, 0, 0, 0};
    string line;
    while (len++ < 6) {
      if (!(getline(file, line))) {
        std::cerr << "Could not parse line" << endl;
        exit(1);
      }
      if (len == 6) {
        // last row do not affect the part
        continue;
      }
      for (int i = 0; i < line.size(); i++) {
        if (line[i] == '#') {
          part[i] += 1;
        }
      }
    }
    parts.push_back(part);
  };

  // Parse through the input filestream and construct keys and locks
  string lock_str = "#####";
  string key_str = ".....";
  string line;
  while (getline(file, line)) {
    if (line == key_str) {
      construct(keys);
    } else if (line == lock_str) {
      construct(locks);
    } else {
    }
  }

  // for (auto key : keys) {
  //   cout << "Key: ";
  //   for (auto i : key) {
  //     cout << i << " ";
  //   }
  //   cout << endl;
  // }
  // for (auto lock : locks) {
  //   cout << "Lock: ";
  //   for (auto i : lock) {
  //     cout << i << " ";
  //   }
  //   cout << endl;
  // }

  cout << "Executing Part 1" << endl;
  uint64_t combos = combinations(locks, keys);
  cout << "Amount of possible combinations are: " << combos << endl;
}
