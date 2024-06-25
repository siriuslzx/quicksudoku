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

#include <vector>

#include "config.h"

namespace li {
bool filter_notes(const Array9i board[], std::vector<Weight> &vec);

void always_easy(std::vector<Weight> &samp, const Array9i &ans);
void often_medium(std::vector<Weight> &samp, const Array9i &ans);
void usually_hard(std::vector<Weight> &samp, const Array9i &ans);

void col_swap_block(int &r, int &c);
bool _remove(Array9i board[], bool once);
bool col_circle_remove(Array9i board[], void (*fun)(int &, int &) = nullptr);
}  // namespace li
