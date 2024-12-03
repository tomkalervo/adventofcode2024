#pragma once
#include <fstream>
#include <vector>
class Day02 {
public:
  void static run(std::ifstream &file);

private:
  int static safe(std::vector<int> const &report, int low, int high);
  int static safe_damp(std::vector<int> const &report, int low, int high);
};
