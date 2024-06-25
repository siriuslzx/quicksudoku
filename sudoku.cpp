/**
 * Copyright (c) 2024 Zhongxian Li
 * quick Sudoku is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 */
#include "sudoku.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <vector>

#include "common.h"
#include "dfs.h"
#include "impl.h"

namespace li {

Sudoku::Sudoku() {
  srand(time(0));
  _diff = 1;
}

Sudoku::~Sudoku() {
  // dtor
}

int Sudoku::newGame(Difficulty dif) {
  _ans.fill(0);
  int a[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  int r, c, num;
  for (int i = 0; i < 9; i += 3) {
    std::random_shuffle(a, a + 9);
    for (int j = 0; j < 9; j++) {
      r = i + j / 3;
      c = i + j % 3;
      _ans(r, c) = a[j];
    }
  }
  Puzzle pu(_ans);
  dfsGuess(pu);

  // create origin
  std::vector<int> pool(81);
  std::vector<Weight> samp;
  std::iota(pool.begin(), pool.end(), 0);
  int times = 30;
  int limit = pool.size();

  _board[0].fill(0);
  for (int i = 0; i < times; i++) {
    int n = rand() % limit;
    int pos = pool[n];
    std::swap(pool[n], pool[--limit]);
    r = pos / 9;
    c = pos % 9;
    _board[0](r, c) = _ans(r, c);
    samp.push_back({r, c, 1});
  }
  init_note(_board);

  std::vector<Weight> vec;
  while (_board[0].minCoeff() <= 0) {
    fill_all_single(_board);
    if (filter_notes(_board, vec)) {
      auto it = max_element(vec.begin(), vec.end());
      num = _ans(it->r, it->c);
      setNum(it->r, it->c, num);
      samp.push_back({it->r, it->c, 1});
    }
  }

  // create hard
  switch (dif) {
    case Difficulty::easy:
      always_easy(samp, _ans);
      break;
    case Difficulty::medium:
      often_medium(samp, _ans);
      break;
    case Difficulty::hard:
      usually_hard(samp, _ans);
      break;
  }

  // check diff
  _board[0].fill(0);
  for (auto &ele : samp) {
    _board[0](ele.r, ele.c) = _ans(ele.r, ele.c);
  }
  init_note(_board);
  _diff = 1;
  if (dif != Difficulty::easy) {
    auto bak = _board[0];
    while (fill_all_single(_board) > 0 || lineRemove() || circleRemove() || assumeRemove()) {
    }
    if (_board[0].minCoeff() <= 0) _diff = 5;
    _board[0] = bak;
    init_note(_board);
  }
  return _diff;
}

bool Sudoku::getNum(int r, int c, int &num) const {
  int n = _board[0](r, c);
  if (n >= 1 && n <= 9) {
    num = n;
    return true;
  } else {
    int res = 0;
    for (int i = 1; i < 10; i++) {
      res |= (_board[i](r, c) << i);
    }
    num = res;
    return false;
  }
}

void Sudoku::setNum(int r, int c, int num) {
  if (_board[0](r, c) <= 0) {
    set_num(r, c, num, _board);
  } else {
    _board[0](r, c) = 0;
    init_note(_board);
  }
}

void Sudoku::flipNote(int r, int c, int num) {
  if (_board[0](r, c) <= 0) {
    _board[num](r, c) = !_board[num](r, c);
  }
}

bool Sudoku::getSingle(int &r, int &c, int &num) const { return get_single(r, c, num, _board); }

bool Sudoku::lineRemove() {
  bool modify = _remove(_board, true);
  if (modify && _diff < 2) _diff = 2;
  return modify;
}

bool Sudoku::circleRemove() {
  bool modify =
      col_circle_remove(_board) | col_circle_remove(_board, std::swap<int>) | col_circle_remove(_board, col_swap_block);
  if (modify && _diff < 3) _diff = 3;
  return modify;
}

bool Sudoku::assumeRemove() {
  bool modify = _remove(_board, false);
  if (modify && _diff < 4) _diff = 4;
  return modify;
}
}  // namespace li
