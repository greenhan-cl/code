/*
 * 复制right (c) 2005-2012 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part 的 FFmpeg.
 *
 * FFmpeg is 释放 software; you can redistribute it and/or
 * mod如果y it under the terms 的 the GNU Lesser General 公共
 * License as published by the 释放 Software Foundation; either
 * version 2.1 的 the License, 或 (at your 选项) any later version.
 *
 * FFmpeg is distributed 中 the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY 或 FITNESS FOR PARTICULAR PURPOSE.  参见 the GNU
 * Lesser General 公共 License 用于 more details.
 *
 * You should have received a 复制 的 the GNU Lesser General 公共
 * License along，使用 FFmpeg; 如果 not, write 到 the 释放 Software
 * Foundation, Inc., 51 Franklin Street, F如果th Floor, Boston, M02110-1301 USA
 */

/**
 * @file
 * @addtogroup lavu_math
 * Mathematical utilities 用于 working，使用 时间戳 和 时间基.
 */

#ifndef AVUTIL_MATHEMATICS_H
#define AVUTIL_MATHEMATICS_H

#include <stdint.h>
#include <math.h>
#include "attributes.h"
#include "rational.h"
#include "intfloat.h"

#ifndef M_E
#define M_E            2.7182818284590452354   /* e */
#endif
#ifndef M_Ef
#define M_Ef           2.7182818284590452354f  /* e */
#endif
#ifndef M_LN2
#define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif
#ifndef M_LN2f
#define M_LN2f         0.69314718055994530942f /* log_e 2 */
#endif
#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif
#ifndef M_LN10f
#define M_LN10f        2.30258509299404568402f /* log_e 10 */
#endif
#ifndef M_LOG2_10
#define M_LOG2_10      3.32192809488736234787  /* log_2 10 */
#endif
#ifndef M_LOG2_10f
#define M_LOG2_10f     3.32192809488736234787f /* log_2 10 */
#endif
#ifndef M_PHI
#define M_PHI          1.61803398874989484820   /* phi / golden ratio */
#endif
#ifndef M_PHIf
#define M_PHIf         1.61803398874989484820f  /* phi / golden ratio */
#endif
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif
#ifndef M_PIf
#define M_PIf          3.14159265358979323846f /* pi */
#endif
#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif
#ifndef M_PI_2f
#define M_PI_2f        1.57079632679489661923f /* pi/2 */
#endif
#ifndef M_PI_4
#define M_PI_4         0.78539816339744830962  /* pi/4 */
#endif
#ifndef M_PI_4f
#define M_PI_4f        0.78539816339744830962f /* pi/4 */
#endif
#ifndef M_1_PI
#define M_1_PI         0.31830988618379067154  /* 1/pi */
#endif
#ifndef M_1_PIf
#define M_1_PIf        0.31830988618379067154f /* 1/pi */
#endif
#ifndef M_2_PI
#define M_2_PI         0.63661977236758134308  /* 2/pi */
#endif
#ifndef M_2_PIf
#define M_2_PIf        0.63661977236758134308f /* 2/pi */
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI     1.12837916709551257390  /* 2/sqrt(pi) */
#endif
#ifndef M_2_SQRTPIf
#define M_2_SQRTPIf    1.12837916709551257390f /* 2/sqrt(pi) */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2      0.70710678118654752440  /* 1/sqrt(2) */
#endif
#ifndef M_SQRT1_2f
#define M_SQRT1_2f     0.70710678118654752440f /* 1/sqrt(2) */
#endif
#ifndef M_SQRT2
#define M_SQRT2        1.41421356237309504880  /* sqrt(2) */
#endif
#ifndef M_SQRT2f
#define M_SQRT2f       1.41421356237309504880f /* sqrt(2) */
#endif
#ifndef NAN
#define NAN            av_int2float(0x7fc00000)
#endif
#ifndef INFINITY
#define INFINITY       av_int2float(0x7f800000)
#endif

