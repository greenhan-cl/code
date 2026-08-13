/*
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

#ifndef AVUTIL_HWCONTEXT_QSV_H
#define AVUTIL_HWCONTEXT_QSV_H

#include <mfxvideo.h>

/**
 * @file
 * AV_HWDEVICE_TYPE_QSV 专用 API 头文件。
 *
 * AVHWFramesContext.pool 必须包含数据指针指向 mfxFrameSurface1 结构的
 * AVBufferRef。
 */

/**
 * 此结构被分配为 AVHWDeviceContext.hwctx。
 */
typedef struct AVQSVDeviceContext {
    mfxSession session;
    /**
     * 用于创建 mfxSession 的 mfxLoader 句柄
     *
     * 此字段仅供 oneVPL 用户使用。对于非 oneVPL 用户，必须将此字段设为 NULL。
     *
     * 由用户在调用 av_hwdevice_ctx_init() 前填充，并应转换为 mfxLoader 句柄。
     * 释放 AVHWDeviceContext 时始终会释放此接口。
     */
    void *loader;
} AVQSVDeviceContext;

/**
 * 此结构被分配为 AVHWFramesContext.hwctx。
 */
typedef struct AVQSVFramesContext {
    /**
     * 指向 mfxFrameSurface1 结构的指针
     *
     * 当 nb_surfaces 非零时可用。
     */
    mfxFrameSurface1 *surfaces;

    /**
     * 池中的帧数量
     *
     * 动态帧池时为 0，固定帧池时为 AVHWFramesContext.initial_pool_size。
     *
     * 注意，只有 oneVPL GPU runtime 2.9+ 能在 d3d11va 或 vaapi 上支持动态帧池。
     */
    int            nb_surfaces;

    /**
     * 描述帧池的 MFX_MEMTYPE_* 组合。
     */
    int frame_type;

    /**
     * 指向 mfxFrameInfo 结构的指针
     *
     * 当 nb_surfaces 为 0 时可用；从池中分配的所有缓冲区具有相同的
     * mfxFrameInfo。
     */
    mfxFrameInfo *info;
} AVQSVFramesContext;

#endif /* AVUTIL_HWCONTEXT_QSV_H */
