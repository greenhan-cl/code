/*
 * Copyright (C) 2007 Michael Niedermayer <michaelni@gmx.at>
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
 * @ingroup lavu_sha
 * SHA-1 和 SHA-256 哈希函数实现的公共头文件。
 */

#ifndef AVUTIL_SHA_H
#define AVUTIL_SHA_H

#include <stddef.h>
#include <stdint.h>

#include "attributes.h"

/**
 * @defgroup lavu_sha SHA
 * @ingroup lavu_hash
 * SHA-1 和 SHA-256（安全哈希算法）哈希函数实现。
 *
 * 此模块支持以下 SHA 哈希函数：
 *
 * - SHA-1: 160 bits
 * - SHA-224: 224 bits, as a variant of SHA-2
 * - SHA-256: 256 bits, as a variant of SHA-2
 *
 * @see SHA-384、SHA-512 及其变体参见 @ref lavu_sha512 。
 *
 * @{
 */

extern const int av_sha_size;

struct AVSHA;

/**
 * 分配 AVSHA 上下文。
 */
struct AVSHA *av_sha_alloc(void);

/**
 * 初始化 SHA-1 或 SHA-2 哈希计算。
 *
 * @param context 指向函数上下文的指针（大小为 av_sha_size）
 * @param bits    摘要位数（SHA-1 为 160 位，SHA-2 为 224 或 256 位）
 * @return        初始化成功返回 0，否则返回 -1
 */
int av_sha_init(struct AVSHA* context, int bits);

/**
 * 更新哈希值。
 *
 * @param ctx     哈希函数上下文
 * @param data    用于更新哈希的输入数据
 * @param len     输入数据长度
 */
void av_sha_update(struct AVSHA *ctx, const uint8_t *data, size_t len);

/**
 * 完成哈希计算并输出摘要值。
 *
 * @param context 哈希函数上下文
 * @param digest  存储输出摘要值的缓冲区
 */
void av_sha_final(struct AVSHA* context, uint8_t *digest);

/**
 * @}
 */

#endif /* AVUTIL_SHA_H */
