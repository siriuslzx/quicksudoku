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

#include "config.h"

namespace li {
bool operator<(const Weight &a, const Weight &b);

bool block_sum_0(const Array9i &arr);
bool row_single(int &r, int &c, const Array9i &arr);
bool col_single(int &r, int &c, const Array9i &arr);
bool block_single(int &r, int &c, const Array9i &arr);

void init_note(Array9i board[]);
bool get_single(int &r, int &c, int &num, const Array9i board[]);
void set_num(int r, int c, int num, Array9i board[]);
int fill_all_single(Array9i board[], bool check = false);

}  // namespace li
