/*
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

#ifndef AVUTIL_PIXELUTILS_H
#define AVUTIL_PIXELUTILS_H

#include <stddef.h>
#include <stdint.h>

/**
 * abs(src1[x] - src2[x]) 的总和
 */
typedef int (*av_pixelutils_sad_fn)(const uint8_t *src1, ptrdiff_t stride1,
                                    const uint8_t *src2, ptrdiff_t stride2);

/**
 * 获取可能经过优化的绝对差之和函数指针（参见 av_pixelutils_sad_fn 原型）。
 *
 * @param w_bits  1<<w_bits 为请求的块宽度
 * @param h_bits  1<<h_bits 为请求的块高度
 * @param aligned 设为 2 时，返回的 sad 函数假定 src1、src2 地址按块大小对齐；
 *                设为 1 时假定 src1 按块大小对齐；设为 0 时不作特定对齐假设
 * @param log_ctx 用于日志记录的上下文，可以为 NULL
 *
 * @return 指向 SAD 函数的指针；因参数无效出错时返回 NULL
 */
av_pixelutils_sad_fn av_pixelutils_get_sad_fn(int w_bits, int h_bits,
                                              int aligned, void *log_ctx);

#endif /* AVUTIL_PIXELUTILS_H */
