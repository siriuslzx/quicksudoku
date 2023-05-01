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
#pragma once

#include "common.h"

class dfsImpl {
 public:
  virtual void getPos(int &deep) const = 0;
  virtual void success() = 0;
  virtual int size() const = 0;
  virtual int begin() const { return 1; }
  virtual int end() const { return 10; }
  virtual bool valid(int deep, int i) const = 0;
  virtual bool find() const = 0;
  virtual void putIn(int deep, int i) = 0;
  virtual void moveOut(int deep, int i) = 0;
};

void dfs_once(int deep, dfsImpl &impl);
void dfs_all(int deep, dfsImpl &impl);

class puzzle : public dfsImpl {
 public:
  explicit puzzle(Matrix9i &mat);
  int merge(int r, int c) const;
  void setLimit(int l) { limit = l; }
  int getCount() const { return cnt; }

  void getPos(int &deep) const override;
  void success() override;
  int size() const override { return _size; }
  bool valid(int deep, int i) const override;
  bool find() const override;
  void putIn(int deep, int i) override;
  void moveOut(int deep, int i) override;

 private:
  void set(int r, int c, int n);
  void unset(int r, int c, int n);
  Matrix9i &_mat;
  int cnt;
  int limit;
  int _size = 81;
  int col[9], row[9], blk[9];
};

class by_col : public dfsImpl {
 public:
  explicit by_col(Vector9i &vec);
  bool modified();

  void getPos(int &deep) const override;
  void success() override;
  int size() const override { return _size; }
  bool valid(int deep, int i) const override;
  bool find() const override;
  void putIn(int deep, int i) override;
  void moveOut(int deep, int i) override;

 private:
  Vector9i &_vec;
  Vector9i all_ans;
  int _size = 9;
  bool used[10];
};

class col_block : public dfsImpl {
 public:
  explicit col_block(Vector9i &vec);
  bool modified();

  void getPos(int &deep) const override;
  void success() override;
  int size() const override { return _size; }
  bool valid(int deep, int i) const override;
  bool find() const override;
  void putIn(int deep, int i) override;
  void moveOut(int deep, int i) override;

 private:
  Vector9i &_vec;
  Vector9i all_ans;
  int _size = 9;
  bool col_used[9];
  bool block_used[9];
};
