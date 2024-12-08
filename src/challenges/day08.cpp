#include "../include/day08.h"
#include <cstdint>
#include <iostream>
#include <list>
#include <vector>

struct Antenna {
  int x;
  int y;
  int idx = -1;
  char type;
  Antenna(int x_in, int y_in, char type_in) : x(x_in), y(y_in), type(type_in) {}
};

/*
 * Helperfunction to print where the program has marked antennas on the Grid
 */
void printLocations(const std::vector<uint8_t> location, const int &wdith) {

  for (int i = 0; i < location.size(); i++) {
    if (i % wdith == 0) {
      std::cout << "\n";
    }
    if (0 == location[i]) {
      std::cout << '.';
    } else if (1 == location[i]) {
      std::cout << 'A';
    } else if (2 == location[i]) {
      std::cout << '#';
    }
  }
  std::cout << std::endl;
}

/*
 * Part 1:
 * For every couple in alist, find the position of their antinodes. Check if
 * antinodes are within the boarder and if their location is free.
 *
 */
int locateAntiNodes(const std::list<Antenna> &alist,
                    std::vector<uint8_t> &location, const int &height,
                    const int &width) {
  int anodes = 0;
  auto it_a = alist.begin();
  while (it_a != alist.end()) {
    auto it_b = it_a;
    it_b++;
    while (it_b != alist.end()) {
      // forst anitnode
      int an_x = it_a->x + (it_a->x - it_b->x);
      int an_y = it_a->y + (it_a->y - it_b->y);
      if (an_x >= 0 && an_x < width && an_y >= 0 && an_y < height) {
        if (2 != location[(an_y * width) + an_x]) {
          location[(an_y * width) + an_x] = 2;
          anodes += 1;
        }
      }
      // second antinode
      an_x = it_b->x + (it_b->x - it_a->x);
      an_y = it_b->y + (it_b->y - it_a->y);
      if (an_x >= 0 && an_x < width && an_y >= 0 && an_y < height) {
        if (2 != location[(an_y * width) + an_x]) {
          location[(an_y * width) + an_x] = 2;
          anodes += 1;
        }
      }
      it_b++;
    }
    it_a++;
  }
  return anodes;
}

/*
 * Part 2:
 * Now antinodes continously appear along the line that intersects the antennas.
 * Having the same distance as before.
 *
 */
int locateResonantAntiNodes(const std::list<Antenna> &alist,
                            std::vector<uint8_t> &location, const int &height,
                            const int &width) {
  int anodes = 0;
  auto it_a = alist.begin();
  while (it_a != alist.end()) {
    auto it_b = it_a;
    it_b++;
    while (it_b != alist.end()) {
      // first direction along path of anitnodes
      int x_dir = (it_a->x - it_b->x);
      int y_dir = (it_a->y - it_b->y);
      int x = it_a->x;
      int y = it_a->y;
      while (x >= 0 && y >= 0 && x < width && y < height) {
        if (2 != location[(y * width) + x]) {
          location[(y * width) + x] = 2;
          anodes += 1;
        }
        x += x_dir;
        y += y_dir;
      }
      // second direction along path of antinodes
      x_dir = (it_b->x - it_a->x);
      y_dir = (it_b->y - it_a->y);
      x = it_b->x;
      y = it_b->y;
      while (x >= 0 && y >= 0 && x < width && y < height) {
        if (2 != location[(y * width) + x]) {
          location[(y * width) + x] = 2;
          anodes += 1;
        }
        x += x_dir;
        y += y_dir;
      }
      it_b++;
    }
    it_a++;
  }
  return anodes;
}

void Day08::run(std::ifstream &file) {
  std::vector<std::list<Antenna>> antennas;
  std::vector<uint8_t> locations;
  std::string line;
  int y = 0;
  int x = 0;
  while (getline(file, line)) {
    x = 0;
    while (x < line.length()) {
      if (line.at(x) != '.') {
        locations.push_back(1);
        bool new_type = true;
        Antenna a(x, y, line.at(x));
        int idx = 0;
        while (idx < antennas.size()) {
          if (line.at(x) == antennas[idx].front().type) {
            new_type = false;
            a.idx = idx;
            antennas[idx].push_back(a);
          }
          idx++;
        }
        if (new_type) {
          a.idx = idx;
          antennas.push_back(std::list<Antenna>({a}));
        }
      } else {
        locations.push_back(0);
      }
      x++;
    }
    y++;
  }
  int width = x;
  int height = y;

  {
    std::cout << "Executing Part 1" << std::endl;
    std::vector<uint8_t> location_part1 = locations;
    int sum = 0;
    for (int i = 0; i < antennas.size(); i++) {
      sum += locateAntiNodes(antennas[i], location_part1, height, width);
    }
    std::cout << "Total sum: " << sum << std::endl;
  }
  {
    std::cout << "Executing Part 2" << std::endl;
    std::vector<uint8_t> location_part2 = locations;
    // printLocations(location_part2, width);
    int sum = 0;
    for (int i = 0; i < antennas.size(); i++) {
      sum +=
          locateResonantAntiNodes(antennas[i], location_part2, height, width);
      // printLocations(location_part2, width);
    }
    std::cout << "Total sum: " << sum << std::endl;
  }
}
