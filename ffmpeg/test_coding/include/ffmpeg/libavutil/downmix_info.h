/*
 * Copyright (c) 2014 Tim Walker <tdskywalker@gmail.com>
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

#ifndef AVUTIL_DOWNMIX_INFO_H
#define AVUTIL_DOWNMIX_INFO_H

#include "avassert.h"
#include "frame.h"

/**
 * @file
 * 音频下混元数据
 */

/**
 * @addtogroup lavu_audio
 * @{
 */

/**
 * @defgroup downmix_info 音频下混元数据
 * @{
 */

/**
 * 可用的下混类型。
 */
enum AVDownmixType {
    AV_DOWNMIX_TYPE_UNKNOWN, /**< 未指明。 */
    AV_DOWNMIX_TYPE_LORO,    /**< Lo/Ro 双声道下混（立体声）。 */
    AV_DOWNMIX_TYPE_LTRT,    /**< Lt/Rt 双声道下混，兼容 Dolby Surround。 */
    AV_DOWNMIX_TYPE_DPLII,   /**< Lt/Rt 双声道下混，兼容 Dolby Pro Logic II。 */
    AV_DOWNMIX_TYPE_NB       /**< 下混类型数量。不属于 ABI。 */
};

/**
 * 此结构描述与下混过程有关的可选元数据。
 *
 * 解码器会将所有字段设置为音频比特流中指示的值（如果存在），否则设置为
 * “合理”的默认值。
 */
typedef struct AVDownmixInfo {
    /**
     * 母带工程师首选的下混类型。
     */
    enum AVDownmixType preferred_downmix_type;

    /**
     * 表示常规下混期间中置声道标称电平的绝对缩放因子。
     */
    double center_mix_level;

    /**
     * 表示 Lt/Rt 兼容下混期间中置声道标称电平的绝对缩放因子。
     */
    double center_mix_level_ltrt;

    /**
     * 表示常规下混期间环绕声道标称电平的绝对缩放因子。
     */
    double surround_mix_level;

    /**
     * 表示 Lt/Rt 兼容下混期间环绕声道标称电平的绝对缩放因子。
     */
    double surround_mix_level_ltrt;

    /**
     * 表示下混期间将 LFE 数据混入 L/R 声道时所用电平的绝对缩放因子。
     */
    double lfe_mix_level;
} AVDownmixInfo;

/**
 * 获取帧的 AV_FRAME_DATA_DOWNMIX_INFO 侧数据以供编辑。
 *
 * 如果侧数据不存在，则创建并添加到帧中。
 *
 * @param frame 要获取或创建侧数据的帧
 *
 * @return 供调用者编辑的 AVDownmixInfo 结构；无法分配该结构时返回 NULL。
 */
AVDownmixInfo *av_downmix_info_update_side_data(AVFrame *frame);

/**
 * 此结构以重混矩阵的形式描述与下混过程有关的可选元数据；矩阵被分配为
 * AVDownmixCoeff 数组。必须使用 @ref av_downmix_matrix_alloc 分配。
 *
 * sizeof(AVDownmixMatrix) 不属于 ABI，可以向其中添加新字段。
 */
typedef struct AVDownmixMatrix {
    /**
     * 系数将产生的下混类型。输出声道数量由此值推导。
     */
    enum AVDownmixType downmix_type;

    /**
     * 输入声道数量。
     */
    int in_ch_count;

    /**
     * 矩阵中的系数数量。
     */
    unsigned int nb_coeffs;

    /**
     * 系数数组起始处相对于此结构开头的字节偏移量。
     */
    size_t coeffs_offset;
} AVDownmixMatrix;

/**
 * 用于存储系数的数据类型。系数作为 AVDownmixMatrix 的一部分分配，应使用
 * @ref av_downmix_matrix_coeff 获取。
 */
typedef double AVDownmixCoeff;

/**
 * 获取表示输入声道 {@code in} 在输出声道 {@code out} 中权重的系数指针。
 * in 必须介于 0 和 @ref AVDownmixMatrix.in_ch_count "in_ch_count" - 1 之间。
 * out 必须介于 0 和由 @ref AVDownmixMatrix.downmix_type "downmix_type"
 * 隐含的输出声道数量 - 1 之间。
 */
static av_always_inline AVDownmixCoeff*
av_downmix_matrix_coeff(AVDownmixMatrix *dm, unsigned int out, unsigned int in)
{
    AVDownmixCoeff *coeff = (AVDownmixCoeff *)((uint8_t *)dm + dm->coeffs_offset);
    return &coeff[in + dm->in_ch_count * out];
}

/**
 * 为给定类型的 AVDownmixMatrix，以及包含 {@code in_ch_count} 乘以
 * {@code type} 隐含输出声道数量个 AVDownmixCoeff 的数组分配内存，并初始化
 * 变量。可以使用普通的 av_free() 调用释放。
 *
 * @param out_size 非 NULL 时，会在这里写入结果数据数组的字节大小。
 */
AVDownmixMatrix *av_downmix_matrix_alloc(enum AVDownmixType type,
                                         int in_ch_count, size_t *out_size);

/**
 * @}
 */

/**
 * @}
 */

#endif /* AVUTIL_DOWNMIX_INFO_H */
