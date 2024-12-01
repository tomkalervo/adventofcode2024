#include "include/day01.h"
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

int DAYS_COMPLETED = 1;

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
  std::cout << "Runnung challenge " << day << std::endl;
  if (1 == day) {
    Day01::run(file);
  } else {
    std::cerr << "Did not find a matching challenge to run." << std::endl;
  }

  return 0;
}