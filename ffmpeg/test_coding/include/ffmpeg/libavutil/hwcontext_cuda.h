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


#ifndef AVUTIL_HWCONTEXT_CUDA_H
#define AVUTIL_HWCONTEXT_CUDA_H

#ifndef CUDA_VERSION
#include <cuda.h>
#endif

#include <stdint.h>

#include "pixfmt.h"

/**
 * @file
 * AV_HWDEVICE_TYPE_CUDA 专用 API 头文件。
 *
 * 此 API 支持动态帧池。AVHWFramesContext.pool 必须返回 data 指针为
 * CUdeviceptr 的 AVBufferRef。
 */

typedef struct AVCUDADeviceContextInternal AVCUDADeviceContextInternal;

/**
 * 此结构体作为 AVHWDeviceContext.hwctx 分配。
 */
typedef struct AVCUDADeviceContext {
    CUcontext cuda_ctx;
    CUstream stream;
    AVCUDADeviceContextInternal *internal;
} AVCUDADeviceContext;

/**
 * 用于 AV_PIX_FMT_CUARRAY 帧池分配的 CUDA 帧描述符。
 *
 * 在用户分配的池中，AVHWFramesContext.pool 必须返回 AVBufferRef，
 * 其 data 指针指向描述帧各平面的此类型对象。
 *
 * 此结构体仅用于自定义分配；AVFrame 的 AVBufferRef 不一定指向此结构体实例。
 */
typedef struct AVCUDAArrayFrameDescriptor {
    /**
     * 包含帧数据的 CUarray。
     *
     * 通常存储在 AVFrame.data[0] 中。
     */
    CUarray array;

    /**
     * AVCUDAFramesContext.cuarray_surfaces 中的索引；不适用时为 0。
     *
     * 通常存储在 AVFrame.data[1] 中（从 intptr_t 转换）。
     */
    intptr_t index;
} AVCUDAArrayFrameDescriptor;

/**
 * 此结构体作为 AVHWFramesContext.hwctx 分配。
 */
typedef struct AVCUDAFramesContext {
    /**
     * 初始化 CUarray 时使用的 CUDA_ARRAY3D_DESCRIPTOR，主要用于提供外部 Flags。
     *
     * Width、Height 和 Format 仅在 != 0 时生效，否则使用 FramesContext 默认参数填充。
     *
     * 仅适用于 AV_PIX_FMT_CUARRAY。
     */
    CUDA_ARRAY3D_DESCRIPTOR cuarray_desc;

    /**
     * 如果 >0，则预分配固定表面池。初始化后可通过 cuarray_surfaces 访问表面，
     * 此后不能更改池大小。
     *
     * 仅适用于 AV_PIX_FMT_CUARRAY。
     */
    int cuarray_num_surfaces;

    /**
     * cuarray_num_surfaces >0 时，包含预分配表面数组。
     *
     * 仅适用于 AV_PIX_FMT_CUARRAY。
     */
    CUarray *cuarray_surfaces;
} AVCUDAFramesContext;

/**
 * CUDA 硬件流水线配置详情。
 *
 * 传给 av_hwdevice_get_hwframe_constraints() 以查询各硬件格式约束。
 * 提供时，valid_sw_formats 仅保留与指定 hw_format 兼容的格式。
 */
typedef struct AVCUDAHWConfig {
    /**
     * 要查询约束的硬件像素格式。必须为 AV_PIX_FMT_CUDA 或 AV_PIX_FMT_CUARRAY。
     */
    enum AVPixelFormat hw_format;
} AVCUDAHWConfig;

/**
 * @defgroup hwcontext_cuda 设备上下文创建标志
 *
 * av_hwdevice_ctx_create 的标志。
 *
 * @{
 */

/**
 * 使用主设备上下文，而不是创建新上下文。
 */
#define AV_CUDA_USE_PRIMARY_CONTEXT (1 << 0)

/**
 * 使用当前设备上下文，而不是创建新上下文。
 */
#define AV_CUDA_USE_CURRENT_CONTEXT (1 << 1)

/**
 * @}
 */

#endif /* AVUTIL_HWCONTEXT_CUDA_H */
