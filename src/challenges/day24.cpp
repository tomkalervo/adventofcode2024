#include "../include/day24.h"
#include <assert.h>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <unordered_map>

using std::cout;
using std::endl;
using std::isdigit;
using std::list;
using std::string;
using std::swap;
using std::unordered_map;

struct Gate {
  string op;
  string a;
  string b;
  Gate(string op_in, string a_in, string b_in) : op(op_in), a(a_in), b(b_in){};
  // Dummy-constructor for unordered_map-compile checks
  Gate() : op(""), a(""), b(""){};

  // **Copy Assignment Operator** (Defaulted)
  // Needed for swap-operation
  Gate &operator=(const Gate &) = default;
};

/* Part 1 functions. Simulate the gates output.
 */

static int8_t simulate(unordered_map<string, Gate> &gates,
                       unordered_map<string, int8_t> &active, string out) {
  {
    auto it = active.find(out);
    if (it != active.end()) {
      return it->second;
    }
  }
  auto it = gates.find(out);
  assert(it != gates.end());
  auto g = it->second;
  auto a = simulate(gates, active, g.a);
  auto b = simulate(gates, active, g.b);
  if (g.op == "OR") {
    active[out] = a | b;
  } else if (g.op == "XOR") {
    active[out] = a ^ b;
  } else if (g.op == "AND") {
    active[out] = a & b;
  } else {
    std::cerr << "Invalid operation: " << g.op << endl;
    exit(2);
  }
  return active[out];
}

static uint64_t simulateState(unordered_map<string, Gate> &gates,
                              unordered_map<string, int8_t> active,
                              const list<string> &zs) {
  uint64_t value = 0;
  int i = 0;
  for (auto z : zs) {
    auto v = simulate(gates, active, z);
    value |= ((uint64_t)v << i);
    i++;
  }

  return value;
}

/* Part 2. Analyze gates that perform addition. The table below demonstrate the
 * addition of an x-value to an y-value resulting in the sum z-value. We have a
 * carry-bit, c-value.
 * +---+---+---+---+---+---+---------------+
 * | 5 | 4 | 3 | 2 | 1 | 0 | i, index      |
 * +---+---+---+---+---+---+---------------+
 * | 1 | 1 | 0 | 1 | 1 | - | C, carry      |
 * +---+---+---+---+---+---+---------------+
 * | - | 1 | 1 | 0 | 1 | 1 | X, x-value 27 |
 * +---+---+---+---+---+---+---------------+
 * | - | 0 | 1 | 0 | 1 | 1 | Y, y-value 11 |
 * +---+---+---+---+---+---+---------------+
 * | 1 | 0 | 0 | 1 | 1 | 0 | Z, z-value 38 |
 * +---+---+---+---+---+---+---------------+
 *
 * The general case is that
 * 1. z_i is the result of XOR c_i,
 *                            (XOR x_i, y_i)
 * 2. c_i is the result of OR
 *                          (AND
 *                            (XOR x_(i-1), y_(i-1)),
 *                            c_(i-1))
 *                          (AND x_(i-1), y_(i-1))
 * Edgecases involves z_0,z_1 and Z_n;
 * 1. z_0 do not depend on Carry: XOR x00, y00
 * 2. z_1; c_i do not depend on c_(i-1)
 * 3. z_n do not depend on x_n,y_n : z_n = c_n
 *
 * z_2 from test input:
 * bfd XOR mrk -> z02
 *   ktq OR fhk -> mrk
 *     wbd AND hfr -> fhk
 *       y01 XOR x01 -> hfr
 *       y00 AND x00 -> wbd
 *     x01 AND y01 -> ktq
 *   x02 XOR y02 -> bfd
 */

/* Returns true if a and b has the number i as a suffix.
 * a and b are wire names of length 3.
 */
static bool xy_in(const string &a, const string &b, const int &i) {

  // printf("Check xy: %s, %s, with i %d\n", a.c_str(), b.c_str(), i);
  if (!(isdigit(a[1]) && isdigit(a[2]))) {
    return false;
  }
  if (a.substr(1, 2) != b.substr(1, 2)) {
    return false;
  }
  if (i != stoi(a.substr(1, 2))) {
    return false;
  }
  return true;
}

static bool xy_op(const unordered_map<string, Gate> &gates, const string &out,
                  const int &i, const string &op) {
  // printf("Check %s_%d\n", op.c_str(), i);
  auto it = gates.find(out);
  if (it == gates.end()) {
    return false;
  }
  Gate g = it->second;
  if (g.op != op) {
    return false;
  }
  return xy_in(g.a, g.b, i);
}

/* Return true if gate is a correctly implemented carry
 */
