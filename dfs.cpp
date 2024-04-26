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
#include "dfs.h"

namespace li {
void dfsGuess(DfsImpl &impl, int deep) {
  impl.getPos(deep);
  if (impl.success(deep)) {
    impl.findOne();
    return;
  }
  for (int i = impl.begin(); i < impl.end(); i++)
    if (impl.valid(deep, i)) {
      impl.putIn(deep, i);
      dfsGuess(impl, deep + 1);
      if (impl.finish()) return;
      impl.moveOut(deep, i);
    }
}

void dfsDeduce(int R, int C, int num, const Array9i bd[], bool &isFind) {
  Array9i board[10];
  for (int i = 0; i < 10; i++) {
    board[i] = bd[i];
  }
  set_num(R, C, num, board);
  if (fill_all_single(board) < 0) {
    isFind = false;
    return;
  }
  int deep = 0;
  for (; deep < 81 && board[0](deep / 9, deep % 9); deep++) {
  }
  if (board[0].minCoeff() > 0) {
    isFind = true;
    return;
  }
  int r = deep / 9, c = deep % 9;
  for (int i = 1; i < 10; i++)
    if (board[i](r, c)) {
      dfsDeduce(r, c, i, board, isFind);
      if (isFind) return;
    }
}

Puzzle::Puzzle(Array9i &puz) : _puz(puz), cnt(0), limit(0) {
  row.fill(1);
  col.fill(1);
  blk.fill(1);
  int t;
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      t = _puz(i, j);
      if (t > 0 && t < 10) {
        unmark(i, j, t);
      } else {
        _puz(i, j) = 0;
      }
    }
}

void Puzzle::mark(int r, int c, int n) {
  row(r, n - 1) = 1;
  col(c, n - 1) = 1;
  blk(r / 3 * 3 + c / 3, n - 1) = 1;
}

void Puzzle::unmark(int r, int c, int n) {
  row(r, n - 1) = 0;
  col(c, n - 1) = 0;
  blk(r / 3 * 3 + c / 3, n - 1) = 0;
}

void Puzzle::getPos(int &deep) const {
  for (; deep < _size && _puz(deep / 9, deep % 9); deep++) {
  }
}

bool Puzzle::success(int deep) const { return deep >= _size; }

void Puzzle::findOne() { cnt++; }

bool Puzzle::valid(int deep, int n) const {
  int r = deep / 9, c = deep % 9;
  return col(c, n - 1) && row(r, n - 1) && blk(r / 3 * 3 + c / 3, n - 1);
}

void Puzzle::putIn(int deep, int n) {
  int r = deep / 9, c = deep % 9;
  _puz(r, c) = n;
  unmark(r, c, n);
}

bool Puzzle::finish() const { return cnt > limit; }

void Puzzle::moveOut(int deep, int n) {
  int r = deep / 9, c = deep % 9;
  mark(r, c, n);
  _puz(r, c) = 0;
}
}  // namespace li
