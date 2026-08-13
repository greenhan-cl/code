/*
 * 复制right (c) 2018 Mohammad Izadi <moh.izadi at gmail.com>
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

#ifndef AVUTIL_HDR_DYNAMIC_METADATA_H
#define AVUTIL_HDR_DYNAMIC_METADATA_H

#include "frame.h"
#include "rational.h"

/**
 * 选项 用于 overlapping elliptical 像素 selectors 中 an image.
 */
enum AVHDRPlusOverlapProcessOption {
    AV_HDR_PLUS_OVERLAP_PROCESS_WEIGHTED_AVERAGING = 0,
    AV_HDR_PLUS_OVERLAP_PROCESS_LAYERING = 1,
};

/**
 * Represents the percentile at a spec如果ic percentage in
 * a distribution.
 */
typedef struct AVHDRPlusPercentile {
    /**
     * percentage 值 corresponding 到 a spec如果ic percentile linearized
     * RGB 值 中 the processing window 中 the scene. 值 shall be in
     * the range 的 0 to100, inclusive.
     */
    uint8_t percentage;

    /**
     * linearized maxRGB 值 at a spec如果ic percentile 中 the processing
     * window 中 the scene. 值 shall be 中 the range 的 0 到 1, inclusive
     * 和 中 multiples 的 0.00001.
     */
    AVRational percentile;
} AVHDRPlusPercentile;

/**
 * Color transform parameters at a processing window 中 a dynamic 元数据 for
 * SMPTE 2094-40.
 */
