#include "../include/day16.h"
#include <array>
#include <cstdint>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

const bool DEBUG = false;

struct Node {
  uint32_t x, y, d, s;
  std::shared_ptr<Node> p;
  Node() : p(nullptr) {}
  Node(uint32_t xin, uint32_t yin, uint32_t din, uint32_t sin,
       Node *pin = nullptr)
      : x(xin), y(yin), d(din), s(sin), p(pin){};
};

struct CompareNode {
  bool operator()(Node &a, Node &b) { return a.s > b.s; }
};

static std::pair<uint32_t, uint32_t>
deerRace(std::vector<std::vector<char>> &maze,
         std::pair<uint32_t, uint32_t> &start,
         std::pair<uint32_t, uint32_t> &goal) {
  uint32_t min_score = UINT32_MAX;
  std::array<int[2], 4> directions({{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}});
  std::vector<std::vector<std::vector<uint32_t>>> scores(
      maze.size(), std::vector<std::vector<uint32_t>>(
                       maze[0].size(), std::vector<uint32_t>(4, UINT32_MAX)));
  std::priority_queue<Node, std::vector<Node>, CompareNode> PQ;
  std::set<std::pair<uint32_t, uint32_t>> UNQ;

  {
    Node n;
    n.x = start.first;
    n.y = start.second;
    n.d = 0;
    n.s = 0;
    scores[n.y][n.x][n.d] = n.s;
    PQ.push(n);
  }

  while (PQ.size()) {
    Node n = PQ.top();
    PQ.pop();
    if (scores[n.y][n.x][n.d] < n.s || min_score < n.s) {
      continue;
    }
    if (goal.first == n.x && goal.second == n.y) {
      if (min_score > n.s) {
        min_score = n.s;
        UNQ.clear();
      }
      if (n.s == min_score) {
        UNQ.insert({n.y, n.x});
        std::shared_ptr<Node> current = n.p;
        while (current != nullptr) {
          UNQ.insert({current->y, current->x});
          current = current->p;
        }
      }
      continue;
    }
    int dy = directions[n.d][0];
    int dx = directions[n.d][1];
    int ny = n.y + dy;
    int nx = n.x + dx;

    if (maze[ny][nx] == '.') {
      int ns = n.s + 1;
      if (scores[ny][nx][n.d] >= ns) {
        scores[ny][nx][n.d] = ns;
        PQ.push(Node(nx, ny, n.d, ns, new Node(n)));
      }
    }
    for (int dd : {1, -1}) {
      int nd = n.d + dd;
      nd = nd > 3 ? nd - 4 : nd;
      nd = nd < 0 ? 4 + nd : nd;
      int ns = n.s + 1000;
      if (scores[n.y][n.x][nd] >= ns) {
        scores[n.y][n.x][nd] = ns;
        PQ.push(Node(n.x, n.y, nd, ns, new Node(n)));
      }
    }
  }

  return {min_score, UNQ.size()};
}

void Day16::run(std::ifstream &filestream) {
  std::vector<std::vector<char>> maze;
  std::string line;
  while (getline(filestream, line)) {
    std::vector<char> row;
    for (char c : line) {
      row.push_back(c);
    }
    maze.push_back(row);
  }
  int y_start = -1;
  int x_start = -1;
  int y_goal = -1;
  int x_goal = -1;
  for (int y = 0; y < maze.size(); y++) {
    for (int x = 0; x < maze[y].size(); x++) {
      if ('E' == maze[y][x]) {
        y_goal = y;
        x_goal = x;
        maze[y][x] = '.';
      } else if ('S' == maze[y][x]) {
        y_start = y;
        x_start = x;
        maze[y][x] = '.';
      }

      if (y_start >= 0 && y_goal >= 0) {
        break;
      }
    }
  }
  if (y_start < 0 || x_start < 0) {
    std::cerr << "Could not get starting position from Maze." << std::endl;
    return;
  }
  std::pair<uint32_t, uint32_t> start = {x_start, y_start};
  std::pair<uint32_t, uint32_t> goal = {x_goal, y_goal};
  std::cout << "Executing Part 1 & Part 2" << std::endl;
  auto score = deerRace(maze, start, goal);
  std::cout << "Lowest possible score: " << score.first << std::endl;
  std::cout << "Amount of tiles along best paths: " << score.second
            << std::endl;
}
