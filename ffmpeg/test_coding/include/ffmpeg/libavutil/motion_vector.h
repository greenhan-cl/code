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

#ifndef AVUTIL_MOTION_VECTOR_H
#define AVUTIL_MOTION_VECTOR_H

#include <stdint.h>

typedef struct AVMotionVector {
    /**
     * 当前宏块来自哪里；来自过去时为负值，来自未来时为正值。
     * XXX：设置精确的相对参考帧引用，而不是 +/- 1 的“方向”。
     */
    int32_t source;
    /**
     * 块的宽度和高度。
     */
    uint8_t w, h;
    /**
     * 绝对源位置，可以位于帧区域之外。
     */
    int16_t src_x, src_y;
    /**
     * 绝对目标位置，可以位于帧区域之外。
     */
    int16_t dst_x, dst_y;
    /**
     * 额外标志信息。
     * 当前未使用。
     */
    uint64_t flags;
    /**
     * 运动矢量
     * src_x = dst_x + motion_x / motion_scale
     * src_y = dst_y + motion_y / motion_scale
     */
    int32_t motion_x, motion_y;
    uint16_t motion_scale;
} AVMotionVector;

#endif /* AVUTIL_MOTION_VECTOR_H */
