#include "../include/day17.h"
#include <bitset>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>

const bool DEBUG = false;
bool printer = false;

struct Register {
  int64_t A, B, C;
  Register() : A(0), B(0), C(0) {}
};
// Global register
Register reg;

struct OutCache {
  std::vector<int32_t> printCache;
  int32_t idx;
  OutCache() : idx(-1) {}
  void clear() {
    printCache.clear();
    idx = -1;
  }
  int32_t size() { return printCache.size(); }
  int32_t empty() { return printCache.empty(); }
  int32_t at(int32_t i) { return printCache.at(i); }
  int32_t last() { return printCache.back(); }
  int32_t top() { return printCache.front(); }
  void add(int32_t v) {
    if (printer) {
      std::cout << std::bitset<64>(v) << " " << v << std::endl;
    }
    printCache.push_back(v);
    idx++;
  }
  void print() {
    if (printCache.empty()) {
      return;
    }
    auto v = printCache.begin();
    std::cout << *v;
    v++;
    while (v != printCache.end()) {
      std::cout << "," << *v;
      v++;
    }
    std::cout << std::endl;
  }
};
// Global print cache
OutCache out;

static int64_t combo(const int32_t value) {
  switch (value) {
  case 0 ... 3:
    return value;
  case 4:
    return reg.A;
  case 5:
    return reg.B;
  case 6:
    return reg.C;
  case 7:
    std::cerr << "Invalid combo value 7" << std::endl;
    exit(-1);
    return -1;
  default:
    std::cerr << "Error value in combo" << value << std::endl;
    exit(-1);
    return -1;
  }
}

static int64_t opDiv(const int64_t &value) {
  return std::floor(reg.A / std::pow(2, value));
}

static void exec(const std::vector<int32_t> &program) {
  if (printer) {
    std::cout << std::bitset<64>(reg.A) << " A" << std::endl;
    std::cout << std::bitset<64>(reg.B) << " B" << std::endl;
    std::cout << std::bitset<64>(reg.C) << " C" << std::endl;
  }
  int32_t pc = 0;
  while (pc < program.size()) {
    int32_t opcode = program[pc++];
    switch (opcode) {
    case 0: {
      // Division
      reg.A = opDiv(combo(program[pc++]));
      break;
    }
    case 1:
      // Bitwise XOR
      reg.B ^= program[pc++];
      break;
    case 2:
      // Modulo 8
      reg.B = combo(program[pc++]) % 8;
      break;
    case 3:
      // jump not zero
      if (reg.A) {
        pc = program[pc];
        continue;
      }
      pc++;
      break;
    case 4:
      // Bitwise XOR
      reg.B ^= reg.C;
      pc++;
      break;
    case 5:
      // Output
      out.add(combo(program[pc++]) % 8);
      break;
    case 6:
      // Division to B
      reg.B = opDiv(combo(program[pc++]));
      break;
    case 7:
      // Division to C
      reg.C = opDiv(combo(program[pc++]));
      break;
    default: {
      std::cerr << "Invalid opcode: " << opcode << std::endl;
      return;
    }
    }
    if (printer) {
      std::cout << std::bitset<64>(reg.A) << " A" << std::endl;
      std::cout << std::bitset<64>(reg.B) << " B" << std::endl;
      std::cout << std::bitset<64>(reg.C) << " C" << std::endl;
    }
  }
}

/*
 * Part 2, analyze the program to find a specific input
 */

static void verify(const std::vector<int32_t> &program, const int64_t &a) {
  if (a < 0) {
    std::cerr << "Cannot enter negative value into register A: " << a
              << std::endl;
    return;
  }
  if (DEBUG) {
    for (int i = 1; i <= 64; i++) {
      if (0 == (64 - i) % 10) {
        printf("%d", (64 - i) / 10);
      } else {
        printf("-");
      }
    }
    printf("\n");
    for (int i = 1; i <= 64; i++) {
      printf("%d", (64 - i) % 10);
    }
    printf("\n");
    std::cout << std::bitset<64>(a) << std::endl;
    for (int i = 0; i < 64; i++) {
      printf("-");
    }
    printf("\n");
  }

  reg.A = a;
  reg.B = 0;
  reg.C = 0;
  out.clear();
  exec(program);
}

