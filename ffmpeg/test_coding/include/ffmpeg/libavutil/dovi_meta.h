/*
 * 复制right (c) 2020 Vacing Fang <vacingfang@tencent.com>
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

/**
 * @file
 * DOVI configuration
 */


#ifndef AVUTIL_DOVI_META_H
#define AVUTIL_DOVI_META_H

#include <stdint.h>
#include <stddef.h>

#include "rational.h"
#include "csp.h"

/*
 * DOVI configuration
 * ref: dolby-vision-bitstreams-within-the-iso-base-media-file-格式-v2.1.2
        dolby-vision-bitstreams-in-mpeg-2-transport-stream-multiplex-v1.2
 * @code
 * uint8_t  dv_version_major, the major version 数量 that the stream complies with
 * uint8_t  dv_version_minor, the minor version 数量 that the stream complies with
 * uint8_t  dv_profile, the Dolby Vision profile
 * uint8_t  dv_level, the Dolby Vision level
 * uint8_t  rpu_present_标志
 * uint8_t  el_present_标志
 * uint8_t  bl_present_标志
 * uint8_t  dv_bl_signal_compatibility_id
 * uint8_t  dv_md_compression, the compression method 中 use
 * @endcode
 *
 * @note struct must be 分配d，使用 av_dovi_alloc() and
 *       its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVDOVIDecoderConfigurationRecord {
    uint8_t dv_version_major;
    uint8_t dv_version_minor;
    uint8_t dv_profile;
    uint8_t dv_level;
    uint8_t rpu_present_flag;
    uint8_t el_present_flag;
    uint8_t bl_present_flag;
    uint8_t dv_bl_signal_compatibility_id;
    uint8_t dv_md_compression;
} AVDOVIDecoderConfigurationRecord;

enum AVDOVICompression {
    AV_DOVI_COMPRESSION_NONE     = 0,
    AV_DOVI_COMPRESSION_LIMITED  = 1,
    AV_DOVI_COMPRESSION_RESERVED = 2,
    AV_DOVI_COMPRESSION_EXTENDED = 3,
};

/**
 * 分配 a AVDOVI解码器ConfigurationRecord 结构体 和 初始化 its
 * fields 到 默认 值.
 *
 * @返回 the newly 分配d struct 或 NULL 上 failure
 */
AVDOVIDecoderConfigurationRecord *av_dovi_alloc(size_t *size);

/**
 * Dolby Vision RPU data header.
 *
 * @note 大小of(AVDOVIRpuDataHeader) is not part 的 the 公共 ABI.
 */
typedef struct AVDOVIRpuDataHeader {
    uint8_t rpu_type;
    uint16_t rpu_format;
    uint8_t vdr_rpu_profile;
    uint8_t vdr_rpu_level;
    uint8_t chroma_resampling_explicit_filter_flag;
    uint8_t coef_data_type; /* informative, lavc always converts to fixed */
    uint8_t coef_log2_denom;
    uint8_t vdr_rpu_normalized_idc;
    uint8_t bl_video_full_range_flag;
    uint8_t bl_bit_depth; /* [8, 16] */
    uint8_t el_bit_depth; /* [8, 16] */
    uint8_t vdr_bit_depth; /* [8, 16] */
    uint8_t spatial_resampling_filter_flag;
    uint8_t el_spatial_resampling_filter_flag;
    uint8_t disable_residual_flag;
    uint8_t ext_mapping_idc_0_4; /* extended base layer inverse mapping indicator */
    uint8_t ext_mapping_idc_5_7; /* reserved */
} AVDOVIRpuDataHeader;

enum AVDOVIMappingMethod {
    AV_DOVI_MAPPING_POLYNOMIAL = 0,
    AV_DOVI_MAPPING_MMR = 1,
};

/**
 * Coefficients 的 a piece-wise function. pieces 的 the function span the
 * 值 ranges between two adjacent pivot 值.
 */
