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
 * 时间戳工具，主要用于调试和日志记录
 */

#ifndef AVUTIL_TIMESTAMP_H
#define AVUTIL_TIMESTAMP_H

#include "avutil.h"

#if defined(__cplusplus) && !defined(__STDC_FORMAT_MACROS) && !defined(PRId64)
#error missing -D__STDC_FORMAT_MACROS / #define __STDC_FORMAT_MACROS
#endif

#define AV_TS_MAX_STRING_SIZE 32

/**
 * 使用包含时间戳表示形式的字符串填充给定缓冲区。
 *
 * @param buf 大小至少为 AV_TS_MAX_STRING_SIZE 字节的缓冲区
 * @param ts 要表示的时间戳
 * @return 输入的缓冲区
 */
static inline char *av_ts_make_string(char *buf, int64_t ts)
{
    if (ts == AV_NOPTS_VALUE) snprintf(buf, AV_TS_MAX_STRING_SIZE, "NOPTS");
    else                      snprintf(buf, AV_TS_MAX_STRING_SIZE, "%" PRId64, ts);
    return buf;
}

/**
 * 便捷宏。返回值只能直接用于函数参数，不能单独使用。
 */
#define av_ts2str(ts) av_ts_make_string((char[AV_TS_MAX_STRING_SIZE]){0}, ts)

/**
 * 使用包含时间戳时间表示形式的字符串填充给定缓冲区。
 *
 * @param buf 大小至少为 AV_TS_MAX_STRING_SIZE 字节的缓冲区
 * @param ts 要表示的时间戳
 * @param tb 时间戳的时间基
 * @return 输入的缓冲区
 */
char *av_ts_make_time_string2(char *buf, int64_t ts, AVRational tb);

/**
 * 使用包含时间戳表示形式的字符串填充给定缓冲区。
 *
 * @see av_ts_make_time_string2
 */
static inline char *av_ts_make_time_string(char *buf, int64_t ts,
                                           const AVRational *tb)
{
    return av_ts_make_time_string2(buf, ts, *tb);
}

/**
 * 便捷宏。返回值只能直接用于函数参数，不能单独使用。
 */
#define av_ts2timestr(ts, tb) av_ts_make_time_string((char[AV_TS_MAX_STRING_SIZE]){0}, ts, tb)

#endif /* AVUTIL_TIMESTAMP_H */
