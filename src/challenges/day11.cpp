#include "../include/day11.h"
#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

struct Stone {
  uint64_t value;
  Stone *next = nullptr;
  Stone(uint64_t value_in) : value(value_in) {}
};

const int digits(const uint64_t &value) {
  int digits = 0;
  uint64_t tmp = value;
  while (tmp) {
    tmp /= 10;
    digits++;
  }
  return digits;
}

static Stone *split(Stone *&stone) {
  // Split the value
  // std::cout << "Split " << stone->value << std::endl;
  int d = digits(stone->value);
  uint64_t value_left = stone->value;
  int i = 0;
  while (i < d / 2) {
    value_left /= 10;
    i++;
  }
  uint64_t value_right = stone->value - (value_left * std::pow(10, i));

  // std::cout << "Left: " << value_left << ", Right: " << value_right
  //          << std::endl;
  // Add a new stone
  // Update the values
  stone->value = value_left;
  Stone *new_stone = new Stone(value_right);
  new_stone->next = stone->next;
  stone->next = new_stone;
  return new_stone;
}

static void blink(Stone *&this_stone) {
  if (!this_stone) {
    return;
  }
  if (!this_stone->value) {
    // std::cout << " 0 " << std ::endl;
    this_stone->value = 1;
  } else if (!(digits(this_stone->value) % 2)) {
    // std::cout << "Even number of digits: " << this_stone->value << std::endl;
    Stone *new_stone = split(this_stone);
    return blink(new_stone->next);
  } else {
    // std::cout << " No action " << std::endl;
    this_stone->value *= 2024;
  }
  // std::cout << this_stone->value << std::endl;
  blink(this_stone->next);
}

const void printStones(Stone *&stone) {
  if (!stone) {
    std::cout << std::endl;
    return;
  }
  std::cout << " " << stone->value;
  printStones(stone->next);
}
static void deleteStones(Stone *&this_stone) {
  if (!this_stone) {
    return;
  }
  deleteStones(this_stone->next);
  delete (this_stone);
}
static int countStones(Stone *&stone, int acc) {
  if (!stone) {
    return acc;
  }
  return countStones(stone->next, acc + 1);
}

static int blinkingStones(Stone *&stone, int blinks) {
  while (blinks--) {
    printStones(stone);
    blink(stone);
  }
  printStones(stone);
  return countStones(stone, 0);
}
void Day11::run(std::ifstream &filepath) {
  std::string line;
  Stone *head = nullptr;
  Stone *tail = nullptr;
  while (getline(filepath, line)) {
    std::istringstream iss(line);
    uint64_t value;
    while (iss >> value) {
      Stone *this_stone = new Stone(value);
      if (!head) {
        head = this_stone;
        tail = this_stone;
      } else {
        tail->next = this_stone;
        tail = this_stone;
      }
    }
  }
  std::cout << "Executing Part 1" << std::endl;
  int stones_part1 = blinkingStones(head, 25);
  std::cout << "Total amount of stones: " << stones_part1 << std::endl;
  // std::cout << "Executing Part 2" << std::endl;
  // int stones_part2 = blinkingStones(head, 50);
  // std::cout << "Total amount of stones: " << stones_part2 << std::endl;
  std::cout << "Delete stones" << std::endl;
  deleteStones(head);
  return;
}