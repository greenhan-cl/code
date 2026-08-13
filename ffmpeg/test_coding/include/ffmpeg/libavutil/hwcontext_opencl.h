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

#ifndef AVUTIL_HWCONTEXT_OPENCL_H
#define AVUTIL_HWCONTEXT_OPENCL_H

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include "frame.h"

/**
 * @file
 * AV_HWDEVICE_TYPE_OPENCL 专用 API 头文件。
 *
 * 内部分配的池始终为动态池，主要用于仅使用 OpenCL 的场景。需要互操作时，
 * 通常应在其他 API 中分配帧，再使用 av_hwframe_ctx_create_derived()
 * 将帧上下文映射到 OpenCL。
 */

/**
 * 用于池分配的 OpenCL 帧描述符。
 *
 * 在用户分配的池中，AVHWFramesContext.pool 必须返回 AVBufferRef，
 * 其 data 指针指向描述帧各平面的此类型对象。
 */
typedef struct AVOpenCLFrameDescriptor {
    /**
     * 帧中的平面数。
     */
    int nb_planes;
    /**
     * 帧各平面的 OpenCL image2d 对象。
     */
    cl_mem planes[AV_NUM_DATA_POINTERS];
} AVOpenCLFrameDescriptor;

/**
 * OpenCL 设备详细信息。
 *
 * 作为 AVHWDeviceContext.hwctx 分配。
 */
typedef struct AVOpenCLDeviceContext {
    /**
     * 设备的主设备 ID。如果上下文关联多个 OpenCL 设备，FFmpeg 内部的
     * 所有操作都使用此设备。
     */
    cl_device_id device_id;
    /**
     * 包含此设备上全部操作和帧的 OpenCL 上下文。
     */
    cl_context context;
    /**
     * 此设备的默认命令队列，由没有自有命令队列的所有帧上下文使用。
     * 如果用户未初始化，则在主设备上创建默认队列。
     */
    cl_command_queue command_queue;
} AVOpenCLDeviceContext;

/**
 * 与帧池关联的 OpenCL 专用数据。
 *
 * 作为 AVHWFramesContext.hwctx 分配。
 */
typedef struct AVOpenCLFramesContext {
    /**
     * 此设备上用于内部异步操作的命令队列
     * （av_hwframe_transfer_data()、av_hwframe_map()）。
     *
     * 如果未设置，则改用关联设备的命令队列。
     */
    cl_command_queue command_queue;
} AVOpenCLFramesContext;

#endif /* AVUTIL_HWCONTEXT_OPENCL_H */
