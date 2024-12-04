#include "../include/day04.h"
#include <iostream>
#include <list>
#include <vector>

// Helperfunction to correctly handle checks outside of the Matrix
bool Day04::hasValue(const std::vector<std::vector<char>> &M, const char &value,
                     const Point &p) {
  // Check valid input
  if (M.empty()) {
    std::cerr << "Error: Empty Matrix" << std::endl;
    return false;
  }
  if (M.at(0).empty()) {
    std::cerr << "Error: Empty Matrix row" << std::endl;
    return false;
  }
  // Check if point within range
  if (0 > p.x || 0 > p.y) {
    return false;
  }
  if (M.size() <= p.y) {
    return false;
  }
  if (M.at(p.y).size() <= p.x) {
    return false;
  }
  // Final check
  if (M.at(p.y).at(p.x) == value) {
    return true;
  } else {
    return false;
  }
}

/*
 * This function does a grid search arount the incoming (x,y).
 * If the second charachter in the string str is found at point (x_i,y_j), that
 * point (x_i,y_j) is added to the list of potentials. After the grid search the
 * list of potentials are handled to se if they contain the entire charachter
 * string str. A valid string str can be written in all direction, thus there
 * can be more than 1 valid occurances.
 */
int Day04::validOccurance(const std::vector<std::vector<char>> &M,
                          const Point &p, const std::string &str) {
  std::list<Point> potentials;
  for (int j = -1; j <= 1; j++) {
    for (int i = -1; i <= 1; i++) {
      if (0 != j || 0 != i) {
        Point p2(p.x + i, p.y + j);
        if (hasValue(M, str.at(1), p2)) {
          potentials.push_back(p2);
        }
      }
    }
  }
  int occ = 0;
  for (Point p2 : potentials) {
    // Getting the direction by taking (x_2,y_2) - (x_1,y_1)
    Point direction((p2.x - p.x), (p2.y - p.y));
    int pos = 2;
    while (pos < str.length()) {
      // Get the next point using linear algebra: [x_1,y_1]' x pos*[x_d,y_d]'
      Point p_next((p.x + (pos * direction.x)), (p.y + (pos * direction.y)));
      if (!hasValue(M, str.at(pos), p_next)) {
        break;
      }
      pos++;
    }
    // check if all charachters were matched
    if (pos == str.length()) {
      occ += 1;
    }
  }

  return occ;
}

/*
 * Part 1 of the problem where we search for the occurances of the string str in
 * the Matrix M.
 * This function iterates through the matrix M searching for the first
 * characther in the string str. If a match is found during search, the function
 * validOccurance returnes how many valid occurances start at that location.
 */
int Day04::countStringOccurrances(const std::vector<std::vector<char>> &M,
                                  std::string &str) {
  int y = M.size();
  if (0 >= y) {
    std::cerr << "Invalid initalization of the matrix M. y = " << y
              << std::endl;
    return -1;
  }
  int x = M.at(0).size();
  if (0 >= x) {
    std::cerr << "Invalid initalization of the matrix M. x = " << x
              << std::endl;
    return -1;
  }
  int occurances = 0;
  for (int j = 0; j < y; j++) {
    for (int i = 0; i < x; i++) {
      if (str.at(0) == M.at(j).at(i)) {
        // Found first charachter
        occurances += validOccurance(M, Point(i, j), str);
      }
    }
  }

  return occurances;
}

/*
 * Part 2 of the problem where we search for X-formations of MAS:
 * M_S
 * _A_
 * M_S
 */
int Day04::countXformations(const std::vector<std::vector<char>> &M) {
  int y = M.size();
  if (0 >= y) {
    std::cerr << "Invalid initalization of the matrix M. y = " << y
              << std::endl;
    return -1;
  }
  int x = M.at(0).size();
  if (0 >= x) {
    std::cerr << "Invalid initalization of the matrix M. x = " << x
              << std::endl;
    return -1;
  }
  int X_formations = 0;
  for (int j = 0; j < y; j++) {
    for (int i = 0; i < x; i++) {
      if ('A' == M.at(j).at(i)) {
        // Found center of possible X formation
        if ((hasValue(M, 'M', Point(i - 1, j - 1)) &&
             hasValue(M, 'S', Point(i + 1, j + 1))) ||
            ((hasValue(M, 'S', Point(i - 1, j - 1)) &&
              hasValue(M, 'M', Point(i + 1, j + 1))))) {
          if ((hasValue(M, 'M', Point(i + 1, j - 1)) &&
               hasValue(M, 'S', Point(i - 1, j + 1))) ||
              ((hasValue(M, 'S', Point(i + 1, j - 1)) &&
                hasValue(M, 'M', Point(i - 1, j + 1))))) {
            X_formations += 1;
          }
        }
      }
    }
  }
  return X_formations;
}

void Day04::run(std::ifstream &file) {
  // Build a char-Matrix datastructure from input
  std::vector<std::vector<char>> M;
  std::string line;
  while (getline(file, line)) {
    std::vector<char> row;
    for (char c : line) {
      row.push_back(c);
    }
    M.push_back(row);
  }

  std::cout << "Executing Part 1" << std::endl;
  std::string xmas = "XMAS";
  int sum = countStringOccurrances(M, xmas);
  std::cout << "Total sum: " << sum << std::endl;

  std::cout << "Executing Part 2" << std::endl;
  sum = countXformations(M);
  std::cout << "Total sum: " << sum << std::endl;
}
