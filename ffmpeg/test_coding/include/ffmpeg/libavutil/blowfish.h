/*
 * Blowfish algorithm
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

#ifndef AVUTIL_BLOWFISH_H
#define AVUTIL_BLOWFISH_H

#include <stdint.h>

/**
 * @defgroup lavu_blowfish Blowfish
 * @ingroup lavu_crypto
 * @{
 */

#define AV_BF_ROUNDS 16

typedef struct AVBlowfish {
    uint32_t p[AV_BF_ROUNDS + 2];
    uint32_t s[4][256];
} AVBlowfish;

/**
 * 分配 AVBlowfish 上下文。
 */
AVBlowfish *av_blowfish_alloc(void);

/**
 * 初始化 AVBlowfish 上下文。
 *
 * @param ctx AVBlowfish 上下文
 * @param key 密钥
 * @param key_len 密钥长度
 */
void av_blowfish_init(struct AVBlowfish *ctx, const uint8_t *key, int key_len);

/**
 * 使用之前初始化的上下文加密或解密缓冲区。
 *
 * @param ctx AVBlowfish 上下文
 * @param xl 待加密输入的左侧 4 字节半块
 * @param xr 待加密输入的右侧 4 字节半块
 * @param decrypt 0 表示加密，1 表示解密
 */
void av_blowfish_crypt_ecb(struct AVBlowfish *ctx, uint32_t *xl, uint32_t *xr,
                           int decrypt);

/**
 * 使用之前初始化的上下文加密或解密缓冲区。
 *
 * @param ctx AVBlowfish 上下文
 * @param dst 目标数组，可以与 src 相同
 * @param src 源数组，可以与 dst 相同
 * @param count 8 字节块的数量
 * @param iv CBC 模式的初始化向量；为 NULL 时使用 ECB
 * @param decrypt 0 表示加密，1 表示解密
 */
void av_blowfish_crypt(struct AVBlowfish *ctx, uint8_t *dst, const uint8_t *src,
                       int count, uint8_t *iv, int decrypt);

/**
 * @}
 */

#endif /* AVUTIL_BLOWFISH_H */