typedef struct AVHDRPlusColorTransformParams {
    /**
     * relative x coordinate 的 the top left 像素 的 the processing
     * window. 值 shall be 中 the range 的 0 和 1, inclusive and
     * 中 multiples 的 1/(宽度 的 Picture - 1). 值 1 corresponds
     * 到 the absolute coordinate 的 宽度 的 Picture - 1. 值 for
     * first processing window shall be 0.
     */
    AVRational window_upper_left_corner_x;

    /**
     * relative y coordinate 的 the top left 像素 的 the processing
     * window. 值 shall be 中 the range 的 0 和 1, inclusive and
     * 中 multiples 的 1/(高度 的 Picture - 1). 值 1 corresponds
     * 到 the absolute coordinate 的 高度 的 Picture - 1. 值 for
     * first processing window shall be 0.
     */
    AVRational window_upper_left_corner_y;

    /**
     * relative x coordinate 的 the bottom right 像素 的 the processing
     * window. 值 shall be 中 the range 的 0 和 1, inclusive and
     * 中 multiples 的 1/(宽度 的 Picture - 1). 值 1 corresponds
     * 到 the absolute coordinate 的 宽度 的 Picture - 1. 值 for
     * first processing window shall be 1.
     */
    AVRational window_lower_right_corner_x;

    /**
     * relative y coordinate 的 the bottom right 像素 的 the processing
     * window. 值 shall be 中 the range 的 0 和 1, inclusive and
     * 中 multiples 的 1/(高度 的 Picture - 1). 值 1 corresponds
     * 到 the absolute coordinate 的 高度 的 Picture - 1. 值 for
     * first processing window shall be 1.
     */
    AVRational window_lower_right_corner_y;

    /**
     * x coordinate 的 the center position 的 the concentric internal and
     * external ellipses 的 the elliptical 像素 selector 中 the processing
     * window. 值 shall be 中 the range 的 0 到 (宽度 的 Picture - 1),
     * inclusive 和 中 multiples 的 1 像素.
     */
    uint16_t center_of_ellipse_x;

    /**
     * y coordinate 的 the center position 的 the concentric internal and
     * external ellipses 的 the elliptical 像素 selector 中 the processing
     * window. 值 shall be 中 the range 的 0 到 (高度 的 Picture - 1),
     * inclusive 和 中 multiples 的 1 像素.
     */
    uint16_t center_of_ellipse_y;

    /**
     * clockwise rotation angle 中 degree 的 arc，使用 respect 到 the
     * positive direction 的 the x-axis 的 the concentric internal 和 external
     * ellipses 的 the elliptical 像素 selector 中 the processing window. The
     * 值 shall be 中 the range 的 0 到 180, inclusive 和 中 multiples 的 1.
     */
    uint8_t rotation_angle;

    /**
     * semi-major axis 值 的 the internal ellipse 的 the elliptical 像素
     * selector 中 amount 的 像素s 中 the processing window. 值 shall be
     * 中 the range 的 1 到 65535, inclusive 和 中 multiples 的 1 像素.
     */
    uint16_t semimajor_axis_internal_ellipse;

    /**
     * semi-major axis 值 的 the external ellipse 的 the elliptical 像素
     * selector 中 amount 的 像素s 中 the processing window. 值
     * shall not be less than semimajor_axis_internal_ellipse 的 the current
     * processing window. 值 shall be 中 the range 的 1 到 65535,
     * inclusive 和 中 multiples 的 1 像素.
     */
    uint16_t semimajor_axis_external_ellipse;

    /**
     * semi-minor axis 值 的 the external ellipse 的 the elliptical 像素
     * selector 中 amount 的 像素s 中 the processing window. 值 shall be
     * 中 the range 的 1 到 65535, inclusive 和 中 multiples 的 1 像素.
     */
    uint16_t semiminor_axis_external_ellipse;

    /**
     * Overlap process 选项 indicates one 的 the two methods 的 combining
     * rendered 像素s 中 the processing window 中 an image，使用 at least one
     * elliptical 像素 selector. For overlapping elliptical 像素 selectors
     * 中 an image, overlap_process_选项 shall have the same 值.
     */
    enum AVHDRPlusOverlapProcessOption overlap_process_option;

    /**
     * maximum 的 the color components 的 linearized RGB 值 中 the
     * processing window 中 the scene. 值 should be 中 the range 的 0 to
     * 1, inclusive 和 中 multiples 的 0.00001. maxscl[ 0 ], maxscl[ 1 ], and
     * maxscl[ 2 ] are corresponding 到 R, G, B color components respectively.
     */
    AVRational maxscl[3];

    /**
     * average 的 linearized maxRGB 值 中 the processing window 中 the
     * scene. 值 should be 中 the range 的 0 到 1, inclusive 和 in
     * multiples 的 0.00001.
     */
    AVRational average_maxrgb;

    /**
     * 数量 的 linearized maxRGB 值 at given percentiles 中 the
     * processing window 中 the scene. maximum 值 shall be 15.
     */
    uint8_t num_distribution_maxrgb_percentiles;

    /**
     * linearized maxRGB 值 at given percentiles 中 the
     * processing window 中 the scene.
     */
    AVHDRPlusPercentile distribution_maxrgb[15];

    /**
     * fraction 的 selected 像素s 中 the image that 包含 brightest
     * 像素 中 the scene. 值 shall be 中 the range 的 0 到 1, inclusive
     * 和 中 multiples 的 0.001.
     */
    AVRational fraction_bright_pixels;

    /**
     * This 标志 indicates that the 元数据 用于 the tone mapping function in
     * the processing window is present (for 值 的 1).
     */
    uint8_t tone_mapping_flag;

    /**
     * x coordinate 的 the separation point between the linear part 和 the
     * curved part 的 the tone mapping function. 值 shall be 中 the range
     * 的 0 到 1, excluding 0 和 中 multiples 的 1/4095.
     */
    AVRational knee_point_x;

    /**
     * y coordinate 的 the separation point between the linear part 和 the
     * curved part 的 the tone mapping function. 值 shall be 中 the range
     * 的 0 到 1, excluding 0 和 中 multiples 的 1/4095.
     */
    AVRational knee_point_y;

    /**
     * 数量 的 the intermediate anchor parameters 的 the tone mapping
     * function 中 the processing window. maximum 值 shall be 15.
     */
    uint8_t num_bezier_curve_anchors;

    /**
     * intermediate anchor parameters 的 the tone mapping function 中 the
     * processing window 中 the scene. 值 should be 中 the range 的 0
     * 到 1, inclusive 和 中 multiples 的 1/1023.
     */
    AVRational bezier_curve_anchors[15];

    /**
     * This 标志 shall be equal 到 0 中 bitstreams conforming 到 this version of
     * this Spec如果ication. Other 值 are reserved 用于 future use.
     */
    uint8_t color_saturation_mapping_flag;

    /**
     * color saturation gain 中 the processing window 中 the scene. The
     * 值 shall be 中 the range 的 0 到 63/8, inclusive 和 中 multiples of
     * 1/8. 默认 值 shall be 1.
     */
    AVRational color_saturation_weight;
} AVHDRPlusColorTransformParams;

