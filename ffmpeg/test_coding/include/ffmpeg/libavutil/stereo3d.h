/*
 * 复制right (c) 2013 Vittorio Giovara <vittorio.giovara@gmail.com>
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
 * @ingroup lavu_视频_stereo3d
 * Stereoscopic 视频
 */

#ifndef AVUTIL_STEREO3D_H
#define AVUTIL_STEREO3D_H

#include <stdint.h>

#include "frame.h"

/**
 * @defgroup lavu_视频_stereo3d Stereo3D types 和 functions
 * @ingroup lavu_视频
 *
 * stereoscopic 视频 file consists 中 multiple views embedded 中 a single
 * 帧, usually describing two views 的 a scene. This file describes all
 * possible codec-independent view arrangements.
 *
 * @{
 */

/**
 * 列表 的 possible 3D Types
 */
enum AVStereo3DType {
    /**
     * 视频 is not stereoscopic (and 元数据 has 到 be there).
     */
    AV_STEREO3D_2D,

    /**
     * Views are next 到 each other.
     *
     * @code{.un解析d}
     *    LLLLRRRR
     *    LLLLRRRR
     *    LLLLRRRR
     *    ...
     * @endcode
     */
    AV_STEREO3D_SIDEBYSIDE,

    /**
     * Views are 上 top 的 each other.
     *
     * @code{.un解析d}
     *    LLLLLLLL
     *    LLLLLLLL
     *    RRRRRRRR
     *    RRRRRRRR
     * @endcode
     */
    AV_STEREO3D_TOPBOTTOM,

    /**
     * Views are alternated temporally.
     *
     * @code{.un解析d}
     *     帧0   帧1   帧2   ...
     *    LLLLLLLL RRRRRRRR LLLLLLLL
     *    LLLLLLLL RRRRRRRR LLLLLLLL
     *    LLLLLLLL RRRRRRRR LLLLLLLL
     *    ...      ...      ...
     * @endcode
     */
    AV_STEREO3D_FRAMESEQUENCE,

    /**
     * Views are packed 中 a 检查erboard-like 结构体 per 像素.
     *
     * @code{.un解析d}
     *    LRLRLRLR
     *    RLRLRLRL
     *    LRLRLRLR
     *    ...
     * @endcode
     */
    AV_STEREO3D_CHECKERBOARD,

    /**
     * Views are next 到 each other, but 当 upscaling
     * apply a 检查erboard pattern.
     *
     * @code{.un解析d}
     *     LLLLRRRR          L L L L    R R R R
     *     LLLLRRRR    =>     L L L L  R R R R
     *     LLLLRRRR          L L L L    R R R R
     *     LLLLRRRR           L L L L  R R R R
     * @endcode
     */
    AV_STEREO3D_SIDEBYSIDE_QUINCUNX,

    /**
     * Views are packed per line, as 如果 interlaced.
     *
     * @code{.un解析d}
     *    LLLLLLLL
     *    RRRRRRRR
     *    LLLLLLLL
     *    ...
     * @endcode
     */
    AV_STEREO3D_LINES,

    /**
     * Views are packed per column.
     *
     * @code{.un解析d}
     *    LRLRLRLR
     *    LRLRLRLR
     *    LRLRLRLR
     *    ...
     * @endcode
     */
    AV_STEREO3D_COLUMNS,

    /**
     * 视频 is stereoscopic but the packing is unspec如果ied.
     */
    AV_STEREO3D_UNSPEC,
};

/**
 * 列表 的 possible view types.
 */
enum AVStereo3DView {
    /**
     * 帧 包含 two packed views.
     */
    AV_STEREO3D_VIEW_PACKED,

    /**
     * 帧 包含 only the left view.
     */
    AV_STEREO3D_VIEW_LEFT,

    /**
     * 帧 包含 only the right view.
     */
    AV_STEREO3D_VIEW_RIGHT,

    /**
     * Content is unspec如果ied.
     */
    AV_STEREO3D_VIEW_UNSPEC,
};

/**
 * 列表 的 possible primary eyes.
 */
enum AVStereo3DPrimaryEye {
    /**
     * Neither eye.
     */
    AV_PRIMARY_EYE_NONE,

