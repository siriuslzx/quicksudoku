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
#include "impl.h"

#include "common.h"
#include "dfs.h"

namespace li {
namespace {
int count_notes(int r, int c, const Array9i board[]) {
  if (board[0](r, c) > 0) return 0;
  int res = 0;
  for (int i = 1; i < 10; ++i) {
    res += board[i](r, c);
  }
  return res;
}

void update_diff(std::vector<Weight> &samp, const Array9i &ans) {
  Array9i bak;
  Array9i board[10];
  bak.fill(0);
  for (auto &ele : samp) {
    bak(ele.r, ele.c) = ans(ele.r, ele.c);
  }
  for (int i = samp.size() - 1; i >= 0; i--)
    if (samp[i].w > 0) {
      board[0] = bak;
      int cur_r = samp[i].r;
      int cur_c = samp[i].c;
      board[0](cur_r, cur_c) = 0;
      init_note(board);
      fill_all_single(board);

      samp[i].hash = rand();
      if (!board[0](cur_r, cur_c)) {
        samp[i].w = 2;
        for (int j = 1; j < 10; ++j)
          if (board[j](cur_r, cur_c) && j != ans(cur_r, cur_c)) {
            bool isFind = false;
            dfsDeduce(cur_r, cur_c, j, board, isFind);
            if (isFind) {
              samp[i].w = -1;
              break;
            }
          }
      }
    }
}

void erase_easy(std::vector<Weight> &samp, const Array9i &ans) {
  Array9i bak;
  Array9i board[10];
  bak.fill(0);
  for (auto &ele : samp) {
    bak(ele.r, ele.c) = ans(ele.r, ele.c);
  }
  for (int i = samp.size() - 1; i >= 0; i--)
    if (samp[i].w > 0) {
      board[0] = bak;
      int cur_r = samp[i].r;
      int cur_c = samp[i].c;
      board[0](cur_r, cur_c) = 0;
      init_note(board);
      fill_all_single(board);

      if (board[0](cur_r, cur_c)) {
        bak(cur_r, cur_c) = 0;
        samp.erase(samp.begin() + i);
      }
    }
}

struct Point {
  int r;
  int c;
};

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
}  // namespace

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

void always_easy(std::vector<Weight> &samp, const Array9i &ans) { erase_easy(samp, ans); }

void often_medium(std::vector<Weight> &samp, const Array9i &ans) {
  while (std::any_of(samp.begin(), samp.end(), [](const Weight &wt) { return wt.w > 0; })) {
    update_diff(samp, ans);
    auto it = max_element(samp.begin(), samp.end());
    int w = it->w;
    samp.erase(it);
    if (w > 1) {
      break;
    }
  }
  erase_easy(samp, ans);
}

void usually_hard(std::vector<Weight> &samp, const Array9i &ans) {
  while (std::any_of(samp.begin(), samp.end(), [](const Weight &wt) { return wt.w > 0; })) {
    update_diff(samp, ans);
    samp.erase(max_element(samp.begin(), samp.end()));
  }
}

void col_swap_block(int &r, int &c) {
  int a = r / 3;
  int b = c / 3;
  int d = r % 3;
  int e = c % 3;
  c = b * 3 + a;
  r = e * 3 + d;
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

bool col_circle_remove(Array9i board[], void (*fun)(int &, int &)) {
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
}  // namespace li
