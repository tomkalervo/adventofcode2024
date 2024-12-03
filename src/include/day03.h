#pragma once
#include <fstream>
#include <list>
#include <sstream>
#include <string>

class Day03 {
public:
  static void run(std::ifstream &file);

private:
  struct Operation {
    const char op;
    const int v1;
    const int v2;

    Operation(char op_in, int v1_in, int v2_in)
        : op(op_in), v1(v1_in), v2(v2_in) {}
  };

  static int getValue(std::istringstream &iss, const char &delimiter);
  static void getOperations(std::istringstream &iss,
                            std::list<Operation *> &ops);
  static std::string getAllowedInput(std::istringstream &iss);
};
