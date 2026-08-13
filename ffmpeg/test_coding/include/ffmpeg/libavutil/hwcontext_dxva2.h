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


#ifndef AVUTIL_HWCONTEXT_DXVA2_H
#define AVUTIL_HWCONTEXT_DXVA2_H

/**
 * @file
 * AV_HWDEVICE_TYPE_DXVA2 专用 API 头文件。
 *
 * 仅支持固定大小的池。
 *
 * 对于用户分配的池，AVHWFramesContext.pool 必须返回数据指针设为
 * IDirect3DSurface9 指针的 AVBufferRef。
 */

#include <d3d9.h>
#include <dxva2api.h>

/**
 * 此结构被分配为 AVHWDeviceContext.hwctx。
 */
typedef struct AVDXVA2DeviceContext {
    IDirect3DDeviceManager9 *devmgr;
} AVDXVA2DeviceContext;

/**
 * 此结构被分配为 AVHWFramesContext.hwctx。
 */
typedef struct AVDXVA2FramesContext {
    /**
     * 表面类型（例如 DXVA2_VideoProcessorRenderTarget 或
     * DXVA2_VideoDecoderRenderTarget）。必须由调用者设置。
     */
    DWORD               surface_type;

    /**
     * 表面池。当调用者未提供外部池时，它由 libavutil 管理（初始化时分配和填充，
     * 反初始化时释放）。
     */
    IDirect3DSurface9 **surfaces;
    int              nb_surfaces;

    /**
     * 某些驱动程序要求先于表面销毁解码器。为使内部管理的池在此情况下正常工作，
     * 提供了此字段。
     *
     * 如果它非 NULL，libavutil 会在释放内部表面池之前对其调用
     * IDirectXVideoDecoder_Release()。
     *
     * 这只是为了方便。某些代码使用其他方法管理解码器引用。
     */
    IDirectXVideoDecoder *decoder_to_release;
} AVDXVA2FramesContext;

#endif /* AVUTIL_HWCONTEXT_DXVA2_H */