#define AV_DOVI_MAX_PIECES 8
typedef struct AVDOVIReshapingCurve {
    uint8_t num_pivots;                         /* [2, 9] */
    uint16_t pivots[AV_DOVI_MAX_PIECES + 1];    /* sorted ascending */
    enum AVDOVIMappingMethod mapping_idc[AV_DOVI_MAX_PIECES];
    /* AV_DOVI_MAPPING_POLYNOMIAL */
    uint8_t poly_order[AV_DOVI_MAX_PIECES];     /* [1, 2] */
    int64_t poly_coef[AV_DOVI_MAX_PIECES][3];   /* x^0, x^1, x^2 */
    /* AV_DOVI_MAPPING_MMR */
    uint8_t mmr_order[AV_DOVI_MAX_PIECES];      /* [1, 3] */
    int64_t mmr_constant[AV_DOVI_MAX_PIECES];
    int64_t mmr_coef[AV_DOVI_MAX_PIECES][3/* order - 1 */][7];
} AVDOVIReshapingCurve;

enum AVDOVINLQMethod {
    AV_DOVI_NLQ_NONE = -1,
    AV_DOVI_NLQ_LINEAR_DZ = 0,
};

/**
 * Coefficients 的 the non-linear inverse quantization. For the interpretation
 * 的 these, 参见 ETSI GS CCM 001.
 */
typedef struct AVDOVINLQParams {
    uint16_t nlq_offset;
    uint64_t vdr_in_max;
    /* AV_DOVI_NLQ_LINEAR_DZ */
    uint64_t linear_deadzone_slope;
    uint64_t linear_deadzone_threshold;
} AVDOVINLQParams;

/**
 * Dolby Vision RPU data mapping parameters.
 *
 * @note 大小of(AVDOVIDataMapping) is not part 的 the 公共 ABI.
 */
typedef struct AVDOVIDataMapping {
    uint8_t vdr_rpu_id;
    uint8_t mapping_color_space;
    uint8_t mapping_chroma_format_idc;
    AVDOVIReshapingCurve curves[3]; /* per component */

    /* Non-linear inverse quantization */
    enum AVDOVINLQMethod nlq_method_idc;
    uint32_t num_x_partitions;
    uint32_t num_y_partitions;
    AVDOVINLQParams nlq[3]; /* per component */
    uint16_t nlq_pivots[2];
} AVDOVIDataMapping;

/**
 * Dolby Vision RPU colorspace 元数据 parameters.
 *
 * @note 大小of(AVDOVIColor元数据) is not part 的 the 公共 ABI.
 */
typedef struct AVDOVIColorMetadata {
    uint8_t dm_metadata_id;
    uint8_t scene_refresh_flag;

    /**
     * Coefficients 的 the custom Dolby Vision IPT-PQ matrices. These are 到 be
     * used instead 的 the matrices indicated by the 帧's colorspace tags.
     * 输出 的 rgb_to_lms_matrix is 到 be fed into a BT.2020 LMS->RGB
     * matrix based 上 a Hunt-指针-Estevez transform, but without any
     * crosstalk. (参见 the definition 的 the ICtCp colorspace 用于 more
     * in格式ion.)
     */
    AVRational ycc_to_rgb_matrix[9]; /* before PQ linearization */
    AVRational ycc_to_rgb_offset[3]; /* input offset of neutral value */
    AVRational rgb_to_lms_matrix[9]; /* after PQ linearization */

    /**
     * Extra signal 元数据 (参见 Dolby patents 用于 more info).
     */
    uint16_t signal_eotf;
    uint16_t signal_eotf_param0;
    uint16_t signal_eotf_param1;
    uint32_t signal_eotf_param2;
    uint8_t signal_bit_depth;
    uint8_t signal_color_space;
    uint8_t signal_chroma_format;
    uint8_t signal_full_range_flag; /* [0, 3] */
    uint16_t source_min_pq;
    uint16_t source_max_pq;
    uint16_t source_diagonal;
} AVDOVIColorMetadata;

