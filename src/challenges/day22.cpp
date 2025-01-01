#include "../include/day22.h"
#include <cstdint>
#include <iostream>
#include <list>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

using std::cout;
using std::endl;
using std::list;
using std::set;
using std::string;
using std::unordered_map;

const bool DEBUG = false;

inline void prune(uint64_t &a) {
  int64_t m = a % 16777216;
  a = m < 0 ? m + 16777216 : m;
}
inline void mix(uint64_t &a, uint64_t &b) { a = a ^ b; }

static void evolve(uint64_t &a) {
  // Step 1, Multiply
  uint64_t b = a << 6;
  mix(a, b);
  prune(a);
  // Step 2, Divide
  b = a >> 5;
  mix(a, b);
  prune(a);
  // Step 3, Multiply
  b = a << 11;
  mix(a, b);
  prune(a);
}

static void test() {
  int evolves = 10;
  uint64_t a = 123;
  for (int i = 1; i <= evolves; i++) {
    evolve(a);
    cout << i << ": " << a << endl;
  }
}

inline string numsToStr(const list<int8_t> &nums) {
  std::ostringstream oss;
  for (auto n : nums) {
    oss << (int)n << ',';
  }
  return oss.str();
}

static uint64_t predictAll(const list<uint64_t> &numbers, int e) {
  uint64_t sum = 0;
  for (auto n : numbers) {
    if (DEBUG) {
      cout << n << ": ";
    }
    int i = 0;
    while (i++ < e) {
      evolve(n);
    }
    if (DEBUG) {
      cout << n << endl;
    }
    sum += n;
  }
  return sum;
}

// Function finds the sequence that generates the most bananas, then sums up and
// returns the total bananas bought
static uint64_t maxBanana(const list<uint64_t> &numbers, int e) {
  uint64_t sum = 0;
  unordered_map<string, uint64_t> bananas;
  for (auto n : numbers) {
    // evolve number n e amount of times, for each evolve - construct 4-element
    // long sequence of differences leading up to and including that value.
    // Store its related selling value in a set to make sure it is only used
    // once per number. Add its value to the map of sequences to keep track of
    // the total selling value over all numbers
    list<int8_t> seq;
    set<string> used;
    auto p = n;
    int i = 0;
    while (i++ < e) {
      evolve(n);
      if (seq.size() < 3) {
        seq.push_back((n % 10) - (p % 10));
      } else if (seq.size() == 3) {
        seq.push_back((n % 10) - (p % 10));
        string seqstr = numsToStr(seq);
        bananas[seqstr] = n % 10;
      } else {
        seq.push_back((n % 10) - (p % 10));
        seq.pop_front();
        string seqstr = numsToStr(seq);
        if (used.find(seqstr) == used.end()) {
          // Only the first occurance of a sequence can be used as an buy-order
          used.insert(seqstr);
          if (bananas.find(seqstr) == bananas.end()) {
            bananas[seqstr] = n % 10;
          } else {
            bananas[seqstr] += n % 10;
          }
        }
      }
      p = n;
    }
  }
  string best_seq = "";

  for (auto [sq, bn] : bananas) {
    if (bn > sum) {
      sum = bn;
      best_seq = sq;
    }
  }

  if (DEBUG) {
    cout << "Best sequence is: " << best_seq.substr(0, best_seq.size() - 1)
         << " resulting in a total of " << sum << " bought bananas." << endl;
  }
  return sum;
}

void Day22::run(std::ifstream &file) {
  // test();
  list<uint64_t> numbers;
  string line;
  while (getline(file, line)) {
    numbers.push_back(stoi(line));
  }
  cout << "Executing Part 1" << endl;
  uint64_t sum = predictAll(numbers, 2000);
  cout << "Sum from predicting numbers: " << sum << endl;

  // list<int8_t> test_n = {1, -2, 0, 6};
  // string testString = numsToStr(test_n);
  // cout << "Test: " << testString << endl;

  cout << "Executing Part 2" << endl;
  sum = maxBanana(numbers, 2000);
  cout << "Max amount of bananas: " << sum << endl;
}
