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

#include <Eigen/Core>

using Matrix9i = Eigen::Matrix<int, 9, 9>;
using Vector9i = Eigen::Matrix<int, 9, 1>;

const int kDet = 4;
const int kNotes = 0b111'111'111'0;
const int kNum = 0b1111;

inline bool getBit(int n, int idx) { return (n >> idx) & 1; }

inline void setBit(int &n, int idx) { n |= (1 << idx); }

inline void unsetBit(int &n, int idx) { n &= ~(1 << idx); }

inline void flipBit(int &n, int idx) { n ^= (1 << idx); }
