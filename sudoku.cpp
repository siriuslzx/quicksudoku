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
#include <tuple>
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

void col_swap_block(int &r, int &c) {
  int a = r / 3;
  int b = c / 3;
  int d = r % 3;
  int e = c % 3;
  c = b * 3 + a;
  r = e * 3 + d;
}

Matrix9i col_trans_block(const Matrix9i &mat) {
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

Matrix9i col_trans_num(const Matrix9i &mat) {
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

void initNote(Matrix9i &mat) {
  puzzle pu(mat);
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++) {
      if (mat(i, j) == 0) {
        mat(i, j) = (pu.merge(i, j) << kDet) + kNum;
      }
    }
}
bool col_single(int &r, int &c, int &num, const Matrix9i &mat, void (*fun)(int &, int &) = nullptr) {
  int stats[10];
  for (int j = 0; j < 9; ++j) {
    std::fill(stats, stats + 10, 0);
    for (int i = 0; i < 9; ++i) {
      int n = mat(i, j);
      if (n > 9) {
        for (int k = 1; k < 10; ++k) {
          if (getBit(n, k + kDet)) {
            setBit(stats[k], i);
          }
        }
      }
    }
    for (int k = 1; k < 10; ++k) {
      if (singleBit(stats[k])) {
        r = log2(stats[k]);
        c = j;
        num = k;
        if (fun) fun(r, c);
        return true;
      }
    }
  }
  return false;
}

bool get_single(int &r, int &c, int &num, const Matrix9i &mat) {
  if (col_single(r, c, num, col_trans_block(mat), col_swap_block) || col_single(r, c, num, mat) ||
      col_single(r, c, num, mat.transpose(), std::swap<int>)) {
    return true;
  }

  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      int n = mat(i, j);
      int notes = n >> kDet;
      if (singleBit(notes)) {
        r = i;
        c = j;
        num = log2(notes);
        return true;
      }
    }
  }
  num = 0;
  return false;
}

void set_num(int r, int c, int num, Matrix9i &mat) {
  mat(r, c) = num;
  for (auto &n : mat.col(c)) {
    unsetBit(n, num + kDet);
  }
  for (auto &n : mat.row(r)) {
    unsetBit(n, num + kDet);
  }
  for (auto &n : mat.block<3, 3>(r / 3 * 3, c / 3 * 3).reshaped()) {
    unsetBit(n, num + kDet);
  }
}

int fillAllSingle(Matrix9i &mat) {
  int r, c, num;
  if (!get_single(r, c, num, mat)) return 0;

  do {
    auto arr = mat.array() / (1 << (num + kDet));
    Eigen::Array<int, 9, 9> cur = (arr - arr / 2 * 2);
    while (true) {
      set_num(r, c, num, mat);
      cur.col(c) = 0;
      cur.row(r) = 0;
      cur.block<3, 3>(r / 3 * 3, c / 3 * 3) = 0;

      Eigen::Index maxIndex;
      for (int j = 0; j < 9; j++) {
        auto col = cur.col(j);
        if (col.sum() == 1) {
          col.maxCoeff(&maxIndex);
          c = j;
          r = maxIndex;
          goto next_step;
        }
      }
      for (int i = 0; i < 9; i++) {
        auto row = cur.row(i);
        if (row.sum() == 1) {
          row.maxCoeff(&maxIndex);
          c = maxIndex;
          r = i;
          goto next_step;
        }
      }
      break;
    next_step: {}
    }
    cur += (mat.array() == num).cast<int>();
    Eigen::Array<int, 9, 9> blk = col_trans_block(cur.matrix()).array();
    if ((cur.colwise().sum() == 0).any() || (cur.rowwise().sum() == 0).any() || (blk.colwise().sum() == 0).any()) {
      return -1;
    }
  } while (get_single(r, c, num, mat));

  return 1;
}

sudoku::sudoku() {
  srand(time(0));
  diff = 1;
  _diff = 1;
}

sudoku::~sudoku() {
  // dtor
}

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

bool isFind;
void dfs_logic(Matrix9i &mat) {
  if (fillAllSingle(mat) < 0) {
    isFind = false;
    return;
  }
  if (mat.maxCoeff() <= 9) {
    isFind = true;
    return;
  }

  Matrix9i bak = mat;
  int deep = 0;
  for (; deep < 81 && mat(deep / 9, deep % 9) < 10; deep++) {
  }
  int r = deep / 9, c = deep % 9;
  for (int i = 1; i < 10; i++)
    if (getBit(mat(r, c), i + kDet)) {
      set_num(r, c, i, mat);
      dfs_logic(mat);
      if (isFind) return;
      mat = bak;
    }
}

