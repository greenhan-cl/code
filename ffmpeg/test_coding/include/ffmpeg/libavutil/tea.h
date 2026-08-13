/*
 * A 32-bit implementation of the TEA algorithm
 * Copyright (c) 2015 Vesselin Bontchev
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

#ifndef AVUTIL_TEA_H
#define AVUTIL_TEA_H

#include <stdint.h>

/**
 * @file
 * @brief libavutil TEA 算法的公共头文件
 * @defgroup lavu_tea TEA
 * @ingroup lavu_crypto
 * @{
 */

extern const int av_tea_size;

struct AVTEA;

/**
  * 分配 AVTEA 上下文。使用 av_free(ptr) 释放结构体。
  */
struct AVTEA *av_tea_alloc(void);

/**
 * 初始化 AVTEA 上下文。
 *
 * @param ctx AVTEA 上下文
 * @param key 用于加密/解密的 16 字节密钥
 * @param rounds TEA 轮数（64 为“标准”值）
 */
void av_tea_init(struct AVTEA *ctx, const uint8_t key[16], int rounds);

/**
 * 使用之前初始化的上下文加密或解密缓冲区。
 *
 * @param ctx AVTEA 上下文
 * @param dst 目标数组，可以等于 src
 * @param src 源数组，可以等于 dst
 * @param count 8 字节块的数量
 * @param iv CBC 模式的初始化向量；为 NULL 时使用 ECB
 * @param decrypt 0 表示加密，1 表示解密
 */
void av_tea_crypt(struct AVTEA *ctx, uint8_t *dst, const uint8_t *src,
                  int count, uint8_t *iv, int decrypt);

/**
 * @}
 */

#endif /* AVUTIL_TEA_H */
