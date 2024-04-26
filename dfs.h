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
class DfsImpl {
 public:
  virtual void getPos(int &deep) const = 0;
  virtual bool success(int deep) const = 0;
  virtual void findOne() = 0;
  virtual int begin() const { return 1; }
  virtual int end() const { return 10; }
  virtual bool valid(int deep, int n) const = 0;
  virtual void putIn(int deep, int n) = 0;
  virtual bool finish() const = 0;
  virtual void moveOut(int deep, int n) = 0;
  virtual ~DfsImpl() {}
};

void dfsGuess(DfsImpl &impl, int deep = 0);
void dfsDeduce(int R, int C, int num, const Array9i bd[], bool &isFind);

class Puzzle : public DfsImpl {
 public:
  explicit Puzzle(Array9i &puz);
  ~Puzzle() {}
  void setLimit(int l) { limit = l; }
  int getCount() const { return cnt; }

  void getPos(int &deep) const override;
  bool success(int deep) const override;
  void findOne() override;
  bool valid(int deep, int n) const override;
  void putIn(int deep, int n) override;
  bool finish() const override;
  void moveOut(int deep, int n) override;

 private:
  void mark(int r, int c, int n);
  void unmark(int r, int c, int n);
  Array9i &_puz;
  int cnt;
  int limit;
  const int _size = 81;
  Array9i col, row, blk;
};

}  // namespace li
