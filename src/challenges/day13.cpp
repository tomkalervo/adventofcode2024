#include "../include/day13.h"
#include <assert.h>
#include <climits>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <list>
#include <sstream>
#include <unistd.h>

const bool DEBUG_PRINT = false;

struct Position {
  uint64_t x;
  uint64_t y;
  Position(uint64_t x_in, uint64_t y_in) : x(x_in), y(y_in) {}
};

struct ClawMachine {
  const Position target;
  const Position buttonA;
  const Position buttonB;
  Position claw;
  ClawMachine(Position target_in, Position buttonB_in, Position buttonA_in)
      : target(target_in), buttonB(buttonB_in), buttonA(buttonA_in),
        claw(0, 0) {}
  void pressA() {
    claw.x += buttonA.x;
    claw.y += buttonA.y;
  }
  void pressB() {
    claw.x += buttonB.x;
    claw.y += buttonB.y;
  }
  void undoPressA() {
    claw.x -= buttonA.x;
    claw.y -= buttonA.y;
  }
  void undoPressB() {
    claw.x -= buttonB.x;
    claw.y -= buttonB.y;
  }
};

static void printMachine(const ClawMachine &m) {
  std::cout << "***********************************" << std::endl;
  std::cout << "Prize located at X: " << m.target.x << " Y: " << m.target.y
            << "\n";
  std::cout << "Button A movement X: " << m.buttonA.x << " Y: " << m.buttonA.y
            << "\n";
  std::cout << "Button B movement X: " << m.buttonB.x << " Y: " << m.buttonB.y
            << "\n";
  std::cout << "Claw located at X: " << m.claw.x << " Y: " << m.claw.y << "\n";
  std::cout << "***********************************" << std::endl;
}

/*
 * A naive approach used for Part 1
 */
static uint64_t getEmAll(std::list<ClawMachine> &machines) {
  uint64_t tokens = 0;
  for (ClawMachine &m : machines) {
    uint32_t pressB = 0;
    uint32_t pressA = 0;
    bool a_cap = false;
    bool b_cap = false;
    float cost_relation =
        (m.buttonA.x + m.buttonA.y) / (3.f * (m.buttonB.x + m.buttonB.y));
    if (cost_relation < 1) {
      // It is most cost effecient to max amount of B presses
      float reach_X = (float)m.target.x / m.buttonB.x;
      float reach_Y = (float)m.target.y / m.buttonB.y;

      pressB = reach_X < reach_Y ? floor(reach_X) : floor(reach_Y);
      if (pressB > 100) {
        pressB = 100;
      }
      b_cap = true;
      m.claw.x += m.buttonB.x * pressB;
      m.claw.y += m.buttonB.y * pressB;
      if (m.claw.x + (m.buttonA.x * 100) < m.target.x) {
        continue;
      }
      if (m.claw.y + (m.buttonA.y * 100) < m.target.y) {
        continue;
      }
    } else {
      float reach_X = (float)m.target.x / m.buttonA.x;
      float reach_Y = (float)m.target.y / m.buttonA.y;

      pressA = reach_X < reach_Y ? floor(reach_X) : floor(reach_Y);
      if (pressA > 100) {
        pressA = 100;
      }
      a_cap = true;
      m.claw.x += m.buttonA.x * pressA;
      m.claw.y += m.buttonA.y * pressA;
      if (m.claw.x + (m.buttonB.x * 100) < m.target.x) {
        continue;
      }
      if (m.claw.y + (m.buttonB.y * 100) < m.target.y) {
        continue;
      }
    }
    if (DEBUG_PRINT) {
      std::cout << "Start with pressing: B " << pressB << " times and A "
                << pressA << "times." << std::endl;
    }
    if (DEBUG_PRINT) {
      printMachine(m);
    }
    while (!a_cap || !b_cap) {
      if (m.claw.x == m.target.x && m.claw.y == m.target.y) {
        tokens += pressB + (3 * pressA);
        break;
      }
      if (!a_cap && m.claw.x + m.buttonA.x <= m.target.x &&
          m.claw.y + m.buttonA.y <= m.target.y) {
        pressA += 1;
        m.pressA();
        if (DEBUG_PRINT) {
          std::cout << "A pressed! " << std::endl;
        }
        if (pressA >= 100) {
          a_cap = true;
        }
      } else if (!b_cap && m.claw.x + m.buttonB.x <= m.target.x &&
                 m.claw.y + m.buttonB.y <= m.target.y) {
        pressB += 1;
        m.pressB();
        if (DEBUG_PRINT) {
          std::cout << "B pressed! " << std::endl;
        }
        if (pressB >= 100) {
          b_cap = true;
        }
      } else if (pressB > 0 && b_cap) {
        pressB -= 1;
        m.undoPressB();
        if (DEBUG_PRINT) {
          std::cout << "B undo a press" << std::endl;
        }
      } else if (pressA > 0 && a_cap) {
        pressA -= 1;
        m.undoPressA();
        if (DEBUG_PRINT) {
          std::cout << "A undo a press" << std::endl;
        }
      } else {
        if (DEBUG_PRINT) {
          std::cout << "No valid combination found." << std::endl;
        }
        break;
      }
      if (DEBUG_PRINT) {
        printMachine(m);
        std::cout << "A pressed: " << pressA << ", B pressed: " << pressB
                  << std::endl;
      }
    }
  }
  return tokens;
}

