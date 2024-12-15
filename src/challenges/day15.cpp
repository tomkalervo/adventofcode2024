/*
 * This is a simple, but complex operation handling class. Could be split into
 * methods to make it more clear/readable. But it does the job!
 */

#include "../include/day15.h"
#include <array>
#include <cstdint>
#include <iostream>
#include <list>
#include <queue>
#include <set>
#include <string>
#include <vector>

const bool DEBUG = false;
struct Move {
  int x;
  int y;
  Move(char c) {
    switch (c) {
    case '>': {
      x = 1;
      y = 0;
      break;
    }
    case '<': {
      x = -1;
      y = 0;
      break;
    }
    case '^': {
      x = 0;
      y = -1;
      break;
    }
    case 'v': {
      x = 0;
      y = 1;
      break;
    }
    default: {
      std::cerr << "Bad type of movement: " << c << std::endl;
      x = -1;
      y = -1;
      break;
    }
    }
  }
};

static void printWarehouse(std::vector<std::vector<char>> &warehouse,
                           std::array<uint32_t, 2> &robot) {
  for (uint32_t i = 0; i < warehouse.size(); i++) {
    for (uint32_t j = 0; j < warehouse[i].size(); j++) {
      if (i == robot[0] && j == robot[1]) {
        printf("%c", '@');
      } else {
        printf("%c", warehouse[i][j]);
      }
    }
    printf("\n");
  }
}
static void addToQueue(std::queue<std::array<int, 2>> &q,
                       std::set<std::array<int, 2>> &s, std::array<int, 2> v) {
  if (s.find(v) == s.end()) {
    q.push(v);
    s.insert(v);
  }
}
static void moveRobotWide(std::vector<std::vector<char>> &warehouse,
                          std::array<uint32_t, 2> &robot,
                          std::queue<Move> &movements) {

  while (movements.size()) {
    if (DEBUG) {
      std::cout << "Current warehouse" << std::endl;
      printWarehouse(warehouse, robot);
    }
    Move m = movements.front();
    movements.pop();
    int x = robot[1] + m.x;
    int y = robot[0] + m.y;
    if (DEBUG) {
      std::cout << "Move (" << m.x << "," << m.y << "), Robot (" << robot[1]
                << "," << robot[0] << ")" << std::endl;
      std::cout << "Checking position (" << x << "," << y << ")" << std::endl;
    }
    if (warehouse[y][x] == '.') {
      robot[1] = x;
      robot[0] = y;
      continue;
    }
    if (warehouse[y][x] == '#') {
      continue;
    }
    // Robot collide with box
    if (m.x != 0) {
      // 1. along X-axis
      if (DEBUG) {
        std::cout << "Box collision along x-axis" << std::endl;
      }
      while (warehouse[y][x] == '[' or warehouse[y][x] == ']') {
        x += m.x;
        y += m.y;
      }
      if (warehouse[y][x] == '#') {
        continue;
      }
      do {
        warehouse[y][x] = warehouse[y - m.y][x - m.x];
        x -= m.x;
        y -= m.y;
      } while (warehouse[y][x] != '.');
      robot[1] = x + m.x;
      robot[0] = y + m.y;
      if (DEBUG) {
        std::cout << "Robot new position: (" << x << "," << y << ")"
                  << std::endl;
      }
      continue;
    } else if (m.y != 0) {
      // 2. along the y-axis
      if (DEBUG) {
        std::cout << "Box collision along y-axis" << std::endl;
      }
      std::set<std::array<int, 2>> visited;
      std::queue<std::array<int, 2>> q;
      q.push({y, x});
      visited.insert({y, x});
      if (warehouse[y][x] == '[') {
        q.push({y, x + 1});
        visited.insert({y, x + 1});
      } else {
        q.push({y, x - 1});
        visited.insert({y, x - 1});
      }
      std::list<std::array<int, 2>> boxes;
      bool wall = false;
      while (q.size() && !wall) {
        std::array<int, 2> box = q.front();
        q.pop();
        int xbox = box[1];
        int ybox = box[0];
        if (warehouse[ybox][xbox] == '.') {
          continue;
        }
        if (warehouse[ybox][xbox] == '#') {
          wall = true;
          continue;
        }
        boxes.push_front(box);
        if (warehouse[ybox][xbox] != warehouse[ybox + m.y][xbox + m.x]) {
          if (warehouse[ybox + m.y][xbox + m.x] == '[') {
            addToQueue(q, visited, {ybox + m.y, xbox + m.x});
            addToQueue(q, visited, {ybox + m.y, xbox + m.x + 1});
            continue;
          }
          if (warehouse[ybox + m.y][xbox + m.x] == ']') {
            addToQueue(q, visited, {ybox + m.y, xbox + m.x});
            addToQueue(q, visited, {ybox + m.y, xbox + m.x - 1});
            continue;
          }
        }
        // push it!
        q.push({ybox + m.y, xbox + m.x});
      }
      if (wall) {
        continue;
      }
      for (std::array<int, 2> b : boxes) {
        int xbox = b[1];
        int ybox = b[0];
        if (DEBUG) {
          std::cout << "Move box (" << xbox << "," << ybox << ")" << std::endl;
        }
        if (warehouse[ybox + m.y][xbox + m.x] != '.') {
          printWarehouse(warehouse, robot);
          std::cout << "Tried to move box to occupied position. Box: (" << xbox
                    << "," << ybox << ") -> (" << xbox + m.x << ","
                    << ybox + m.y << ")" << std::endl;
          return;
        }
        char c = warehouse[ybox][xbox];
        warehouse[ybox + m.y][xbox + m.x] = c;
        warehouse[ybox][xbox] = '.';
      }

      robot[1] = x;
      robot[0] = y;
      if (DEBUG) {
        std::cout << "Robot new position: (" << x << "," << y << ")"
                  << std::endl;
      }
      continue;
    } else {
      std::cerr << "Bad spot in warehouse: " << warehouse[y][x] << std::endl;
      return;
    }
  }
  if (DEBUG) {
    std::cout << "Current warehouse" << std::endl;
    printWarehouse(warehouse, robot);
  }
}
static void moveRobot(std::vector<std::vector<char>> &warehouse,
                      std::array<uint32_t, 2> &robot,
                      std::queue<Move> &movements) {
  while (movements.size()) {
    if (DEBUG) {
      std::cout << "Current warehouse" << std::endl;
      printWarehouse(warehouse, robot);
    }
    Move m = movements.front();
    movements.pop();
    int x = robot[1] + m.x;
    int y = robot[0] + m.y;
    if (DEBUG) {
      std::cout << "Move (" << m.x << "," << m.y << "), Robot (" << robot[1]
                << "," << robot[0] << ")" << std::endl;
      std::cout << "Checking position (" << x << "," << y << ")" << std::endl;
    }
    if (warehouse[y][x] == '.') {
      robot[0] = y;
      robot[1] = x;
      continue;
    }
    if (warehouse[y][x] == '#') {
      continue;
    }
    // Robot collide with box

    while (warehouse[y][x] == 'O') {
      x += m.x;
      y += m.y;
    }
    if (warehouse[y][x] == '#') {
      continue;
    }
    warehouse[y][x] = 'O';
    x = robot[1] + m.x;
    y = robot[0] + m.y;
    warehouse[y][x] = '.';
    robot[1] = x;
    robot[0] = y;
  }
  if (DEBUG) {
    std::cout << "Current warehouse" << std::endl;
    printWarehouse(warehouse, robot);
  }
}

