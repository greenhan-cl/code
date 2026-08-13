/*
 * Copyright (c) 2014 Vittorio Giovara <vittorio.giovara@gmail.com>
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

/**
 * @file
 * @ingroup lavu_video_display
 * 显示矩阵
 */

#ifndef AVUTIL_DISPLAY_H
#define AVUTIL_DISPLAY_H

#include <stdint.h>

/**
 * @defgroup lavu_video_display 显示变换矩阵函数
 * @ingroup lavu_video
 *
 * 显示变换矩阵指定应应用于视频帧以正确呈现的仿射变换。它与 ISO/IEC 14496-12
 * 容器格式中存储的矩阵兼容。
 *
 * 数据是用 9 元素数组表示的 3x3 矩阵：
 *
 * @code{.unparsed}
 *                                  | a b u |
 *   (a, b, u, c, d, v, x, y, w) -> | c d v |
 *                                  | x y w |
 * @endcode
 *
 * 所有数均以本机字节序存储；除 u、v、w 使用 2.30 定点值外，其余使用
 * 16.16 定点值。
 *
 * 该变换按如下方式将源（变换前）帧中的点 (p, q) 映射到目标（变换后）帧中的
 * 点 (p', q')：
 *
 * @code{.unparsed}
 *               | a b u |
 *   (p, q, 1) . | c d v | = z * (p', q', 1)
 *               | x y w |
 * @endcode
 *
 * 也可以更明确地按分量写出该变换：
 *
 * @code{.unparsed}
 *   p' = (a * p + c * q + x) / z;
 *   q' = (b * p + d * q + y) / z;
 *   z  =  u * p + v * q + w
 * @endcode
 *
 * @{
 */

/**
 * 提取变换矩阵的旋转分量。
 *
 * @param matrix 变换矩阵
 * @return 变换使帧逆时针旋转的角度（单位为度）。角度范围为 [-180.0, 180.0]；
 *         如果矩阵是奇异矩阵，则返回 NaN。
 *
 * @note 浮点数本质上并不精确，因此建议调用者在使用前将返回值舍入到最接近的整数。
 */
double av_display_rotation_get(const int32_t matrix[9]);

/**
 * 初始化一个描述按指定角度（单位为度）纯顺时针旋转的变换矩阵。
 *
 * @param[out] matrix 变换矩阵（会被此函数完全覆盖）
 * @param angle 旋转角度，单位为度。
 */
void av_display_rotation_set(int32_t matrix[9], double angle);

/**
 * 水平和/或垂直翻转输入矩阵。
 *
 * @param[in,out] matrix 变换矩阵
 * @param hflip 是否水平翻转矩阵
 * @param vflip 是否垂直翻转矩阵
 */
void av_display_matrix_flip(int32_t matrix[9], int hflip, int vflip);

/**
 * @}
 */

#endif /* AVUTIL_DISPLAY_H */
