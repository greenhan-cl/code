/*
 * Copyright (c) 2009 Baptiste Coudurier <baptiste.coudurier@gmail.com>
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

#ifndef AVUTIL_RANDOM_SEED_H
#define AVUTIL_RANDOM_SEED_H

#include <stddef.h>
#include <stdint.h>
/**
 * @addtogroup lavu_crypto
 * @{
 */

/**
 * 获取与随机函数配合使用的种子。此函数会尽力提供高质量种子；需要更多位时可多次调用。
 * 它可能很慢，因此只应作为更快 PRNG 的种子。种子质量取决于平台。
 */
uint32_t av_get_random_seed(void);

/**
 * 生成密码学安全的随机数据，适合用作加密密钥等。
 *
 * @param buf 写入随机数据的缓冲区
 * @param len buf 的大小，单位为字节
 *
 * @retval 0                         成功，已向 buf 写入 len 字节随机数据
 * @retval "a negative AVERROR code" 无法生成随机数据
 */
int av_random_bytes(uint8_t *buf, size_t len);

/**
 * @}
 */

#endif /* AVUTIL_RANDOM_SEED_H */