void sudoku::newGame() {
  _ans.fill(0);
  int a[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  int r, c, num;
  for (int i = 0; i < 9; i += 3) {
    shuffle(a, 9);
    for (int j = 0; j < 9; j++) {
      r = i + j / 3;
      c = i + j % 3;
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

    r = pos / 9;
    c = pos % 9;
    _mat(r, c) = _ans(r, c);
    samp.push_back({r, c, 1});
  }
  initNote(_mat);

  std::vector<helper> vec;
  while (_mat.maxCoeff() > 9) {
    while (getSingle(r, c, num)) setNum(r, c, num);

    if (filter_notes(_mat, vec)) {
      auto it = max_element(vec.begin(), vec.end());
      num = _ans(it->r, it->c);
      setNum(it->r, it->c, num);
      samp.push_back({it->r, it->c, 1});
    }
  }
  // end easy */
  diff = 1;
  // create hard
  for (int i = samp.size() - 1; i >= 0; i--) {
    _mat.fill(0);
    for (auto &ele : samp) {
      _mat(ele.r, ele.c) = _ans(ele.r, ele.c);
    }
    int cur_r = samp[i].r;
    int cur_c = samp[i].c;
    _mat(cur_r, cur_c) = 0;
    initNote(_mat);

    int notes = _mat(cur_r, cur_c) >> kDet;
    for (int j = 1; j < 10; ++j)
      if (getBit(notes, j) && j != _ans(cur_r, cur_c)) {
        Matrix9i mat = _mat;
        set_num(cur_r, cur_c, j, mat);
        isFind = false;
        dfs_logic(mat);
        if (isFind) {
          samp[i].w = -1;
          break;
        }
      }
    if (samp[i].w > 0) {
      samp.erase(samp.begin() + i);
    }
  }
  // end hard */
  _mat.fill(0);
  for (auto ele : samp) {
    _mat(ele.r, ele.c) = _ans(ele.r, ele.c);
  }
  initNote(_mat);

  Matrix9i bak = _mat;
  _diff = 1;
  while (getSingle(r, c, num) || lineRemove() || circleRemove() || assumeRemove()) {
    if (num) {
      setNum(r, c, num);
    }
  }
  diff = (_mat.maxCoeff() > 9) ? 5 : _diff;
  _mat = bak;
}

bool sudoku::getNum(int r, int c, int &num) const {
  int n = _mat(r, c);
  if (n >= 1 && n <= 9) {
    num = n;
    return true;
  } else {
    num = n >> kDet;
    return false;
  }
}

void sudoku::setNum(int r, int c, int num) {
  if (_mat(r, c) > 9) {
    set_num(r, c, num, _mat);
  } else {
    _mat(r, c) = 0;
    initNote(_mat);
  }
}

void sudoku::flipNote(int r, int c, int num) {
  int &n = _mat(r, c);
  if (n > 9) {
    flipBit(n, num + kDet);
  }
}

bool sudoku::getSingle(int &r, int &c, int &num) const { return get_single(r, c, num, _mat); }

bool exist3x1(const Matrix9i &mat, int block_r, int block_c, int col_idx, int num) {
#if EIGEN_WORLD_VERSION == 3 && EIGEN_MAJOR_VERSION < 4
#error "need eigen 3.4"
#endif
  auto col = mat.block<3, 1>(block_r * 3, block_c * 3 + col_idx);
  return std::any_of(col.begin(), col.end(), [num](int n) { return getBit(n, num + kDet); });
}

void remove3x1(Matrix9i &mat, int block_r, int block_c, int col_idx, int num) {
  auto col = mat.block<3, 1>(block_r * 3, block_c * 3 + col_idx);
  std::for_each(col.begin(), col.end(), [num](int &n) { unsetBit(n, num + kDet); });
}

bool col_line_remove(Matrix9i &mat, Matrix9i (*fun)(const Matrix9i &) = nullptr) {
  Matrix9i mat0 = fun ? fun(mat) : mat;
  bool modify = false;

  for (int num = 1; num <= 9; num++) {
    for (int i = 0; i < 3; i++) {
      auto exist = [&mat0, i, num](int j, int k) { return exist3x1(mat0, j, i, k, num); };
      auto remove = [&mat0, i, num](int j, int k) { remove3x1(mat0, j, i, k, num); };
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
  if (modify) mat = fun ? fun(mat0) : mat0;
  return modify;
}

Matrix9i trans(const Matrix9i &mat) { return mat.transpose(); }
bool sudoku::lineRemove() {
  bool modify = col_line_remove(_mat) | col_line_remove(_mat, trans);

  if (modify && _diff < 2) _diff = 2;

  return modify;
}

bool col_circle_remove(Matrix9i &mat, Matrix9i (*fun)(const Matrix9i &) = nullptr) {
  Matrix9i mat0 = fun ? fun(mat) : mat;
  bool modify = false;

  for (int i = 0; i < 9; i++) {
    Vector9i col = mat0.col(i);
    {
      by_col bc(col);
      dfs_all(0, bc);
    }
    if (mat0.col(i) != col) {
      modify = true;
      mat0.col(i) = col;
    }
  }
  if (modify) mat = fun ? fun(mat0) : mat0;
  return modify;
}

bool sudoku::circleRemove() {
  bool modify = col_circle_remove(_mat) | col_circle_remove(_mat, trans) | col_circle_remove(_mat, col_trans_block);

  if (modify && _diff < 3) _diff = 3;

  return modify;
}

bool sudoku::assumeRemove() {
  bool modify = false;

  Matrix9i mat = col_trans_num(_mat);
  for (int i = 0; i < 9; i++) {
    Vector9i col = mat.col(i);
    {
      col_block cb(col);
      dfs_all(0, cb);
    }
    if (mat.col(i) != col) {
      modify = true;
      mat.col(i) = col;
    }
  }
  _mat = col_trans_num(mat);

  if (modify && _diff < 4) _diff = 4;

  return modify;
}
