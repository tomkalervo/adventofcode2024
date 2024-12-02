#include "../include/day02.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

/*
 * A report is considered safe if consecutive integers are all decreasing or all
 * increasing. The increase or deacrease must be within a certain interval (low,
 * high). For the second part of the problem there is a "dampener" that allows a
 * report to be considered safe even with one violation.
 */

int Day02::safe(std::vector<int> const &report, int low, int high) {
  if (report.size() < 2) {
    std::cerr << "Invalid length of report: " << report.size() << std::endl;
    return -1;
  }

  int i = 1;
  int j = 0;

  // Step through each value and compare to the previous
  // If the report passes the loop it is considered safe
  while (i < report.size()) {
    int diff = report.at(i) - report.at(j);

    // check if invalid increase
    if (diff <= low || diff >= high) {
      return 0;
    }
    j = i;
    i++;
  }

  return 1;
}

int Day02::safe_damp(std::vector<int> const &report, int low, int high) {
  if (report.size() < 2) {
    std::cerr << "Invalid length of report: " << report.size() << std::endl;
    return -1;
  }

  int i = 1;
  int j = 0;
  bool skipped = false;
  // Step through each value and compare to the previous
  // If the report passes the loop it is considered safe
  while (i < report.size()) {
    int diff = report.at(i) - report.at(j);

    // check if not valid level
    if (diff <= low || diff >= high) {
      if (skipped) {
        return 0;
      }
      // skipp current value at i
      if (report.size() - 1 == i) {
        // last level, can be ignored
        return 1;
      }

      int diff_skip_i = report.at(i + 1) - report.at(j);
      if (diff_skip_i > low && diff_skip_i < high) {
        // valid level found when skipping i´th value
        skipped = true;
        i++;
        continue;
      }
      if (0 < j) {
        int diff_skip_j = report.at(i) - report.at(j - 1);
        if (diff_skip_j > low && diff_skip_j < high) {
          // valid level found when skipping j´th value
          skipped = true;
          j--;
          continue;
        } else {
          // No valid skipp found
          return 0;
        }
      }

      // We end up here when j == 0
      // Since we always have the option to skip first level
      skipped = true;
    }

    j = i;
    i++;
  }
  return 1;
}

void Day02::run(std::ifstream &file) {
  std::cout << "Executeing part 1 & 2." << std::endl;
  int safe_reports = 0;
  int safe_reports_damp = 0;
  std::string line;
  while (getline(file, line)) {
    std::istringstream iss(line);
    std::vector<int> report;
    while (!iss.eof()) {
      int v;
      if (iss >> v) {
        report.push_back(v);
      } else {
        std::cerr << "Error reading integer from line: " << line << std::endl;
      }
    }
    // Check if valid increase
    int res = safe(report, 0, 4);
    if (0 == res) {
      // Check if valid deacrease
      res = safe(report, -4, 0);
    }
    safe_reports += res;

    // Use dampner for part 2
    if (0 == res) {
      res = safe_damp(report, 0, 4);
    }
    if (0 == res) {
      res = safe_damp(report, -4, 0);
    }
    safe_reports_damp += res;

    // std::cout << "report: " << line << ", safe reports: " << safe_reports
    //           << ", with dampener: " << safe_reports_damp << std::endl;
  }

  std::cout << "Part 1, without dampener. Amount of safe reports are: "
            << safe_reports << std::endl;
  std::cout << "Part 2, with dampener. Amount of safe reports are: "
            << safe_reports_damp << std::endl;
}
