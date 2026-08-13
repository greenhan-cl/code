/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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
 * @ingroup lavu_md5
 * MD5 哈希函数实现的公共头文件。
 */

#ifndef AVUTIL_MD5_H
#define AVUTIL_MD5_H

#include <stddef.h>
#include <stdint.h>

#include "attributes.h"

/**
 * @defgroup lavu_md5 MD5
 * @ingroup lavu_hash
 * MD5 哈希函数实现。
 *
 * @{
 */

extern const int av_md5_size;

struct AVMD5;

/**
 * 分配 AVMD5 上下文。
 */
struct AVMD5 *av_md5_alloc(void);

/**
 * 初始化 MD5 哈希。
 *
 * @param ctx 指向函数上下文的指针（大小为 av_md5_size）
 */
void av_md5_init(struct AVMD5 *ctx);

/**
 * 更新哈希值。
 *
 * @param ctx 哈希函数上下文
 * @param src 用于更新哈希的输入数据
 * @param len 输入数据长度
 */
void av_md5_update(struct AVMD5 *ctx, const uint8_t *src, size_t len);

/**
 * 完成哈希并输出摘要值。
 *
 * @param ctx 哈希函数上下文
 * @param dst 存储输出摘要值的缓冲区
 */
void av_md5_final(struct AVMD5 *ctx, uint8_t *dst);

/**
 * 对数据数组进行哈希。
 *
 * @param dst 写入摘要的输出缓冲区
 * @param src 要进行哈希的数据
 * @param len 数据长度，单位为字节
 */
void av_md5_sum(uint8_t *dst, const uint8_t *src, size_t len);

/**
 * @}
 */

#endif /* AVUTIL_MD5_H */
