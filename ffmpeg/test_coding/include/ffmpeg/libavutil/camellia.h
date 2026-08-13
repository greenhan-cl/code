/*
 * An implementation of the CAMELLIA algorithm as mentioned in RFC3713
 * Copyright (c) 2014 Supraja Meedinti
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

#ifndef AVUTIL_CAMELLIA_H
#define AVUTIL_CAMELLIA_H

#include <stdint.h>


/**
  * @file
  * @brief libavutil CAMELLIA 算法的公共头文件
  * @defgroup lavu_camellia CAMELLIA
  * @ingroup lavu_crypto
  * @{
  */

extern const int av_camellia_size;

struct AVCAMELLIA;

/**
  * 分配 AVCAMELLIA 上下文。
  * 使用 av_free(ptr) 释放该结构。
  */
struct AVCAMELLIA *av_camellia_alloc(void);

/**
  * 初始化 AVCAMELLIA 上下文。
  *
  * @param ctx AVCAMELLIA 上下文
  * @param key 用于加密/解密的 16、24 或 32 字节密钥
  * @param key_bits 密钥位数：可为 128、192、256
 */
int av_camellia_init(struct AVCAMELLIA *ctx, const uint8_t *key, int key_bits);

/**
  * 使用之前初始化的上下文加密或解密缓冲区。
  *
  * @param ctx AVCAMELLIA 上下文
  * @param dst 目标数组，可以与 src 相同
  * @param src 源数组，可以与 dst 相同
  * @param count 16 字节块的数量
  * @param iv CBC 模式的初始化向量；ECB 模式时为 NULL
  * @param decrypt 0 表示加密，1 表示解密
 */
void av_camellia_crypt(struct AVCAMELLIA *ctx, uint8_t *dst, const uint8_t *src, int count, uint8_t* iv, int decrypt);

/**
 * @}
 */
#endif /* AVUTIL_CAMELLIA_H */
