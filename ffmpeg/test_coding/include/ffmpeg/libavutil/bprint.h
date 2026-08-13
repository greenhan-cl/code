/*
 * Copyright (c) 2012 Nicolas George
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
 * @ingroup lavu_avbprint
 * AVBPrint 公共头文件
 */

#ifndef AVUTIL_BPRINT_H
#define AVUTIL_BPRINT_H

#include <stdarg.h>

#include "attributes.h"
#include "avstring.h"

/**
 * @defgroup lavu_avbprint AVBPrint
 * @ingroup lavu_data
 *
 * 用于逐步打印数据的缓冲区
 * @{
 */

/**
 * 定义带额外填充、具有固定大小的结构。这有助于确保与未来版本二进制兼容。
 */

#define FF_PAD_STRUCTURE(name, size, ...) \
struct ff_pad_helper_##name { __VA_ARGS__ }; \
typedef struct name { \
    __VA_ARGS__ \
    char reserved_padding[size - sizeof(struct ff_pad_helper_##name)]; \
} name;

/**
 * 用于逐步打印数据的缓冲区
 *
 * 字符串缓冲区会按需增长，并始终以 0 结尾。字符串内容从不被访问，因此与编码
 * 无关，甚至可以保存二进制数据。
 *
 * 小缓冲区保存在结构本身中，因此完全无需分配内存（除非结构离开作用域后仍需
 * 使用缓冲区内容）。这几乎和声明局部 `char buf[512]` 一样轻量。
 *
 * 字符串长度可以超过已分配大小：此时缓冲区会被截断，但函数仍会记录实际总长度。
 *
 * 换言之，AVBPrint.len 可以大于 AVBPrint.size，它记录内存足够时本应写入缓冲区
 * 的总长度。
 *
 * 无需测试追加操作是否失败：内存分配失败时，数据停止追加到缓冲区，但长度仍会
 * 更新。可使用 av_bprint_is_complete() 测试这种情况。
 *
 * AVBPrint.size_max 字段决定以下行为：
 * - `size_max = -1`（即 `UINT_MAX`）或任意大值允许缓冲区按需重新分配，
 *   摊销成本为线性。
 * - `size_max = 0` 禁止向缓冲区写入，只计算总长度。随后可在大小恰好满足要求的
 *   缓冲区中重复写操作（使用 `size_init = size_max = len + 1`）。
 * - `size_max = 1` 会自动替换为结构本身可用的确切大小，从而保证不动态分配
 *   内存。内部缓冲区足以保存一段合理长度的文本，例如当前段落。
 */

FF_PAD_STRUCTURE(AVBPrint, 1024,
    char *str;         /**< 当前字符串 */
    unsigned len;      /**< 当前长度 */
    unsigned size;     /**< 已分配内存 */
    unsigned size_max; /**< 最大分配内存 */
    char reserved_internal_buffer[1];
)

/**
 * @name 最大大小特殊值
 * av_bprint_init() 的 size_max 参数特殊值便捷宏。
 * @{
 */

/**
 * 缓冲区将按需重新分配，摊销成本为线性。
 */
#define AV_BPRINT_SIZE_UNLIMITED  ((unsigned)-1)
/**
 * 使用 AVBPrint 结构本身可用的确切大小。
 *
 * 从而确保不动态分配内存。内部缓冲区足以保存一段合理长度的文本，例如当前段落。
 */
#define AV_BPRINT_SIZE_AUTOMATIC  1
/**
 * 不向缓冲区写入任何内容，只计算总长度。
 *
 * 随后可在大小恰好满足要求的缓冲区中重复写操作
 * （使用 `size_init = size_max = AVBPrint.len + 1`）。
 */
#define AV_BPRINT_SIZE_COUNT_ONLY 0
/** @} */

/**
 * 初始化打印缓冲区。
 *
 * @param buf        要初始化的缓冲区
 * @param size_init  初始大小（包含末尾的 0）
 * @param size_max   最大大小：
 *                   - `0` 表示不写入，只计算长度
 *                   - `1` 替换为自动存储的最大值；任意大值表示内部缓冲区可按需
 *                     重新分配，最大到该限制
 *                   - `-1` 转换为 `UINT_MAX`，即最大可能限制。
 *                   另请参见 `AV_BPRINT_SIZE_*` 宏。
 */
void av_bprint_init(AVBPrint *buf, unsigned size_init, unsigned size_max);

/**
 * 使用预先存在的缓冲区初始化打印缓冲区。
 *
 * 缓冲区不会重新分配。size 为零时，AVBPrint 会初始化为使用内部缓冲区，
 * 等同于以 AV_BPRINT_SIZE_COUNT_ONLY 调用 av_bprint_init()。
 *
 * @param buf     要初始化的缓冲区结构
 * @param buffer  用于字符串数据的字节缓冲区
 * @param size    缓冲区大小
 */
void av_bprint_init_for_buffer(AVBPrint *buf, char *buffer, unsigned size);

/**
 * 将格式化字符串追加到打印缓冲区。
 */
void av_bprintf(AVBPrint *buf, const char *fmt, ...) av_printf_format(2, 3);

/**
 * 将格式化字符串追加到打印缓冲区。
 */
void av_vbprintf(AVBPrint *buf, const char *fmt, va_list vl_arg);

/**
 * 将字符 c 追加 n 次到打印缓冲区。
 */
void av_bprint_chars(AVBPrint *buf, char c, unsigned n);

/**
 * 将数据追加到打印缓冲区。
 *
 * @param buf  要使用的 bprint 缓冲区
 * @param data 指向数据的指针
 * @param size 数据大小
 */
void av_bprint_append_data(AVBPrint *buf, const char *data, unsigned size);

struct tm;
/**
 * 将格式化日期和时间追加到打印缓冲区。
 *
 * @param buf  要使用的 bprint 缓冲区
 * @param fmt  日期和时间格式字符串，参见 strftime()
 * @param tm   要转换的分解时间结构
 *
 * @note 由于标准 strftime 函数设计不佳，如果格式字符串展开为很长文本，且
 * bprint 缓冲区接近 size_max 选项规定的限制，可能产生不理想的结果。
 */
void av_bprint_strftime(AVBPrint *buf, const char *fmt, const struct tm *tm);

/**
 * 在缓冲区中分配字节供外部使用。
 *
 * @param[in]  buf          缓冲区结构
 * @param[in]  size         所需大小
 * @param[out] mem          指向内存区域的指针
 * @param[out] actual_size  分配后内存区域的大小，可以大于或小于 size
 */
void av_bprint_get_buffer(AVBPrint *buf, unsigned size,
                          unsigned char **mem, unsigned *actual_size);

/**
 * 将字符串重置为 ""，但保留内部已分配数据。
 */
void av_bprint_clear(AVBPrint *buf);

/**
 * 测试打印缓冲区是否完整（未截断）。
 *
 * 它可能因内存分配失败或 size_max 限制而被截断（必要时比较 size 和 size_max）。
 */
static inline int av_bprint_is_complete(const AVBPrint *buf)
{
    return buf->len < buf->size;
}

/**
 * 完成打印缓冲区。
 *
 * 此后不能再使用打印缓冲区，但 len 和 size 字段仍有效。
 *
 * @arg[out] ret_str  非 NULL 时用于返回缓冲区内容的持久副本；内存分配失败时
 *                    返回 NULL。其本身为 NULL 时，缓冲区会被丢弃并释放
 * @return 成功时返回 0，否则返回错误码（可能是 AVERROR(ENOMEM)）
 */
int av_bprint_finalize(AVBPrint *buf, char **ret_str);

/**
 * 转义 src 中的内容并追加到 dstbuf。
 *
 * @param dstbuf        已初始化的目标 bprint 缓冲区
 * @param src           包含待转义文本的字符串
 * @param special_chars 包含需要转义特殊字符的字符串，可以为 NULL
 * @param mode          使用的转义模式，参见 AV_ESCAPE_MODE_* 宏。任何未知值均
 *                      视同 AV_ESCAPE_MODE_BACKSLASH，但此行为可能不经通知而改变。
 * @param flags         控制转义方式的标志，参见 AV_ESCAPE_FLAG_* 宏
 */
void av_bprint_escape(AVBPrint *dstbuf, const char *src, const char *special_chars,
                      enum AVEscapeMode mode, int flags);

/** @} */

#endif /* AVUTIL_BPRINT_H */
