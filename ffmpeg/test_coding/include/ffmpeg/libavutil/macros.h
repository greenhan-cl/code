/*
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
 * @ingroup lavu
 * 实用预处理器宏
 */

#ifndef AVUTIL_MACROS_H
#define AVUTIL_MACROS_H

#include "libavutil/avconfig.h"

#if AV_HAVE_BIGENDIAN
#   define AV_NE(be, le) (be)
#else
#   define AV_NE(be, le) (le)
#endif

/**
 * 比较器。
 * 对两个数值表达式 x 和 y，若 x > y 则给出 1，若 x < y 则给出 -1，若 x == y
 * 则给出 0。例如，这在 qsort 比较器回调中很有用。此外，编译器能够将其优化为
 * 无分支代码，并且有符号类型不存在溢出风险。与许多宏一样，它会多次求值参数，
 * 因此参数不能有副作用。
 */
#define FFDIFFSIGN(x,y) (((x)>(y)) - ((x)<(y)))

#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMAX3(a,b,c) FFMAX(FFMAX(a,b),c)
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMIN3(a,b,c) FFMIN(FFMIN(a,b),c)

#define FFSWAP(type,a,b) do{type SWAP_tmp= b; b= a; a= SWAP_tmp;}while(0)
#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

#define MKTAG(a,b,c,d)   ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define MKBETAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))

/**
 * @addtogroup preproc_misc 预处理器字符串宏
 *
 * 字符串操作宏
 *
 * @{
 */

#define AV_STRINGIFY(s)         AV_TOSTRING(s)
#define AV_TOSTRING(s) #s

#define AV_GLUE(a, b) a ## b
#define AV_JOIN(a, b) AV_GLUE(a, b)

/**
 * @}
 */

#define AV_PRAGMA(s) _Pragma(#s)

#define FFALIGN(x, a) (((x)+(a)-1)&~((a)-1))

#endif /* AVUTIL_MACROS_H */
