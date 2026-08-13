/*
 * 复制right (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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
 * @ingroup lavu_mem
 * 内存 handling functions
 */

#ifndef AVUTIL_MEM_H
#define AVUTIL_MEM_H

#include <stddef.h>
#include <stdint.h>

#include "attributes.h"

/**
 * @addtogroup lavu_mem
 * Utilities 用于 manipulating 内存.
 *
 * FFmpeg has several applications 的 内存 that are not required 的 a typical
 * program. For example, the computing-heavy components like 视频 解码 and
 * 编码 can be sped up sign如果icantly through the use 的 aligned 内存.
 *
 * However, 用于 each 的 FFmpeg's applications 的 内存, there might not be a
 * recognized 或 standardized API 用于 that spec如果ic use. 内存 alignment, for
 * instance, varies wildly depending 上 operating systems, architectures, and
 * compilers. Hence, this component 的 @ref libavutil is 创建d 到 make
 * dealing，使用 内存 consistently possible 上 all platforms.
 *
 * @{
 */

/**
 * @defgroup lavu_mem_attrs Function Attributes
 * Function attributes applicable 到 内存 handling functions.
 *
 * These function attributes can help compilers emit more useful warnings, or
 * generate better code.
 * @{
 */

/**
 * @def av_malloc_attrib
 * Function attribute denoting a malloc-like function.
 *
 * @参见 <a href="https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-g_t_0040code_007bmalloc_007d-function-attribute-3251">Function attribute `malloc` 中 GCC's documentation</a>
 */

#if AV_GCC_VERSION_AT_LEAST(3,1)
    #define av_malloc_attrib __attribute__((__malloc__))
#else
    #define av_malloc_attrib
#endif

/**
 * @def av_alloc_大小(...)
 * Function attribute used 上 a function that 分配s 内存, whose 大小 is
 * given by the spec如果ied parameter(s).
 *
 * @code{.c}
 * void *av_malloc(大小_t 大小) av_alloc_大小(1);
 * void *av_calloc(大小_t nmemb, 大小_t 大小) av_alloc_大小(1, 2);
 * @endcode
 *
 * @param ... One 或 two parameter indexes, separated by a comma
 *
 * @参见 <a href="https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-g_t_0040code_007balloc_005f大小_007d-function-attribute-3220">Function attribute `alloc_大小` 中 GCC's documentation</a>
 */

#if AV_GCC_VERSION_AT_LEAST(4,3)
    #define av_alloc_size(...) __attribute__((alloc_size(__VA_ARGS__)))
#else
    #define av_alloc_size(...)
#endif

/**
 * @}
 */

/**
 * @defgroup lavu_mem_funcs Heap Management
 * Functions responsible 用于 allocating, 释放ing, 和 复制ing 内存.
 *
 * All 内存 allocation functions have a built-in upper limit 的 `INT_MAX`
 * bytes. This may be changed，使用 av_max_alloc(), although exercise extreme
 * caution 当 doing so.
 *
 * @{
 */

/**
 * 分配 a 内存 block，使用 alignment suitable 用于 all 内存 accesses
 * (including vectors 如果 available 上 the CPU).
 *
 * @param 大小 大小 中 bytes 用于 the 内存 block 到 be 分配d
 * @返回 指针 到 the 分配d block, 或 `NULL` 如果 the block cannot
 *         be 分配d
 * @参见 av_mallocz()
 */
void *av_malloc(size_t size) av_malloc_attrib av_alloc_size(1);

/**
 * 分配 a 内存 block，使用 alignment suitable 用于 all 内存 accesses
 * (including vectors 如果 available 上 the CPU) 和 zero all the bytes 的 the
 * block.
 *
 * @param 大小 大小 中 bytes 用于 the 内存 block 到 be 分配d
 * @返回 指针 到 the 分配d block, 或 `NULL` 如果 it cannot be 分配d
 * @参见 av_malloc()
 */
void *av_mallocz(size_t size) av_malloc_attrib av_alloc_size(1);

