#include "../include/day20.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <utility>
#include <vector>

const bool DEBUG = false;

/*
 * this function is used in part 2 and finds all cheat that would decrease the
 * time of the race. The cheat rule is that once per race, collision damage may
 * be deactivated for up to 20 picoseconds(steps)
 */
static uint64_t modernCheatRace(
    const std::pair<uint32_t, uint32_t> &start,
    const std::pair<uint32_t, uint32_t> &end,
    const std::vector<std::vector<std::pair<uint32_t, uint32_t>>> &path,
    int32_t &limit) {
  // Build datastructure that has time at each step along the honest path
  std::vector<std::pair<uint32_t, uint32_t>> path_order;
  std::vector<std::vector<uint64_t>> times(
      path.size(), std::vector<uint64_t>(path[0].size(), UINT64_MAX));
  uint64_t time = 0;
  auto p = end;
  times[p.first][p.second] = time;
  path_order.push_back(p);
  while (p != start) {
    time++;
    p = path[p.first][p.second];
    times[p.first][p.second] = time;
    path_order.push_back(p);
  }
  // Make a 20x20 window to scan the track
  uint64_t cheat_times = 0;
  const uint64_t max_time = time;
  int32_t window = 20;
  for (auto [y1, x1] : path_order) {
    for (uint32_t y2 = y1 < window ? 0 : y1 - window;
         y2 <= y1 + window && y2 < times.size(); y2++) {
      for (uint32_t x2 = x1 < window ? 0 : x1 - window;
           x2 <= x1 + window && x2 < times[y2].size(); x2++) {
        if (times[y2][x2] > max_time) {
          continue;
        }
        int32_t dx = (y2 - y1);
        int32_t dy = (x2 - x1);
        int32_t distance = std::abs(dy) + std::abs(dx);
        if (distance > 20 or distance < 1) {
          continue;
        }
        time = times[y1][x1] + distance + (max_time - times[y2][x2]);
        if (time <= max_time - limit) {
          cheat_times++;
          continue;
        }
      }
    }
  }

  return cheat_times;
}

static std::list<uint32_t>
cheatRace(const std::pair<uint32_t, uint32_t> &start,
          const std::pair<uint32_t, uint32_t> &end,
          const std::vector<std::vector<std::pair<uint32_t, uint32_t>>> &path) {
  // Build datastructure that has time at each step along the honest path
  std::vector<std::vector<uint32_t>> times(
      path.size(), std::vector<uint32_t>(path[0].size(), UINT32_MAX));
  uint32_t time = 0;
  auto p = end;
  times[p.first][p.second] = time;
  while (p != start) {
    time++;
    p = path[p.first][p.second];
    times[p.first][p.second] = time;
  }
  // Follow the path and store potential cheat paths time improvement
  std::list<uint32_t> cheat_times;
  const uint32_t max_time = time;
  if (DEBUG) {
    std::cout << "Max time: " << max_time << std::endl;
  }
  time = 0;
  p = end;
  while (p != start) {
    if (DEBUG) {
      std::cout << "At (" << p.first << "," << p.second
                << "), time: " << times[p.first][p.second] << std::endl;
    }
    for (int32_t d : {-2, 2}) {
      int32_t dy = p.first + d;
      if (dy < 0 || dy >= times.size()) {
        continue;
      }
      time = times[p.first][p.second] + (max_time - times[dy][p.second]) + 2;
      if (time >= max_time) {
        continue;
      }
      cheat_times.insert(std::lower_bound(cheat_times.begin(),
                                          cheat_times.end(), (max_time - time)),
                         (max_time - time));
    }
    for (int32_t d : {-2, 2}) {
      int32_t dx = p.second + d;
      if (dx < 0 || dx >= times[0].size()) {
        continue;
      }
      time = times[p.first][p.second] + (max_time - times[p.first][dx]) + 2;
      if (time >= max_time) {
        continue;
      }
      cheat_times.insert(std::lower_bound(cheat_times.begin(),
                                          cheat_times.end(), (max_time - time)),
                         (max_time - time));
    }
    p = path[p.first][p.second];
  }
  return cheat_times;
}
static std::vector<std::vector<std::pair<uint32_t, uint32_t>>>
honestRace(const std::vector<std::vector<char>> &track,
           const std::pair<uint32_t, uint32_t> &start,
           const std::pair<uint32_t, uint32_t> &end) {
  // Visited Matrix to keep track of visited paths
  std::vector<std::vector<bool>> visited(
      track.size(), std::vector<bool>(track[0].size(), false));
  // Path Matrix to enable backtracking from End to Start
  std::vector<std::vector<std::pair<uint32_t, uint32_t>>> path(
      track.size(),
      std::vector<std::pair<uint32_t, uint32_t>>(track[0].size(), {0, 0}));
  // A FIFO queue to fuel the BFS
  std::queue<std::pair<int32_t, int32_t>> q;
  q.push(start);
  while (q.size()) {
    auto p = q.front();
    q.pop();
    if (p == end) {
      break;
    }
    auto [y, x] = p;
    for (int32_t dy : {-1, 1}) {
      if (track[y + dy][x] != '.') {
        continue;
      }
      if (visited[y + dy][x]) {
        continue;
      }
      visited[y + dy][x] = true;
      path[y + dy][x] = {y, x};
      q.push({y + dy, x});
    }
    for (int32_t dx : {-1, 1}) {
      if (track[y][x + dx] != '.') {
        continue;
      }
      if (visited[y][x + dx]) {
        continue;
      }
      visited[y][x + dx] = true;
      path[y][x + dx] = {y, x};
      q.push({y, x + dx});
    }
  }

  return path;
}

