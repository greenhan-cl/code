/*
 * Copyright (c) 2006 Ryan Martell. (rdm4@martellventures.com)
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

#ifndef AVUTIL_BASE64_H
#define AVUTIL_BASE64_H

#include <stdint.h>

/**
 * @defgroup lavu_base64 Base64
 * @ingroup lavu_crypto
 * @{
 */

/**
 * 解码 base64 编码的字符串。
 *
 * @param out      解码数据的缓冲区
 * @param in       以 null 结尾的输入字符串
 * @param out_size out 缓冲区的字节大小，必须至少为 in 长度的 3/4，
 *                 即 AV_BASE64_DECODE_SIZE(strlen(in))
 * @return         写入的字节数；输入无效时返回负值
 */
int av_base64_decode(uint8_t *out, const char *in, int out_size);

/**
 * 计算将长度为 x 的 base64 字符串解码到数据缓冲区所需的输出字节数。
 */
#define AV_BASE64_DECODE_SIZE(x) ((x) * 3LL / 4)

/**
 * 将数据编码为 base64，并以 null 结尾。
 *
 * @param out      编码数据的缓冲区
 * @param out_size out 缓冲区的字节大小（包含 null 终止符），必须至少为
 *                 AV_BASE64_SIZE(in_size)
 * @param in       包含待编码数据的输入缓冲区
 * @param in_size  in 缓冲区的字节大小
 * @return         out；出错时返回 NULL
 */
char *av_base64_encode(char *out, int out_size, const uint8_t *in, int in_size);

/**
 * 计算将 x 字节编码为以 null 结尾的 base64 字符串所需的输出大小。
 */
#define AV_BASE64_SIZE(x)  (((x)+2) / 3 * 4 + 1)

 /**
  * @}
  */

#endif /* AVUTIL_BASE64_H */