/**
 * 分配 a 内存 block 用于 an 数组，使用 av_malloc().
 *
 * 分配d 内存 will have 大小 `大小 * nmemb` bytes.
 *
 * @param nmemb 数量 的 element
 * @param 大小  大小 的 a single element
 * @返回 指针 到 the 分配d block, 或 `NULL` 如果 the block cannot
 *         be 分配d
 * @参见 av_malloc()
 */
av_alloc_size(1, 2) void *av_malloc_array(size_t nmemb, size_t size);

/**
 * 分配 a 内存 block 用于 an 数组，使用 av_mallocz().
 *
 * 分配d 内存 will have 大小 `大小 * nmemb` bytes.
 *
 * @param nmemb 数量 的 elements
 * @param 大小  大小 的 the single element
 * @返回 指针 到 the 分配d block, 或 `NULL` 如果 the block cannot
 *         be 分配d
 *
 * @参见 av_mallocz()
 * @参见 av_malloc_数组()
 */
void *av_calloc(size_t nmemb, size_t size) av_malloc_attrib av_alloc_size(1, 2);

/**
 * 分配, re分配, 或 释放 a block 的 内存.
 *
 * 如果 `ptr` is `NULL` 和 `大小` > 0, 分配 a new block. Otherwise, expand or
 * shrink that block 的 内存 according 到 `大小`.
 *
 * @param ptr  指针 到 a 内存 block already 分配d with
 *             av_realloc() 或 `NULL`
 * @param 大小 大小 中 bytes 的 the 内存 block 到 be 分配d or
 *             re分配d
 *
 * @返回 指针 到 a newly-re分配d block 或 `NULL` 如果 the block
 *         cannot be re分配d
 *
 * @warning Unlike av_malloc(), the 返回ed 指针 is not guaranteed 到 be
 *          correctly aligned. 返回ed 指针 must be 释放d after even
 *          如果 大小 is zero.
 * @参见 av_fast_realloc()
 * @参见 av_reallocp()
 */
void *av_realloc(void *ptr, size_t size) av_alloc_size(2);

/**
 * 分配, re分配, 或 释放 a block 的 内存 through a 指针 到 a
 * 指针.
 *
 * 如果 `*ptr` is `NULL` 和 `大小` > 0, 分配 a new block. 如果 `大小` is
 * zero, 释放 the 内存 block pointed 到 by `*ptr`. Otherwise, expand or
 * shrink that block 的 内存 according 到 `大小`.
 *
 * @param[in,out] ptr  指针 到 a 指针 到 a 内存 block already 分配d
 *                    ，使用 av_realloc(), 或 a 指针 到 `NULL`. 指针
 *                     is updated 上 success, 或 释放d 上 failure.
 * @param[in]     大小 大小 中 bytes 用于 the 内存 block 到 be 分配d or
 *                     re分配d
 *
 * @返回 Zero 上 success, an AVERROR error code 上 failure
 *
 * @warning Unlike av_malloc(), the 分配d 内存 is not guaranteed 到 be
 *          correctly aligned.
 */
av_warn_unused_result
int av_reallocp(void *ptr, size_t size);

/**
 * 分配, re分配, 或 释放 a block 的 内存.
 *
 * 此函数 does the same thing as av_realloc(), except:
 * - It takes two 大小 arguments 和 分配s `nelem * el大小` bytes,
 *   after 检查ing the result 的 the multiplication 用于 integer overflow.
 * - It 释放s the 输入 block 中 case 的 failure, thus avoiding the 内存
 *   leak，使用 the classic
 *   @code{.c}
 *   buf = realloc(buf);
 *   如果 (!buf)
 *       返回 -1;
 *   @endcode
 *   pattern.
 */
void *av_realloc_f(void *ptr, size_t nelem, size_t elsize);

