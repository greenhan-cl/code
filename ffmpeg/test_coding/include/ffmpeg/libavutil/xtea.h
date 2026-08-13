/*
 * A 32-bit implementation of the XTEA algorithm
 * Copyright (c) 2012 Samuel Pitoiset
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

#ifndef AVUTIL_XTEA_H
#define AVUTIL_XTEA_H

#include <stdint.h>

/**
 * @file
 * @brief libavutil XTEA 算法的公共头文件
 * @defgroup lavu_xtea XTEA
 * @ingroup lavu_crypto
 * @{
 */

typedef struct AVXTEA {
    uint32_t key[16];
} AVXTEA;

/**
 * 分配 AVXTEA 上下文。
 */
AVXTEA *av_xtea_alloc(void);

/**
 * 初始化 AVXTEA 上下文。
 *
 * @param ctx AVXTEA 上下文
 * @param key 用于加密/解密的 16 字节密钥，解释为大端 32 位数
 */
void av_xtea_init(struct AVXTEA *ctx, const uint8_t key[16]);

/**
 * 初始化 AVXTEA 上下文。
 *
 * @param ctx AVXTEA 上下文
 * @param key 用于加密/解密的 16 字节密钥，解释为小端 32 位数
 */
void av_xtea_le_init(struct AVXTEA *ctx, const uint8_t key[16]);

/**
 * 使用之前初始化的上下文，以大端格式加密或解密缓冲区。
 *
 * @param ctx AVXTEA 上下文
 * @param dst 目标数组，可以等于 src
 * @param src 源数组，可以等于 dst
 * @param count 8 字节块的数量
 * @param iv CBC 模式初始化向量；为 NULL 时使用 ECB
 * @param decrypt 0 表示加密，1 表示解密
 */
void av_xtea_crypt(struct AVXTEA *ctx, uint8_t *dst, const uint8_t *src,
                   int count, uint8_t *iv, int decrypt);

/**
 * 使用之前初始化的上下文，以小端格式加密或解密缓冲区。
 *
 * @param ctx AVXTEA 上下文
 * @param dst 目标数组，可以等于 src
 * @param src 源数组，可以等于 dst
 * @param count 8 字节块的数量
 * @param iv CBC 模式初始化向量；为 NULL 时使用 ECB
 * @param decrypt 0 表示加密，1 表示解密
 */
void av_xtea_le_crypt(struct AVXTEA *ctx, uint8_t *dst, const uint8_t *src,
                      int count, uint8_t *iv, int decrypt);

/**
 * @}
 */

#endif /* AVUTIL_XTEA_H */
