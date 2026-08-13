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

#ifndef AVUTIL_HWCONTEXT_VDPAU_H
#define AVUTIL_HWCONTEXT_VDPAU_H

#include <vdpau/vdpau.h>

/**
 * @file
 * AV_HWDEVICE_TYPE_VDPAU 专用 API 头文件。
 *
 * 此 API 支持动态帧池。AVHWFramesContext.pool 必须返回数据指针为
 * VdpVideoSurface 的 AVBufferRef。
 */

/**
 * 此结构被分配为 AVHWDeviceContext.hwctx。
 */
typedef struct AVVDPAUDeviceContext {
    VdpDevice          device;
    VdpGetProcAddress *get_proc_address;
} AVVDPAUDeviceContext;

/**
 * AVHWFramesContext.hwctx 当前未使用。
 */

#endif /* AVUTIL_HWCONTEXT_VDPAU_H */