static bool carry(const unordered_map<string, Gate> &gates, const string &out,
                  const int &i) {
  // printf("Check c_%d\n", i);
  auto it = gates.find(out);
  if (it == gates.end()) {
    return false;
  }

  auto direct = [&gates](const string &out, const int &i) {
    // printf("Check direct c_%d\n", i);
    auto it = gates.find(out);
    if (it == gates.end()) {
      return false;
    }
    Gate g = it->second;
    if (g.op != "AND") {
      return false;
    }
    return xy_in(g.a, g.b, i);
  };

  auto indirect = [&gates](const string &out, const int &i) {
    // printf("Check indirect c_%d\n", i);
    auto it = gates.find(out);
    if (it == gates.end()) {
      return false;
    }
    Gate g = it->second;
    if (g.op != "AND") {
      return false;
    }
    return (xy_op(gates, g.a, i, "XOR") && carry(gates, g.b, i)) ||
           (xy_op(gates, g.b, i, "XOR") && carry(gates, g.a, i));
  };

  Gate g = it->second;
  // Edgecase z_01
  if (i == 1) {
    return direct(out, i - 1);
  }
  // General case, carry <= AND direct-bit, indirect-bit
  if (g.op != "OR") {
    return false;
  }
  return (direct(g.a, i - 1) && indirect(g.b, i - 1)) ||
         (direct(g.b, i - 1) && indirect(g.a, i - 1));
}

/* Return true if supplied z_i has the correct structure of gates
 */
static bool checkZi(const unordered_map<string, Gate> &gates, const int &i) {
  string out = "z";
  out += (char)((i / 10) + 0x30);
  out += (char)((i % 10) + 0x30);
  // printf("Check %s\n", out.c_str());
  if (i == 0) {
    return xy_op(gates, out, i, "XOR");
  } else if (i == 45) {
    return carry(gates, out, i);
  } else {
    // General case
    auto it = gates.find(out);
    if (it == gates.end()) {
      return false;
    }
    Gate g = it->second;
    if (g.op != "XOR") {
      return false;
    }
    return (xy_op(gates, g.a, i, "XOR") && carry(gates, g.b, i)) ||
           (xy_op(gates, g.b, i, "XOR") && carry(gates, g.a, i));
  }
}

/* returns the gates that needs to be swapped, in a comma-seperated value list,
 * ordered alphabetically
 */
static string reWire(const unordered_map<string, Gate> &gates,
                     const list<string> &xs, const list<string> &ys,
                     const list<string> &zs) {
  // Lambda to iterate over each z_i
  auto check = [](const auto &g) {
    int i = 0;
    while (checkZi(g, i)) {
      i++;
    }
    return i;
  };

  auto swapped_gates = gates;
  list<string> swapped;
  for (int i = 0; i < 4; i++) {
    int valid = check(swapped_gates);
    int progress = valid;
    auto ita = swapped_gates.begin();
    while (ita != swapped_gates.end() && progress <= valid) {
      auto itb = ita;
      itb++;
      while (itb != swapped_gates.end() && progress <= valid) {
        swap(swapped_gates[ita->first], swapped_gates[itb->first]);
        progress = check(swapped_gates);
        if (progress > valid) {
          auto it = swapped.begin();
          while (it != swapped.end()) {
            if (ita->first < *it) {
              break;
            }
            it++;
          }
          swapped.insert(it, ita->first);
          it = swapped.begin();
          while (it != swapped.end()) {
            if (itb->first < *it) {
              break;
            }
            it++;
          }
          swapped.insert(it, itb->first);
          break;
        }
        swap(swapped_gates[ita->first], swapped_gates[itb->first]);
        itb++;
      }
      ita++;
    }
    printf("Search and swap [%d] - correct gates [%d/46]\n", i + 1, progress);
    if (progress > valid) {
      valid = progress;
    } else {
      printf("no progress at %d.\n", i);
      break;
    }
  }
  string csv = "";
  for (auto s : swapped) {
    csv += s + ",";
  }

  return csv.substr(0, csv.length() - 1);
}

void Day24::run(std::ifstream &file) {
  unordered_map<string, Gate> gates;
  unordered_map<string, int8_t> active;
  list<string> zs;
  list<string> xs;
  list<string> ys;
  string line;
  while (getline(file, line)) {
    if (line == "") {
      break;
    }
    string out = line.substr(0, 3);
    int8_t value = line.substr(5, 1) == "1" ? 1 : 0;
    active[out] = value;
    if (out[0] == 'x') {
      auto it = xs.begin();
      while (it != xs.end()) {
        if (out < *it) {
          break;
        }
        it++;
      }
      xs.insert(it, out);
    } else if (out[0] == 'y') {
      auto it = ys.begin();
      while (it != ys.end()) {
        if (out < *it) {
          break;
        }
        it++;
      }
      ys.insert(it, out);
    }
  }
  while (getline(file, line)) {
    string a, op, b, dummy, out;
    std::istringstream iss(line);
    if (!(iss >> a >> op >> b >> dummy >> out)) {
      std::cerr << "Could not parse line: " << line << endl;
      exit(1);
    }
    gates.emplace(out, Gate(op, a, b));

    if (out[0] == 'z') {
      auto it = zs.begin();
      while (it != zs.end()) {
        if (out < *it) {
          break;
        }
        it++;
      }
      zs.insert(it, out);
    }
  }

  cout << "Executing Part 1" << endl;
  uint64_t state = simulateState(gates, active, zs);
  cout << "Final state value: " << state << endl;
  cout << "Executing Part 2" << endl;
  string corrected_outputs = reWire(gates, xs, ys, zs);
  cout << "Gates to swap: " << corrected_outputs << endl;
}
