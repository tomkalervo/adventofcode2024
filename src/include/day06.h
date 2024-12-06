#pragma once
#include <fstream>
#include <list>
#include <string>
#include <vector>

class Day06 {
public:
  static void run(std::ifstream &file);

private:
  struct Position {
    int x;
    int y;
    int xd;
    int yd;
    int in_range = 1;
    Position(int x_in, int y_in, int x_dir, int y_dir)
        : x(x_in), y(y_in), xd(x_dir), yd(y_dir) {}
    void advance(const std::vector<std::vector<char>> &map) {
      if (x + xd < 0 || y + yd < 0) {
        in_range = 0;
        return;
      }
      if (y + yd >= map.size()) {
        in_range = 0;
        return;
      }
      if (x + xd >= map[0].size()) {
        in_range = 0;
        return;
      }

      if ('#' == map[y + yd][x + xd]) {
        // Rotate guard 90 degrees clockwise
        // Rotation matrix * [x y]^T:
        // x = x cos(90) - y sin(90)
        // y = x sin(90) + y cos(90)
        int tmp_d = xd;
        xd = 0 - yd;
        yd = tmp_d;
        return;
      }
      x += xd;
      y += yd;
    }
  } Position;

  static bool guardLoop(int x, int y, int xd, int xy,
                        const std::vector<std::vector<char>> &map,
                        const std::vector<std::vector<char>> &visit);
  static bool patrolGuard(const std::vector<std::vector<char>> &map,
                          std::vector<std::vector<char>> &visit);
};