/**
 * This struct represents dynamic 元数据 用于 color volume transform -
 * application 4 的 SMPTE 2094-40:2016 standard.
 *
 * To be used as payload 的 a AV帧SideData 或 AVPacketSideData，使用 the
 * appropriate type.
 *
 * @note struct should be 分配d with
 * av_dynamic_hdr_plus_alloc() 和 its 大小 is not a part of
 * the 公共 ABI.
 */
typedef struct AVDynamicHDRPlus {
    /**
     * Country code by Rec. ITU-T T.35 Annex A. 值 shall be 0xB5.
     */
    uint8_t itu_t_t35_country_code;

    /**
     * Application version 中 the application defining document 中 ST-2094
     * suite. 值 shall be 设置 到 0.
     */
    uint8_t application_version;

    /**
     * 数量 的 processing windows. 值 shall be 中 the range
     * 的 1 到 3, inclusive.
     */
    uint8_t num_windows;

    /**
     * color transform parameters 用于 every processing window.
     */
    AVHDRPlusColorTransformParams params[3];

    /**
     * nominal maximum display luminance 的 the targeted system display,
     * 中 units 的 0.0001 candelas per square metre. 值 shall be in
     * the range 的 0 到 10000, inclusive.
     */
    AVRational targeted_system_display_maximum_luminance;

    /**
     * This 标志 shall be equal 到 0 中 bit streams conforming 到 this version
     * 的 this Spec如果ication. 值 1 is reserved 用于 future use.
     */
    uint8_t targeted_system_display_actual_peak_luminance_flag;

    /**
     * 数量 的 rows 中 the targeted system_display_actual_peak_luminance
     * 数组. 值 shall be 中 the range 的 2 到 25, inclusive.
     */
    uint8_t num_rows_targeted_system_display_actual_peak_luminance;

    /**
     * 数量 的 columns 中 the
     * targeted_system_display_actual_peak_luminance 数组. 值 shall be
     * 中 the range 的 2 到 25, inclusive.
     */
    uint8_t num_cols_targeted_system_display_actual_peak_luminance;

    /**
     * normalized actual peak luminance 的 the targeted system display. The
     * 值 should be 中 the range 的 0 到 1, inclusive 和 中 multiples of
     * 1/15.
     */
    AVRational targeted_system_display_actual_peak_luminance[25][25];

    /**
     * This 标志 shall be equal 到 0 中 bitstreams conforming 到 this version of
     * this Spec如果ication. 值 1 is reserved 用于 future use.
     */
    uint8_t mastering_display_actual_peak_luminance_flag;

    /**
     * 数量 的 rows 中 the mastering_display_actual_peak_luminance 数组.
     * 值 shall be 中 the range 的 2 到 25, inclusive.
     */
    uint8_t num_rows_mastering_display_actual_peak_luminance;

    /**
     * 数量 的 columns 中 the mastering_display_actual_peak_luminance
     * 数组. 值 shall be 中 the range 的 2 到 25, inclusive.
     */
    uint8_t num_cols_mastering_display_actual_peak_luminance;

    /**
     * normalized actual peak luminance 的 the mastering display 用于
     * mastering the image essence. 值 should be 中 the range 的 0 到 1,
     * inclusive 和 中 multiples 的 1/15.
     */
    AVRational mastering_display_actual_peak_luminance[25][25];
} AVDynamicHDRPlus;

