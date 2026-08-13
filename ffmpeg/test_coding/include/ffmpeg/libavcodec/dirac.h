/*
 * Copyright (C) 2007 Marco Gerards <marco@gnu.org>
 * Copyright (C) 2009 David Conrad
 * Copyright (C) 2011 Jordi Ortiz
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

#ifndef AVCODEC_DIRAC_H
#define AVCODEC_DIRAC_H

/**
 * @file
 * Dirac 解码器/编码器接口
 * @author Marco Gerards <marco@gnu.org>
 * @author David Conrad
 * @author Jordi Ortiz
 */

#include <stddef.h>
#include <stdint.h>

#include "libavutil/pixfmt.h"
#include "libavutil/rational.h"

/**
 * 规范将 level 1（VC-2）和 128（长 GOP 默认值）的小波分解次数均限制为 4。
 * 在需要大于 16 位的缓冲区之前，最多可分解 5 次。
 * Schroedinger 仅允许 DD 9,7 和 13,7 小波采用此设置，其他小波限制为
 * 4 次分解（保真滤波器为 3 次）。
 *
 * 为节省内存，这里使用该值而不是 MAX_DECOMPOSITIONS。
 */
#define MAX_DWT_LEVELS 5

/**
 * 解析码值：
 *
 * Dirac Specification ->
 * 9.6.1  Table 9.1
 *
 * VC-2 Specification  ->
 * 10.4.1 Table 10.1
 */

enum DiracParseCodes {
    DIRAC_PCODE_SEQ_HEADER      = 0x00,
    DIRAC_PCODE_END_SEQ         = 0x10,
    DIRAC_PCODE_AUX             = 0x20,
    DIRAC_PCODE_PAD             = 0x30,
    DIRAC_PCODE_PICTURE_CODED   = 0x08,
    DIRAC_PCODE_PICTURE_RAW     = 0x48,
    DIRAC_PCODE_PICTURE_LOW_DEL = 0xC8,
    DIRAC_PCODE_PICTURE_HQ      = 0xE8,
    DIRAC_PCODE_INTER_NOREF_CO1 = 0x0A,
    DIRAC_PCODE_INTER_NOREF_CO2 = 0x09,
    DIRAC_PCODE_INTER_REF_CO1   = 0x0D,
    DIRAC_PCODE_INTER_REF_CO2   = 0x0E,
    DIRAC_PCODE_INTRA_REF_CO    = 0x0C,
    DIRAC_PCODE_INTRA_REF_RAW   = 0x4C,
    DIRAC_PCODE_INTRA_REF_PICT  = 0xCC,
    DIRAC_PCODE_MAGIC           = 0x42424344,
};

typedef struct DiracVersionInfo {
    int major;
    int minor;
} DiracVersionInfo;

typedef struct AVDiracSeqHeader {
    unsigned width;
    unsigned height;
    uint8_t chroma_format;          ///< 0: 444  1: 422  2: 420

    uint8_t interlaced;
    uint8_t top_field_first;

    uint8_t frame_rate_index;       ///< dirac_frame_rate[] 的索引
    uint8_t aspect_ratio_index;     ///< dirac_aspect_ratio[] 的索引

    uint16_t clean_width;
    uint16_t clean_height;
    uint16_t clean_left_offset;
    uint16_t clean_right_offset;

    uint8_t pixel_range_index;      ///< dirac_pixel_range_presets[] 的索引
    uint8_t color_spec_index;       ///< dirac_color_spec_presets[] 的索引

    int profile;
    int level;

    AVRational framerate;
    AVRational sample_aspect_ratio;

    enum AVPixelFormat pix_fmt;
    enum AVColorRange color_range;
    enum AVColorPrimaries color_primaries;
    enum AVColorTransferCharacteristic color_trc;
    enum AVColorSpace colorspace;

    DiracVersionInfo version;
    int bit_depth;
} AVDiracSeqHeader;

/**
 * 解析 Dirac 序列头。
 *
 * @param dsh 此函数将分配并填充 AVDiracSeqHeader 结构体，然后写入此指针。
 *            调用方必须使用 av_free() 将其释放。
 * @param buf 数据缓冲区
 * @param buf_size 数据缓冲区大小，单位为字节
 * @param log_ctx 非 NULL 时，函数会在此处记录错误
 * @return 成功返回 0，失败返回负的 AVERROR 错误码
 */
int av_dirac_parse_sequence_header(AVDiracSeqHeader **dsh,
                                   const uint8_t *buf, size_t buf_size,
                                   void *log_ctx);

#endif /* AVCODEC_DIRAC_H */
