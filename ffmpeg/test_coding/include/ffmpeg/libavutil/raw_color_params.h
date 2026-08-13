/*
 * Copyright (c) 2026 Lynne <dev@lynne.ee>
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

#ifndef AVUTIL_RAW_COLOR_PARAMS_H
#define AVUTIL_RAW_COLOR_PARAMS_H

#include <stddef.h>
#include <stdint.h>

#include "frame.h"
#include "rational.h"

enum AVRawColorParamsType {
    AV_RAW_COLOR_PARAMS_NONE = 0,

    /**
     * 将联合体解释为 AVProResRawColorParams（codec.prores_raw）时有效。
     */
    AV_RAW_COLOR_PARAMS_PRORES_RAW,
};

/**
 * 从 prrf 帧头解析的 ProRes RAW 逐帧色彩变换。
 *
 * 正确的渲染流水线为：
 *  -> (sample - black_level) / (white_level - black_level)
 *  -> per-channel white balance (wb_red, 1.0 for G, wb_blue) pre-debayer
 *  -> debayer
 *  -> color_matrix (camera RGB -> CIE 1931 XYZ relative to D65)
 *  -> gain (scene-linear scale)
 *
 * 黑/白电平位于外层 AVRawColorParams。矩阵输出为线性光 CIE XYZ D65，
 * 应在下游转换为工作 RGB 空间。
 *
 * @note 此结构体必须作为 AVRawColorParams 的一部分使用
 *       av_raw_color_params_alloc() 分配。其大小不属于公共 ABI。
 */
typedef struct AVProResRawColorParams {
    /**
     * 红色通道的白平衡乘数，在去马赛克前应用。
     */
    AVRational wb_red;

    /**
     * 蓝色通道的白平衡乘数，在去马赛克前应用。
     * （ProRes RAW 的绿色通道隐含为 1.0）
     */
    AVRational wb_blue;

    /**
     * 从相机 RGB 到相对于 D65 光源的线性光 CIE 1931 XYZ 的 3x3 行主序色彩矩阵，
     * 在去马赛克后应用。
     * out[i] = sum_j color_matrix[i][j] * in[j].
     */
    AVRational color_matrix[3][3];

    /**
     * 矩阵后的场景线性缩放因子。编码器以此编码保留的高光余量；
     * 将矩阵变换后的值乘以此因子可恢复场景线性光。
     */
    AVRational gain;
} AVProResRawColorParams;

/**
 * RAW 相机编解码器的逐帧色彩信息，以 AV_FRAME_DATA_RAW_COLOR_PARAMS 类型侧数据携带。
 *
 * 外层结构体包含每种 RAW 编解码器都公开的字段：传感器有效采样范围和白平衡相关色温。
 * codec 联合体保存编解码器专用变换参数；`type` 选择联合体中的有效成员。
 *
 * 编解码器专用变换（color_matrix 或等效变换）始终输出相对于 D65 光源的线性光
 * CIE 1931 XYZ。相机没有标准基色，因此 XYZ 是唯一通用目标。
 *
 * @note 必须使用 av_raw_color_params_alloc() 或
 *       av_raw_color_params_create_side_data() 分配此结构体。其大小不属于公共 ABI。
 */
typedef struct AVRawColorParams {
    /**
     * 选择 `codec` 中的有效成员。
     */
    enum AVRawColorParamsType type;

    /**
     * 最低有效原始采样码（传感器黑点）
     */
    AVRational black_level;

    /**
     * 最高有效原始采样码（传感器白点）
     */
    AVRational white_level;

    /**
     * 相机白平衡的色温，单位为开尔文。仅供参考；计算使用编解码器专用白平衡字段。
     * 未指示时为 0。
     */
    uint32_t wb_cct;

    /**
     * 其他编解码器专用字段。
     */
    union {
        AVProResRawColorParams prores_raw;
    } codec;
} AVRawColorParams;

/**
 * 分配 AVRawColorParams 结构体并将其清零初始化。
 *
 * @param size 非 NULL 时，设为 sizeof(AVRawColorParams)
 * @return 新分配的结构体，失败时返回 NULL
 */
AVRawColorParams *av_raw_color_params_alloc(size_t *size);

/**
 * 分配 AVRawColorParams 结构体，并作为 AV_FRAME_DATA_RAW_COLOR_PARAMS 侧数据
 * 添加到现有 AVFrame。
 *
 * @return 新分配的结构体，失败时返回 NULL
 */
AVRawColorParams *av_raw_color_params_create_side_data(AVFrame *frame);

#endif /* AVUTIL_RAW_COLOR_PARAMS_H */
