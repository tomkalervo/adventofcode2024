#include "../include/day11.h"
#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

struct Stone {
  uint64_t amount;
  uint64_t value;
  Stone *next = nullptr;
  Stone(uint64_t value_in) : value(value_in), amount(1) {}
};

// Helper-function to calculate nr of digits in an integer
const int digits(const uint64_t &value) {
  int digits = 0;
  uint64_t tmp = value;
  while (tmp) {
    tmp /= 10;
    digits++;
  }
  return digits;
}

/*
 * Split one stone into two stones, linked together in the list.
 * The left stone gets the first half of the digits of the value,
 * the right stone gets the second half of the digits of the value.
 */
static Stone *split(Stone *&stone_left) {
  // Split the value by even digits
  int d = digits(stone_left->value);
  uint64_t value_left = stone_left->value;
  int i = 0;
  while (i < d / 2) {
    value_left /= 10;
    i++;
  }
  uint64_t value_right = stone_left->value - (value_left * std::pow(10, i));

  // Add a new stone
  // Update the values
  stone_left->value = value_left;
  Stone *stone_right = new Stone(value_right);
  stone_right->next = stone_left->next;
  stone_right->amount = stone_left->amount;
  stone_left->next = stone_right;
  return stone_right;
}

/*
 * This functions updates the linked list according to the rules of the problem:
 * If the stones value is 0, set it to 1.
 * Else if the stones value is an even digit number, split the stone in two and
 * give them each a half of the values digits (Eg. 2024 -> 20 24).
 * Else multiply the stones value by 2024.
 */
static void blink(Stone *&this_stone) {
  if (!this_stone) {
    return;
  }
  if (!this_stone->value) {
    // std::cout << " 0 " << std ::endl;
    this_stone->value = 1;
  } else if (!(digits(this_stone->value) % 2)) {
    Stone *new_stone = split(this_stone);
    return blink(new_stone->next);
  } else {
    this_stone->value *= 2024;
  }
  blink(this_stone->next);
}

// Helper-function for visually inspecting the linked list of stones
const void printStones(Stone *&stone) {
  if (!stone) {
    std::cout << std::endl;
    return;
  }
  std::cout << " " << stone->value;
  printStones(stone->next);
}

// Function for cleaning up memory allocated to the linked list
static void deleteStones(Stone *&this_stone) {
  if (!this_stone) {
    return;
  }
  deleteStones(this_stone->next);
  delete (this_stone);
}

// Iterate through the linked list summing the amount of each stone
static uint64_t countStones(Stone *&stone, uint64_t acc) {
  if (!stone) {
    return acc;
  }
  acc += stone->amount;
  return countStones(stone->next, acc);
}

/*
 * Iterate through the linked list, if there are any duplicates, sum up their
 * amount and remove all but one.
 */
static void pruneStones(Stone *&head) {
  if (!head) {
    return;
  }
  Stone *active_stone = head;
  Stone *stone = active_stone->next;
  Stone *prev_stone = active_stone;
  while (stone) {
    if (stone->value == active_stone->value) {
      active_stone->amount += stone->amount;
      prev_stone->next = stone->next;
      delete (stone);
      stone = prev_stone;
    }
    prev_stone = stone;
    stone = stone->next;
  }
  pruneStones(active_stone->next);
}

/*
 * For each blink, prune the stones for duplicates and keep an internal count.
 * When blinking is done, sum up all amount of each unique stone.
 */
static uint64_t blinkingStones(Stone *&stone, int blinks) {
  while (blinks--) {
    blink(stone);
    pruneStones(stone);
  }
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
  uint64_t stones_part1 = blinkingStones(head, 25);
  std::cout << "Total amount of stones: " << stones_part1 << std::endl;
  std::cout << "Executing Part 2" << std::endl;
  uint64_t stones_part2 = blinkingStones(head, 50);
  std::cout << "Total amount of stones: " << stones_part2 << std::endl;
  std::cout << "Delete stones" << std::endl;
  deleteStones(head);
  return;
}
