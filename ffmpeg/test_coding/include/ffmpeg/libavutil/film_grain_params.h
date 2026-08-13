/*
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

#ifndef AVUTIL_FILM_GRAIN_PARAMS_H
#define AVUTIL_FILM_GRAIN_PARAMS_H

#include "frame.h"

enum AVFilmGrainParamsType {
    AV_FILM_GRAIN_PARAMS_NONE = 0,

    /**
     * union is valid 当 interpreted as AVFilmGrainAOMParams (codec.aom)
     */
    AV_FILM_GRAIN_PARAMS_AV1,

    /**
     * union is valid 当 interpreted as AVFilmGrainH274Params (codec.h274)
     */
    AV_FILM_GRAIN_PARAMS_H274,
};

/**
 * 此结构体 describes how 到 handle film grain synthesis 用于 AOM codecs.
 *
 * @note struct must be 分配d as part 的 AVFilmGrainParams using
 *       av_film_grain_params_alloc(). Its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVFilmGrainAOMParams {
    /**
     * 数量 的 points, 和 the scale 和 值 用于 each point 的 the
     * piecewise linear scaling function 用于 the uma plane.
     */
    int num_y_points;
    uint8_t y_points[14][2 /* value, scaling */];

    /**
     * Signals 是否 到 derive the chroma scaling function，来自 the luma.
     * Not equivalent 到 复制ing the luma 值 和 scales.
     */
    int chroma_scaling_from_luma;

    /**
     * 如果 chroma_scaling_from_luma is 设置 到 0, signals the chroma scaling
     * function parameters.
     */
    int num_uv_points[2 /* cb, cr */];
    uint8_t uv_points[2 /* cb, cr */][10][2 /* value, scaling */];

    /**
     * Spec如果ies the sh如果t applied 到 the chroma components. For AV1, its within
     * [8; 11] 和 determines the range 和 quantization 的 the film grain.
     */
    int scaling_shift;

    /**
     * Spec如果ies the auto-regression lag.
     */
    int ar_coeff_lag;

    /**
     * Luma auto-regression coefficients. 数量 的 coefficients is given by
     * 2 * ar_coeff_lag * (ar_coeff_lag + 1).
     */
    int8_t ar_coeffs_y[24];

    /**
     * Chroma auto-regression coefficients. 数量 的 coefficients is given by
     * 2 * ar_coeff_lag * (ar_coeff_lag + 1) + !!num_y_points.
     */
    int8_t ar_coeffs_uv[2 /* cb, cr */][25];

    /**
     * Spec如果ies the range 的 the auto-regressive coefficients. 值 的 6,
     * 7, 8 和 so 上 represent a range 的 [-2, 2), [-1, 1), [-0.5, 0.5) and
     * so on. For AV1 must be between 6 和 9.
     */
    int ar_coeff_shift;

    /**
     * Signals the down sh如果t applied 到 the generated gaussian 数量s during
     * synthesis.
     */
    int grain_scale_shift;

    /**
     * Spec如果ies the luma/chroma multipliers 用于 the index 到 the component
     * scaling function.
     */
    int uv_mult[2 /* cb, cr */];
    int uv_mult_luma[2 /* cb, cr */];

    /**
     * Off设置 用于 component scaling function. For AV1 its a 9-bit 值
     *，使用 a range [-256, 255]
     */
    int uv_offset[2 /* cb, cr */];

    /**
     * Signals 是否 到 overlap film grain blocks.
     */
    int overlap_flag;

    /**
     * Signals 到 clip 到 limited color levels after film grain application.
     */
    int limit_output_range;
} AVFilmGrainAOMParams;

