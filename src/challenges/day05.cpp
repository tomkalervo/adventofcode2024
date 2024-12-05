#include "../include/day05.h"
#include <cstddef>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

/*
 * This function corrects invalid updates. This means that pages need to change
 * position in order to comply with teir rules. In order to do effecient
 * swapping I use the vector structure. As part of the 2nd problem the result is
 * the page value of the index in the middle, that is why the function returns
 * that page valu.
 */
int Day05::correctUpdate(const std::vector<std::list<int>> &rules,
                         const std::list<int> &update_list) {
  std::vector<int> corrected_update;
  for (int page : update_list) {
    corrected_update.push_back(page);
  }
  int size = corrected_update.size();

  for (int i = 0; i < size - 1; i++) {
    int page_X = corrected_update[i];
    // We could run into an infit loop with reseting the j-value after the swap.
    // Depending on the input (and/or poor logic).
    // The saftey check is precautionary measure to handle that case.
    int safety = 0;
    for (int j = i + 1; j < size; j++) {
      int page_Y = corrected_update[j];
      safety++;
      if (safety > 200) {
        std::cout << "Saftey cap reached. Correcting :";
        for (int page : update_list) {
          std::cout << " " << page;
        }
        std::cout << "\nCorrected vector: ";
        for (int page : corrected_update) {
          std::cout << " " << page;
        }
        std::cout << std::endl;
      }
      for (int rule : rules.at(page_X)) {
        if (rule < page_Y) {
          break;
        }
        if (rule == page_Y) {
          // Swap
          corrected_update[i] = page_Y;
          corrected_update[j] = page_X;
          page_X = page_Y;
          j = i + 1;
          break;
        }
      }
    }
  }
  return corrected_update[(size / 2)];
}

int Day05::validUpdate(const std::vector<std::list<int>> &rules,
                       const std::list<int> &update_list) {
  std::list<int>::const_iterator it_x = update_list.begin();
  while (it_x != update_list.end()) {
    std::list<int>::const_iterator it_y = it_x;
    it_y++;
    while (it_y != update_list.end()) {
      for (int rule : rules.at(*it_x)) {
        if (rule < *it_y) {
          // Y is not in Xs rules
          // rules are ordered larger -> smaller
          // goto next page
          break;
        }
        if (rule == *it_y) {
          // Y is in Xs rules and occurs before X in the page update
          // Invalid update
          return 0;
        }
      }
      it_y++;
    }
    it_x++;
  }

  // return the page in the middle
  int len = update_list.size();
  std::list<int>::const_iterator value = update_list.begin();
  advance(value, len / 2);
  return *value;
}

// Helper-function to get a string of updated pages into a list structure
// Input: string of pages 'x1,x2,x3'
// Output: list of integers: (x3-x2-x1)
std::list<int> Day05::updateList(const std::string &update) {
  std::list<int> update_list;
  std::istringstream iss(update);
  int in;
  while (iss >> in) {
    // reverse the order
    update_list.push_front(in);
    // skip delimiter ','
    char c;
    if (!(iss >> c))
      break;
  }
  return update_list;
}

void Day05::run(std::ifstream &file) {
  std::cout << "Executing Part 1 & Part 2" << std::endl;
  int max_pagenr = 100;
  std::vector<std::list<int>> rules(max_pagenr);
  if (rules.capacity() < 100) {
    std::cerr << "Not enough capacity in rules vector: " << rules.capacity()
              << std::endl;
    return;
  }

  for (int i = 0; i < max_pagenr; i++) {
    std::list<int> tmp;
    rules.push_back(tmp);
  }

  int sum = 0;           // Part 1
  int corrected_sum = 0; // Part 2
  std::string line;
  bool page_rules = true;
  while (getline(file, line)) {
    if (line == "") {
      // We have reached the end of the rules in the file.
      // Next part is the updates
      page_rules = false;
      continue;
    }

    if (page_rules) {
      // Build the rules-vector from the first part of the file.
      int x, y;
      char c;
      std::istringstream iss(line);
      if (iss >> x >> c >> y) {
        if (c != '|') {
          std::cerr << "Caught invalid delimiter in c: " << c << std::endl;
          return;
        }
        std::list<int>::iterator it = rules.at(x).begin();
        for (int w : rules.at(x)) {
          if (y > w) {
            break;
          }
          std::advance(it, 1);
        }
        rules.at(x).insert(it, y);

      } else {
        std::cerr << "Could not parse line: " << line << std::endl;
        return;
      }
    } else {
      // The rules-vector is build and we parse the update pages to sum up the
      // result from valid updates
      std::list<int> update_list = updateList(line);
      int result = validUpdate(rules, update_list);
      // std::cout << line << ": " << result << std::endl;
      sum += result;

      if (0 == result) {
        // Part 2 of the problem, correct invalid updates and get their result
        int corrected_result = correctUpdate(rules, update_list);
        if (0 == corrected_result) {
          std::cerr << "Got zero result when correcting the line: " << line
                    << std::endl;
          return;
        }
        corrected_sum += corrected_result;
      }
    }
  }

  std::cout << "Part 1 resulting sum: " << sum << std::endl;
  std::cout << "Part 2 corrected sum: " << corrected_sum << std::endl;
}