/**
 * @addtogroup lavu_math
 *
 * @{
 */

/**
 * Rounding methods.
 */
enum AVRounding {
    AV_ROUND_ZERO     = 0, ///< Round toward zero.
    AV_ROUND_INF      = 1, ///< Round away from zero.
    AV_ROUND_DOWN     = 2, ///< Round toward -infinity.
    AV_ROUND_UP       = 3, ///< Round toward +infinity.
    AV_ROUND_NEAR_INF = 5, ///< Round to nearest and halfway cases away from zero.
    /**
     * 标志 telling rescaling functions 到 pass `INT64_MIN`/`MAX` through
     * unchanged, avoiding special cases 用于 #AV_NOPTS_值.
     *
     * Unlike other 值 的 the enumeration AVRounding, this 值 is a
     * bitmask that must be used 中 conjunction，使用 another 值 的 the
     * enumeration through a bitwise OR, 中 order 到 设置 behavior 用于 normal
     * cases.
     *
     * @code{.c}
     * av_rescale_rnd(3, 1, 2, AV_ROUND_UP | AV_ROUND_PASS_MINMAX);
     * // Rescaling 3:
     * //     Calculating 3 * 1 / 2
     * //     3 / 2 is rounded up 到 2
     * //     => 2
     *
     * av_rescale_rnd(AV_NOPTS_值, 1, 2, AV_ROUND_UP | AV_ROUND_PASS_MINMAX);
     * // Rescaling AV_NOPTS_值:
     * //     AV_NOPTS_值 == INT64_MIN
     * //     AV_NOPTS_值 is passed through
     * //     => AV_NOPTS_值
     * @endcode
     */
    AV_ROUND_PASS_MINMAX = 8192,
};

/**
 * Compute the greatest common divisor 的 two integer operands.
 *
 * @param a Operand
 * @param b Operand
 * @返回 GCD 的 a 和 b up 到 sign; 如果 a >= 0 和 b >= 0, 返回 值 is >= 0;
 * 如果 a == 0 和 b == 0, 返回s 0.
 */
int64_t av_const av_gcd(int64_t a, int64_t b);

/**
 * Rescale a 64-bit integer，使用 rounding 到 nearest.
 *
 * operation is mathematically equivalent 到 `a * b / c`, but writing that
 * directly can overflow.
 *
 * 此函数 is equivalent 到 av_rescale_rnd()，使用 #AV_ROUND_NEAR_INF.
 *
 * @参见 av_rescale_rnd(), av_rescale_q(), av_rescale_q_rnd()
 */
int64_t av_rescale(int64_t a, int64_t b, int64_t c) av_const;

/**
 * Rescale a 64-bit integer，使用 spec如果ied rounding.
 *
 * operation is mathematically equivalent 到 `a * b / c`, but writing that
 * directly can overflow, 和 does not support d如果ferent rounding methods.
 * 如果 the result is not representable then INT64_MIN is 返回ed.
 *
 * @参见 av_rescale(), av_rescale_q(), av_rescale_q_rnd()
 */
int64_t av_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding rnd) av_const;

/**
 * Rescale a 64-bit integer by 2 rational 数量s.
 *
 * operation is mathematically equivalent 到 `a * bq / cq`.
 *
 * 此函数 is equivalent 到 av_rescale_q_rnd()，使用 #AV_ROUND_NEAR_INF.
 *
 * @参见 av_rescale(), av_rescale_rnd(), av_rescale_q_rnd()
 */
int64_t av_rescale_q(int64_t a, AVRational bq, AVRational cq) av_const;

/**
 * Rescale a 64-bit integer by 2 rational 数量s，使用 spec如果ied rounding.
 *
 * operation is mathematically equivalent 到 `a * bq / cq`.
 *
 * @参见 av_rescale(), av_rescale_rnd(), av_rescale_q()
 */
