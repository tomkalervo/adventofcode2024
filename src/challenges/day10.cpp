#include "../include/day10.h"
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <list>
#include <queue>
#include <vector>

struct Point {
  int x;
  int y;
  Point(int x_in, int y_in) : x(x_in), y(y_in) {}
};

static bool validPoint(const Point &p, const int &height, const int &width) {
  if (p.x < 0 || p.y < 0)
    return false;

  if (p.y >= height) {
    return false;
  }

  if (p.x >= width) {
    return false;
  }

  return true;
}

static bool isVisited(const Point &p,
                      const std::vector<std::vector<uint8_t>> &v) {
  if (!validPoint(p, v.size(), v.at(0).size()))
    return true;

  return (int)v[p.y][p.x];
}

static void markVisited(const Point &p, std::vector<std::vector<uint8_t>> &v) {
  v[p.y][p.x] = 1;
}

static bool validPath(const Point &p, const Point &p_next,
                      const std::vector<std::vector<uint8_t>> &grid) {
  return (grid[p_next.y][p_next.x] == 1 + grid[p.y][p.x]);
}
/*
 * Breadth first search to find the amount of target values that has a path from
 * startin position.
 */
static int trails(const Point &startpos, const uint8_t &target,
                  const std::vector<std::vector<uint8_t>> &grid,
                  std::vector<std::vector<uint8_t>> visited) {
  int path = 0;
  std::queue<Point> fifo;
  if (isVisited(startpos, visited)) {
    std::cerr << "Invalid starting point" << std::endl;
    return 0;
  }
  markVisited(startpos, visited);
  fifo.push(startpos);

  while (fifo.size()) {
    Point p = fifo.front();
    fifo.pop();
    // Base case: At end of trail
    if (target == grid[p.y][p.x]) {
      path += 1;
      continue;
    }

    // Recursive: search west, east, north and south
    std::list<Point> points;
    points.push_back(Point(p.x - 1, p.y));
    points.push_back(Point(p.x + 1, p.y));
    points.push_back(Point(p.x, p.y - 1));
    points.push_back(Point(p.x, p.y + 1));
    for (Point &p_next : points) {
      if (!isVisited(p_next, visited)) {
        if (validPath(p, p_next, grid)) {
          markVisited(p_next, visited);
          fifo.push(p_next);
        }
      }
    }
  }

  return path;
}

/*
 * A recursive approach for Part 2. The function returns the amount of distinct
 * trails from Point p to a target all target values. Visited matrix keep track
 * och the amount of targets that can be reached from a single Point. Thus,
 * after running this function recursively, the total number of distinct trails
 * can also be found in the visited matrix for the starting position.
 */
static int trailsDistinct(const Point &p, const uint8_t &target,
                          const std::vector<std::vector<uint8_t>> &grid,
                          std::vector<std::vector<uint8_t>> &visited) {
  // Base case: trail at the target
  if (target == grid[p.y][p.x]) {
    visited[p.y][p.x] = 1;
    return 1;
  }

  // Recursive: search west, east, north and south
  int trails = 0;
  std::list<Point> points;
  points.push_back(Point(p.x - 1, p.y));
  points.push_back(Point(p.x + 1, p.y));
  points.push_back(Point(p.x, p.y - 1));
  points.push_back(Point(p.x, p.y + 1));
  for (Point &p_next : points) {
    if (validPoint(p_next, visited.size(), visited.at(0).size())) {
      if (validPath(p, p_next, grid)) {
        if (visited[p_next.y][p_next.x] > 0) {
          trails += visited[p_next.y][p_next.x];
        } else {
          trails += trailsDistinct(p_next, target, grid, visited);
        }
      }
    }
  }

  visited[p.y][p.x] = trails;
  // std::cout << "Size of lifo: " << lifo.size() << std::endl;
  return trails;
}

void Day10::run(std::ifstream &filepath) {
  std::string line;
  std::vector<std::vector<uint8_t>> grid;
  char zero = '0';
  while (getline(filepath, line)) {
    std::vector<uint8_t> row;
    for (uint8_t p : line) {
      row.push_back(p - (uint8_t)zero);
    }
    grid.push_back(row);
  }
  if (grid.empty()) {
    std::cerr << "Could not load grid." << std::endl;
    return;
  }
  std::cout << "Grid heigth: " << grid.size()
            << ", Grid length: " << grid.at(0).size() << std::endl;

  // Build clean visited Matrix
  std::vector<std::vector<uint8_t>> visited;
  visited.reserve(grid.size());
  for (int i = 0; i < grid.size(); i++) {
    std::vector<uint8_t> unvisited_row(grid.at(0).size());
    fill(unvisited_row.begin(), unvisited_row.end(), 0);
    visited.push_back(unvisited_row);
  }
  if (visited.empty()) {
    std::cerr << "Could not build visited matrix." << std::endl;
    return;
  }
  std::cout << "Visited Matrix heigth: " << visited.size()
            << ", Visited Matrix length: " << visited.at(0).size() << std::endl;

  // Get all starting points
  std::list<Point> starting_positions;
  for (int y = 0; y < grid.size(); y++) {
    for (int x = 0; x < grid.at(y).size(); x++) {
      if (0 == grid[y][x]) {
        starting_positions.push_back(Point(x, y));
      }
    }
  }

  std::cout << "Executing Part 1 & Part 2" << std::endl;
  int paths = 0;
  int paths_distinct = 0;
  for (Point &p : starting_positions) {
    paths += trails(p, 9, grid, visited);
    paths_distinct += trailsDistinct(p, 9, grid, visited);
  }
  std::cout << "Total paths: " << paths << std::endl;
  std::cout << "Total distinct paths: " << paths_distinct << std::endl;
}
