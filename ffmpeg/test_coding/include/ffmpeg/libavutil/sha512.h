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
 * @ingroup lavu_sha512
 * SHA-512 实现的公共头文件。
 */

#ifndef AVUTIL_SHA512_H
#define AVUTIL_SHA512_H

#include <stddef.h>
#include <stdint.h>

#include "attributes.h"

/**
 * @defgroup lavu_sha512 SHA-512
 * @ingroup lavu_hash
 * SHA-512（安全哈希算法）哈希函数实现。
 *
 * 此模块支持以下 SHA-2 哈希函数：
 *
 * - SHA-512/224: 224 bits
 * - SHA-512/256: 256 bits
 * - SHA-384: 384 bits
 * - SHA-512: 512 bits
 *
 * @see SHA-1、SHA-256 及其变体参见 @ref lavu_sha 。
 *
 * @{
 */

extern const int av_sha512_size;

struct AVSHA512;

/**
 * 分配 AVSHA512 上下文。
 */
struct AVSHA512 *av_sha512_alloc(void);

/**
 * 初始化 SHA-2 512 哈希计算。
 *
 * @param context 指向函数上下文的指针（大小为 av_sha512_size）
 * @param bits    摘要位数（224、256、384 或 512 位）
 * @return        初始化成功返回 0，否则返回 -1
 */
int av_sha512_init(struct AVSHA512* context, int bits);

/**
 * 更新哈希值。
 *
 * @param context 哈希函数上下文
 * @param data    用于更新哈希的输入数据
 * @param len     输入数据长度
 */
void av_sha512_update(struct AVSHA512* context, const uint8_t* data, size_t len);

/**
 * 完成哈希计算并输出摘要值。
 *
 * @param context 哈希函数上下文
 * @param digest  存储输出摘要值的缓冲区
 */
void av_sha512_final(struct AVSHA512* context, uint8_t *digest);

/**
 * @}
 */

#endif /* AVUTIL_SHA512_H */