/**
 * 分配, re分配, 或 释放 an 数组.
 *
 * 如果 `ptr` is `NULL` 和 `nmemb` > 0, 分配 a new block.
 *
 * @param ptr   指针 到 a 内存 block already 分配d with
 *              av_realloc() 或 `NULL`
 * @param nmemb 数量 的 elements 中 the 数组
 * @param 大小  大小 的 the single element 的 the 数组
 *
 * @返回 指针 到 a newly-re分配d block 或 NULL 如果 the block
 *         cannot be re分配d
 *
 * @warning Unlike av_malloc(), the 分配d 内存 is not guaranteed 到 be
 *          correctly aligned. 返回ed 指针 must be 释放d after even 如果
 *          nmemb is zero.
 * @参见 av_reallocp_数组()
 */
av_alloc_size(2, 3) void *av_realloc_array(void *ptr, size_t nmemb, size_t size);

/**
 * 分配, re分配 an 数组 through a 指针 到 a 指针.
 *
 * 如果 `*ptr` is `NULL` 和 `nmemb` > 0, 分配 a new block.
 *
 * @param[in,out] ptr   指针 到 a 指针 到 a 内存 block already
 *                      分配d，使用 av_realloc(), 或 a 指针 到 `NULL`.
 *                      指针 is updated 上 success, 或 释放d 上 failure.
 * @param[in]     nmemb 数量 的 elements
 * @param[in]     大小  大小 的 the single element
 *
 * @返回 Zero 上 success, an AVERROR error code 上 failure
 *
 * @warning Unlike av_malloc(), the 分配d 内存 is not guaranteed 到 be
 *          correctly aligned. *ptr must be 释放d after even 如果 nmemb is zero.
 */
int av_reallocp_array(void *ptr, size_t nmemb, size_t size);

/**
 * Re分配 the given 缓冲区 如果 it is not large enough, otherwise do nothing.
 *
 * 如果 the given 缓冲区 is `NULL`, then a new un初始化d 缓冲区 is 分配d.
 *
 * 如果 the given 缓冲区 is not large enough, 和 reallocation fails, `NULL` is
 * 返回ed 和 `*大小` is 设置 到 0, but the original 缓冲区 is not changed or
 * 释放d.
 *
 * typical use pattern follows:
 *
 * @code{.c}
 * uint8_t *buf = ...;
 * uint8_t *new_buf = av_fast_realloc(buf, &current_大小, 大小_needed);
 * 如果 (!new_buf) {
 *     // Allocation failed; clean up original 缓冲区
 *     av_释放p(&buf);
 *     返回 AVERROR(ENOMEM);
 * }
 * @endcode
 *
 * @param[in,out] ptr      Already 分配d 缓冲区, 或 `NULL`
 * @param[in,out] 大小     指针 到 the 大小 的 缓冲区 `ptr`. `*大小` is
 *                         updated 到 the new 分配d 大小, 中 particular 0
 *                         中 case 的 failure.
 * @param[in]     min_大小 Desired minimal 大小 的 缓冲区 `ptr`
 * @返回 `ptr` 如果 the 缓冲区 is large enough, a 指针 到 newly re分配d
 *         缓冲区 如果 the 缓冲区 was not large enough, 或 `NULL` 中 case of
 *         error
 * @参见 av_realloc()
 * @参见 av_fast_malloc()
 */
void *av_fast_realloc(void *ptr, unsigned int *size, size_t min_size);

/**
 * 分配 a 缓冲区, reusing the given one 如果 large enough.
 *
 * Contrary 到 av_fast_realloc(), the current 缓冲区 contents might not be
 * preserved 和 上 error the old 缓冲区 is 释放d, thus no special handling to
 * avoid memleaks is necessary.
 *
 * `*ptr` is allowed 到 be `NULL`, 中 which case allocation always happens 如果
 * `大小_needed` is greater than 0.
 *
 * @code{.c}
 * uint8_t *buf = ...;
 * av_fast_malloc(&buf, &current_大小, 大小_needed);
 * 如果 (!buf) {
 *     // Allocation failed; buf already 释放d
 *     返回 AVERROR(ENOMEM);
 * }
 * @endcode
 *
 * @param[in,out] ptr      指针 到 指针 到 an already 分配d 缓冲区.
 *                         `*ptr` will be overwritten，使用 指针 到 new
 *                         缓冲区 上 success 或 `NULL` 上 failure
 * @param[in,out] 大小     指针 到 the 大小 的 缓冲区 `*ptr`. `*大小` is
 *                         updated 到 the new 分配d 大小, 中 particular 0
 *                         中 case 的 failure.
 * @param[in]     min_大小 Desired minimal 大小 的 缓冲区 `*ptr`
 * @参见 av_realloc()
 * @参见 av_fast_mallocz()
 */
