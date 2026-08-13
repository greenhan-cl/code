/*
 * Copyright (C) 2012 Martin Storsjo
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

#ifndef AVUTIL_HMAC_H
#define AVUTIL_HMAC_H

#include <stdint.h>

/**
 * @defgroup lavu_hmac HMAC
 * @ingroup lavu_crypto
 * @{
 */

enum AVHMACType {
    AV_HMAC_MD5,
    AV_HMAC_SHA1,
    AV_HMAC_SHA224,
    AV_HMAC_SHA256,
    AV_HMAC_SHA384,
    AV_HMAC_SHA512,
};

typedef struct AVHMAC AVHMAC;

/**
 * 分配 AVHMAC 上下文。
 * @param type HMAC 使用的哈希函数。
 */
AVHMAC *av_hmac_alloc(enum AVHMACType type);

/**
 * 释放 AVHMAC 上下文。
 * @param ctx 要释放的上下文，可以为 NULL
 */
void av_hmac_free(AVHMAC *ctx);

/**
 * 使用认证密钥初始化 AVHMAC 上下文。
 * @param ctx    HMAC 上下文
 * @param key    认证密钥
 * @param keylen 密钥长度，单位为字节
 */
void av_hmac_init(AVHMAC *ctx, const uint8_t *key, unsigned int keylen);

/**
 * 使用 HMAC 对数据进行哈希。
 * @param ctx  HMAC 上下文
 * @param data 要进行哈希的数据
 * @param len  数据长度，单位为字节
 */
void av_hmac_update(AVHMAC *ctx, const uint8_t *data, unsigned int len);

/**
 * 完成哈希并输出 HMAC 摘要。
 * @param ctx    HMAC 上下文
 * @param out    写入摘要的输出缓冲区
 * @param outlen out 缓冲区的长度，单位为字节
 * @return       写入 out 的字节数，或负错误码。
 */
int av_hmac_final(AVHMAC *ctx, uint8_t *out, unsigned int outlen);

/**
 * 使用密钥对数据数组进行哈希。
 * @param ctx    HMAC 上下文
 * @param data   要进行哈希的数据
 * @param len    数据长度，单位为字节
 * @param key    认证密钥
 * @param keylen 密钥长度，单位为字节
 * @param out    写入摘要的输出缓冲区
 * @param outlen out 缓冲区的长度，单位为字节
 * @return       写入 out 的字节数，或负错误码。
 */
int av_hmac_calc(AVHMAC *ctx, const uint8_t *data, unsigned int len,
                 const uint8_t *key, unsigned int keylen,
                 uint8_t *out, unsigned int outlen);

/**
 * @}
 */

#endif /* AVUTIL_HMAC_H */
