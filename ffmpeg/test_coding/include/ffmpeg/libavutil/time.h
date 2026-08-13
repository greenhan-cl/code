/*
 * Copyright (c) 2000-2003 Fabrice Bellard
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

#ifndef AVUTIL_TIME_H
#define AVUTIL_TIME_H

#include <stdint.h>

/**
 * 获取当前时间，单位为微秒。
 */
int64_t av_gettime(void);

/**
 * 获取从某个未指定起点开始的当前时间，单位为微秒。支持的平台使用单调时钟，
 * 因而非常适合测量相对时间；不提供单调时钟的平台上返回值可能不单调。
 */
int64_t av_gettime_relative(void);

/**
 * 以布尔结果指示 av_gettime_relative() 的时间源是否单调。
 */
int av_gettime_relative_is_monotonic(void);

/**
 * 休眠一段时间。虽然时长以微秒表示，实际延迟可能按系统定时器精度取整。
 *
 * @param  usec 要休眠的微秒数
 * @return 成功返回 0，否则返回负错误码
 */
int av_usleep(unsigned usec);

#endif /* AVUTIL_TIME_H */
