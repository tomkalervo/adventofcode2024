#pragma once
#include <fstream>
#include <vector>
class Day01 {
public:
  void static run(std::ifstream &file);

private:
  void static insertion_sort(std::vector<int> &, const int &);
};