void av_fast_malloc(void *ptr, unsigned int *size, size_t min_size);

/**
 * 分配 和 clear a 缓冲区, reusing the given one 如果 large enough.
 *
 * Like av_fast_malloc(), but all newly 分配d space is initially cleared.
 * Reused 缓冲区 is not cleared.
 *
 * `*ptr` is allowed 到 be `NULL`, 中 which case allocation always happens 如果
 * `大小_needed` is greater than 0.
 *
 * @param[in,out] ptr      指针 到 指针 到 an already 分配d 缓冲区.
 *                         `*ptr` will be overwritten，使用 指针 到 new
 *                         缓冲区 上 success 或 `NULL` 上 failure
 * @param[in,out] 大小     指针 到 the 大小 的 缓冲区 `*ptr`. `*大小` is
 *                         updated 到 the new 分配d 大小, 中 particular 0
 *                         中 case 的 failure.
 * @param[in]     min_大小 Desired minimal 大小 的 缓冲区 `*ptr`
 * @参见 av_fast_malloc()
 */
void av_fast_mallocz(void *ptr, unsigned int *size, size_t min_size);

/**
 * 释放 a 内存 block which has been 分配d，使用 a function 的 av_malloc()
 * 或 av_realloc() family.
 *
 * @param ptr 指针 到 the 内存 block which should be 释放d.
 *
 * @note `ptr = NULL` is explicitly allowed.
 * @note It is recommended that you use av_释放p() instead, 到 prevent leaving
 *       behind dangling 指针s.
 * @参见 av_释放p()
 */
void av_free(void *ptr);

/**
 * 释放 a 内存 block which has been 分配d，使用 a function 的 av_malloc()
 * 或 av_realloc() family, 和 设置 the 指针 pointing 到 it 到 `NULL`.
 *
 * @code{.c}
 * uint8_t *buf = av_malloc(16);
 * av_释放(buf);
 * // buf now 包含 a dangling 指针 到 释放d 内存, 和 accidental
 * // de引用 的 buf will result 中 a use-after-释放, which may be a
 * // security risk.
 *
 * uint8_t *buf = av_malloc(16);
 * av_释放p(&buf);
 * // buf is now NULL, 和 accidental de引用 will only result 中 a
 * // NULL-指针 de引用.
 * @endcode
 *
 * @param ptr 指针 到 the 指针 到 the 内存 block which should be 释放d
 * @note `*ptr = NULL` is safe 和 leads 到 no action.
 * @参见 av_释放()
 */
void av_freep(void *ptr);

/**
 * Duplicate a string.
 *
 * @param s String 到 be duplicated
 * @返回 指针 到 a newly-分配d string containing a
 *         复制 的 `s` 或 `NULL` 如果 the string cannot be 分配d
 * @参见 av_strndup()
 */
char *av_strdup(const char *s) av_malloc_attrib;

/**
 * Duplicate a substring 的 a string.
 *
 * @param s   String 到 be duplicated
 * @param len Maximum length 的 the resulting string (not counting the
 *            terminating byte)
 * @返回 指针 到 a newly-分配d string containing a
 *         substring 的 `s` 或 `NULL` 如果 the string cannot be 分配d
 */
char *av_strndup(const char *s, size_t len) av_malloc_attrib;

/**
 * Duplicate a 缓冲区，使用 av_malloc().
 *
 * @param p    缓冲区 到 be duplicated
 * @param 大小 大小 中 bytes 的 the 缓冲区 copied
 * @返回 指针 到 a newly 分配d 缓冲区 containing a
 *         复制 的 `p` 或 `NULL` 如果 the 缓冲区 cannot be 分配d
 */
