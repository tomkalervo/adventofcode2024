#include "../include/day21.h"
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

const bool DEBUG = false;

const std::vector<char> NumPad{'7', '8', '9', '4', '5', '6',
                               '1', '2', '3', '#', '0', 'A'};
/*
 *  +---+---+---+
 *  |   | ^ | A |
 *  +---+---+---+
 *  | < | v | > |
 *  +---+---+---+
 *
 *  +---+---+---+
 *  | 0 | 1 | 2 |
 *  +---+---+---+
 *  | 3 | 4 | 5 |
 *  +---+---+---+
 */
const std::vector<char> KeyPad{'#', '^', 'A', '<', 'v', '>'};

static std::pair<char, char> getDistance(const char &from, const char &to) {
  // return (x,y) distance vector
  return {(to % 3) - (from % 3), (to / 3) - (from / 3)};
}

static int32_t number(const std::string &s) {
  int32_t number = 0;
  for (char c : s) {
    int32_t n = c - '0';
    if (n < 10 && n >= 0) {
      number *= 10;
      number += n;
    }
  }
  return number;
}

static std::vector<char> codeFromString(const std::string &s) {
  std::vector<char> code;
  code.push_back(11); // Add A as start position
  for (char c : s) {
    for (int i = 0; i < NumPad.size(); i++) {
      if (NumPad[i] == c) {
        code.push_back(i);
      }
    }
  }
  return code;
}

static std::string getSequence(const char &from, const char &to) {
  std::string seq;
  if (from == to) {
    seq.push_back(2);
    return seq;
  }
  auto [x, y] = getDistance(from, to);
  while (x > 0 && y == 0) {
    seq.push_back(5);
    x--;
  }
  while (x < 0 && y == 0) {
    seq.push_back(3);
    x++;
  }
  while (y < 0 && x == 0) {
    seq.push_back(1);
    y++;
  }
  while (y > 0 && x == 0) {
    seq.push_back(4);
    y--;
  }
  // Left 3 > Up/down 1/4 > Right 5
  if (x < 0) {
    if (to == 3) {
      // we want to move to '<' but are on the 'upper' row. This would lead
      // to a prohibited move if not first drop down.
      assert(y > 0);
      while (y > 0) {
        seq.push_back(4);
        y--;
      }
    }
    while (x < 0) {
      seq.push_back(3);
      x++;
    }
  }
  if (y < 0) {
    if (from == 3) {
      // move up before moving right prohibited
      assert(x > 0);
      while (x > 0) {
        seq.push_back(5);
        x--;
      }
    }
    while (y < 0) {
      seq.push_back(1);
      y++;
    }
  }
  while (y > 0) {
    seq.push_back(4);
    y--;
  }
  while (x > 0) {
    seq.push_back(5);
    x--;
  }
  // final A
  seq.push_back(2);
  return seq;
}

/*
 * The recursive part. Returns the amount of directional keys needed to be
 * pressed at the required depth. Uses memoization to save re-calculation of
 * already explored paths.
 */
static uint64_t deepSearch(char from, char to, std::vector<std::string> &map,
                           std::unordered_map<std::string, uint64_t> &memo,
                           int32_t d) {
  int8_t idx = (from * 6) + to;
  if (map[idx].empty()) {
    map[idx] = getSequence(from, to);
  }

  if (d == 0) {
    return map[idx].size();
  }

  std::string seq = (char)2 + map[idx];

  // add 6*6 = 36 to seq to avoid integers related to keypad
  auto it = memo.find(std::string(char(d + 36) + seq));
  if (it != memo.end()) {
    return it->second;
  }

  uint64_t acc = 0;
  for (int i = 0; i < seq.size() - 1; i++) {
    acc += deepSearch(seq[i], seq[i + 1], map, memo, d - 1);
  }

  memo[std::string(char(d + 36) + seq)] = acc;

  return acc;
}

