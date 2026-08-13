/*
 * Videotoolbox hardware acceleration
 *
 * copyright (c) 2012 Sebastien Zwickert
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVCODEC_VIDEOTOOLBOX_H
#define AVCODEC_VIDEOTOOLBOX_H

/**
 * @file
 * @ingroup lavc_codec_hwaccel_videotoolbox
 * libavcodec 的公共 Videotoolbox 头文件。
 */

/**
 * @defgroup lavc_codec_hwaccel_videotoolbox VideoToolbox Decoder
 * @ingroup lavc_codec_hwaccel
 *
 * 在 Apple 平台上使用 VideoToolbox 进行硬件加速解码
 *
 * @{
 */

#include <stdint.h>

#define Picture QuickdrawPicture
#include <VideoToolbox/VideoToolbox.h>
#undef Picture

#include "libavcodec/avcodec.h"

#include "libavutil/attributes.h"

/**
 * 此结构体保存初始化 Videotoolbox 解码时调用方与 libavcodec 之间需要传递的所有信息。
 * 它的大小不属于公共 ABI，必须使用 av_videotoolbox_alloc_context() 分配，
 * 并使用 av_free() 释放。
 */
typedef struct AVVideotoolboxContext {
    /**
     * Videotoolbox 解压会话对象。
     */
    VTDecompressionSessionRef session;

    /**
     * Videotoolbox 用于解码帧的 CVPixelBuffer 格式类型，由调用方设置。
     * 如果设置为 0，则不向解码器请求特定格式，而是输出其原生格式。
     */
    OSType cv_pix_fmt_type;

    /**
     * Videotoolbox 用于创建解压会话的 CoreMedia 格式描述。
     */
    CMVideoFormatDescriptionRef cm_fmt_desc;

    /**
     * Videotoolbox 用于创建解压会话的 CoreMedia 编解码器类型。
     */
    int cm_codec_type;
} AVVideotoolboxContext;

/**
 * @}
 */

#endif /* AVCODEC_VIDEOTOOLBOX_H */
