/*
 * DES encryption/decryption
 * Copyright (c) 2007 Reimar Doeffinger
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

#ifndef AVUTIL_DES_H
#define AVUTIL_DES_H

#include <stdint.h>

/**
 * @defgroup lavu_des DES
 * @ingroup lavu_crypto
 * @{
 */

typedef struct AVDES {
    uint64_t round_keys[3][16];
    int triple_des;
} AVDES;

/**
 * 分配 AVDES 上下文。
 */
AVDES *av_des_alloc(void);

/**
 * @brief 初始化 AVDES 上下文。
 *
 * @param d 指向要初始化的 AVDES 结构的指针
 * @param key 指向所用密钥的指针
 * @param key_bits 必须为 64 或 192
 * @param decrypt 0 表示加密/CBC-MAC，1 表示解密
 * @return 成功时返回 0，否则返回负值
 */
int av_des_init(struct AVDES *d, const uint8_t *key, int key_bits, int decrypt);

/**
 * @brief 使用 DES 算法加密/解密。
 *
 * @param d 指向 AVDES 结构的指针
 * @param dst 目标数组，可以与 src 相同，必须按 8 字节对齐
 * @param src 源数组，可以与 dst 相同，必须按 8 字节对齐，可以为 NULL
 * @param count 8 字节块的数量
 * @param iv CBC 模式的初始化向量；为 NULL 时使用 ECB，必须按 8 字节对齐
 * @param decrypt 0 表示加密，1 表示解密
 */
void av_des_crypt(struct AVDES *d, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt);

/**
 * @brief 使用 DES 算法计算 CBC-MAC。
 *
 * @param d 指向 AVDES 结构的指针
 * @param dst 目标数组，可以与 src 相同，必须按 8 字节对齐
 * @param src 源数组，可以与 dst 相同，必须按 8 字节对齐，可以为 NULL
 * @param count 8 字节块的数量
 */
void av_des_mac(struct AVDES *d, uint8_t *dst, const uint8_t *src, int count);

/**
 * @}
 */

#endif /* AVUTIL_DES_H */
