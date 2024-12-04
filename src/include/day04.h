#pragma once
#include <fstream>
#include <string>
#include <vector>

class Day04 {
public:
  static void run(std::ifstream &file);

private:
  struct Point {
    int x;
    int y;
    Point(int x_in, int y_in) : x(x_in), y(y_in) {}
  };

  static bool hasValue(const std::vector<std::vector<char>> &M,
                       const char &value, const Point &p);
  static int validOccurance(const std::vector<std::vector<char>> &M,
                            const Point &p, const std::string &str);
  static int countStringOccurrances(const std::vector<std::vector<char>> &M,
                                    std::string &str);
  static int countXformations(const std::vector<std::vector<char>> &M);
};
