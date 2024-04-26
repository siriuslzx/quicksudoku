/**
 * Copyright (c) 2024 Zhongxian Li
 * quick sudoku is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 */
#pragma once

#include "common.h"

namespace li {
class Sudoku {
 public:
  Sudoku();
  ~Sudoku();

  void newGame();
  bool getNum(int r, int c, int &num) const;
  void setNum(int r, int c, int num);
  void flipNote(int r, int c, int num);

  bool getSingle(int &r, int &c, int &num) const;
  bool lineRemove();
  bool circleRemove();
  bool assumeRemove();

  int diff;

 private:
  int _diff;
  Array9i _board[10];
  Array9i _ans;
};
}  // namespace li