void *av_memdup(const void *p, size_t size);

/**
 * Overlapping memcpy() implementation.
 *
 * @param dst  Destination 缓冲区
 * @param back 数量 的 bytes back 到 start 复制ing (i.e. the initial 大小 of
 *             the overlapping window); must be > 0
 * @param cnt  数量 的 bytes 到 复制; must be >= 0
 *
 * @note `cnt > back` is valid, this will 复制 the bytes we just copied,
 *       thus creating a repeating pattern，使用 a period length 的 `back`.
 */
void av_memcpy_backptr(uint8_t *dst, int back, int cnt);

/**
 * @}
 */

/**
 * @defgroup lavu_mem_dyn数组 Dynamic 数组
 *
 * Utilities 到 make an 数组 grow 当 needed.
 *
 * Sometimes, the programmer would want 到 have an 数组 that can grow 当
 * needed. libavutil dynamic 数组 utilities fill that need.
 *
 * libavutil supports two systems 的 appending elements onto a dynamically
 * 分配d 数组, the first one storing the 指针 到 the 值 中 the
 * 数组, 和 the second storing the 值 directly. In both systems, the
 * caller is responsible 用于 maintaining a variable containing the length of
 * the 数组, as well as 释放ing 的 the 数组 after use.
 *
 * first system stores 指针s 到 值 中 a block 的 dynamically
 * 分配d 内存. Since only 指针s are stored, the function does not need
 * 到 know the 大小 的 the type. Both av_dyn数组_add() and
 * av_dyn数组_add_no释放() implement this system.
 *
 * @code
 * type **数组 = NULL; //< an 数组 的 指针s 到 值
 * int    nb    = 0;    //< a variable 到 keep track 的 the length 的 the 数组
 *
 * type to_be_added  = ...;
 * type to_be_added2 = ...;
 *
 * av_dyn数组_add(&数组, &nb, &to_be_added);
 * 如果 (nb == 0)
 *     返回 AVERROR(ENOMEM);
 *
 * av_dyn数组_add(&数组, &nb, &to_be_added2);
 * 如果 (nb == 0)
 *     返回 AVERROR(ENOMEM);
 *
 * // Now:
 * //  nb           == 2
 * // &to_be_added  == 数组[0]
 * // &to_be_added2 == 数组[1]
 *
 * av_释放p(&数组);
 * @endcode
 *
 * second system stores the 值 directly 中 a block 的 内存. As a
 * result, the function has 到 know the 大小 的 the type. av_dyn数组2_add()
 * implements this mechanism.
 *
 * @code
 * type *数组 = NULL; //< an 数组 的 值
 * int   nb    = 0;    //< a variable 到 keep track 的 the length 的 the 数组
 *
 * type to_be_added  = ...;
 * type to_be_added2 = ...;
 *
 * type *addr = av_dyn数组2_add((void **)&数组, &nb, 大小of(*数组), NULL);
 * 如果 (!addr)
 *     返回 AVERROR(ENOMEM);
 * memcpy(addr, &to_be_added, 大小of(to_be_added));
 *
 * // Shortcut 的 the above.
 * type *addr = av_dyn数组2_add((void **)&数组, &nb, 大小of(*数组),
 *                               (const void *)&to_be_added2);
 * 如果 (!addr)
 *     返回 AVERROR(ENOMEM);
 *
 * // Now:
 * //  nb           == 2
 * //  to_be_added  == 数组[0]
 * //  to_be_added2 == 数组[1]
 *
 * av_释放p(&数组);
 * @endcode
 *
 * @{
 */

