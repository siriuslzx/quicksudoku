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

#include "dfs.h"

namespace li {

void col_swap_block(int &r, int &c) {
  int a = r / 3;
  int b = c / 3;
  int d = r % 3;
  int e = c % 3;
  c = b * 3 + a;
  r = e * 3 + d;
}

bool filter_notes(const Array9i board[], std::vector<Weight> &vec) {
  vec.clear();
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      int n = board[0](i, j);
      if (n <= 0) {
        vec.push_back({i, j, count_notes(i, j, board), rand()});
      }
    }
  }
  return !vec.empty();
}

std::vector<Point> note_remove(const Array9i &asp, bool once, bool cons_block = true) {
  std::vector<Point> vec;
  Array9i cur;
  int r, c;
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (asp(i, j) == 1) {
        cur = asp;
        r = i;
        c = j;
        do {
          cur.col(c) = 0;
          cur.row(r) = 0;
          if (cons_block) {
            cur.block<3, 3>(r / 3 * 3, c / 3 * 3) = 0;
          }
          cur(r, c) = 10;
          if (once) break;
        } while (row_single(r, c, cur) || col_single(r, c, cur) || (cons_block && block_single(r, c, cur)));

        bool modify = false;
        if (cons_block) {
          modify |= block_sum_0(cur);
        }

        if ((cur.rowwise().sum() == 0).any() || (cur.colwise().sum() == 0).any() || modify) {
          vec.push_back({i, j});
        }
      }
    }
  }
  return vec;
}

bool _remove(Array9i board[], bool once) {
  bool modify = false;
  std::vector<Point> vec;
  Array9i asp;
  for (int n = 1; n < 10; n++) {
    asp = (board[0] == n).cast<int>() * 10 + board[n];
    vec = note_remove(asp, once);
    if (!vec.empty()) modify = true;
    for (auto &ele : vec) {
      board[n](ele.r, ele.c) = 0;
    }
  }
  return modify;
}

bool col_circle_remove(Array9i board[], void (*fun)(int &, int &) = nullptr) {
  bool modify = false;
  Array9i asp;
  int r, c;
  std::vector<Point> vec;
  for (int tid = 0; tid < 9; ++tid) {
    asp.fill(0);
    for (int idx = 0; idx < 9; ++idx) {
      // asp(i,j) == board_view[n](r, c) == board[n](R, C)
      // i = idx, j = n-1
      // r = idx, c = tid
      // [R, C] = fun(r,c)
      r = idx;
      c = tid;
      if (fun) fun(r, c);
      if (board[0](r, c)) {
        asp(idx, board[0](r, c) - 1) = 10;
      } else {
        for (int n = 1; n < 10; n++) {
          if (board[n](r, c)) {
            asp(idx, n - 1) = 1;
          }
        }
      }
    }
    vec = note_remove(asp, false, false);
    if (!vec.empty()) modify = true;
    for (auto &ele : vec) {
      r = ele.r;
      c = tid;
      if (fun) fun(r, c);
      board[ele.c + 1](r, c) = 0;
    }
  }
  return modify;
}

Sudoku::Sudoku() {
  srand(time(0));
  diff = 1;
  _diff = 1;
}

Sudoku::~Sudoku() {
  // dtor
}

void Sudoku::newGame() {
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

  // create easy
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
  for (int i = samp.size() - 1; i >= 0; i--) {
    _board[0].fill(0);
    for (auto &ele : samp) {
      _board[0](ele.r, ele.c) = _ans(ele.r, ele.c);
    }
    int cur_r = samp[i].r;
    int cur_c = samp[i].c;
    _board[0](cur_r, cur_c) = 0;
    init_note(_board);

    for (int j = 1; j < 10; ++j)
      if (_board[j](cur_r, cur_c) && j != _ans(cur_r, cur_c)) {
        bool isFind = false;
        dfsDeduce(cur_r, cur_c, j, _board, isFind);

        if (isFind) {
          samp[i].w = -1;
          break;
        }
      }
    if (samp[i].w > 0) {
      samp.erase(samp.begin() + i);
    }
  }

  // check diff
  diff = 1;
  _board[0].fill(0);
  for (auto ele : samp) {
    _board[0](ele.r, ele.c) = _ans(ele.r, ele.c);
  }
  init_note(_board);

  auto bak = _board[0];
  _diff = 1;
  while (getSingle(r, c, num) || lineRemove() || circleRemove() || assumeRemove()) {
    if (num) {
      setNum(r, c, num);
    }
  }
  diff = (_board[0].minCoeff() <= 0) ? 5 : _diff;
  _board[0] = bak;
  init_note(_board);
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
