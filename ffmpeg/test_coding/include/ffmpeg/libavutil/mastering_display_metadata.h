/*
 * Copyright (c) 2016 Neil Birkbeck <neil.birkbeck@gmail.com>
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

#ifndef AVUTIL_MASTERING_DISPLAY_METADATA_H
#define AVUTIL_MASTERING_DISPLAY_METADATA_H

#include "frame.h"
#include "rational.h"


/**
 * 能够表示内容制作时所用显示器色彩体积的母版显示元数据（SMPTE 2086:2014）。
 *
 * 用作具有适当类型的 AVFrameSideData 或 AVPacketSideData 的载荷。
 *
 * @note 应使用 av_mastering_display_metadata_alloc() 分配该结构，其大小不属于
 *       公共 ABI。
 */
typedef struct AVMasteringDisplayMetadata {
    /**
     * 色度原色的 CIE 1931 xy 色度坐标（r、g、b 顺序）。
     */
    AVRational display_primaries[3][2];

    /**
     * 白点的 CIE 1931 xy 色度坐标。
     */
    AVRational white_point[2];

    /**
     * 母版显示器的最小亮度（cd/m^2）。
     */
    AVRational min_luminance;

    /**
     * 母版显示器的最大亮度（cd/m^2）。
     */
    AVRational max_luminance;

    /**
     * 表示是否已设置显示原色（和白点）的标志。
     */
    int has_primaries;

    /**
     * 表示是否已设置亮度（min_ 和 max_）的标志。
     */
    int has_luminance;

} AVMasteringDisplayMetadata;

/**
 * 分配 AVMasteringDisplayMetadata 结构并将其字段设为默认值。结果结构可以使用
 * av_freep() 释放。
 *
 * @return 填有默认值的 AVMasteringDisplayMetadata；失败时返回 NULL。
 */
AVMasteringDisplayMetadata *av_mastering_display_metadata_alloc(void);

/**
 * 分配 AVMasteringDisplayMetadata 结构并将其字段设为默认值。结果结构可以使用
 * av_freep() 释放。
 *
 * @return 填有默认值的 AVMasteringDisplayMetadata；失败时返回 NULL。
 */
AVMasteringDisplayMetadata *av_mastering_display_metadata_alloc_size(size_t *size);

/**
 * 分配完整的 AVMasteringDisplayMetadata 并将其添加到帧中。
 *
 * @param frame 要添加侧数据的帧。
 *
 * @return 由调用者填充的 AVMasteringDisplayMetadata 结构。
 */
AVMasteringDisplayMetadata *av_mastering_display_metadata_create_side_data(AVFrame *frame);

/**
 * 通过 HDMI 传输 HDR 所需的内容光照级别（CTA-861.3）。
 *
 * 用作具有适当类型的 AVFrameSideData 或 AVPacketSideData 的载荷。
 *
 * @note 应使用 av_content_light_metadata_alloc() 分配该结构，其大小不属于
 *       公共 ABI。
 */
typedef struct AVContentLightMetadata {
    /**
     * 最大内容光照级别（cd/m^2）。
     */
    unsigned MaxCLL;

    /**
     * 每帧最大平均光照级别（cd/m^2）。
     */
    unsigned MaxFALL;
} AVContentLightMetadata;

/**
 * 分配 AVContentLightMetadata 结构并将其字段设为默认值。结果结构可以使用
 * av_freep() 释放。
 *
 * @return 填有默认值的 AVContentLightMetadata；失败时返回 NULL。
 */
AVContentLightMetadata *av_content_light_metadata_alloc(size_t *size);

/**
 * 分配完整的 AVContentLightMetadata 并将其添加到帧中。
 *
 * @param frame 要添加侧数据的帧。
 *
 * @return 由调用者填充的 AVContentLightMetadata 结构。
 */
AVContentLightMetadata *av_content_light_metadata_create_side_data(AVFrame *frame);

#endif /* AVUTIL_MASTERING_DISPLAY_METADATA_H */
