/*
 * 复制right (c) 2021 Limin Wang <lance.lmwang at gmail.com>
 *
 * This file is part 的 FFmpeg.
 *
 * FFmpeg is 释放 software; you can redistribute it and/or
 * mod如果y it under the terms 的 the GNU Lesser General 公共
 * License as published by the 释放 Software Foundation; either
 * version 2.1 的 the License, 或 (at your 选项) any later version.
 *
 * FFmpeg is distributed 中 the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY 或 FITNESS FOR PARTICULAR PURPOSE.  参见 the GNU
 * Lesser General 公共 License 用于 more details.
 *
 * You should have received a 复制 的 the GNU Lesser General 公共
 * License along，使用 FFmpeg; 如果 not, write 到 the 释放 Software
 * Foundation, Inc., 51 Franklin Street, F如果th Floor, Boston, M02110-1301 USA
 */

#ifndef AVUTIL_HDR_DYNAMIC_VIVID_METADATA_H
#define AVUTIL_HDR_DYNAMIC_VIVID_METADATA_H

#include "frame.h"
#include "rational.h"

/**
 * HDR Vivid three spline params.
 */
typedef struct AVHDRVivid3SplineParams {
    /**
     * mode 的 three Spline. the 值 shall be 中 the range
     * 的 0 到 3, inclusive.
     */
    int th_mode;

    /**
     * three_Spline_TH_enable_MB is 中 the range 的 0.0 到 1.0, inclusive
     * 和 中 multiples 的 1.0/255.
     *
     */
    AVRational th_enable_mb;

    /**
     * 3Spline_TH_enable 的 three Spline.
     * 值 shall be 中 the range 的 0.0 到 1.0, inclusive.
     * 和 中 multiples 的 1.0/4095.
     */
    AVRational th_enable;

    /**
     * 3Spline_TH_Delta1 的 three Spline.
     * 值 shall be 中 the range 的 0.0 到 0.25, inclusive,
     * 和 中 multiples 的 0.25/1023.
     */
    AVRational th_delta1;

    /**
     * 3Spline_TH_Delta2 的 three Spline.
     * 值 shall be 中 the range 的 0.0 到 0.25, inclusive,
     * 和 中 multiples 的 0.25/1023.
     */
    AVRational th_delta2;

    /**
     * 3Spline_enable_Strength 的 three Spline.
     * 值 shall be 中 the range 的 0.0 到 1.0, inclusive,
     * 和 中 multiples 的 1.0/255.
     */
    AVRational enable_strength;
} AVHDRVivid3SplineParams;

/**
 * Color tone mapping parameters at a processing window 中 a dynamic 元数据 for
 * CUV005.1:2021.
 */
typedef struct AVHDRVividColorToneMappingParams {
    /**
     * nominal maximum display luminance 的 the targeted system display,
     * 中 multiples 的 1.0/4095 candelas per square metre. 值 shall be in
     * the range 的 0.0 到 1.0, inclusive.
     */
    AVRational targeted_system_display_maximum_luminance;

    /**
     * This 标志 indicates that transfer the base parameter(for 值 的 1)
     */
    int base_enable_flag;

    /**
     * base_param_m_p 中 the base parameter,
     * 中 multiples 的 1.0/16383. 值 shall be in
     * the range 的 0.0 到 1.0, inclusive.
     */
    AVRational base_param_m_p;

    /**
     * base_param_m_m 中 the base parameter,
     * 中 multiples 的 1.0/10. 值 shall be in
     * the range 的 0.0 到 6.3, inclusive.
     */
    AVRational base_param_m_m;

    /**
     * base_param_m_a 中 the base parameter,
     * 中 multiples 的 1.0/1023. 值 shall be in
     * the range 的 0.0 到 1.0 inclusive.
     */
    AVRational base_param_m_a;

    /**
     * base_param_m_b 中 the base parameter,
     * 中 multiples 的 1/1023. 值 shall be in
     * the range 的 0.0 到 1.0, inclusive.
     */
    AVRational base_param_m_b;

    /**
     * base_param_m_n 中 the base parameter,
     * 中 multiples 的 1.0/10. 值 shall be in
     * the range 的 0.0 到 6.3, inclusive.
     */
    AVRational base_param_m_n;

    /**
     * indicates k1_0 中 the base parameter,
     * base_param_k1 <= 1: k1_0 = base_param_k1
     * base_param_k1 > 1: reserved
     */
    int base_param_k1;

    /**
     * indicates k2_0 中 the base parameter,
     * base_param_k2 <= 1: k2_0 = base_param_k2
     * base_param_k2 > 1: reserved
     */
    int base_param_k2;

    /**
     * indicates k3_0 中 the base parameter,
     * base_param_k3 == 1: k3_0 = base_param_k3
     * base_param_k3 == 2: k3_0 = maximum_maxrgb
     * base_param_k3 > 2: reserved
     */
    int base_param_k3;

    /**
     * This 标志 indicates that delta mode 的 base parameter(for 值 的 1)
     */
    int base_param_Delta_enable_mode;

    /**
     * base_param_Delta 中 the base parameter,
     * 中 multiples 的 1.0/127. 值 shall be in
     * the range 的 0.0 到 1.0, inclusive.
     */
    AVRational base_param_Delta;

    /**
     * indicates 3Spline_enable_标志 中 the base parameter,
     * This 标志 indicates that transfer three Spline 的 base parameter(for 值 的 1)
     */
    int three_Spline_enable_flag;

    /**
     * 数量 的 three Spline. 值 shall be 中 the range
     * 的 1 到 2, inclusive.
     */
    int three_Spline_num;

    AVHDRVivid3SplineParams three_spline[2];
} AVHDRVividColorToneMappingParams;


