/*
 * copyright (c) 2010 Michael Niedermayer <michaelni@gmx.at>
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
 * 比 ISO C assert() 更灵活一些的简单断言宏。
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#ifndef AVUTIL_AVASSERT_H
#define AVUTIL_AVASSERT_H

#include <stdlib.h>
#ifdef HAVE_AV_CONFIG_H
#   include "config.h"
#endif
#include "attributes.h"
#include "log.h"
#include "macros.h"
#include "version.h"

/**
 * 始终启用的 assert() 等效项。
 */
#define av_assert0(cond) do {                                           \
    if (!(cond)) {                                                      \
        av_log(NULL, AV_LOG_PANIC, "Assertion %s failed at %s:%d\n",    \
               AV_STRINGIFY(cond), __FILE__, __LINE__);                 \
        abort();                                                        \
    }                                                                   \
} while (0)


/**
 * 不位于性能关键代码中的 assert() 等效项。因此可启用这些 assert()，
 * 而不必担心性能损失。
 */
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL > 0
#define av_assert1(cond) av_assert0(cond)
#else
#define av_assert1(cond) ((void)0)
#endif


/**
 * 位于性能关键代码中的 assert() 等效项。
 */
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL > 1
#define av_assert2(cond) av_assert0(cond)
#else
#define av_assert2(cond) ((void)0)
#endif

#if FF_API_ASSERT_FPU
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL > 1
#define av_assert2_fpu() av_assert0_fpu()
#else
#define av_assert2_fpu() ((void)0)
#endif
/**
 * 断言可以执行浮点运算。
 *
 * 在 X86 上，这会使用 av_assert0() 断言 CPU 不处于 MMX 状态。
 * @deprecated 无替代项
 */
attribute_deprecated
void av_assert0_fpu(void);
#endif

/**
 * 根据 ASSERT_LEVEL 和 NBDEBUG 用作编译器优化提示的断言。
 *
 * 如果执行到达 av_unreachable() 标记的位置，或 av_assume() 使用的条件为假，
 * 则会发生未定义行为。
 *
 * av_assume() 使用的条件不应有副作用，并且应对编译器可见。
 */
#if defined(ASSERT_LEVEL) ? ASSERT_LEVEL > 0 : !defined(HAVE_AV_CONFIG_H) && !defined(NDEBUG)
#define av_unreachable(msg)                                             \
do {                                                                    \
    av_log(NULL, AV_LOG_PANIC,                                          \
           "Reached supposedly unreachable code at %s:%d: %s\n",        \
           __FILE__, __LINE__, msg);                                    \
    abort();                                                            \
} while (0)
#define av_assume(cond) av_assert0(cond)
#else
#if AV_GCC_VERSION_AT_LEAST(4, 5) || AV_HAS_BUILTIN(__builtin_unreachable)
#define av_unreachable(msg) __builtin_unreachable()
#elif  defined(_MSC_VER)
#define av_unreachable(msg) __assume(0)
#elif __STDC_VERSION__ >= 202311L
#include <stddef.h>
#define av_unreachable(msg) unreachable()
#else
#define av_unreachable(msg) ((void)0)
#endif

#define av_assume(cond) do { \
    if (!(cond))             \
        av_unreachable();    \
} while (0)
#endif

#endif /* AVUTIL_AVASSERT_H */