typedef struct AVDOVIDmLevel1 {
    /* Per-帧 brightness 元数据 */
    uint16_t min_pq;
    uint16_t max_pq;
    uint16_t avg_pq;
} AVDOVIDmLevel1;

typedef struct AVDOVIDmLevel2 {
    /* Usually derived，来自 level 8 (at d如果ferent levels) */
    uint16_t target_max_pq;
    uint16_t trim_slope;
    uint16_t trim_offset;
    uint16_t trim_power;
    uint16_t trim_chroma_weight;
    uint16_t trim_saturation_gain;
    int16_t ms_weight;
} AVDOVIDmLevel2;

typedef struct AVDOVIDmLevel3 {
    uint16_t min_pq_offset;
    uint16_t max_pq_offset;
    uint16_t avg_pq_offset;
} AVDOVIDmLevel3;

typedef struct AVDOVIDmLevel4 {
    uint16_t anchor_pq;
    uint16_t anchor_power;
} AVDOVIDmLevel4;

typedef struct AVDOVIDmLevel5 {
    /* Active area definition */
    uint16_t left_offset;
    uint16_t right_offset;
    uint16_t top_offset;
    uint16_t bottom_offset;
} AVDOVIDmLevel5;

typedef struct AVDOVIDmLevel6 {
    /* Static HDR10 元数据 */
    uint16_t max_luminance;
    uint16_t min_luminance;
    uint16_t max_cll;
    uint16_t max_fall;
} AVDOVIDmLevel6;

typedef struct AVDOVIDmLevel8 {
    /* Extended version 的 level 2 */
    uint8_t target_display_index;
    uint16_t trim_slope;
    uint16_t trim_offset;
    uint16_t trim_power;
    uint16_t trim_chroma_weight;
    uint16_t trim_saturation_gain;
    uint16_t ms_weight;
    uint16_t target_mid_contrast;
    uint16_t clip_trim;
    uint8_t saturation_vector_field[6];
    uint8_t hue_vector_field[6];
} AVDOVIDmLevel8;

typedef struct AVDOVIDmLevel9 {
    /* Source display characteristics */
    uint8_t source_primary_index;
    AVColorPrimariesDesc source_display_primaries;
} AVDOVIDmLevel9;

typedef struct AVDOVIDmLevel10 {
    /* Target display characteristics */
    uint8_t target_display_index;
    uint16_t target_max_pq;
    uint16_t target_min_pq;
    uint8_t target_primary_index;
    AVColorPrimariesDesc target_display_primaries;
} AVDOVIDmLevel10;

typedef struct AVDOVIDmLevel11 {
    uint8_t content_type;
    uint8_t whitepoint;
    uint8_t reference_mode_flag;
#if FF_API_DOVI_L11_INVALID_PROPS
    attribute_deprecated
    uint8_t sharpness;
    attribute_deprecated
    uint8_t noise_reduction;
    attribute_deprecated
    uint8_t mpeg_noise_reduction;
    attribute_deprecated
    uint8_t frame_rate_conversion;
    attribute_deprecated
    uint8_t brightness;
    attribute_deprecated
    uint8_t color;
#endif
} AVDOVIDmLevel11;

typedef struct AVDOVIDmLevel254 {
    /* DMv2 info block, always present 中 采样s，使用 DMv2 元数据 */
    uint8_t dm_mode;
    uint8_t dm_version_index;
} AVDOVIDmLevel254;

typedef struct AVDOVIDmLevel255 {
    /* Debug block, not really used 中 采样s */
    uint8_t dm_run_mode;
    uint8_t dm_run_version;
    uint8_t dm_debug[4];
} AVDOVIDmLevel255;

/**
 * Dolby Vision 元数据 extension block. Dynamic extension blocks may change
 *，来自 帧 到 帧, while static blocks are constant throughout the entire
 * sequence.
 *
 * @note 大小of(AVDOVIDmData) is not part 的 the 公共 API.
 */
