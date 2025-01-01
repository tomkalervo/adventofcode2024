#include "../include/day23.h"
#include <assert.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <list>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using std::cout;
using std::endl;
using std::list;
using std::set;
using std::string;
using std::unordered_map;
using std::vector;

const bool DEBUG = false;

// Recursive function for finding all connected vertices in a set of a certain
// depth
static void checkConnection(list<list<uint32_t>> &set, list<uint32_t> conn,
                            const vector<list<uint32_t>> &adj_v,
                            vector<bool> visited, int depth) {
  if (depth < 0) {
    std::cerr << "invalid depth." << endl;
    exit(1);
  }
  if (depth == 0) {
    for (auto u : adj_v[conn.back()]) {
      if (u == conn.front()) {
        set.push_back(conn);
        return;
      }
    }
    return;
  }
  for (auto u : adj_v[conn.back()]) {
    if (visited[u]) {
      continue;
    }
    visited[u] = true;
    list<uint32_t> c = conn;
    c.push_back(u);
    checkConnection(set, c, adj_v, visited, depth - 1);
  }
}
static uint32_t getConnectedSet(const vector<list<uint32_t>> &adj_v,
                                const vector<string> &map_i) {
  uint32_t sets = 0;
  int depth = 3;
  list<list<uint32_t>> set;
  vector<bool> visited(map_i.size(), false);
  for (uint32_t v = 0; v < adj_v.size(); v++) {
    if (visited[v]) {
      continue;
    }
    visited[v] = true;
    vector<bool> set_visit = visited;
    checkConnection(set, list<uint32_t>{v}, adj_v, set_visit, depth - 1);
  }

  for (auto s : set) {
    for (auto u : s) {
      if (map_i[u][0] == 't') {
        // The Chief Historians Computer start with 't'
        sets += 1;
        break;
      }
    }
  }

  return sets;
}

/* Pseudocode for BronKerbosch1:
 *
  algorithm BronKerbosch1(R, P, X) is
    if P and X are both empty then
        report R as a maximal clique
    for each vertex v in P do
        BronKerbosch1(R ⋃ {v}, P ⋂ N(v), X ⋂ N(v))
        P := P \ {v}
        X := X ⋃ {v}algorithm BronKerbosch1(R, P, X) is
    if P and X are both empty then
        report R as a maximal clique
    for each vertex v in P do
        BronKerbosch1(R ⋃ {v}, P ⋂ N(v), X ⋂ N(v))
        P := P \ {v}
        X := X ⋃ {v}
*
* Source: https://en.wikipedia.org/wiki/Bron–Kerbosch_algorithm
*/
static void findClique(set<uint32_t> R, set<uint32_t> P, set<uint32_t> X,
                       list<set<uint32_t>> &cliques,
                       const vector<list<uint32_t>> &adj_v) {
  if (P.empty() && X.empty()) {
    cliques.push_back(R);
    return;
  }
  auto P_fixed = P;
  for (auto v : P_fixed) {
    auto r = R;
    if (r.find(v) == r.end()) {
      r.insert(v);
    }
    set<uint32_t> p;
    set<uint32_t> x;
    for (auto u : adj_v[v]) {
      if (P.find(u) != P.end()) {
        p.insert(u);
      }
      if (X.find(u) != X.end()) {
        x.insert(u);
      }
    }
    findClique(r, p, x, cliques, adj_v);
    P.erase(v);
    X.insert(v);
  }
}

static string largestConnectedSet(const vector<list<uint32_t>> &adj_v,
                                  const vector<string> &map_i) {
  list<set<uint32_t>> cliques;
  set<uint32_t> P;
  for (uint32_t p = 0; p < adj_v.size(); p++) {
    P.insert(p);
  }
  findClique(set<uint32_t>(), P, set<uint32_t>(), cliques, adj_v);

  set<uint32_t> max_clique;
  for (auto clique : cliques) {
    if (clique.size() > max_clique.size()) {
      max_clique = clique;
    }
  }

  // Convert result into a alphabetically sorted csv string
  string connected_computers = "";
  list<string> comps;
  for (auto v : max_clique) {
    auto it = comps.begin();
    while (it != comps.end()) {
      if (map_i[v] < *it) {
        break;
      }
      it++;
    }
    comps.insert(it, map_i[v]);
  }

  auto it = comps.begin();
  while (it != comps.end()) {
    connected_computers += *it;
    connected_computers.push_back(',');
    it++;
  }

  return connected_computers.substr(0, connected_computers.size() - 1);
}

void Day23::run(std::ifstream &file) {
  string line;
  unordered_map<string, uint32_t> map_v;
  vector<string> map_i;
  vector<list<uint32_t>> adj_v;
  while (getline(file, line)) {
    string a = line.substr(0, 2);
    string b = line.substr(3, 2);
    assert(a + '-' + b == line);
    if (map_v.find(a) == map_v.end()) {
      map_v[a] = map_i.size();
      map_i.push_back(a);
      adj_v.push_back(list<uint32_t>());
    }
    if (map_v.find(b) == map_v.end()) {
      map_v[b] = map_i.size();
      map_i.push_back(b);
      adj_v.push_back(list<uint32_t>());
    }
    adj_v[map_v[a]].push_back(map_v[b]);
    adj_v[map_v[b]].push_back(map_v[a]);
  }

  cout << "Executing Part 1" << endl;
  uint32_t sum = getConnectedSet(adj_v, map_i);
  cout << "Number of interconnected sets: " << sum << endl;
  cout << "Executing Part 2" << endl;
  string computers = largestConnectedSet(adj_v, map_i);
  cout << "The computers involved in the LAN party are: " << computers << endl;
}
