#pragma once
#include <fstream>
#include <list>
#include <string>
#include <vector>

class Day05 {
public:
  static void run(std::ifstream &file);

private:
  static int validUpdate(const std::vector<std::list<int>> &rules,
                         const std::list<int> &update_list);
  static int correctUpdate(const std::vector<std::list<int>> &rules,
                           const std::list<int> &update_list);
  static std::list<int> updateList(const std::string &update);
};