// code source:
// https://cp-algorithms.com/algebra/extended-euclid-algorithm.html#practice-problems
static int64_t gcd(int64_t a, int64_t b, int64_t &x, int64_t &y) {
  if (b == 0) {
    x = 1;
    y = 0;
    return a;
  }
  int64_t x1, y1;
  int64_t d = gcd(b, a % b, x1, y1);
  x = y1;
  y = x1 - y1 * (a / b);
  return d;
}

/*
 * Using discrete math to solve the problem instead (needed for Part 2)
 */
static uint64_t getEmBig(std::list<ClawMachine> &machines) {
  uint64_t tokens = 0;
  for (ClawMachine &m : machines) {
    if (DEBUG_PRINT) {
      std::cout << " ---------------------------------------------"
                << std::endl;
      std::cout << " ---------------------------------------------"
                << std::endl;
      printMachine(m);
    }
    uint64_t pressB = 0;
    uint64_t pressA = 0;
    // X-equation:
    //  Ax * i + Bx * j = Tx, where Ax is button A's x-coefficient, Bx is
    //  button B's x-coefficient, Tx is the prize's x-coefficient, i is the
    //  amount of presses on button A and i is the amount of presses on button
    //  B.
    int64_t Ax = m.buttonA.x, Ay = m.buttonA.y;
    int64_t Bx = m.buttonB.x, By = m.buttonB.y;
    int64_t Tx = m.target.x, Ty = m.target.y;

    // Background for theory: https://en.wikipedia.org/wiki/Bézout%27s_identity
    // Get the greatest common divider, gcd, for X-coefficient.
    // Ax*x + Bx*y = gcd(Ax,Bx) = d
    int64_t x, y;
    int64_t d = gcd(Ax, Bx, x, y);
    if (d < 0) {
      d *= -1;
      Ax *= -1;
      Bx *= -1;
      x *= -1;
      y *= -1;
    }
    assert((Ax * x + (Bx * y)) == d);
    if (0 != (Tx % d)) {
      if (DEBUG_PRINT) {
        printf("Tx %c d = %lld\n", '%', Tx % d);
      }
      // no solution exists
      continue;
    }

    // Simplify coefficents for X-equation (by scaling down)
    Ax /= d;
    Bx /= d;
    x *= (Tx / d);
    y *= (Tx / d);
    if (DEBUG_PRINT) {
      printf("GCD: %lld, Scaled Ax: %lld, Scaled Bx: %lld, Scaled Tx: %lld\n",
             d, Ax, Bx, Tx / d);
      printf("x: %lld, y: %lld\n", x, y);
    }
    // i = x - Bx*k
    // j = y + Ax*k
    // Y-equation:
    // Ay * i + By * j = Ty
    // Substitute i & j:
    // Ay(x - Bx*k) + By(y + Ax*k) = Ty
    // k = (Ty - Ay*x - By*y) / (-Ay*Bx + By*Ay)
    int64_t left = Ty - (Ay * x) - (By * y);
    int64_t right = (-Ay * Bx + By * Ax); // * k
    if (DEBUG_PRINT) {
      printf("Ay: %lld Bx: %lld By: %lld, Ax: %lld \n", Ay, Bx, By, Ax);
      printf("%lld / %lld = k \n", left, right);
    }
    if (right < 0) {
      right *= -1;
      left *= -1;
    }
    if (0 != (left % right)) {
      // no solution exists
      if (DEBUG_PRINT) {
        printf("No solution: left %% right != 0. left = %lld, right = %lld, "
               "left %% right = "
               "%lld\n",
               left, right, left % right);
      }
      continue;
    }
    int64_t k = left / right;
    if (DEBUG_PRINT) {
      printf("New start k: %lld\n", k);
    }

    // Now we need to find best k.
    int64_t k_high = ceil((double)x / Bx);  // x - (Bx * k) >= 0
    int64_t k_low = floor((double)-y / Ax); // y + (Ax * k) >= 0
    if (DEBUG_PRINT) {
      printf("k bounds: [%lld,%lld]\n", k_low, k_high);
    }
    if (k < k_low || k > k_high) {
      if (DEBUG_PRINT) {
        printf("No valid interval for k\n");
      }
      continue;
    }

    int64_t min_cost = INT_MAX;
    for (int64_t k_current = k_low; k_current <= k_high; k_current++) {
      int64_t i = x - Bx * k_current;
      int64_t j = y + Ax * k_current;
      if (DEBUG_PRINT) {
        printf("Checking k:%lld, i:%lld and j:%lld \n", k_current, i, j);
      }
      if (i < 0 || j < 0) {
        continue;
      }
      int64_t cost = 3 * j + i;
      if (cost < min_cost) {
        min_cost = cost;
        k = k_current;
      } else {
        break;
      }
    }
    if (DEBUG_PRINT) {
      printf("Found k=%lld\n", k);
    }
    // Compute final i and j
    int64_t final_i = x - Bx * k;
    int64_t final_j = y + Ax * k;
    if (DEBUG_PRINT) {
      printf("Press A: %lld, Press B: %lld\n", final_i, final_j);
    }
    tokens += 3 * final_i;
    tokens += final_j;
    // printf("Press A: %d, press B: %d \n", b_scaled - Ax * k, a_scaled + Bx *
    // k);
    //
    // double d = (m.buttonA.x*m.buttonA.y - m.buttonB.x*m.buttonA.y) /
    // (m.buttonA.y*m.target.x - m.target.x*) Solve for i = k(Tx-Ty) / k * j see
    // of i is an integer?
  }
  return tokens;
}