/**
 * 分配 an AVDynamicHDRPlus 结构体 和 设置 its fields to
 * 默认 值. resulting struct can be 释放d using av_释放p().
 *
 * @返回 AVDynamicHDRPlus filled，使用 默认 值 或 NULL
 *         上 failure.
 */
AVDynamicHDRPlus *av_dynamic_hdr_plus_alloc(size_t *size);

/**
 * 分配 a complete AVDynamicHDRPlus 和 add it 到 the 帧.
 * @param 帧 帧 which side data is added to.
 *
 * @返回 AVDynamicHDRPlus 结构体 到 be filled by caller 或 NULL
 *         上 failure.
 */
AVDynamicHDRPlus *av_dynamic_hdr_plus_create_side_data(AVFrame *frame);

/**
 * 解析 the user data registered ITU-T T.35 到 AV缓冲区 (AVDynamicHDRPlus).
 * T.35 缓冲区 must begin，使用 the application mode, skipping the
 * country code, terminal provider codes, 和 application ident如果ier.
 * @param s 指针 containing the decoded AVDynamicHDRPlus 结构体.
 * @param data byte 数组 containing the raw ITU-T T.35 data.
 * @param 大小 大小 的 the data 数组 中 bytes.
 *
 * @返回 >= 0 上 success. Otherwise, 返回s the appropriate AVERROR.
 */
int av_dynamic_hdr_plus_from_t35(AVDynamicHDRPlus *s, const uint8_t *data,
                                 size_t size);

#define AV_HDR_PLUS_MAX_PAYLOAD_SIZE 907

/**
 * Serialize dynamic HDR10+ 元数据 到 a user data registered ITU-T T.35 缓冲区,
 * excluding the first 48 bytes 的 the header, 和 beginning，使用 the application mode.
 * @param s 指针 containing the decoded AVDynamicHDRPlus 结构体.
 * @param[in,out] data 指针 到 指针 到 a byte 缓冲区 到 be filled，使用 the
 *                     serialized 元数据.
 *                     如果 *data is NULL, a 缓冲区 be will be 分配d 和 a 指针 to
 *                     it stored 中 its place. caller assumes ownership 的 the 缓冲区.
 *                     May be NULL, 中 which case the function will only store the
 *                     required 缓冲区 大小 中 *大小.
 * @param[in,out] 大小 指针 到 a 大小 到 be 设置 到 the 返回ed 缓冲区's 大小.
 *                     如果 *data is not NULL, *大小 must contain the 大小 的 the 输入
 *                     缓冲区. May be NULL only 如果 *data is NULL.
 *
 * @返回 >= 0 上 success. Otherwise, 返回s the appropriate AVERROR.
 */
int av_dynamic_hdr_plus_to_t35(const AVDynamicHDRPlus *s, uint8_t **data, size_t *size);

/**
 * This struct represents dynamic 元数据 用于 color volume transform as
 * spec如果ied 中 the SMPTE 2094-50 standard.
 *
 * To be used as payload 的 a AV帧SideData 或 AVPacketSideData，使用 the
 * appropriate type.
 *
 * @note struct should be 分配d with
 * av_dynamic_smpte2094_app5_alloc() 和 its 大小 is not a part of
 * the 公共 ABI.
 */
