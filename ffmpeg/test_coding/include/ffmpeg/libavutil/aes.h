/*
 * copyright (c) 2007 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef AVUTIL_AES_H
#define AVUTIL_AES_H

#include <stdint.h>

#include "attributes.h"

/**
 * @defgroup lavu_aes AES
 * @ingroup lavu_crypto
 * @{
 */

extern const int av_aes_size;

struct AVAES;

/**
 * 分配 AVAES 上下文。
 */
struct AVAES *av_aes_alloc(void);

/**
 * 初始化 AVAES 上下文。
 *
 * @param a AVAES 上下文
 * @param key 指向密钥的指针
 * @param key_bits 128、192 或 256
 * @param decrypt 0 表示加密，1 表示解密
 */
int av_aes_init(struct AVAES *a, const uint8_t *key, int key_bits, int decrypt);

/**
 * 使用之前初始化的上下文加密或解密缓冲区。
 *
 * @param a AVAES 上下文
 * @param dst 目标数组，可以与 src 相同
 * @param src 源数组，可以与 dst 相同
 * @param count 16 字节块的数量
 * @param iv CBC 模式的初始化向量；为 NULL 时使用 ECB
 * @param decrypt 0 表示加密，1 表示解密
 */
void av_aes_crypt(struct AVAES *a, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt);

/**
 * @}
 */

#endif /* AVUTIL_AES_H */
