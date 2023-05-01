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
#include "sudoku.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "dfs.h"

bool singleBit(int n) { return n > 0 && (n & (n - 1)) == 0; }

int countBit(int n) {
  int cnt = 0;
  for (; n > 0; n >>= 1) {
    if (n & 1) cnt++;
  }
  return cnt;
}

void shuffle(int arr[], int len) {
  for (; len > 1; len--) {
    int n = rand() % len;
    std::swap(arr[n], arr[len - 1]);
  }
}

void initNote(Matrix9i &mat) {
  puzzle pu(mat);
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      if (mat(i, j) == 0) {
        mat(i, j) = (pu.merge(i, j) << kDet) + kNum;
      }
    }
}

void col_swap_block(int &x, int &y) {
  int a = x / 3;
  int b = y / 3;
  int c = x % 3;
  int d = y % 3;
  x = a * 3 + b;
  y = c * 3 + d;
}

Matrix9i col_swap_block(const Matrix9i &mat) {
  Matrix9i res;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < 3; k++) {
        res.block<3, 1>(j * 3, i * 3 + k) = mat.block<3, 1>(k * 3, i * 3 + j);
      }
    }
  }
  return res;
}

Matrix9i col_swap_num(const Matrix9i &mat) {
  Matrix9i res;
  res.fill(kNum);
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      int n = mat(i, j);
      if (n < 10) {
        res(i, n - 1) = j + 1;
        continue;
      }
      for (int k = 1; k <= 9; k++) {
        if (getBit(n, k + kDet)) {
          setBit(res(i, k - 1), j + 1 + kDet);
        }
      }
    }
  }
  return res;
}

sudoku::sudoku() { srand(time(0)); }

sudoku::~sudoku() {}

struct helper {
  int r;
  int c;
  int w;
  int hash;
};
bool operator<(const helper &a, const helper &b) { return std::tie(a.w, a.hash) < std::tie(b.w, b.hash); }

bool filter_notes(const Matrix9i &mat, std::vector<helper> &vec) {
  vec.clear();
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      int n = mat(i, j);
      if (n > 9) {
        vec.push_back({i, j, countBit(n >> kDet), rand()});
      }
    }
  }
  return !vec.empty();
}

void sudoku::newGame() {
  _ans.fill(0);
  int a[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  for (int i = 0; i < 9; i += 3) {
    shuffle(a, 9);
    for (int j = 0; j < 9; j++) {
      int r = i + j / 3;
      int c = i + j % 3;
      _ans(r, c) = a[j];
    }
  }
  puzzle pu(_ans);
  dfs_once(0, pu);

  // create easy
  std::vector<int> pool(81);
  std::vector<helper> samp;
  for (size_t i = 0; i < pool.size(); i++) pool[i] = i;
  int times = 30;
  int limit = pool.size();

  _mat.fill(0);
  for (int i = 0; i < times; i++) {
    int n = rand() % limit;
    int pos = pool[n];
    std::swap(pool[n], pool[--limit]);

    int r = pos / 9;
    int c = pos % 9;
    _mat(r, c) = _ans(r, c);
    samp.push_back({r, c, 1, rand()});
  }
  initNote(_mat);

  std::vector<helper> vec;
  while (_mat.maxCoeff() > 9) {
    int x, y, num;
    while (getSingle(x, y, num)) {
      setNum(x, y, num);
    }

    if (filter_notes(_mat, vec)) {
      auto it = max_element(vec.begin(), vec.end());
      num = _ans(it->r, it->c);
      setNum(it->c, it->r, num);
      samp.push_back({it->r, it->c, 1, rand()});
    }
  }
  // end easy */

  // create hard
  while (true) {
#define DIFF w
    for (int i = samp.size() - 1; i >= 0; i--)
      if (samp[i].DIFF > 0) {
        _mat.fill(0);
        for (auto &ele : samp) {
          _mat(ele.r, ele.c) = _ans(ele.r, ele.c);
        }
        int cur_r = samp[i].r;
        int cur_c = samp[i].c;
        _mat(cur_r, cur_c) = 0;
        initNote(_mat);

        int x, y, num;
        _diff = 1;
        samp[i].DIFF = -1;
        while (getSingle(x, y, num) || lineRemove() || circleRemove() || assumeRemove()) {
          if (num) {
            if (x == cur_c && y == cur_r) {
              samp[i].DIFF = _diff;
              samp[i].hash = rand();
              break;
            }
            setNum(x, y, num);
          }
        }
      }

    auto it = std::max_element(samp.begin(), samp.end());
    if (it->DIFF < 0) break;
    samp.erase(it);
  }
  // end hard */

  _mat.fill(0);
  for (auto ele : samp) {
    _mat(ele.r, ele.c) = _ans(ele.r, ele.c);
  }
  initNote(_mat);
}

bool sudoku::getNum(int x, int y, int &num) const {
  int n = _mat(y, x);
  if (n >= 1 && n <= 9) {
    num = n;
    return true;
  } else {
    num = n >> kDet;
    return false;
  }
}

void sudoku::setNum(int x, int y, int num) {
  if (_mat(y, x) > 9) {
    _mat(y, x) = num;

    for (int i = 0; i < 9; i++) {
      int &n = _mat(y, i);
      if (n > 9 && getBit(n, num + kDet)) {
        unsetBit(n, num + kDet);
      }
    }
    for (int j = 0; j < 9; j++) {
      int &n = _mat(j, x);
      if (n > 9 && getBit(n, num + kDet)) {
        unsetBit(n, num + kDet);
      }
    }
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++) {
        int &n = _mat(y / 3 * 3 + j, x / 3 * 3 + i);
        if (n > 9 && getBit(n, num + kDet)) {
          unsetBit(n, num + kDet);
        }
      }
  } else {
    _mat(y, x) = 0;
    initNote(_mat);
  }
}

