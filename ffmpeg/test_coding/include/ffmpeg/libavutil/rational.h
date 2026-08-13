/*
 * rational numbers
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * @ingroup lavu_math_rational
 * 有理数计算工具。
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#ifndef AVUTIL_RATIONAL_H
#define AVUTIL_RATIONAL_H

#include <stdint.h>
#include <limits.h>
#include "attributes.h"

/**
 * @defgroup lavu_math_rational AVRational
 * @ingroup lavu_math
 * 有理数计算。
 *
 * 有理数虽可表示为浮点数，但转换和浮点运算都会损失精度；FFmpeg 又要求高精度时间戳计算。
 * 因此，这组工具以分子/分母对形式提供操作有理数的通用接口。
 *
 * 许多操作 AVRational 的函数以 `_q` 为后缀，对应表示全体有理数集合的数学符号“ℚ”(Q)。
 *
 * @{
 */

/**
 * 有理数（分子与分母组成的数对）。
 */
typedef struct AVRational{
    int num; ///< 分子
    int den; ///< 分母
} AVRational;

/**
 * 创建 AVRational。
 *
 * 适用于不支持复合字面量的编译器。
 *
 * @note 返回值未约分。
 * @see av_reduce()
 */
static inline AVRational av_make_q(int num, int den)
{
    AVRational r = { num, den };
    return r;
}

/**
 * 比较两个有理数。
 *
 * @param a 第一个有理数
 * @param b 第二个有理数
 *
 * @return 下列值之一：
 *         - `a == b` 时为 0
 *         - `a > b` 时为 1
 *         - `a < b` 时为 -1
 *         - 任一值形如 `0 / 0` 时为 `INT_MIN`
 */
static inline int av_cmp_q(AVRational a, AVRational b){
    const int64_t tmp= a.num * (int64_t)b.den - b.num * (int64_t)a.den;

    if(tmp) return (int)((tmp ^ a.den ^ b.den)>>63)|1;
    else if(b.den && a.den) return 0;
    else if(a.num && b.num) return (a.num>>31) - (b.num>>31);
    else                    return INT_MIN;
}

/**
 * 将 AVRational 转换为 `double`。
 * @param a 要转换的 AVRational
 * @return `a` 的浮点形式
 * @see av_d2q()
 */
static inline double av_q2d(AVRational a){
    return a.num / (double) a.den;
}

/**
 * 约分分数。
 *
 * 适用于帧率计算。
 *
 * @param[out] dst_num 目标分子
 * @param[out] dst_den 目标分母
 * @param[in]      num 源分子
 * @param[in]      den 源分母
 * @param[in]      max `dst_num` 和 `dst_den` 允许的最大值
 * @return 操作精确时返回 1，否则返回 0
 */
int av_reduce(int *dst_num, int *dst_den, int64_t num, int64_t den, int64_t max);

/**
 * 两个有理数相乘。
 * @param b 第一个有理数
 * @param c 第二个有理数
 * @return b*c
 */
AVRational av_mul_q(AVRational b, AVRational c) av_const;

/**
 * 一个有理数除以另一个有理数。
 * @param b 第一个有理数
 * @param c 第二个有理数
 * @return b/c
 */
AVRational av_div_q(AVRational b, AVRational c) av_const;

/**
 * 两个有理数相加。
 * @param b 第一个有理数
 * @param c 第二个有理数
 * @return b+c
 */
AVRational av_add_q(AVRational b, AVRational c) av_const;

/**
 * 从一个有理数中减去另一个有理数。
 * @param b 第一个有理数
 * @param c 第二个有理数
 * @return b-c
 */
AVRational av_sub_q(AVRational b, AVRational c) av_const;

/**
 * 对有理数求倒数。
 * @param q 值
 * @return 1 / q
 */
static av_always_inline AVRational av_inv_q(AVRational q)
{
    AVRational r = { q.den, q.num };
    return r;
}

/**
 * 将双精度浮点数转换为有理数。
 *
 * 对于无穷大，根据符号将返回值表示为 `{1, 0}` 或 `{-1, 0}`。
 *
 * 通常，|num| <= 1<<26 且 |den| <= 1<<26 的有理数可从 double 表示精确恢复。
 * （在 10 亿个随机值中未发现例外）
 *
 * @param d   要转换的 `double`
 * @param max 允许的最大分子和分母
 * @return AVRational 形式的 `d`
 * @see av_q2d()
 */
AVRational av_d2q(double d, int max) av_const;

/**
 * 判断两个有理数中哪个更接近另一个有理数。
 *
 * @param q     比较基准有理数
 * @param q1    待测试有理数
 * @param q2    待测试有理数
 * @return 下列值之一：q1 更近时为 1，q2 更近时为 -1，距离相同时为 0
 */
int av_nearer_q(AVRational q, AVRational q1, AVRational q2);

/**
 * 在有理数列表中查找最接近给定参考有理数的值。
 *
 * @param q      参考有理数
 * @param q_list 以 `{0, 0}` 结尾的有理数数组
 * @return 数组中最近值的索引
 */
int av_find_nearest_q_idx(AVRational q, const AVRational* q_list);

/**
 * 将 AVRational 转换为以定点格式表示的 IEEE 32 位 `float`。
 *
 * @param q 要转换的有理数
 * @return 等效浮点值，以无符号 32 位整数表示
 * @note 返回值与平台无关。
 */
uint32_t av_q2intfloat(AVRational q);

/**
 * 返回使 a 和 b 都是其倍数的最佳有理数。所得分母大于 max_den 时返回 def。
 */
AVRational av_gcd_q(AVRational a, AVRational b, int max_den, AVRational def);

/**
 * @}
 */

#endif /* AVUTIL_RATIONAL_H */
