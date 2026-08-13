/*
 * Copyright (C) 2007 Michael Niedermayer <michaelni@gmx.at>
 * Copyright (C) 2013 James Almer <jamrial@gmail.com>
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
 * @ingroup lavu_ripemd
 * RIPEMD 哈希函数实现的公共头文件。
 */

#ifndef AVUTIL_RIPEMD_H
#define AVUTIL_RIPEMD_H

#include <stddef.h>
#include <stdint.h>

#include "attributes.h"

/**
 * @defgroup lavu_ripemd RIPEMD
 * @ingroup lavu_hash
 * RIPEMD 哈希函数实现。
 *
 * @{
 */

extern const int av_ripemd_size;

struct AVRIPEMD;

/**
 * 分配 AVRIPEMD 上下文。
 */
struct AVRIPEMD *av_ripemd_alloc(void);

/**
 * 初始化 RIPEMD 哈希计算。
 *
 * @param context 指向函数上下文的指针（大小为 av_ripemd_size）
 * @param bits    摘要位数（128、160、256 或 320 位）
 * @return        初始化成功返回 0，否则返回 -1
 */
int av_ripemd_init(struct AVRIPEMD* context, int bits);

/**
 * 更新哈希值。
 *
 * @param context 哈希函数上下文
 * @param data    用于更新哈希的输入数据
 * @param len     输入数据长度
 */
void av_ripemd_update(struct AVRIPEMD* context, const uint8_t* data, size_t len);

/**
 * 完成哈希计算并输出摘要值。
 *
 * @param context 哈希函数上下文
 * @param digest  存储输出摘要值的缓冲区
 */
void av_ripemd_final(struct AVRIPEMD* context, uint8_t *digest);

/**
 * @}
 */

#endif /* AVUTIL_RIPEMD_H */