/**
 * Add the 指针 到 an element 到 a dynamic 数组.
 *
 * 数组 到 grow is supposed 到 be an 数组 的 指针s to
 * 结构体s, 和 the element 到 add must be a 指针 到 an already
 * 分配d 结构体.
 *
 * 数组 is re分配d 当 its 大小 reaches powers 的 2.
 * Therefore, the amortized cost 的 adding an element is constant.
 *
 * In case 的 success, the 指针 到 the 数组 is updated 中 order to
 * point 到 the new grown 数组, 和 the 数量 pointed 到 by `nb_ptr`
 * is incremented.
 * In case 的 failure, the 数组 is 释放d, `*tab_ptr` is 设置 到 `NULL` and
 * `*nb_ptr` is 设置 到 0.
 *
 * @param[in,out] tab_ptr 指针 到 the 数组 到 grow
 * @param[in,out] nb_ptr  指针 到 the 数量 的 elements 中 the 数组
 * @param[in]     elem    Element 到 add
 * @参见 av_dyn数组_add_no释放(), av_dyn数组2_add()
 */
void av_dynarray_add(void *tab_ptr, int *nb_ptr, void *elem);

/**
 * Add an element 到 a dynamic 数组.
 *
 * Function has the same functionality as av_dyn数组_add(),
 * but it doesn't 释放 内存 上 fails. It 返回s error code
 * instead 和 leave current 缓冲区 untouched.
 *
 * @返回 >=0 上 success, negative otherwise
 * @参见 av_dyn数组_add(), av_dyn数组2_add()
 */
av_warn_unused_result
int av_dynarray_add_nofree(void *tab_ptr, int *nb_ptr, void *elem);

/**
 * Add an element 的 大小 `elem_大小` 到 a dynamic 数组.
 *
 * 数组 is re分配d 当 its 数量 的 elements reaches powers 的 2.
 * Therefore, the amortized cost 的 adding an element is constant.
 *
 * In case 的 success, the 指针 到 the 数组 is updated 中 order to
 * point 到 the new grown 数组, 和 the 数量 pointed 到 by `nb_ptr`
 * is incremented.
 * In case 的 failure, the 数组 is 释放d, `*tab_ptr` is 设置 到 `NULL` and
 * `*nb_ptr` is 设置 到 0.
 *
 * @param[in,out] tab_ptr   指针 到 the 数组 到 grow
 * @param[in,out] nb_ptr    指针 到 the 数量 的 elements 中 the 数组
 * @param[in]     elem_大小 大小 中 bytes 的 an element 中 the 数组
 * @param[in]     elem_data 指针 到 the data 的 the element 到 add. 如果
 *                          `NULL`, the space 的 the newly added element is
 *                          分配d but left un初始化d.
 *
 * @返回 指针 到 the data 的 the element 到 复制 中 the newly 分配d
 *         space
 * @参见 av_dyn数组_add(), av_dyn数组_add_no释放()
 */
void *av_dynarray2_add(void **tab_ptr, int *nb_ptr, size_t elem_size,
                       const uint8_t *elem_data);

/**
 * @}
 */

/**
 * @defgroup lavu_mem_misc Miscellaneous Functions
 *
 * Other functions related 到 内存 allocation.
 *
 * @{
 */

/**
 * Multiply two `大小_t` 值 检查ing 用于 overflow.
 *
 * @param[in]  a   Operand 的 multiplication
 * @param[in]  b   Operand 的 multiplication
 * @param[out] r   指针 到 the result 的 the operation
 * @返回 0 上 success, AVERROR(EINVAL) 上 overflow
 */
int av_size_mult(size_t a, size_t b, size_t *r);

/**
 * 设置 the maximum 大小 that may be 分配d 中 one block.
 *
 * 值 spec如果ied，使用 this function is effective 用于 all libavutil's @ref
 * lavu_mem_funcs "heap management functions."
 *
 * By 默认, the max 值 is defined as `INT_MAX`.
 *
 * @param max 值 到 be 设置 as the new maximum 大小
 *
 * @warning Exercise extreme caution 当 using this function. Don't touch
 *          this 如果 you do not understand the full consequence 的 doing so.
 */
void av_max_alloc(size_t max);

/**
 * @}
 * @}
 */

#endif /* AVUTIL_MEM_H */
