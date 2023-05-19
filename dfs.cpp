/**
 * Copyright (c) 2023 Zhongxian Li
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

#include <algorithm>

void dfs_once(int deep, dfsImpl &impl) {
  impl.getPos(deep);
  if (deep >= impl.size()) {
    impl.success();
    return;
  }
  for (int i = impl.begin(); i < impl.end(); i++)
    if (impl.valid(deep, i)) {
      impl.putIn(deep, i);
      dfs_once(deep + 1, impl);
      if (impl.find()) return;
      impl.moveOut(deep, i);
    }
}

void dfs_all(int deep, dfsImpl &impl) {
  impl.getPos(deep);
  if (deep >= impl.size()) {
    impl.success();
    return;
  }
  for (int i = impl.begin(); i < impl.end(); i++)
    if (impl.valid(deep, i)) {
      impl.putIn(deep, i);
      dfs_all(deep + 1, impl);
      impl.moveOut(deep, i);
    }
}

puzzle::puzzle(Matrix9i &mat) : _mat(mat) {
  std::fill(row, row + 9, kNotes);
  std::fill(col, col + 9, kNotes);
  std::fill(blk, blk + 9, kNotes);
  cnt = 0;
  limit = 0;
  int t;
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      t = _mat(i, j);
      if (t > 0 && t < 10) {
        unset(i, j, t);
      } else {
        _mat(i, j) = 0;
      }
    }
}

void puzzle::set(int r, int c, int n) {
  setBit(row[r], n);
  setBit(col[c], n);
  setBit(blk[r / 3 * 3 + c / 3], n);
}

void puzzle::unset(int r, int c, int n) {
  unsetBit(row[r], n);
  unsetBit(col[c], n);
  unsetBit(blk[r / 3 * 3 + c / 3], n);
}

int puzzle::merge(int r, int c) const { return row[r] & col[c] & blk[r / 3 * 3 + c / 3]; }

void puzzle::getPos(int &deep) const {
  for (; deep < _size && _mat(deep / 9, deep % 9); deep++) {
  }
}

void puzzle::success() { cnt++; }

bool puzzle::valid(int deep, int i) const {
  int r = deep / 9, c = deep % 9;
  return getBit(col[c], i) && getBit(row[r], i) && getBit(blk[r / 3 * 3 + c / 3], i);
}

bool puzzle::find() const { return cnt > limit; }

void puzzle::putIn(int deep, int i) {
  int r = deep / 9, c = deep % 9;
  _mat(r, c) = i;
  unset(r, c, i);
}

void puzzle::moveOut(int deep, int i) {
  int r = deep / 9, c = deep % 9;
  set(r, c, i);
  _mat(r, c) = 0;
}

by_col::by_col(Vector9i &vec) : _vec(vec) {
  std::fill(used, used + 10, false);
  all_ans.fill(kNum);
  for (int i = 0; i < 9; i++) {
    int n = _vec(i);
    if (n < 10) {
      all_ans(i) = n;
      used[n] = true;
    }
  }
}

by_col::~by_col() {
  if (all_ans != _vec) {
    _vec = all_ans;
  }
}

void by_col::getPos(int &deep) const {
  for (; deep < _size && _vec(deep) % 16 < 10; deep++) {
  }
}

void by_col::success() {
  for (int i = 0; i < 9; i++) {
    if (all_ans(i) > 9) {
      setBit(all_ans(i), _vec(i) % 16 + kDet);
    }
  }
}

bool by_col::valid(int deep, int i) const { return getBit(_vec(deep), i + kDet) && !used[i]; }

bool by_col::find() const { return false; }

void by_col::putIn(int deep, int i) {
  int n = (_vec(deep) >> kDet) << kDet;
  _vec(deep) = n + i;
  used[i] = true;
}

void by_col::moveOut(int deep, int i) {
  used[i] = false;
  int n = (_vec(deep) >> kDet) << kDet;
  _vec(deep) = n + kNum;
}

col_block::col_block(Vector9i &vec) : _vec(vec) {
  std::fill(col_used, col_used + 9, false);
  std::fill(block_used, block_used + 9, false);
  all_ans.fill(kNum);
  for (int i = 0; i < 9; i++) {
    int n = _vec(i);
    if (n < 10) {
      all_ans(i) = n;
      int j = n - 1;
      col_used[j] = true;
      block_used[i / 3 * 3 + j / 3] = true;
    }
  }
}

col_block::~col_block() {
  if (all_ans != _vec) {
    _vec = all_ans;
  }
}

void col_block::getPos(int &deep) const {
  for (; deep < _size && _vec(deep) % 16 < 10; deep++) {
  }
}

void col_block::success() {
  for (int i = 0; i < 9; i++) {
    if (all_ans(i) > 9) {
      setBit(all_ans(i), _vec(i) % 16 + kDet);
    }
  }
}

bool col_block::valid(int deep, int i) const {
  int j = i - 1;
  return !block_used[deep / 3 * 3 + j / 3] && !col_used[j] && getBit(_vec(deep), i + kDet);
}

bool col_block::find() const { return false; }

void col_block::putIn(int deep, int i) {
  int n = (_vec(deep) >> kDet) << kDet;
  _vec(deep) = n + i;
  int j = i - 1;
  col_used[j] = true;
  block_used[deep / 3 * 3 + j / 3] = true;
}

void col_block::moveOut(int deep, int i) {
  int j = i - 1;
  block_used[deep / 3 * 3 + j / 3] = false;
  col_used[j] = false;

  int n = (_vec(deep) >> kDet) << kDet;
  _vec(deep) = n + kNum;
}
