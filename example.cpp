#include <iostream>

#include "sudoku.h"

int main() {
  int num;
  sudoku game;
  game.newGame();
  for (int y = 0; y < 9; y++) {
    for (int x = 0; x < 9; x++) {
      if (game.getNum(x, y, num)) {
        std::cout << num;
      } else {
        std::cout << 0;
      }
    }
    std::cout << std::endl;
  }
}
