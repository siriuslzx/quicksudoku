#include <ctime>
#include <iostream>

#include "sudoku.h"

const int times = 10000;
const int kLevel = 6;
constexpr double kilo = 1000.0;

int main() {
  li::Sudoku game;
  std::clock_t t1, t2;
  int a[kLevel] = {0};

  t1 = clock();
  for (int i = 0; i < times; i++) {
    game.newGame();
    a[game.diff]++;
  }
  t2 = clock() - t1;

  for (int i = 1; i < kLevel; i++) std::cout << a[i] << " ";
  std::cout << std::endl;
  std::cout << kilo * t2 / CLOCKS_PER_SEC << "ms" << std::endl;
}
