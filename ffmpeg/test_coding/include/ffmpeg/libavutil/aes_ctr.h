/*
 * AES-CTR cipher
 * Copyright (c) 2015 Eran Kornblau <erankor at gmail dot com>
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

#ifndef AVUTIL_AES_CTR_H
#define AVUTIL_AES_CTR_H

/**
 * @defgroup lavu_aes_ctr AES-CTR
 * @ingroup lavu_crypto
 * @{
 */

#include <stdint.h>

#include "attributes.h"

#define AES_CTR_KEY_SIZE (16)
#define AES_CTR_IV_SIZE (8)

struct AVAESCTR;

/**
 * 分配 AVAESCTR 上下文。
 */
struct AVAESCTR *av_aes_ctr_alloc(void);

/**
 * 初始化 AVAESCTR 上下文。
 *
 * @param a 要初始化的 AVAESCTR 上下文
 * @param key 加密密钥，长度必须为 AES_CTR_KEY_SIZE
 */
int av_aes_ctr_init(struct AVAESCTR *a, const uint8_t *key);

/**
 * 释放 AVAESCTR 上下文。
 *
 * @param a AVAESCTR 上下文
 */
void av_aes_ctr_free(struct AVAESCTR *a);

/**
 * 使用之前初始化的上下文处理缓冲区。
 *
 * @param a AVAESCTR 上下文
 * @param dst 目标数组，可以与 src 相同
 * @param src 源数组，可以与 dst 相同
 * @param size src 和 dst 的大小
 */
void av_aes_ctr_crypt(struct AVAESCTR *a, uint8_t *dst, const uint8_t *src, int size);

/**
 * 获取当前 iv。
 */
const uint8_t* av_aes_ctr_get_iv(struct AVAESCTR *a);

/**
 * 生成随机 iv。
 */
void av_aes_ctr_set_random_iv(struct AVAESCTR *a);

/**
 * 强制更改 8 字节 iv。
 */
void av_aes_ctr_set_iv(struct AVAESCTR *a, const uint8_t* iv);

/**
 * 强制更改包含计数器在内的“完整”16 字节 iv。
 */
void av_aes_ctr_set_full_iv(struct AVAESCTR *a, const uint8_t* iv);

/**
 * 递增 iv 的高 64 位（在每帧之后执行）。
 */
void av_aes_ctr_increment_iv(struct AVAESCTR *a);

/**
 * @}
 */

#endif /* AVUTIL_AES_CTR_H */
