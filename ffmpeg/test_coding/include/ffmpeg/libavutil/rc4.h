/*
 * RC4 encryption/decryption/pseudo-random number generator
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

#ifndef AVUTIL_RC4_H
#define AVUTIL_RC4_H

#include <stdint.h>

/**
 * @defgroup lavu_rc4 RC4
 * @ingroup lavu_crypto
 * @{
 */

typedef struct AVRC4 {
    uint8_t state[256];
    int x, y;
} AVRC4;

/**
 * 分配 AVRC4 上下文。
 */
AVRC4 *av_rc4_alloc(void);

/**
 * @brief 初始化 AVRC4 上下文。
 *
 * @param d 指向 AVRC4 上下文的指针
 * @param key 包含密钥的缓冲区
 * @param key_bits 必须为 8 的倍数
 * @param decrypt 0 表示加密，1 表示解密；当前不起作用
 * @return 成功返回 0，否则返回负值
 */
int av_rc4_init(struct AVRC4 *d, const uint8_t *key, int key_bits, int decrypt);

/**
 * @brief 使用 RC4 算法加密/解密。
 *
 * @param d 指向 AVRC4 上下文的指针
 * @param count 字节数
 * @param dst 目标数组，可以等于 src
 * @param src 源数组，可以等于 dst，也可以为 NULL
 * @param iv RC4 尚未使用，应为 NULL
 * @param decrypt 0 表示加密，1 表示解密；尚未使用
 */
void av_rc4_crypt(struct AVRC4 *d, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt);

/**
 * @}
 */

#endif /* AVUTIL_RC4_H */
