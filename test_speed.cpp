#include <ctime>
#include <iostream>

#include "sudoku.h"

int main() {
  sudoku game;
  std::clock_t t1, t2;
  int a[6] = {0};

  t1 = clock();
  for (int i = 0; i < 1000; i++) {
    game.newGame();
    a[game.diff]++;
  }
  t2 = clock() - t1;

  for (int i = 1; i < 6; i++) std::cout << a[i] << " ";
  std::cout << std::endl;
  std::cout << 1000.0 * t2 / CLOCKS_PER_SEC << "ms" << std::endl;
}