void sudoku::flipNote(int x, int y, int num) {
  int &n = _mat(y, x);
  if (n > 9) {
    flipBit(n, num + kDet);
  }
}

bool col_single(int &x, int &y, int &num, const Matrix9i &mat) {
  int stats[10];
  for (int j = 0; j < 9; ++j) {
    std::fill(stats, stats + 10, 0);
    for(int i = 0; i < 9; ++i) {
      int n = mat(i,j);
      if(n > 9) {
        for(int k = 1; k < 10; ++k) {
          if(getBit(n, k+kDet)) {
            setBit(stats[k],i);
          }
		}
      }
    }
    for(int k = 1; k < 10; ++k) {
      if(singleBit(stats[k])) {
        y = log2(stats[k]);
        x = j;
        num = k;
        return true;
      }
    }
  }
  return false;
}

bool sudoku::getSingle(int &x, int &y, int &num) const {
  Matrix9i trans = col_swap_block(_mat);
  if (col_single(x, y, num, trans)) {
    col_swap_block(x, y);
    return true;
  }

  if (col_single(x, y, num, _mat)) {
    return true;
  }

  trans = _mat.transpose();
  if (col_single(x, y, num, trans)) {
    std::swap(x, y);
    return true;
  }

  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      int n = _mat(i, j);
      int notes = n >> kDet;
      if (singleBit(notes)) {
        y = i;
        x = j;
        num = log2(notes);
        return true;
      }
    }
  }

  num = 0;
  return false;
}

bool exist3x1(const Matrix9i &mat, int block_r, int block_c, int col_idx, int num) {
#if EIGEN_WORLD_VERSION==3 && EIGEN_MAJOR_VERSION < 4
#error "need eigen 3.4"
#endif
  auto col = mat.block<3, 1>(block_r * 3, block_c * 3 + col_idx);
  return std::any_of(col.begin(), col.end(), [num](int n) { return getBit(n, num + kDet); });
}

void remove3x1(Matrix9i &mat, int block_r, int block_c, int col_idx, int num) {
  auto col = mat.block<3, 1>(block_r * 3, block_c * 3 + col_idx);
  std::for_each(col.begin(), col.end(), [num](int &n) {
    if (n > 9) {
      unsetBit(n, num + kDet);
    }
  });
}

bool col_line_remove(Matrix9i &mat) {
  bool modify = false;
  for (int num = 1; num <= 9; num++) {
    for (int i = 0; i < 3; i++) {
      auto exist = [&mat, i, num](int j, int k) { return exist3x1(mat, j, i, k, num); };
      auto remove = [&mat, i, num](int j, int k) { remove3x1(mat, j, i, k, num); };
      for (int j = 0; j < 3; j++) {
        for (int k = 0; k < 3; k++) {
          if (exist(j, k) && !exist(j, (k + 1) % 3) && !exist(j, (k + 2) % 3) &&
              (exist((j + 1) % 3, k) || exist((j + 2) % 3, k))) {
            remove((j + 1) % 3, k);
            remove((j + 2) % 3, k);
            modify = true;
          }
          if (exist(j, k) && !exist((j + 1) % 3, k) && !exist((j + 2) % 3, k) &&
              (exist(j, (k + 1) % 3) || exist(j, (k + 2) % 3))) {
            remove(j, (k + 1) % 3);
            remove(j, (k + 2) % 3);
            modify = true;
          }
        }
      }
    }
  }
  return modify;
}

bool sudoku::lineRemove() {
  bool modify = false;

  if (col_line_remove(_mat)) modify = true;

  Matrix9i trans = _mat.transpose();
  if (col_line_remove(trans)) modify = true;
  _mat = trans.transpose();

  if (modify && _diff < 2) _diff = 2;

  return modify;
}

bool col_circle_remove(Matrix9i &mat) {
  bool modify = false;

  for (int i = 0; i < 9; i++) {
    Vector9i col = mat.col(i);
    by_col bc(col);
    dfs_all(0, bc);
    if (bc.modified()) {
      modify = true;
      mat.col(i) = col;
    }
  }

  return modify;
}

bool sudoku::circleRemove() {
  bool modify = false;

  if (col_circle_remove(_mat)) modify = true;

  Matrix9i trans = _mat.transpose();
  if (col_circle_remove(trans)) modify = true;
  _mat = trans.transpose();

  trans = col_swap_block(_mat);
  if (col_circle_remove(trans)) modify = true;
  _mat = col_swap_block(trans);

  if (modify && _diff < 3) _diff = 3;

  return modify;
}

bool sudoku::assumeRemove() {
  bool modify = false;

  Matrix9i mat = col_swap_num(_mat);
  for (int i = 0; i < 9; i++) {
    Vector9i col = mat.col(i);
    col_block cb(col);
    dfs_all(0, cb);
    if (cb.modified()) {
      modify = true;
      mat.col(i) = col;
    }
  }
  _mat = col_swap_num(mat);

  if (modify && _diff < 4) _diff = 4;

  return modify;
}