    /**
     * Left eye.
     */
    AV_PRIMARY_EYE_LEFT,

    /**
     * Right eye
     */
    AV_PRIMARY_EYE_RIGHT,
};

/**
 * Inverted views, Right/Bottom represents the left view.
 */
#define AV_STEREO3D_FLAG_INVERT     (1 << 0)

/**
 * Stereo 3D type: this 结构体 describes how two 视频s are packed
 * within a single 视频 surface,，使用 additional in格式ion as needed.
 *
 * @note struct must be 分配d，使用 av_stereo3d_alloc() and
 *       its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVStereo3D {
    /**
     * How views are packed within the 视频.
     */
    enum AVStereo3DType type;

    /**
     * Additional in格式ion about the 帧 packing.
     */
    int flags;

    /**
     * Determines which views are packed.
     */
    enum AVStereo3DView view;

    /**
     * Which eye is the primary eye 当 rendering 中 2D.
     */
    enum AVStereo3DPrimaryEye primary_eye;

    /**
     * distance between the centres 的 the lenses 的 the camera system,
     * 中 micrometers. Zero 如果 un设置.
     */
    uint32_t baseline;

    /**
     * Relative sh如果t 的 the left 和 right images, which changes the zero parallax plane.
     * Range is -1.0 到 1.0. Zero 如果 un设置.
     */
    AVRational horizontal_disparity_adjustment;

    /**
     * Horizontal field 的 view, 中 degrees. Zero 如果 un设置.
     */
    AVRational horizontal_field_of_view;
} AVStereo3D;

/**
 * 分配 an AVStereo3D 结构体 和 设置 its fields 到 默认 值.
 * resulting struct can be 释放d using av_释放p().
 *
 * @返回 AVStereo3D filled，使用 默认 值 或 NULL 上 failure.
 */
AVStereo3D *av_stereo3d_alloc(void);

/**
 * 分配 an AVStereo3D 结构体 和 设置 its fields 到 默认 值.
 * resulting struct can be 释放d using av_释放p().
 *
 * @返回 AVStereo3D filled，使用 默认 值 或 NULL 上 failure.
 */
AVStereo3D *av_stereo3d_alloc_size(size_t *size);

/**
 * 分配 a complete AV帧SideData 和 add it 到 the 帧.
 *
 * @param 帧 帧 which side data is added to.
 *
 * @返回 AVStereo3D 结构体 到 be filled by caller.
 */
AVStereo3D *av_stereo3d_create_side_data(AVFrame *frame);

/**
 * Provide a human-readable name 的 a given stereo3d type.
 *
 * @param type 输入 stereo3d type 值.
 *
 * @返回 name 的 the stereo3d 值, 或 "unknown".
 */
const char *av_stereo3d_type_name(unsigned int type);

/**
 * 获取 the AVStereo3DType form a human-readable name.
 *
 * @param name 输入 string.
 *
 * @返回 AVStereo3DType 值, 或 -1 如果 not found.
 */
int av_stereo3d_from_name(const char *name);

/**
 * Provide a human-readable name 的 a given stereo3d view.
 *
 * @param type 输入 stereo3d view 值.
 *
 * @返回 name 的 the stereo3d view 值, 或 "unknown".
 */
const char *av_stereo3d_view_name(unsigned int view);

/**
 * 获取 the AVStereo3DView form a human-readable name.
 *
 * @param name 输入 string.
 *
 * @返回 AVStereo3DView 值, 或 -1 如果 not found.
 */
int av_stereo3d_view_from_name(const char *name);

/**
 * Provide a human-readable name 的 a given stereo3d primary eye.
 *
 * @param type 输入 stereo3d primary eye 值.
 *
 * @返回 name 的 the stereo3d primary eye 值, 或 "unknown".
 */
const char *av_stereo3d_primary_eye_name(unsigned int eye);

/**
 * 获取 the AVStereo3DPrimaryEye form a human-readable name.
 *
 * @param name 输入 string.
 *
 * @返回 AVStereo3DPrimaryEye 值, 或 -1 如果 not found.
 */
int av_stereo3d_primary_eye_from_name(const char *name);

/**
 * @}
 */

#endif /* AVUTIL_STEREO3D_H */