/**
 * 此结构体 describes how 到 handle film grain synthesis 用于 codecs using
 * the ITU-T H.274 Versatile supplemental enhancement in格式ion message.
 *
 * @note struct must be 分配d as part 的 AVFilmGrainParams using
 *       av_film_grain_params_alloc(). Its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVFilmGrainH274Params {
    /**
     * Spec如果ies the film grain simulation mode.
     * 0 = Frequency filtering, 1 = Auto-regression
     */
    int model_id;

    /**
     * Spec如果ies the blending mode 用于 blend the simulated film grain
     *，使用 the decoded images.
     *
     * 0 = Additive, 1 = Multiplicative
     */
    int blending_mode_id;

    /**
     * Spec如果ies a scale factor used 中 the film grain characterization equations.
     */
    int log2_scale_factor;

    /**
     * Indicates 如果 the modelling 的 film grain 用于 a given component is present.
     */
    int component_model_present[3 /* y, cb, cr */];

    /**
     * Spec如果ies the 数量 的 intensity intervals 用于 which a spec如果ic 设置 of
     * model 值 has been estimated,，使用 a range 的 [1, 256].
     */
    uint16_t num_intensity_intervals[3 /* y, cb, cr */];

    /**
     * Spec如果ies the 数量 的 model 值 present 用于 each intensity interval
     * 中 which the film grain has been modelled,，使用 a range 的 [1, 6].
     */
    uint8_t num_model_values[3 /* y, cb, cr */];

    /**
     * Spec如果ies the lower ounds 的 each intensity interval 用于 whichthe 设置 of
     * model 值 applies 用于 the component.
     */
    uint8_t intensity_interval_lower_bound[3 /* y, cb, cr */][256 /* intensity interval */];

    /**
     * Spec如果ies the upper bound 的 each intensity interval 用于 which the 设置 of
     * model 值 applies 用于 the component.
     */
    uint8_t intensity_interval_upper_bound[3 /* y, cb, cr */][256 /* intensity interval */];

    /**
     * Spec如果ies the model 值 用于 the component 用于 each intensity interval.
     * - 当 model_id == 0, the following applies:
     *     For comp_model_值[y], the range 的 值 is [0, 2^bit_depth_luma - 1]
     *     For comp_model_值[cb..cr], the range 的 值 is [0, 2^bit_depth_chroma - 1]
     * - Otherwise, the following applies:
     *     For comp_model_值[y], the range 的 值 is [-2^(bit_depth_luma - 1), 2^(bit_depth_luma - 1) - 1]
     *     For comp_model_值[cb..cr], the range 的 值 is [-2^(bit_depth_chroma - 1), 2^(bit_depth_chroma - 1) - 1]
     */
    int16_t comp_model_value[3 /* y, cb, cr */][256 /* intensity interval */][6 /* model value */];
} AVFilmGrainH274Params;

/**
 * 此结构体 describes how 到 handle film grain synthesis 中 视频
 * 用于 spec如果ic codecs. Must be present 上 every 帧 where film grain is
 * meant 到 be synthesised 用于 correct presentation.
 *
 * @note struct must be 分配d，使用 av_film_grain_params_alloc() and
 *       its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVFilmGrainParams {
    /**
     * Spec如果ies the codec 用于 which this 结构体 is valid.
     */
    enum AVFilmGrainParamsType type;

    /**
     * Seed 到 use 用于 the synthesis process, 如果 the codec allows 用于 it.
     *
     * @note For H.264, this refers 到 `pic_off设置` as defined in
     *       SMPTE RDD 5-2006.
     */
    uint64_t seed;

    /**
     * Intended display resolution. May be 0 如果 the codec does not spec如果y
     * any restrictions.
     */

    int width, height;

    /**
     * Intended subsampling ratio, 或 0 用于 luma-only streams.
     */
    int subsampling_x, subsampling_y;

    /**
     * Intended 视频 signal characteristics.
     */
    enum AVColorRange                  color_range;
    enum AVColorPrimaries              color_primaries;
    enum AVColorTransferCharacteristic color_trc;
    enum AVColorSpace                  color_space;

    /**
     * Intended bit depth, 或 0 用于 unknown/unspec如果ied.
     */
    int bit_depth_luma;
    int bit_depth_chroma;

    /**
     * Additional fields may be added both here 和 中 any 结构体 included.
     * 如果 a codec's film grain 结构体 d如果fers slightly over another
     * codec's, fields within may change meaning depending 上 the type.
     */
    union {
        AVFilmGrainAOMParams aom;
        AVFilmGrainH274Params h274;
    } codec;
} AVFilmGrainParams;

/**
 * 分配 an AVFilmGrainParams 结构体 和 设置 its fields to
 * 默认 值. resulting struct can be 释放d using av_释放p().
 * 如果 大小 is not NULL it will be 设置 到 the 数量 的 bytes 分配d.
 *
 * @返回 AVFilmGrainParams filled，使用 默认 值 或 NULL
 *         上 failure.
 */
AVFilmGrainParams *av_film_grain_params_alloc(size_t *size);

/**
 * 分配 a complete AVFilmGrainParams 和 add it 到 the 帧.
 *
 * @param 帧 帧 which side data is added to.
 *
 * @返回 AVFilmGrainParams 结构体 到 be filled by caller.
 */
AVFilmGrainParams *av_film_grain_params_create_side_data(AVFrame *frame);

/**
 * Select the most appropriate film grain parameters 设置 用于 the 帧,
 * taking into account the 帧's 格式, resolution 和 视频 signal
 * characteristics.
 *
 * @note, 用于 H.274, this may select a film grain parameter 设置 with
 * greater chroma resolution than the 帧. Users should take care to
 * correctly adjust the chroma grain frequency 到 the 帧.
 */
const AVFilmGrainParams *av_film_grain_params_select(const AVFrame *frame);

#endif /* AVUTIL_FILM_GRAIN_PARAMS_H */
