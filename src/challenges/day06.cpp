#include "../include/day06.h"
#include <iostream>
#include <sstream>
#include <vector>

static void printMatrix(std::vector<std::vector<char>> &M) {
  std::cout << ">> Printing Matrix: \n";
  for (int i = 0; i < M.size(); i++) {
    std::cout << ">> ";
    for (int j = 0; j < M[i].size(); j++) {
      std::cout << M[i][j];
    }
    std::cout << "\n";
  }
  std::cout << std::endl;
}

/*
 * This function maps out the path of the guard, marking visited positions with
 * '1' in the visit_M. Added the '2' to mark potential loop-paths.
 *  Returns true if guard is caught in a loop
 */
bool Day06::patrolGuard(const std::vector<std::vector<char>> &map,
                        std::vector<std::vector<char>> &visit) {

  int x_in, y_in;
  for (int y = 0; y < map.size(); y++) {
    for (int x = 0; x < map[y].size(); x++) {
      if ('^' == map[y][x]) {
        x_in = x;
        y_in = y;
      }
    }
  }
  // Guard start with direction [0,-1] == ^
  struct Day06::Position guard(x_in, y_in, 0, -1);
  while (guard.in_range) {
    if ('0' == visit[guard.y][guard.x]) {
      visit[guard.y][guard.x] = '1';
    } else if ('1' == visit[guard.y][guard.x]) {
      visit[guard.y][guard.x] = '2';
    } else if ('2' == visit[guard.y][guard.x]) {
      // Check if the guard is stuck in a loop
      if ('#' == map[guard.y + guard.yd][guard.x + guard.xd]) {
        return true;
      }
    }
    guard.advance(map);
  }
  return false;
}

void Day06::run(std::ifstream &file) {
  std::string line;
  std::vector<std::vector<char>> map_M;
  while (getline(file, line)) {
    std::vector<char> row;
    std::istringstream iss(line);
    char c;
    while (iss >> c) {
      row.push_back(c);
    }
    map_M.push_back(row);
  }

  int column_size = map_M.size();
  if (column_size < 1) {
    std::cerr << "Invalid init of map Matrix." << std::endl;
    return;
  }

  int row_size = map_M[0].size();
  if (row_size < 1) {
    std::cerr << "Invalid init of map Matrix." << std::endl;
    return;
  }
  {
    std::cout << "Executing Part 1" << std::endl;

    // visit_M : Matrix that holds info on what positions the guard has
    // visited.
    std::vector<std::vector<char>> visit_M(column_size);
    std::vector<char> tmp_row(row_size);
    std::fill(tmp_row.begin(), tmp_row.end(), '0');
    std::fill(visit_M.begin(), visit_M.end(), tmp_row);

    // printMatrix(map_M);
    // printMatrix(visit_M);

    patrolGuard(map_M, visit_M);
    // printMatrix(visit_M);
    int visited = 0;
    for (int y = 0; y < visit_M.size(); y++) {
      for (int x = 0; x < visit_M[y].size(); x++) {
        if ('0' != visit_M[y][x]) {
          visited += 1;
        }
      }
    }
    std::cout << "Patrolled positions by the guard: " << visited << std::endl;
  }
  {
    std::cout << "Executing Part 2" << std::endl;
    // This is a naive solution: check all possible placements on the board.
    // It takes a while, but works.
    int obstruction = 0;
    for (int y = 0; y < map_M.size(); y++) {
      for (int x = 0; x < map_M[y].size(); x++) {
        std::vector<std::vector<char>> visit_M(column_size);
        std::vector<char> tmp_row(row_size);
        std::fill(tmp_row.begin(), tmp_row.end(), '0');
        std::fill(visit_M.begin(), visit_M.end(), tmp_row);

        if ('.' == map_M[y][x]) {
          map_M[y][x] = '#';
          if (patrolGuard(map_M, visit_M)) {
            obstruction += 1;
            // map_M[y][x] = 'O';
            // printMatrix(map_M);
          }
          map_M[y][x] = '.';
        }
      }
    }

    std::cout << "Possible obstructions to trap the guard: " << obstruction
              << std::endl;
  }
}
