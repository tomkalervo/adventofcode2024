#include "../include/day01.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

/*
 * We look from the smallest index to the largest index. Comparing the value at
 * index i, v_i, with the new value, v_n. If v_i is larger than the v_n, we
 * insert v_n at i's position. If no v_i is larger thant v_n, v_n is inserted at
 * the end.
 */
void Day01::insertion_sort(std::vector<int> &vector, const int &value) {
  auto p = vector.end();
  for (int i = 0; i < vector.size(); i++) {
    if (vector[i] > value) {
      p = vector.begin() + i;
      break;
    }
  }

  vector.insert(p, value);
}

void Day01::run(std::ifstream &file) {
  std::cout << "Executeing Day 01 part 1." << std::endl;
  std::vector<int> left, right;
  std::string line;
  while (getline(file, line)) {
    std::istringstream iss(line);
    int l, r;
    if (iss >> l >> r) {
      insertion_sort(left, l);
      insertion_sort(right, r);
    } else {
      std::cerr << "Could not parse two integer from line: " << line
                << std::endl;
    }
  }
  // add together the difference between the pairs
  int diff = 0;
  for (int i = 0; i < left.size(); i++) {
    diff += abs(left[i] - right[i]);
  }
  std::cout << "Total differene between left and right lists are: " << diff
            << std::endl;

  std::cout << "Executeing Day 01 part 2." << std::endl;
  // We continue to use the ordered vector left and right
  // This will be an O(n^2) operation
  long long similarity = 0;
  int i = 0;
  int j = 0;
  while (i < left.size() && j < right.size()) {
    int value = left[i];
    int occ_l = 0;
    while (left[i] == value) {
      occ_l++;
      i++;
      if (i >= left.size()) {
        break;
      }
    }

    int occ_r = 0;
    while (right[j] <= value) {
      if (value == right[j]) {
        occ_r++;
      }
      j++;
      if (j >= right.size()) {
        break;
      }
    }
    similarity += (value * occ_l * occ_r);
  }
  std::cout << "Total similarity score is " << similarity << std::endl;
}