/**
 * Color transform parameters at a processing window 中 a dynamic 元数据 for
 * CUV005.1:2021.
 */
typedef struct AVHDRVividColorTransformParams {
    /**
     * Indicates the minimum brightness 的 the displayed content.
     * 值 should be 中 the range 的 0.0 到 1.0,
     * inclusive 和 中 multiples 的 1/4095.
     */
    AVRational minimum_maxrgb;

    /**
     * Indicates the average brightness 的 the displayed content.
     * 值 should be 中 the range 的 0.0 到 1.0,
     * inclusive 和 中 multiples 的 1/4095.
     */
    AVRational average_maxrgb;

    /**
     * Indicates the variance brightness 的 the displayed content.
     * 值 should be 中 the range 的 0.0 到 1.0,
     * inclusive 和 中 multiples 的 1/4095.
     */
    AVRational variance_maxrgb;

    /**
     * Indicates the maximum brightness 的 the displayed content.
     * 值 should be 中 the range 的 0.0 到 1.0, inclusive
     * 和 中 multiples 的 1/4095.
     */
    AVRational maximum_maxrgb;

    /**
     * This 标志 indicates that the 元数据 用于 the tone mapping function in
     * the processing window is present (for 值 的 1).
     */
    int tone_mapping_mode_flag;

    /**
     * 数量 的 tone mapping param. 值 shall be 中 the range
     * 的 1 到 2, inclusive.
     */
    int tone_mapping_param_num;

    /**
     * color tone mapping parameters.
     */
    AVHDRVividColorToneMappingParams tm_params[2];

    /**
     * This 标志 indicates that the 元数据 用于 the color saturation mapping in
     * the processing window is present (for 值 的 1).
     */
    int color_saturation_mapping_flag;

    /**
     * 数量 的 color saturation param. 值 shall be 中 the range
     * 的 0 到 7, inclusive.
     */
    int color_saturation_num;

    /**
     * Indicates the color correction strength parameter.
     * 值 should be 中 the range 的 0.0 到 2.0, inclusive
     * 和 中 multiples 的 1/128.
     */
    AVRational color_saturation_gain[8];
} AVHDRVividColorTransformParams;

/**
 * This struct represents dynamic 元数据 用于 color volume transform -
 * CUV005.1:2021 standard
 *
 * To be used as payload 的 a AV帧SideData 或 AVPacketSideData，使用 the
 * appropriate type.
 *
 * @note struct should be 分配d with
 * av_dynamic_hdr_vivid_alloc() 和 its 大小 is not a part of
 * the 公共 ABI.
 */
typedef struct AVDynamicHDRVivid {
    /**
     * system start code. 值 shall be 设置 到 0x01.
     */
    uint8_t system_start_code;

    /**
     * 数量 的 processing windows. 值 shall be 设置 到 0x01
     * 如果 the system_start_code is 0x01.
     */
    uint8_t num_windows;

    /**
     * color transform parameters 用于 every processing window.
     */
    AVHDRVividColorTransformParams params[3];
} AVDynamicHDRVivid;

/**
 * 分配 an AVDynamicHDRVivid 结构体 和 设置 its fields to
 * 默认 值. resulting struct can be 释放d using av_释放p().
 *
 * @返回 AVDynamicHDRVivid filled，使用 默认 值 或 NULL
 *         上 failure.
 */
AVDynamicHDRVivid *av_dynamic_hdr_vivid_alloc(size_t *size);

/**
 * 分配 a complete AVDynamicHDRVivid 和 add it 到 the 帧.
 * @param 帧 帧 which side data is added to.
 *
 * @返回 AVDynamicHDRVivid 结构体 到 be filled by caller 或 NULL
 *         上 failure.
 */
AVDynamicHDRVivid *av_dynamic_hdr_vivid_create_side_data(AVFrame *frame);

#endif /* AVUTIL_HDR_DYNAMIC_VIVID_METADATA_H */
