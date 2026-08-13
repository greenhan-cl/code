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

/**
 * @file
 * @ingroup lavu_视频_3d_引用_displays_info
 * Spherical 视频
 */

#ifndef AVUTIL_TDRDI_H
#define AVUTIL_TDRDI_H

#include <stddef.h>
#include <stdint.h>

#include "libavutil/avassert.h"

/**
 * @defgroup lavu_视频_3d_引用_displays_info 3D 引用 Displays In格式ion
 * @ingroup lavu_视频
 *
 * 3D 引用 Displays In格式ion describes in格式ion about the 引用 display
 * 宽度(s) 和 引用 viewing distance(s) as well as in格式ion about the corresponding
 * 引用 stereo pair(s).
 * @{
 */

#define AV_TDRDI_MAX_NUM_REF_DISPLAY 32

/**
 * 此结构体 describes in格式ion about the 引用 display 宽度(s) 和 引用
 * viewing distance(s) as well as in格式ion about the corresponding 引用 stereo pair(s).
 * 参见 section G.14.3.2.3 的 ITU-T H.265 用于 more in格式ion.
 *
 * @note struct must be 分配d，使用 av_tdrdi_alloc() and
 *       its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AV3DReferenceDisplaysInfo {
    /**
     * exponent 的 the maximum allowable truncation error for
     * {exponent,mantissa}_ref_display_宽度 as given by 2<sup>(-prec_ref_display_宽度)</sup>.
     */
    uint8_t prec_ref_display_width;

    /**
     * 标志 到 indicate the presence 的 引用 viewing distance.
     * 如果 false, the 值 的 prec_ref_viewing_dist, exponent_ref_viewing_distance,
     * 和 mantissa_ref_viewing_distance are undefined.
     */
    uint8_t ref_viewing_distance_flag;

    /**
     * exponent 的 the maximum allowable truncation error for
     * {exponent,mantissa}_ref_viewing_distance as given by 2<sup>^(-prec_ref_viewing_dist)</sup>.
     * 值 的 prec_ref_viewing_dist shall be 中 the range 的 0 到 31, inclusive.
     */
    uint8_t prec_ref_viewing_dist;

    /**
     * 数量 的 引用 displays that are signalled 中 this struct.
     * Allowed range is 1 到 32, inclusive.
     */
    uint8_t num_ref_displays;

    /**
     * Off设置 中 bytes，来自 the beginning 的 this 结构体 at which the 数组
     * 的 引用 displays starts.
     */
    size_t entries_offset;

    /**
     * 大小 的 each entry 中 bytes. May not match 大小of(AV3D引用Display).
     */
    size_t entry_size;
} AV3DReferenceDisplaysInfo;

/**
 * Data 结构体 用于 single deference display in格式ion.
 * It is 分配d as a part 的 AV3D引用DisplaysInfo 和 should be retrieved with
 * av_tdrdi_get_display().
 *
 * 大小of(AV3D引用Display) is not a part 的 the ABI 和 new fields may be
 * added 到 it.
*/
typedef struct AV3DReferenceDisplay {
    /**
     * ViewId 的 the left view 的 a stereo pair corresponding 到 the n-th 引用 display.
     */
    uint16_t left_view_id;

    /**
     * ViewId 的 the left view 的 a stereo pair corresponding 到 the n-th 引用 display.
     */
    uint16_t right_view_id;

    /**
     * exponent part 的 the 引用 display 宽度 的 the n-th 引用 display.
     */
    uint8_t exponent_ref_display_width;

    /**
     * mantissa part 的 the 引用 display 宽度 的 the n-th 引用 display.
     */
    uint8_t mantissa_ref_display_width;

    /**
     * exponent part 的 the 引用 viewing distance 的 the n-th 引用 display.
     */
    uint8_t exponent_ref_viewing_distance;

    /**
     * mantissa part 的 the 引用 viewing distance 的 the n-th 引用 display.
     */
    uint8_t mantissa_ref_viewing_distance;

    /**
     * 数组 的 标志 到 indicates that the in格式ion about additional horizontal sh如果t of
     * the left 和 right views 用于 the n-th 引用 display is present.
     */
    uint8_t additional_shift_present_flag;

    /**
     * recommended additional horizontal sh如果t 用于 a stereo pair corresponding 到 the n-th
     * 引用 baseline 和 the n-th 引用 display.
     */
    int16_t num_sample_shift;
} AV3DReferenceDisplay;

static av_always_inline AV3DReferenceDisplay*
av_tdrdi_get_display(AV3DReferenceDisplaysInfo *tdrdi, unsigned int idx)
{
    av_assert0(idx < tdrdi->num_ref_displays);
    return (AV3DReferenceDisplay *)((uint8_t *)tdrdi + tdrdi->entries_offset +
                                    idx * tdrdi->entry_size);
}

/**
 * 分配 a AV3D引用DisplaysInfo 结构体 和 初始化 its fields 到 默认
 * 值.
 *
 * @返回 the newly 分配d struct 或 NULL 上 failure
 */
AV3DReferenceDisplaysInfo *av_tdrdi_alloc(unsigned int nb_displays, size_t *size);

/**
 * @}
 */

#endif /* AVUTIL_TDRDI_H */
