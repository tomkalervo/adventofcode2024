#include "include/day01.h"
#include "include/day02.h"
#include "include/day03.h"
#include "include/day04.h"
#include "include/day05.h"
#include "include/day06.h"
#include "include/day07.h"
#include "include/day08.h"
#include "include/day09.h"
#include "include/day10.h"
#include "include/day11.h"
#include "include/day12.h"
#include "include/day13.h"
#include "include/day14.h"
#include "include/day15.h"
#include "include/day16.h"
#include "include/day17.h"
#include "include/day18.h"
#include "include/day19.h"
#include "include/day20.h"
#include "include/day21.h"
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

int DAYS_COMPLETED = 21;

void usage() {
  std::cout << "./aoc --Day challenge --File filepath" << std::endl;
  std::cout << "Example that runs the first challenge:" << std::endl;
  std::cout << "./aoc --Day 1 --File files/aoc_01.txt" << std::endl;
}

bool isValidDay(const std::string &str) {
  // Check if all characters in the string are digits
  for (char ch : str) {
    if (!std::isdigit(ch)) {
      return false;
    }
  }

  // Convert to integer and check the range
  int num = std::stoi(str);
  return num >= 1 && num <= DAYS_COMPLETED;
}

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cout << "Invalid number of arguments." << std::endl;
    usage();
    return 1;
  }

  // Get which daily challenge to run
  int i = 1;
  if (0 != strcmp("--Day", *(argv + i))) {
    std::cout << "Invalid command: " << *(argv + i) << std::endl;
    usage();
    return 1;
  }

  i++;
  std::string arg = *(argv + i);
  if (!isValidDay(arg)) {
    std::cout << "Invalid digit: " << arg << std::endl;
    usage();
    return 1;
  }
  int day = std::stoi(arg);

  // Get a file handler
  i++;
  if (0 != strcmp("--File", *(argv + i))) {
    std::cout << "Invalid command: " << *(argv + i) << std::endl;
    usage();
    return 1;
  }
  i++;
  arg = *(argv + i);
  std::ifstream file(arg);
  if (!file.good()) {
    std::cout << "Invalid or unreadable file: " << arg << std::endl;
    usage();
    return 1;
  }

  // Launch the challenge
  std::cout << "Running challenge " << day << std::endl;
  if (1 == day) {
    Day01::run(file);
  } else if (2 == day) {
    Day02::run(file);
  } else if (3 == day) {
    Day03::run(file);
  } else if (4 == day) {
    Day04::run(file);
  } else if (5 == day) {
    Day05::run(file);
  } else if (6 == day) {
    Day06::run(file);
  } else if (7 == day) {
    Day07::run(file);
  } else if (8 == day) {
    Day08::run(file);
  } else if (9 == day) {
    Day09::run(file);
  } else if (10 == day) {
    Day10::run(file);
  } else if (11 == day) {
    Day11::run(file);
  } else if (12 == day) {
    Day12::run(file);
  } else if (13 == day) {
    Day13::run(file);
  } else if (14 == day) {
    Day14::run(file);
  } else if (15 == day) {
    Day15::run(file);
  } else if (16 == day) {
    Day16::run(file);
  } else if (17 == day) {
    Day17::run(file);
  } else if (18 == day) {
    Day18::run(file);
  } else if (19 == day) {
    Day19::run(file);
  } else if (20 == day) {
    Day20::run(file);
  } else if (21 == day) {
    Day21::run(file);
  } else {
    std::cerr << "Did not find a matching challenge to run." << std::endl;
  }

  return 0;
}