static bool literalSearch(const std::vector<int32_t> &program, int32_t &pos,
                          int64_t &input, int64_t bits) {
  if (pos < 0) {
    for (int i = 0; i < program.size(); i++) {
      if (program[i] != out.at(i)) {
        return false;
      }
    }
    return true;
  }
  if (pos == program.size()) {
    std::cerr << "pos to large: " << pos << std::endl;
    return false;
  }
  if (DEBUG) {
    std::cout << "Literal Search at pos " << pos
              << ". Program: " << program[pos] << std::endl;
  }
  int32_t position = pos;
  int64_t shift = input << 3;
  for (int64_t i = bits; i < 0x7; i++) {
    // Funny note: Accidently used 0x7 instead of 0x8 in the for loop. Only
    // going to 0b110 turned to be a success. Going to 0b111 makes infinit loop.
    // Haven't figured out why.
    verify(program, shift | i);
    if (program[pos] == out.top()) {
      input = shift | i;
      pos--;
      return literalSearch(program, pos, input, 0);
    }
  }
  if (DEBUG) {
    std::cout << "Could not produce correct output: " << program[pos]
              << std::endl;
    out.print();
  }
  pos++;
  bits = (1 + (0x7 & input)) & 0x7;
  input >>= 3;
  return literalSearch(program, pos, input, bits);
}

/*
 * This function kicks off the analysis of part 2. From inspection of the
 * program we can see that register A is shifted 3 bits between every print
 * iteration. Thus, we do a search for a 3-bit combination for every literal
 * needed.
 */
static void analyze(const std::vector<int32_t> &program) {
  std::vector<std::bitset<64>> dependencies;

  int32_t idx = program.size() - 1;
  int64_t bits = 0LL;
  bool success = literalSearch(program, idx, bits, 0);
  if (DEBUG) {
    printer = true;
    verify(program, bits);
    out.print();
  }
  if (success) {
    std::cout << "Success, Wow! value: " << bits << std::endl;
  } else {
    std::cout << "No wow. value: " << bits << std::endl;
  }
}

void Day17::run(std::ifstream &filestream) {
  std::string line;
  std::vector<int32_t> program;

  while (getline(filestream, line)) {
    if (line == "") {
      continue;
    }
    std::istringstream iss(line);
    std::string target;
    if (!(iss >> target)) {
      std::cerr << "Could not read target from line: " << line << std::endl;
    }
    if (target == "Register") {
      int32_t value;
      if (!(iss >> target >> value)) {
        std::cerr << "Could not read register from line: " << line << std::endl;
      }
      if (target == "A:") {
        reg.A = value;
      } else if (target == "B:") {
        reg.B = value;
      } else if (target == "C:") {
        reg.C = value;
      } else {
        std::cerr << "Invalid register read: " << target << std::endl;
      }

    } else if (target == "Program:") {
      int32_t literal;
      char dummy;
      while (iss >> literal) {
        program.push_back(literal);
        if (!(iss >> dummy)) {
          break;
        }
      }
    } else {
      std::cerr << "Target not matched: " << target << std::endl;
    }
  }
  if (DEBUG) {
    std::cout << "Register load. A: " << reg.A << ", B: " << reg.B
              << ", C: " << reg.C << std::endl;
    std::cout << "Program code: ";
    for (int32_t in : program) {
      std::cout << in << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "Executing program Part 1" << std::endl;
  exec(program);
  std::cout << "Program out:" << std::endl;
  out.print();
  std::cout << "Running program analysis Part 2" << std::endl;
  analyze(program);
}
