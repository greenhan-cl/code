/*
 * Intel MediaSDK QSV public API
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

#ifndef AVCODEC_QSV_H
#define AVCODEC_QSV_H

#include <mfxvideo.h>

#include "libavutil/buffer.h"

/**
 * 此结构体用于在 libavcodec 与调用方之间传递 QSV 参数。它由调用方管理，
 * 且必须赋给 AVCodecContext.hwaccel_context。
 * - 解码：必须在 get_format() 回调返回时设置 hwaccel_context
 * - 编码：必须在 avcodec_open2() 之前设置 hwaccel_context
 */
typedef struct AVQSVContext {
    /**
     * 非 NULL 时，指定用于编码或解码的会话。
     * 否则，libavcodec 将尝试创建内部会话。
     */
    mfxSession session;

    /**
     * 要使用的 IO 模式。
     */
    int iopattern;

    /**
     * 初始化编码器或解码器时传入的额外缓冲区。
     */
    mfxExtBuffer **ext_buffers;
    int         nb_ext_buffers;

    /**
     * 仅用于编码。如果调用方将此字段设为非零，libavcodec 将创建
     * mfxExtOpaqueSurfaceAlloc 扩展缓冲区并将其传给编码器初始化过程。
     * 仅当 iopattern 同时设为 MFX_IOPATTERN_IN_OPAQUE_MEMORY 时才有意义。
     *
     * 分配的不透明表面数量等于编码器所需数量加上用户提供的
     * nb_opaque_surfaces。其数组通过 opaque_surfaces 字段导出给调用方。
     *
     * 使用 oneVPL（MFX_VERSION >= 2.0）时，调用方必须将此字段设为零。
     */
    int opaque_alloc;

    /**
     * 仅用于编码，且仅当 opaque_alloc 非零时有效。调用 avcodec_open2() 前，
     * 调用方应将此字段设为在编码器需求之外额外分配的不透明表面数量。
     *
     * avcodec_open2() 返回时，libavcodec 会将此字段设为已分配不透明表面的总数。
     */
    int nb_opaque_surfaces;

    /**
     * 仅用于编码，且仅当 opaque_alloc 非零时有效。avcodec_open2() 返回时，
     * libavcodec 使用此字段将已分配的不透明表面数组导出给调用方，
     * 以便传递给处理流水线的其他部分。
     *
     * 此处导出的缓冲区引用由 libavcodec 拥有并管理。调用方应使用
     * av_buffer_ref() 创建自己的引用，不再需要时使用 av_buffer_unref() 释放。
     *
     * 缓冲区数据是一个包含 nb_opaque_surfaces 个 mfxFrameSurface1 的数组。
     */
    AVBufferRef *opaque_surfaces;

    /**
     * 仅用于编码，且仅当 opaque_alloc 非零时有效。avcodec_open2() 返回时，
     * 此字段会被设为不透明分配请求中使用的表面类型。
     */
    int opaque_alloc_type;
} AVQSVContext;

/**
 * 分配新上下文。
 *
 * 调用方必须使用 av_free() 将其释放。
 */
AVQSVContext *av_qsv_alloc_context(void);

#endif /* AVCODEC_QSV_H */