static uint64_t
findModernCheatPaths(const std::vector<std::vector<char>> &track,
                     const std::pair<uint32_t, uint32_t> &start,
                     const std::pair<uint32_t, uint32_t> &end) {
  auto path = honestRace(track, start, end);
  int32_t limit = 100;
  auto cheat_times = modernCheatRace(start, end, path, limit);

  return cheat_times;
}

static uint32_t findCheatPaths(const std::vector<std::vector<char>> &track,
                               const std::pair<uint32_t, uint32_t> &start,
                               const std::pair<uint32_t, uint32_t> &end) {
  auto path = honestRace(track, start, end);
  auto cheat_times = cheatRace(start, end, path);
  int limit = 100;
  uint32_t cheats = 0;
  for (auto time : cheat_times) {
    if (time >= limit) {
      if (DEBUG) {
        std::cout << "Cheat time improvement: " << time << std::endl;
      }
      cheats++;
    }
  }
  return cheats;
}
void Day20::run(std::ifstream &file) {
  std::string line;
  std::vector<std::vector<char>> track;
  while (getline(file, line)) {
    std::vector<char> row;
    for (char c : line) {
      row.push_back(c);
    }
    track.push_back(row);
  }
  std::pair<uint32_t, uint32_t> start;
  std::pair<uint32_t, uint32_t> end;
  for (int y = 0; y < track.size(); y++) {
    for (int x = 0; x < track[y].size(); x++) {
      if (track[y][x] == 'S') {
        start = {y, x};
        track[y][x] = '.';
      } else if (track[y][x] == 'E') {
        end = {y, x};
        track[y][x] = '.';
      }
    }
  }
  std::cout << "start at (" << start.second << "," << start.first
            << "), end at (" << end.first << "," << end.second << ")."
            << std::endl;
  std::cout << "Executing Part 1" << std::endl;
  uint32_t cheats = findCheatPaths(track, start, end);
  std::cout << "Number of (better) ways to cheat: " << cheats << std::endl;
  std::cout << "Executing Part 2" << std::endl;
  cheats = findModernCheatPaths(track, start, end);
  std::cout << "Number of (better) ways to cheat: " << cheats << std::endl;
}
