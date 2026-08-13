/*
 * copyright (c) 2006 Mans Rullgard
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
 * @ingroup lavu_adler32
 * Adler-32 哈希函数实现的公共头文件。
 */

#ifndef AVUTIL_ADLER32_H
#define AVUTIL_ADLER32_H

#include <stddef.h>
#include <stdint.h>
#include "attributes.h"

/**
 * @defgroup lavu_adler32 Adler-32
 * @ingroup lavu_hash
 * Adler-32 哈希函数实现。
 *
 * @{
 */

typedef uint32_t AVAdler;

/**
 * 计算缓冲区的 Adler32 校验和。
 *
 * 将返回值传给后续的 av_adler32_update() 调用，可以像多个缓冲区已拼接在一起
 * 那样计算它们的校验和。
 *
 * @param adler 初始校验和值
 * @param buf   指向输入缓冲区的指针
 * @param len   输入缓冲区大小
 * @return      更新后的校验和
 */
AVAdler av_adler32_update(AVAdler adler, const uint8_t *buf,
                          size_t len) av_pure;

/**
 * @}
 */

#endif /* AVUTIL_ADLER32_H */
