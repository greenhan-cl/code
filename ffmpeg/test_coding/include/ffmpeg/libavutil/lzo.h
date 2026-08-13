/*
 * LZO 1x decompression
 * copyright (c) 2006 Reimar Doeffinger
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

#ifndef AVUTIL_LZO_H
#define AVUTIL_LZO_H

/**
 * @defgroup lavu_lzo LZO
 * @ingroup lavu_crypto
 *
 * @{
 */

#include <stdint.h>

/** @name av_lzo1x_decode 返回的错误标志
 * @{ */
/// 解码完成前已到达输入缓冲区末尾
#define AV_LZO_INPUT_DEPLETED  1
/// 解码数据无法放入输出缓冲区
#define AV_LZO_OUTPUT_FULL     2
/// 对先前解码数据的引用错误
#define AV_LZO_INVALID_BACKPTR 4
/// 压缩比特流中的非特定错误
#define AV_LZO_ERROR           8
/** @} */

#define AV_LZO_INPUT_PADDING   8
#define AV_LZO_OUTPUT_PADDING 12

/**
 * @brief 解码 LZO 1x 压缩数据。
 * @param out 输出缓冲区
 * @param outlen 输出缓冲区大小；剩余字节数会通过此参数返回
 * @param in 输入缓冲区
 * @param inlen 输入缓冲区大小；剩余字节数会通过此参数返回
 * @return 成功时返回 0，否则返回上述错误标志的组合
 *
 * 确保所有缓冲区均正确填充：in 必须额外提供 AV_LZO_INPUT_PADDING 字节，
 * out 必须额外提供 AV_LZO_OUTPUT_PADDING 字节。
 */
int av_lzo1x_decode(void *out, int *outlen, const void *in, int *inlen);

/**
 * @}
 */

#endif /* AVUTIL_LZO_H */
