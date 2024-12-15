#include "../include/day14.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <list>
#include <sstream>
#include <stdio.h>

const bool DEBUG = false;
const uint32_t WIDTH = 101;
const uint32_t HEIGHT = 103;

struct Robot {
  int32_t px;
  int32_t py;
  int32_t vx;
  int32_t vy;
  Robot(int32_t px_in, int32_t py_in, int32_t vx_in, int32_t vy_in)
      : px(px_in), py(py_in), vx(vx_in), vy(vy_in) {}
};

static void mapRobots(std::list<Robot> &bots) {
  const uint32_t width = DEBUG ? 11 : WIDTH;
  const uint32_t height = DEBUG ? 7 : HEIGHT;
  int map[height][width];
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      map[i][j] = 0;
    }
  }
  for (Robot r : bots) {
    // std::cerr << r.py << r.px << std::endl;
    map[r.py][r.px] += 1;
  }
  printf("***Robots position***\n");
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (map[i][j]) {
        printf("%d", map[i][j]);
      } else {
        printf(".");
      }
    }
    printf("\n");
  }
  printf("*********************\n");
}

/*
 * To find the christmas tree I inspected the output and saw repeating patterns.
 * This functions reuturns the time when the two repeating patterns intersect.
 */
static uint32_t findTree() {
  if (DEBUG) {
    return false;
  }
  // after inspecting the output we have that two states need to intersect to
  // form the image of the tree
  uint32_t stateA_pos = 12;
  uint32_t stateB_pos = 88;
  uint32_t stateA_increment = 101;
  uint32_t stateB_increment = 103;

  while (stateA_pos != stateB_pos) {
    if (stateA_pos < stateB_pos) {
      stateA_pos += stateA_increment;
    } else {
      stateB_pos += stateB_increment;
    }
    if (stateA_pos > 6600) {
      return 0;
    }
  }

  return stateA_pos;
}

static uint64_t safetyFactor(std::list<Robot> &bots) {
  const uint32_t width = DEBUG ? 11 : WIDTH;
  const uint32_t height = DEBUG ? 7 : HEIGHT;
  uint64_t nw_factor = 0;
  uint64_t ne_factor = 0;
  uint64_t sw_factor = 0;
  uint64_t se_factor = 0;
  for (Robot &r : bots) {
    if (r.px < width / 2) {
      if (r.py < height / 2) {
        nw_factor += 1;
      } else if (r.py > height / 2) {
        sw_factor += 1;
      }
    } else if (r.px > width / 2) {
      if (r.py < height / 2) {
        ne_factor += 1;
      } else if (r.py > height / 2) {
        se_factor += 1;
      }
    }
  }
  return nw_factor * ne_factor * sw_factor * se_factor;
}

static void moveBots(std::list<Robot> &bots, const uint32_t &time) {
  uint32_t width = DEBUG ? 11 : WIDTH;
  uint32_t height = DEBUG ? 7 : HEIGHT;
  /*
   * Update the position of all the robots.
   * Adding width/height to negative results turns
   * the remainder into a modulo operator.
   */
  for (Robot &r : bots) {
    // std::cout << "Current (" << r.px << "," << r.py << ") v: (" << r.vx <<
    // ","
    //           << r.vy << ")" << std::endl;
    int32_t vx = r.vx < 0 ? width + r.vx : r.vx;
    int32_t vy = r.vy < 0 ? height + r.vy : r.vy;
    r.px = (r.px + vx * time) % width;
    r.py = (r.py + vy * time) % height;
    // std::cout << "New (" << r.px << "," << r.py << ") v: (" << r.vx << ","
    //           << r.vy << ")" << std::endl;
  }
}

void Day14::run(std::ifstream &file) {
  std::string line;
  std::list<Robot> bots;
  while (getline(file, line)) {
    std::istringstream iss(line);
    char c;
    int32_t px;
    int32_t py;
    int32_t vx;
    int32_t vy;
    if (!(iss >> c)) {
      std::cerr << "could not get p from line: " << line << std::endl;
      return;
    }
    if ('p' != c) {
      std::cerr << "Did not get 'p' from c: " << c << std::endl;
      return;
    }
    if (!(iss >> c)) {
      std::cerr << "could not get = from line: " << line << std::endl;
      return;
    }
    if ('=' != c) {
      std::cerr << "Did not get '=' from c: " << c << std::endl;
      return;
    }
    if (!(iss >> px)) {
      std::cerr << "could not get px from line: " << line << std::endl;
      return;
    }
    if (!(iss >> c)) {
      std::cerr << "could not get ',' from line: " << line << std::endl;
      return;
    }
    if (',' != c) {
      std::cerr << "Did not get ',' from c: " << c << std::endl;
      return;
    }
    if (!(iss >> py)) {
      std::cerr << "could not get py from line: " << line << std::endl;
      return;
    }
    // velocity
    if (!(iss >> c)) {
      std::cerr << "could not get v from line: " << line << std::endl;
      return;
    }
    if ('v' != c) {
      std::cerr << "Did not get 'v' from c: " << c << std::endl;
      return;
    }
    if (!(iss >> c)) {
      std::cerr << "could not get = from line: " << line << std::endl;
      return;
    }
    if ('=' != c) {
      std::cerr << "Did not get '=' from c: " << c << std::endl;
      return;
    }
    if (!(iss >> vx)) {
      std::cerr << "could not get vx from line: " << line << std::endl;
      return;
    }
    if (!(iss >> c)) {
      std::cerr << "could not get ',' from line: " << line << std::endl;
      return;
    }
    if (',' != c) {
      std::cerr << "Did not get ',' from c: " << c << std::endl;
      return;
    }
    if (!(iss >> vy)) {
      std::cerr << "could not get vy from line: " << line << std::endl;
      return;
    }

    bots.push_back(Robot(px, py, vx, vy));
  }

  std::cout << "Loaded " << bots.size() << " number of robots." << std::endl;
  std::list<Robot> bots_part2 = bots;
  std::cout << "Executing Part 1" << std::endl;
  // mapRobots(bots);
  moveBots(bots, 100);
  // mapRobots(bots);
  uint64_t factor = safetyFactor(bots);
  std::cout << "Saftey factor is " << factor << std::endl;

  std::cout << "Executing Part 2" << std::endl;
  // Loop intervan 120060 - 109657 = 10403
  uint32_t time = findTree();
  if (time) {
    moveBots(bots_part2, time);
    mapRobots(bots_part2);
  }
  std::cout << "Seconds until Christmas Tree: " << time << std::endl;
}
