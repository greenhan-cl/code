/*
 * An implementation of the CAST128 algorithm as mentioned in RFC2144
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

#ifndef AVUTIL_CAST5_H
#define AVUTIL_CAST5_H

#include <stdint.h>


/**
  * @file
  * @brief libavutil CAST5 算法的公共头文件
  * @defgroup lavu_cast5 CAST5
  * @ingroup lavu_crypto
  * @{
  */

extern const int av_cast5_size;

struct AVCAST5;

/**
  * 分配 AVCAST5 上下文。
  * 使用 av_free(ptr) 释放该结构。
  */
struct AVCAST5 *av_cast5_alloc(void);
/**
  * 初始化 AVCAST5 上下文。
  *
  * @param ctx AVCAST5 上下文
  * @param key 用于加密/解密的 5、6、...、16 字节密钥
  * @param key_bits 密钥位数：可为 40、48、...、128
  * @return 成功时返回 0，失败时返回小于 0 的值
 */
int av_cast5_init(struct AVCAST5 *ctx, const uint8_t *key, int key_bits);

/**
  * 使用之前初始化的上下文加密或解密缓冲区，仅支持 ECB 模式。
  *
  * @param ctx AVCAST5 上下文
  * @param dst 目标数组，可以与 src 相同
  * @param src 源数组，可以与 dst 相同
  * @param count 8 字节块的数量
  * @param decrypt 0 表示加密，1 表示解密
 */
void av_cast5_crypt(struct AVCAST5 *ctx, uint8_t *dst, const uint8_t *src, int count, int decrypt);

/**
  * 使用之前初始化的上下文加密或解密缓冲区。
  *
  * @param ctx AVCAST5 上下文
  * @param dst 目标数组，可以与 src 相同
  * @param src 源数组，可以与 dst 相同
  * @param count 8 字节块的数量
  * @param iv CBC 模式的初始化向量；ECB 模式时为 NULL
  * @param decrypt 0 表示加密，1 表示解密
 */
void av_cast5_crypt2(struct AVCAST5 *ctx, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt);
/**
 * @}
 */
#endif /* AVUTIL_CAST5_H */