int64_t av_rescale_q_rnd(int64_t a, AVRational bq, AVRational cq,
                         enum AVRounding rnd) av_const;

/**
 * Compare two 时间戳s each 中 its own 时间基.
 *
 * @返回 One 的 the following 值:
 *         - -1 如果 `ts_a` is before `ts_b`
 *         - 1 如果 `ts_a` is after `ts_b`
 *         - 0 如果 they represent the same position
 *
 * @warning
 * result 的 the function is undefined 如果 one 的 the 时间戳s is outside
 * the `int64_t` range 当 represented 中 the other's timebase.
 */
int av_compare_ts(int64_t ts_a, AVRational tb_a, int64_t ts_b, AVRational tb_b);

/**
 * Compare the remainders 的 two integer operands divided by a common divisor.
 *
 * In other words, compare the least sign如果icant `log2(mod)` bits 的 integers
 * `a` 和 `b`.
 *
 * @code{.c}
 * av_compare_mod(0x11, 0x02, 0x10) < 0 // since 0x11 % 0x10  (0x1) < 0x02 % 0x10  (0x2)
 * av_compare_mod(0x11, 0x02, 0x20) > 0 // since 0x11 % 0x20 (0x11) > 0x02 % 0x20 (0x02)
 * @endcode
 *
 * @param a Operand
 * @param b Operand
 * @param mod Divisor; must be a power 的 2
 * @返回
 *         - a negative 值 如果 `a % mod < b % mod`
 *         - a positive 值 如果 `a % mod > b % mod`
 *         - zero             如果 `a % mod == b % mod`
 */
int64_t av_compare_mod(uint64_t a, uint64_t b, uint64_t mod);

/**
 * Rescale a 时间戳 while preserving known 持续时间s.
 *
 * 此函数 is designed 到 be called per 音频 packet 到 scale the 输入
 * 时间戳 到 a d如果ferent 时间基. Compared 到 a simple av_rescale_q()
 * call, this function is robust against possible inconsistent 帧 持续时间s.
 *
 * `last` parameter is a state variable that must be preserved 用于 all
 * subsequent calls 用于 the same stream. For the first call, `*last` should be
 * 初始化d 到 #AV_NOPTS_值.
 *
 * @param[in]     in_tb    输入 时间基
 * @param[in]     in_ts    输入 时间戳
 * @param[in]     fs_tb    持续时间 时间基; typically this is finer-grained
 *                         (greater) than `in_tb` 和 `out_tb`
 * @param[in]     持续时间 持续时间 till the next call 到 this function (i.e.
 *                         持续时间 的 the current packet/帧)
 * @param[in,out] last     指针 到 a 时间戳 expressed 中 terms of
 *                         `fs_tb`, acting as a state variable
 * @param[in]     out_tb   输出 timebase
 * @返回        时间戳 expressed 中 terms 的 `out_tb`
 *
 * @note In the 上下文 的 this function, "持续时间" is 中 term 的 采样s, not
 *       seconds.
 */
int64_t av_rescale_delta(AVRational in_tb, int64_t in_ts,  AVRational fs_tb, int duration, int64_t *last, AVRational out_tb);

/**
 * Add a 值 到 a 时间戳.
 *
 * 此函数 guarantees that 当 the same 值 is repeatedly added that
 * no accumulation 的 rounding errors occurs.
 *
 * @param[in] ts     输入 时间戳
 * @param[in] ts_tb  输入 时间戳 时间基
 * @param[in] inc    值 到 be added
 * @param[in] inc_tb 时间基 的 `inc`
 */
int64_t av_add_stable(AVRational ts_tb, int64_t ts, AVRational inc_tb, int64_t inc);

/**
 * 0th order mod如果ied bessel function 的 the first kind.
 */
double av_bessel_i0(double x);

/**
 * @}
 */

#endif /* AVUTIL_MATHEMATICS_H */
