/*
 * An implementation of the TwoFish algorithm
 * Copyright (c) 2015 Supraja Meedinti
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

#ifndef AVUTIL_TWOFISH_H
#define AVUTIL_TWOFISH_H

#include <stdint.h>


/**
  * @file
  * @brief libavutil TWOFISH 算法的公共头文件
  * @defgroup lavu_twofish TWOFISH
  * @ingroup lavu_crypto
  * @{
  */

extern const int av_twofish_size;

struct AVTWOFISH;

/**
  * 分配 AVTWOFISH 上下文。使用 av_free(ptr) 释放结构体。
  */
struct AVTWOFISH *av_twofish_alloc(void);

/**
  * 初始化 AVTWOFISH 上下文。
  *
  * @param ctx AVTWOFISH 上下文
  * @param key 用于加密/解密的密钥，大小为 1 到 32 字节
  * @param key_bits 密钥位数：128、192、256。不足时补零到最近有效值；
  *                 key_bits 为 128/192/256 时返回 0，小于 0 时返回 -1，否则返回 1
 */
int av_twofish_init(struct AVTWOFISH *ctx, const uint8_t *key, int key_bits);

/**
  * 使用之前初始化的上下文加密或解密缓冲区。
  *
  * @param ctx AVTWOFISH 上下文
  * @param dst 目标数组，可以等于 src
  * @param src 源数组，可以等于 dst
  * @param count 16 字节块的数量
  * @param iv CBC 模式初始化向量；NULL 表示 ECB 模式
  * @param decrypt 0 表示加密，1 表示解密
 */
void av_twofish_crypt(struct AVTWOFISH *ctx, uint8_t *dst, const uint8_t *src, int count, uint8_t* iv, int decrypt);

/**
 * @}
 */
#endif /* AVUTIL_TWOFISH_H */