typedef struct AVDynamicHDRSmpte2094App5 {
    /**
     * Section C.2.1. smpte_st_2094_50_application_info()
     */
    uint8_t application_version;
    uint8_t minimum_application_version;

    /**
     * Section C.2.2 smpte_st_2094_50_color_volume_transform()
     */
    uint8_t has_custom_hdr_reference_white_flag;
    uint8_t has_adaptive_tone_map_flag;
    uint16_t hdr_reference_white;

    /**
     * Section C.2.3 smpte_st_2094_50_adaptive_tone_map()
     */
    uint16_t baseline_hdr_headroom;
    uint8_t use_reference_white_tone_mapping_flag;
    uint8_t num_alternate_images;
    uint8_t gain_application_space_chromaticities_flag;
    uint8_t has_common_component_mix_params_flag;
    uint8_t has_common_curve_params_flag;
    uint16_t gain_application_space_chromaticities[8];
    uint16_t alternate_hdr_headrooms[4];

    /**
     * Section C.2.4 smpte_st_2094_50_component_mixing()
     */
    uint8_t component_mixing_type[4];
    uint8_t has_component_mixing_coefficient_flag[4][6];
    uint16_t component_mixing_coefficient[4][6];

    /**
     * Section C.2.5 smpte_st_2094_50_gain_curve()
     */
    uint8_t gain_curve_num_control_points_minus_1[4];
    uint8_t gain_curve_use_pchip_slope_flag[4];
    uint16_t gain_curve_control_points_x[4][32];
    uint16_t gain_curve_control_points_y[4][32];
    uint16_t gain_curve_control_points_theta[4][32];
} AVDynamicHDRSmpte2094App5;

/**
 * 分配 an AVDynamicHDRSmpte2094App5 结构体 和 设置 its fields to
 * 默认 值. resulting struct can be 释放d using av_释放p().
 *
 * @返回 AVDynamicHDRSmpte2094App5 filled，使用 默认 值 或 NULL
 *         上 failure.
 */
AVDynamicHDRSmpte2094App5* av_dynamic_hdr_smpte2094_app5_alloc(size_t* size);

/**
 * 分配 a complete AVDynamicHDRSmpte2094App5 和 add it 到 the 帧.
 *
 * @param 帧 帧 which side data is added to.
 *
 * @返回 AVDynamicHDRSmpte2094App5 结构体 到 be filled by caller or
 *         NULL 上 failure.
 */
AVDynamicHDRSmpte2094App5* av_dynamic_hdr_smpte2094_app5_create_side_data(AVFrame* frame);

/**
 * 解析 the user data 格式ted as ITU-T T.35 message 到 AVDynamicHDRSmpte2094App5.
 *
 * @param s 指针 containing the decoded AVDynamicHDRSmpte2094App5 结构体.
 * @param data byte 数组 containing the raw ITU-T T.35 data.
 * @param 大小 大小 的 the data 数组 中 bytes.
 *
 * @返回 >= 0 上 success. Otherwise, 返回s the appropriate AVERROR.
 */
int av_dynamic_hdr_smpte2094_app5_from_t35(AVDynamicHDRSmpte2094App5* s, const uint8_t* data,
                                           size_t size);

/**
 * Serialize dynamic SMPTE-2094-50 元数据 到 a ITU-T T.35 message. Excluding
 * the country_code, provider_code 和 provider_oriented_code.
 *
 * @param s 指针 containing the AVDynamicHDRSmpte2094App5 data.
 * @param[in,out] data 指针 到 指针 到 a byte 缓冲区 到 be filled with
 * the serialized 元数据. 如果 *data is NULL, a 缓冲区 be will be 分配d and
 * a 指针 到 it stored 中 its place. caller assumes ownership 的 the
 * 缓冲区. May be NULL, 中 which case the function will only store the required
 * 缓冲区 大小 中 *大小.
 * @param[in,out] 大小 指针 到 a 大小 到 be 设置 到 the 返回ed 缓冲区's
 * 大小. 如果 *data is not NULL, *大小 must contain the 大小 的 the 输入 缓冲区.
 * May be NULL only 如果 *data is NULL.
 *
 * @返回 >= 0 上 success. Otherwise, 返回s the appropriate AVERROR.
 */
int av_dynamic_hdr_smpte2094_app5_to_t35(const AVDynamicHDRSmpte2094App5* s, uint8_t** data,
                                         size_t* size);

#endif /* AVUTIL_HDR_DYNAMIC_METADATA_H */