void Day13::run(std::ifstream &file) {
  std::string line;
  // machines is used in part 1 and bigMachines in part 2
  std::list<ClawMachine> machines;
  std::list<ClawMachine> bigMachines;
  uint64_t big = 10000000000000;
  /*
   * Button A: X+94, Y+34
   * Button B: X+22, Y+67
   * Prize: X=8400, Y=5400
   */

  uint32_t values[6];
  int idx = 0;
  while (getline(file, line)) {
    if (line.empty()) {
      continue;
    }
    std::string dummy;
    char c;
    uint32_t v1, v2;
    std::istringstream iss(line);
    if (!(iss >> dummy)) {
      std::cerr << "Cout not parse line: " << line << std::endl;
    }
    if (idx < 4) {
      if (!(iss >> dummy >> c >> c >> v1 >> c >> c >> c >> v2)) {
        std::cerr << "Could not parse values from line: " << line << std::endl;
      }
      values[idx++] = v1;
      values[idx++] = v2;
    } else {
      if (!(iss >> c >> c >> v1 >> c >> c >> c >> v2)) {
        std::cerr << "Could not parse values from line: " << line << std::endl;
      }
      values[idx++] = v1;
      values[idx++] = v2;
    }
    if (idx == 6) {
      ClawMachine m(Position(values[4], values[5]),
                    Position(values[2], values[3]),
                    Position(values[0], values[1]));
      machines.push_back(m);
      ClawMachine mBig(Position(values[4] + big, values[5] + big),
                       Position(values[2], values[3]),
                       Position(values[0], values[1]));
      bigMachines.push_back(mBig);
      idx = 0;
    }
  }

  if (DEBUG_PRINT) {
    std::cout << "nr of machines: " << machines.size() << std::endl;
    for (ClawMachine &m : machines) {
      printMachine(m);
    }
  }

  std::cout << "Executing Part 1" << std::endl;
  uint64_t tokens = getEmAll(machines);
  for (ClawMachine &m : machines) {
    m.claw.x = 0;
    m.claw.y = 0;
  }
  std::cout << "Total amount of tokens needed: " << tokens << std::endl;
  std::cout << "Executing Part 2" << std::endl;
  tokens = getEmBig(bigMachines);
  std::cout << "Total amount of tokens needed: " << tokens << std::endl;
}