void Day15::run(std::ifstream &file) {
  std::string line;
  std::vector<std::vector<char>> warehouse;
  std::queue<Move> movements;
  std::array<uint32_t, 2> robot;
  bool build_warehouse = true;
  while (getline(file, line)) {
    if (line == "") {
      build_warehouse = false;
      continue;
    }
    if (build_warehouse) {
      std::vector<char> row;
      for (char c : line) {
        row.push_back(c);
      }
      warehouse.push_back(row);
    } else {
      for (char c : line) {
        movements.push(Move(c));
      }
    }
  }

  for (uint32_t i = 0; i < warehouse.size(); i++) {
    for (uint32_t j = 0; j < warehouse[i].size(); j++) {
      if (warehouse[i][j] == '@') {
        warehouse[i][j] = '.';
        robot[0] = i;
        robot[1] = j;
        break;
      }
    }
  }
  // Build the wide warehouse for part 2
  std::queue<Move> movements2 = movements;
  std::array<uint32_t, 2> robot2 = robot;
  robot2[1] *= 2;
  std::vector<std::vector<char>> warehouse_wide;
  for (uint32_t i = 0; i < warehouse.size(); i++) {
    std::vector<char> row;
    for (uint32_t j = 0; j < warehouse[i].size(); j++) {
      if (warehouse[i][j] == 'O') {
        row.push_back('[');
        row.push_back(']');
      } else {
        row.push_back(warehouse[i][j]);
        row.push_back(warehouse[i][j]);
      }
    }
    warehouse_wide.push_back(row);
  }
  if (DEBUG) {
    printWarehouse(warehouse, robot);
    std::cout << "Robot location: (" << robot[1] << "," << robot[0] << ")"
              << std::endl;
  }
  std::cout << "Executing Part 1" << std::endl;
  moveRobot(warehouse, robot, movements);
  uint64_t gps_sum = 0;
  for (uint32_t i = 0; i < warehouse.size(); i++) {
    for (uint32_t j = 0; j < warehouse[i].size(); j++) {
      if (warehouse[i][j] == 'O') {
        gps_sum += 100 * i;
        gps_sum += j;
      }
    }
  }
  std::cout << "Sum of all boxes' GPS: " << gps_sum << std::endl;
  std::cout << "Executing Part 2" << std::endl;
  if (DEBUG) {
    printWarehouse(warehouse_wide, robot2);
  }
  printWarehouse(warehouse_wide, robot2);
  moveRobotWide(warehouse_wide, robot2, movements2);
  gps_sum = 0;
  for (uint32_t i = 0; i < warehouse_wide.size(); i++) {
    for (uint32_t j = 0; j < warehouse_wide[i].size(); j++) {
      if (warehouse_wide[i][j] == '[') {
        gps_sum += 100 * i;
        gps_sum += j;
      }
    }
  }
  printWarehouse(warehouse_wide, robot2);
  std::cout << "Sum of all boxes' GPS: " << gps_sum << std::endl;
}
