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
#include "common.h"

#include <cmath>
#include <tuple>

namespace li {
inline bool single_bit(int n) { return n > 0 && !(n & (n - 1)); }
bool operator<(const Weight &a, const Weight &b) { return std::tie(a.w, a.hash) < std::tie(b.w, b.hash); }

bool block_sum_0(const Array9i &arr) {
  for (int i = 0; i < 9; i += 3) {
    for (int j = 0; j < 9; j += 3) {
      if (arr.block<3, 3>(i, j).sum() == 0) {
        return true;
      }
    }
  }
  return false;
}

void init_note(Array9i board[]) {
  for (int i = 1; i < 10; i++) {
    board[i].fill(1);
  }
  int t;
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      t = board[0](i, j);
      if (t > 0 && t < 10) {
        board[t].col(j) = 0;
        board[t].row(i) = 0;
        board[t].block<3, 3>(i / 3 * 3, j / 3 * 3) = 0;
        for (int k = 1; k < 10; k++) {
          board[k](i, j) = 0;
        }
      }
    }
}

int count_notes(int r, int c, const Array9i board[]) {
  if (board[0](r, c) > 0) return 0;
  int res = 0;
  for (int i = 1; i < 10; ++i) {
    res += board[i](r, c);
  }
  return res;
}

Array9i get_notes(const Array9i board[]) {
  Array9i notes;
  notes.fill(0);
  for (int i = 9; i > 0; i--) {
    notes += board[i];
    notes *= 2;
  }
  return notes;
}

bool row_single(int &r, int &c, const Array9i &arr) {
  Eigen::Index maxJ;
  for (int i = 0; i < 9; i++) {
    if (arr.row(i).sum() == 1) {
      arr.row(i).maxCoeff(&maxJ);
      r = i;
      c = maxJ;
      return true;
    }
  }
  return false;
}

bool col_single(int &r, int &c, const Array9i &arr) {
  Eigen::Index maxI;
  for (int j = 0; j < 9; j++) {
    if (arr.col(j).sum() == 1) {
      arr.col(j).maxCoeff(&maxI);
      r = maxI;
      c = j;
      return true;
    }
  }
  return false;
}

bool block_single(int &r, int &c, const Array9i &arr) {
  Eigen::Index maxI, maxJ;
  for (int i = 0; i < 9; i += 3) {
    for (int j = 0; j < 9; j += 3) {
      if (arr.block<3, 3>(i, j).sum() == 1) {
        arr.block<3, 3>(i, j).maxCoeff(&maxI, &maxJ);
        r = i + maxI;
        c = j + maxJ;
        return true;
      }
    }
  }
  return false;
}

bool get_single(int &r, int &c, int &num, const Array9i board[]) {
  for (int k = 1; k < 10; k++) {
    if (row_single(r, c, board[k]) || col_single(r, c, board[k]) || block_single(r, c, board[k])) {
      num = k;
      return true;
    }
  }

  Array9i notes = get_notes(board);
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (single_bit(notes(i, j))) {
        std::tie(r, c, num) = std::make_tuple(i, j, log2(notes(i, j)));
        return true;
      }
    }
  }
  num = 0;
  return false;
}

void set_num(int r, int c, int num, Array9i board[]) {
  board[0](r, c) = num;
  board[num].col(c) = 0;
  board[num].row(r) = 0;
  board[num].block<3, 3>(r / 3 * 3, c / 3 * 3) = 0;
  for (int k = 1; k < 10; k++) {
    board[k](r, c) = 0;
  }
}

int fill_all_single(Array9i board[]) {
  int r, c, num;
  while (get_single(r, c, num, board)) {
    do {
      set_num(r, c, num, board);
    } while (row_single(r, c, board[num]) || col_single(r, c, board[num]));

    Array9i cur = board[num] + (board[0] == num).cast<int>();
    if ((cur.colwise().sum() == 0).any() || (cur.rowwise().sum() == 0).any() || block_sum_0(cur)) {
      return -1;
    }
  }
  return 1;
}
}  // namespace li
