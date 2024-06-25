#include <ctime>
#include <iostream>
#include <string>

#include "sudoku.h"

const int times = 1000;
const int kLevel = 6;
constexpr double kilo = 1000.0;

using li::Difficulty;

void print(li::Sudoku &game, Difficulty dif, const std::string &s) {
  std::clock_t t1, t2;
  int a[kLevel] = {0};
  t1 = clock();
  for (int i = 0; i < times; i++) {
    a[game.newGame(dif)]++;
  }
  t2 = clock() - t1;
  std::cout << s << ": " << kilo * t2 / CLOCKS_PER_SEC << "ms" << std::endl;
  for (int i = 1; i < kLevel; i++) std::cout << a[i] << " ";
  std::cout << std::endl << std::endl;
}

int main() {
  li::Sudoku game;

  print(game, Difficulty::easy, "easy");
  print(game, Difficulty::medium, "medium");
  print(game, Difficulty::hard, "hard");
}
