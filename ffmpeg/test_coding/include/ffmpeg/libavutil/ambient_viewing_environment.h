/*
 * Copyright (c) 2023 Jan Ekström <jeebjp@gmail.com>
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

#ifndef AVUTIL_AMBIENT_VIEWING_ENVIRONMENT_H
#define AVUTIL_AMBIENT_VIEWING_ENVIRONMENT_H

#include <stddef.h>
#include "frame.h"
#include "rational.h"

/**
 * H.274 定义的环境观看条件元数据。这些值保存在 AVRational 中，以保持精确性，
 * 同时可通过例如 av_q2d 方便地取得 double 值。
 *
 * @note sizeof(AVAmbientViewingEnvironment) 不属于公共 ABI，必须使用
 *       av_ambient_viewing_environment_alloc 分配该结构。
 */
typedef struct AVAmbientViewingEnvironment {
    /**
     * 环境观看条件的环境照度，单位为 lux。
     */
    AVRational ambient_illuminance;

    /**
     * 按 ISO/CIE 11664-1 规定的 CIE 1931 x、y 定义，标称观看环境中环境光的
     * 归一化 x 色度坐标。
     */
    AVRational ambient_light_x;

    /**
     * 按 ISO/CIE 11664-1 规定的 CIE 1931 x、y 定义，标称观看环境中环境光的
     * 归一化 y 色度坐标。
     */
    AVRational ambient_light_y;
} AVAmbientViewingEnvironment;

/**
 * 分配 AVAmbientViewingEnvironment 结构。
 *
 * @return 新分配的结构；失败时返回 NULL
 */
AVAmbientViewingEnvironment *av_ambient_viewing_environment_alloc(size_t *size);

/**
 * 分配 AVAmbientViewingEnvironment 结构，并将其作为侧数据添加到现有 AVFrame。
 *
 * @return 新分配的结构；失败时返回 NULL
 */
AVAmbientViewingEnvironment *av_ambient_viewing_environment_create_side_data(AVFrame *frame);

#endif /* AVUTIL_AMBIENT_VIEWING_ENVIRONMENT_H */
