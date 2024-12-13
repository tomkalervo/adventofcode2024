#include "../include/day12.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stack>
#include <string>
#include <unistd.h>
#include <vector>

static const bool DEBUG_PRINT = false;

struct Plot {
  char type;
  int x;
  int y;
  bool fenced;
  int r = -1;
  Plot(char type_in, int x_in, int y_in)
      : type(type_in), x(x_in), y(y_in), fenced(false) {}
};

static void printGarden(const std::vector<std::vector<Plot>> &garden) {
  for (std::vector<Plot> row : garden) {
    for (Plot p : row) {
      std::cout << "[" << p.type << " " << (int)p.fenced << " (" << p.x << ","
                << p.y << ") r:" << p.r << "] ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

static uint32_t fenceCostPlot(Plot &p_start,
                              std::vector<std::vector<Plot>> &garden) {
  int adj[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  std::stack<Plot> plots;
  uint32_t fences = 0;
  uint32_t area = 0;
  plots.push(p_start);
  while (plots.size()) {
    Plot p = plots.top();
    plots.pop();
    if (garden[p.y][p.x].fenced) {
      continue;
    }
    garden[p.y][p.x].fenced = true;
    fences += 4;
    area += 1;
    if (DEBUG_PRINT) {
      std::cout << "At plot (" << p.x << "," << p.y
                << ") with area of: " << area << " and fences of: " << fences
                << std::endl;
    }
    int i = 0;
    while (i < 4) {
      int x = p.x + adj[i][0];
      int y = p.y + adj[i][1];
      i++;
      if (DEBUG_PRINT) {
        std::cout << "Checking adj position (" << x << "," << y << "): ";
      }

      if (y < 0 || x < 0 || y >= garden.size() || x >= garden.at(0).size()) {
        if (DEBUG_PRINT) {
          std::cout << "out of bounds." << std::endl;
        }
        continue;
      }

      if (garden[y][x].type != p.type) {
        if (DEBUG_PRINT) {
          std::cout << "bad type." << std::endl;
        }
        continue;
      }

      if (garden[y][x].fenced) {
        fences -= 2;
        if (DEBUG_PRINT) {
          std::cout << "already fenced, update fences: " << fences << std::endl;
        }
        continue;
      }

      plots.push(garden[y][x]);
      if (DEBUG_PRINT) {
        std::cout << "continous plot found." << std::endl;
      }
    }
  }

  return fences * area;
}

static uint64_t fenceCost(std::vector<std::vector<Plot>> &garden) {
  uint64_t cost = 0;
  for (int y = 0; y < garden.size(); y++) {
    for (int x = 0; x < garden.at(y).size(); x++) {

      if (!garden[y][x].fenced) {
        cost += fenceCostPlot(garden[y][x], garden);
        if (DEBUG_PRINT) {
          printGarden(garden);
          std::cout << "current cost: " << cost << std::endl;
          std::cout << "--------------------------------------------------"
                    << std::endl;
        }
      }
    }
  }
  return cost;
}

static uint32_t markRegion(Plot &p_start,
                           std::vector<std::vector<Plot>> &garden, int r) {
  int adj[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  std::stack<Plot> plots;
  uint32_t size = 0;
  p_start.r = r;
  plots.push(p_start);
  while (plots.size()) {
    Plot p = plots.top();
    plots.pop();
    size += 1;
    int i = 0;
    if (DEBUG_PRINT) {
      std::cout << "At position (" << p.x << "," << p.y << ") " << std::endl;
    }
    while (i < 4) {
      int x = p.x + adj[i][0];
      int y = p.y + adj[i][1];
      i++;
      if (DEBUG_PRINT) {
        std::cout << "Checking adj position (" << x << "," << y << "): ";
      }

      if (y < 0 || x < 0 || y >= garden.size() || x >= garden.at(0).size()) {
        if (DEBUG_PRINT) {
          std::cout << "out of bounds." << std::endl;
        }
        continue;
      }

      if (garden[y][x].type != p.type) {
        if (DEBUG_PRINT) {
          std::cout << "bad type." << std::endl;
        }
        continue;
      }
      if (garden[y][x].r != -1) {
        if (DEBUG_PRINT) {

          std::cout << "region already marked." << std::endl;
        }
        continue;
      }

      garden[y][x].r = r;
      plots.push(garden[y][x]);
      if (DEBUG_PRINT) {
        std::cout << "continous plot found." << std::endl;
        std::cout << "Region: " << garden[y][x].r << std::endl;
      }
    }
  }

  return size;
}

/*
 * Counting the bulk cost of fences. Each edge require 1 fence. Since an edge
 * divides regions and can be of different length, we iterate through the garden
 * from each of the four sides, and also count all the regions on the boarder.
 */
static uint64_t bulkCostFence(std::vector<std::vector<Plot>> &garden,
                              std::vector<std::array<uint32_t, 2>> &regions) {
  uint64_t fences = 0;
  int x = 0;
  int y = 0;

  // Horisontal fences
  //  Check top row
  uint32_t delta_fences = 0;
  int this_region = -1; // historic value of current row region
  int that_region = -1; // historic value of adj row region

  while (x < garden[y].size()) {
    int r = garden[y][x].r;
    if (garden[y][x].r != this_region) {
      regions[r][1] += 1;
      delta_fences += 1;
      this_region = garden[y][x].r;
    }
    x++;
  }

  if (DEBUG_PRINT) {
    std::cout << "top row count: " << delta_fences << std::endl;
  }
  fences += delta_fences;
  delta_fences = 0;

  /*
   * Count horisontal edges from top to bottom.
   * Definition of (new) horizontal edge:
   * (x,y) != (x,y+1) AND ( (x,y) != (x-1,y) OR (x,y+1) != (x-1,y+1) )
   *
   * Since we want to count edges from two sides, top bottom - bottom top, and
   * left right - right left, we need to keep track if there is an ongoing edge
   * or not.
   * Example:
   * 112
   * 444
   * Give 1 edge from top to bottom and 2 edges from bottom to top.
   * top row count gives 2, bottom row count gives 1. Total of 6 horisontal
   * edges.
   */
  while (y < garden.size() - 1) {
    x = 0;
    this_region = -1;
    that_region = -1;
    bool edge = false;
    while (x < garden[y].size()) {
      int r = garden[y + 1][x].r;
      if (garden[y][x].r != garden[y + 1][x].r) {
        if (garden[y][x].r != this_region && !edge) {
          delta_fences += 1;
          regions[r][1] += 1;
        } else if (garden[y + 1][x].r != that_region) {
          delta_fences += 1;
          regions[r][1] += 1;
        }
        edge = true;
      } else {
        edge = false;
      }
      this_region = garden[y][x].r;
      that_region = garden[y + 1][x].r;
      x++;
    }
    y++;
  }

  if (DEBUG_PRINT) {
    std::cout << "top to bottom count: " << delta_fences << std::endl;
  }
  fences += delta_fences;
  delta_fences = 0;

  // count bottom row
  x = 0;
  this_region = -1;
  x++;
  while (x < garden[y].size()) {
    int r = garden[y][x].r;
    if (garden[y][x].r != this_region) {
      delta_fences += 1;
      regions[r][1] += 1;
      this_region = garden[y][x].r;
    }
    x++;
  }

  if (DEBUG_PRINT) {
    std::cout << "bottom row count: " << delta_fences << std::endl;
  }
  fences += delta_fences;
  delta_fences = 0;

  // bottom to top
  while (y > 0) {
    x = 0;
    this_region = -1;
    that_region = -1;
    bool edge = false;
    while (x < garden[y].size()) {
      int r = garden[y - 1][x].r;
      if (garden[y][x].r != garden[y - 1][x].r) {
        if (garden[y][x].r != this_region && !edge) {
          delta_fences += 1;
          regions[r][1] += 1;
        } else if (garden[y - 1][x].r != that_region) {
          delta_fences += 1;
          regions[r][1] += 1;
        }
        edge = true;
      } else {
        edge = false;
      }
      this_region = garden[y][x].r;
      that_region = garden[y - 1][x].r;
      x++;
    }
    y--;
  }

  if (DEBUG_PRINT) {
    std::cout << "bottom to top count: " << delta_fences << std::endl;
  }

  fences += delta_fences;
  delta_fences = 0;

  if (DEBUG_PRINT) {
    std::cout << "horisontal fences: " << fences << std::endl;
  }

  // count left edge
  x = 0;
  y = 0;
  this_region = -1;
  while (y < garden.size()) {
    int r = garden[y][x].r;
    if (garden[y][x].r != this_region) {
      delta_fences += 1;
      regions[r][1] += 1;
      this_region = garden[y][x].r;
    }
    y++;
  }

  if (DEBUG_PRINT) {
    std::cout << "left edge count: " << delta_fences << std::endl;
  }
  fences += delta_fences;
  delta_fences = 0;

  // count from left to right
  while (x < garden[0].size() - 1) {
    this_region = -1;
    that_region = -1;
    bool edge = false;
    y = 0;
    while (y < garden.size()) {
      int r = garden[y][x + 1].r;
      if (garden[y][x].r != garden[y][x + 1].r) {
        if (garden[y][x].r != this_region && !edge) {
          delta_fences += 1;
          regions[r][1] += 1;
        } else if (garden[y][x + 1].r != that_region) {
          delta_fences += 1;
          regions[r][1] += 1;
        }
        edge = true;
      } else {
        edge = false;
      }

      this_region = garden[y][x].r;
      that_region = garden[y][x + 1].r;
      y++;
    }
    x++;
  }

  if (DEBUG_PRINT) {
    std::cout << "left to right edge count: " << delta_fences << std::endl;
  }
  fences += delta_fences;
  delta_fences = 0;

  // count right edge
  y = 0;
  this_region = -1;
  while (y < garden.size()) {
    int r = garden[y][x].r;
    if (garden[y][x].r != this_region) {
      delta_fences += 1;
      regions[r][1] += 1;
      this_region = garden[y][x].r;
    }
    y++;
  }

  if (DEBUG_PRINT) {
    std::cout << "right edge count: " << delta_fences << std::endl;
  }
  fences += delta_fences;
  delta_fences = 0;

  // count from right to left
  while (x > 0) {
    this_region = -1;
    that_region = -1;
    bool edge = false;
    y = 0;
    while (y < garden.size()) {
      int r = garden[y][x - 1].r;
      if (garden[y][x].r != garden[y][x - 1].r) {
        if (garden[y][x].r != this_region && !edge) {
          delta_fences += 1;
          regions[r][1] += 1;
        } else if (garden[y][x - 1].r != that_region) {
          delta_fences += 1;
          regions[r][1] += 1;
        }
        edge = true;
      } else {
        edge = false;
      }

      this_region = garden[y][x].r;
      that_region = garden[y][x - 1].r;
      y++;
    }
    x--;
  }

  if (DEBUG_PRINT) {
    std::cout << "right to left edge count: " << delta_fences << std::endl;
  }
  fences += delta_fences;
  delta_fences = 0;

  return fences;
}

static uint64_t fenceBulkCost(std::vector<std::vector<Plot>> &garden) {
  uint64_t cost = 0;
  std::vector<std::array<uint32_t, 2>> regions;
  for (int y = 0; y < garden.size(); y++) {
    for (int x = 0; x < garden.at(y).size(); x++) {
      if (garden[y][x].r == -1) {
        uint32_t size = markRegion(garden[y][x], garden, regions.size());
        regions.push_back({size, 0});
        if (DEBUG_PRINT) {
          printGarden(garden);
          std::cout << "current regions: " << cost << std::endl;
          std::cout << "--------------------------------------------------"
                    << std::endl;
        }
      }
    }
  }

  uint32_t total_fences = bulkCostFence(garden, regions);
  if (DEBUG_PRINT) {
    std::cout << "Total amount of needed fences are " << total_fences
              << std::endl;
  }

  for (int r = 0; r < regions.size(); r++) {
    if (DEBUG_PRINT) {
      std::cout << "Region " << r << " has size of " << regions[r][0]
                << " and needs " << regions[r][1] << " fences." << std::endl;
    }
    cost += (regions[r][0] * regions[r][1]);
  }

  return cost;
}

void Day12::run(std::ifstream &file) {
  std::vector<std::vector<Plot>> garden;
  std::string line;
  int y = 0;
  while (getline(file, line)) {
    std::vector<Plot> row;
    int x = 0;
    for (char c : line) {
      row.push_back(Plot(c, x, y));
      x++;
    }
    y++;
    garden.push_back(row);
  }
  if (DEBUG_PRINT) {
    printGarden(garden);
  }
  std::cout << "Executing Part 1" << std::endl;
  uint64_t cost = fenceCost(garden);
  std::cout << "Total cost is: " << cost << std::endl;
  std::cout << "Executing Part 2" << std::endl;
  cost = fenceBulkCost(garden);
  std::cout << "Total cost is: " << cost << std::endl;
}