/* Kicks off a recursive search for keypresses. Going from a to b, seq[i] to
 * seq[i+1], through the required depth of robots. Returns the amount of presses
 * for the seq string.
 */
static uint64_t deepSearch(std::string seq, std::vector<std::string> &map,
                           const int32_t &depth) {
  std::unordered_map<std::string, uint64_t> memo;
  seq = (char)2 + seq;
  uint64_t acc = 0;
  for (int i = 0; i < seq.size() - 1; i++) {
    acc += deepSearch(seq[i], seq[i + 1], map, memo, depth - 1);
  }

  return acc;
}

/* This function converts the string code into an inital sequence of
 * directional keypresses. Order matter, never pass x=0,y=max.
 */
static std::string numSequence(const std::string &code_string) {
  std::vector<char> code_vector = codeFromString(code_string);
  std::string sequence;
  for (int i = 0; i < code_vector.size() - 1; i++) {
    auto [x, y] = getDistance(code_vector[i], code_vector[i + 1]);
    if (DEBUG) {
      std::cout << "got x: " << (int)x << " y: " << (int)y << " from "
                << (int)code_vector[i] << " to " << (int)code_vector[i + 1]
                << std::endl;
    }
    if (y == 0) {
      while (x > 0) {
        sequence.push_back(5);
        x--;
      }
      while (x < 0) {
        sequence.push_back(3);
        x++;
      }
    }
    if (x == 0) {
      while (y < 0) {
        sequence.push_back(1);
        y++;
      }
      while (y > 0) {
        sequence.push_back(4);
        y--;
      }
    }
    if (code_vector[i] / 3 == 3 && code_vector[i + 1] % 3 == 0) {
      // handle prohibited move from bottom row to leftmost column.
      assert(y < 0);
      while (y < 0) {
        sequence.push_back(1);
        y++;
      }
    }
    while (x < 0) {
      sequence.push_back(3);
      x++;
    }
    if (code_vector[i] % 3 == 0 && code_vector[i + 1] / 3 == 3) {
      // handle prohibited move from leftmost column to bottom row
      assert(x > 0);
      while (x > 0) {
        sequence.push_back(5);
        x--;
      }
    }
    while (y < 0) {
      sequence.push_back(1);
      y++;
    }
    while (y > 0) {
      sequence.push_back(4);
      y--;
    }
    while (x > 0) {
      sequence.push_back(5);
      x--;
    }
    sequence.push_back(2);
  }
  return sequence;
}

static std::list<uint64_t> operation(const std::list<std::string> &codes,
                                     const uint32_t &robots) {
  std::list<uint64_t> presses;
  std::vector<std::string> map(6 * 6, "");
  for (auto sequence : codes) {
    sequence = numSequence(sequence);
    uint64_t press = deepSearch(sequence, map, robots);
    presses.push_back(press);
  }
  return presses;
}

void Day21::run(std::ifstream &file) {
  std::string line;
  std::list<std::string> codes;
  std::list<int32_t> num;
  while (getline(file, line)) {
    codes.push_back(line);
    num.push_back(number(line));
  }

  std::cout << "Executing Part 1" << std::endl;
  auto lengths = operation(codes, 2);
  assert(num.size() == lengths.size());
  uint64_t sum = 0;
  for (int32_t n : num) {
    if (DEBUG) {
      std::cout << "Length: " << lengths.front() << ", number: " << n
                << std::endl;
    }
    sum += lengths.front() * n;
    lengths.pop_front();
  }
  std::cout << "Sum of complexities: " << sum << std::endl;

  std::cout << "Executing Part 2" << std::endl;
  lengths = operation(codes, 25);
  assert(num.size() == lengths.size());
  sum = 0;
  for (uint64_t n : num) {
    if (DEBUG) {
      std::cout << "Length: " << lengths.front() << ", number: " << n
                << std::endl;
    }
    sum += lengths.front() * n;
    lengths.pop_front();
  }
  std::cout << "Sum of complexities: " << sum << std::endl;
}
