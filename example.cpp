#include <iostream>

#include "sudoku.h"

int main() {
  int num;
  li::Sudoku game;
  game.newGame();
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (game.getNum(i, j, num)) {
        std::cout << num;
      } else {
        std::cout << 0;
      }
    }
    std::cout << std::endl;
  }
}
