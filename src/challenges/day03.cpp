#include "../include/day03.h"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>

/*
 * If the following 1+ characters are digits, they are returned as an integer.
 * If not, the function returns -1
 * Example with delimiter as ')':
 * "32)" is returned as 32
 * "32L" is returned as -1
 * ")" is returned as -1
 *
 */
int Day03::getValue(std::istringstream &iss, char const &delimiter) {
  // get value 1
  std::string v = "";
  char c;
  char p = iss.peek();
  while (EOF != p && std::isdigit(p)) {
    iss.get(c);
    v.append(1, c);
    p = iss.peek();
  }
  if (EOF == p) {
    return -1;
  }
  if (delimiter != p) {
    return -1;
  }
  if (v.size() == 0) {
    return -1;
  }
  iss.get(c); // discard the peeked value
  return std::stoi(v);
}

/*
 * Parse the stringstream iss and locate valid operations: mul(X,Y) - X,Y are
 * 1-3 digits. I made a struct, Operation, in case the future challange needed
 * other types of operations.
 */
void Day03::getOperations(std::istringstream &iss,
                          std::list<Operation *> &ops) {
  const char mul[] = {"mul"};
  int i = 0;
  char c;
  while (iss.get(c)) {
    if (c == mul[i]) {
      i++;
    } else {
      i = 0;
    }
    if (3 == i) {
      // mul found
      i = 0;
      char p = iss.peek();
      if (EOF == p) {
        return;
      }
      if ('(' != p) {
        continue;
      }
      iss.get(c); // discard the peeked value

      // get value 1
      int v1 = getValue(iss, ',');
      if (v1 < 0) {
        continue;
      }
      int v2 = getValue(iss, ')');
      if (v2 < 0) {
        continue;
      }

      // We now have a valid mul operation
      Operation *op = new Operation('*', v1, v2);
      ops.push_back(op);
    }
  }
}

/*
 * For part 2 of Day 3: We are not allowed to accept operations that occurs
 * after a "don't()" statment. If a "don't()" statment occurs, a "do()" statment
 * is needed to again allow (the following) operations
 *
 * This function iterates through the stringstream and keeps track if we are in
 * the allowed space of do() and don't(). It appends the valid sections into a
 * delta_string that is returned.
 */
std::string Day03::getAllowedInput(std::istringstream &iss) {
  std::string delta_string = "";
  std::string peak_string = "";
  std::string do_str = "do()";
  std::string dont_str = "don't()";
  char c;
  int i = 0;
  bool allowed = true;
  while (iss.get(c)) {
    if (allowed) {
      if (c == dont_str[i]) {
        i++;
        peak_string.append(1, c);
        if (i == dont_str.size()) {
          i = 0;
          allowed = false;
          peak_string = "";
        }
        continue;
      }
      if (peak_string.size() > 0) {
        delta_string.append(peak_string);
        peak_string = "";
      }
      delta_string.append(1, c);
    }
    if (!allowed) {
      if (c == do_str[i]) {
        i++;
        peak_string.append(1, c);
        if (i == do_str.size()) {
          i = 0;
          allowed = true;
          peak_string = "";
        }
        continue;
      }
      if (peak_string.size() > 0) {
        peak_string = "";
      }
    }
    i = 0;
  }
  return delta_string;
}

void Day03::run(std::ifstream &file) {
  std::ostringstream oss;
  oss << file.rdbuf();
  {
    std::cout << "Executing Part 1" << std::endl;
    std::istringstream iss(oss.str());
    std::list<Operation *> ops;
    getOperations(iss, ops);
    int sum = 0;
    for (Operation *op : ops) {
      if ('*' == op->op) {
        sum += op->v1 * op->v2;
      }
    }
    // clean up
    for (auto op : ops) {
      delete op;
    }
    ops.clear();

    std::cout << "Total sum: " << sum << std::endl;
  }
  {
    std::cout << "Executing Part 2" << std::endl;
    std::istringstream iss(oss.str());
    std::string delta_string = getAllowedInput(iss);
    std::list<Operation *> ops;
    std::istringstream delta_iss(delta_string);
    getOperations(delta_iss, ops);
    int sum = 0;
    for (Operation *op : ops) {
      if ('*' == op->op) {
        sum += op->v1 * op->v2;
      }
    }
    // clean up
    for (auto op : ops) {
      delete op;
    }
    ops.clear();

    std::cout << "Total sum: " << sum << std::endl;
  }
}