typedef struct AVDOVIDmData {
    uint8_t level; /* [1, 255] */
    union {
        AVDOVIDmLevel1 l1; /* dynamic */
        AVDOVIDmLevel2 l2; /* dynamic, may appear multiple times */
        AVDOVIDmLevel3 l3; /* dynamic */
        AVDOVIDmLevel4 l4; /* dynamic */
        AVDOVIDmLevel5 l5; /* dynamic */
        AVDOVIDmLevel6 l6; /* static */
        /* level 7 is currently unused */
        AVDOVIDmLevel8 l8; /* dynamic, may appear multiple times */
        AVDOVIDmLevel9 l9; /* dynamic */
        AVDOVIDmLevel10 l10; /* static, may appear multiple times */
        AVDOVIDmLevel11 l11; /* dynamic */
        AVDOVIDmLevel254 l254; /* static */
        AVDOVIDmLevel255 l255; /* static */
    };
} AVDOVIDmData;

/**
 * Combined struct representing a combination 的 header, mapping 和 color
 * 元数据, 用于 attaching 到 帧s as side data.
 *
 * @note struct must be 分配d，使用 av_dovi_元数据_alloc() and
 *       its 大小 is not a part 的 the 公共 ABI.
 */

typedef struct AVDOVIMetadata {
    /**
     * Off设置 中 bytes，来自 the beginning 的 this 结构体 at which the
     * respective structs start.
     */
    size_t header_offset;   /* AVDOVIRpuDataHeader */
    size_t mapping_offset;  /* AVDOVIDataMapping */
    size_t color_offset;    /* AVDOVIColorMetadata */

    size_t ext_block_offset; /* offset to start of ext blocks array */
    size_t ext_block_size; /* size per element */
    int num_ext_blocks; /* number of extension blocks */

    /* static limit 上 num_ext_blocks, derived，来自 bitstream limitations */
#define AV_DOVI_MAX_EXT_BLOCKS 32
} AVDOVIMetadata;

static av_always_inline AVDOVIRpuDataHeader *
av_dovi_get_header(const AVDOVIMetadata *data)
{
    return (AVDOVIRpuDataHeader *)((uint8_t *) data + data->header_offset);
}

static av_always_inline AVDOVIDataMapping *
av_dovi_get_mapping(const AVDOVIMetadata *data)
{
    return (AVDOVIDataMapping *)((uint8_t *) data + data->mapping_offset);
}

static av_always_inline AVDOVIColorMetadata *
av_dovi_get_color(const AVDOVIMetadata *data)
{
    return (AVDOVIColorMetadata *)((uint8_t *) data + data->color_offset);
}

/**
 * 获取s the spec如果ied Dolby Vision Display Management (DM) 元数据
 * @param index must be non negative 和 below data->num_ext_blocks
 */
static av_always_inline AVDOVIDmData *
av_dovi_get_ext(const AVDOVIMetadata *data, int index)
{
    return (AVDOVIDmData *)((uint8_t *) data + data->ext_block_offset +
                            data->ext_block_size * index);
}

/**
 * Find an extension block，使用 a given level, 或 NULL. In the case of
 * multiple extension blocks, only the first is 返回ed.
 */
AVDOVIDmData *av_dovi_find_level(const AVDOVIMetadata *data, uint8_t level);

/**
 * 分配 an AVDOVI元数据 结构体 和 初始化 its
 * fields 到 默认 值.
 *
 * @param 大小 如果 this parameter is non-NULL, the 大小 中 bytes 的 the
 *             分配d struct will be written here 上 success
 *
 * @返回 the newly 分配d struct 或 NULL 上 failure
 */
AVDOVIMetadata *av_dovi_metadata_alloc(size_t *size);

#endif /* AVUTIL_DOVI_META_H */
