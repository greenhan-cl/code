/*
 * Direct3D 12 HW acceleration.
 *
 * copyright (c) 2022-2023 Wu Jianhua <toqsxw@outlook.com>
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

#ifndef AVUTIL_HWCONTEXT_D3D12VA_H
#define AVUTIL_HWCONTEXT_D3D12VA_H

/**
 * @file
 * AV_HWDEVICE_TYPE_D3D12VA 专用 API 头文件。
 *
 * AVHWFramesContext.pool 必须包含 data 指针指向 AVD3D12VAFrame 结构体的 AVBufferRef。
 */
#include <stdint.h>
#include <initguid.h>
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3d12video.h>

/**
 * @brief 此结构体作为 AVHWDeviceContext.hwctx 分配
 *
 */
typedef struct AVD3D12VADeviceContext {
    /**
     * 用于创建和访问对象的设备，也可用于设置 libavcodec 解码设备。
     *
     * 可由用户设置。这是唯一必填字段，其他设备上下文字段由此设置，仅为方便使用。
     *
     * 释放 AVHWDeviceContext 时始终释放此接口，无论它是否由用户分配。
     */
    ID3D12Device *device;

    /**
     * 未设置时，将在初始化时根据 device 字段设置。
     *
     * 释放 AVHWDeviceContext 时始终释放此接口，无论它是否由用户分配。
     */
    ID3D12VideoDevice *video_device;

    /**
     * 加锁回调。用于保护对内部暂存纹理的访问（供 av_hwframe_transfer_data() 调用），
     * 但不保护对 hwcontext 或一般解码器状态的访问。
     *
     * 初始化时未设置，则 hwcontext 实现会将其设为使用内部互斥锁。
     *
     * 底层锁必须可递归。locking 实现可自由使用 lock_ctx。
     */
    void (*lock)(void *lock_ctx);
    void (*unlock)(void *lock_ctx);
    void *lock_ctx;

    /**
     * 应用于使用此设备上下文为帧分配的 D3D12 资源的资源标志。
     *
     * 未设置时为 D3D12_RESOURCE_FLAG_NONE。
     *
     * 全局应用于从此设备上下文分配的所有 AVD3D12VAFramesContext。
     */
    D3D12_RESOURCE_FLAGS resource_flags;

    /**
     * 应用于使用此设备上下文为帧分配的 D3D12 资源的堆标志。
     *
     * 未设置时为 D3D12_HEAP_FLAG_NONE。
     *
     * 全局应用于从此设备上下文分配的所有 AVD3D12VAFramesContext。
     */
    D3D12_HEAP_FLAGS heap_flags;
} AVD3D12VADeviceContext;

/**
 * @brief 此结构体用于同步 D3D12 执行
 *
 */
typedef struct AVD3D12VASyncContext {
    /**
     * D3D12 fence 对象
     */
    ID3D12Fence *fence;

    /**
     * fence 达到特定值时触发的事件对象句柄。
     */
    HANDLE event;

    /**
     * 用于同步的 fence 值
     */
    uint64_t fence_value;
} AVD3D12VASyncContext;

/**
 * 定义帧分配行为。
 */
typedef enum AVD3D12VAFrameFlags {
    AV_D3D12VA_FRAME_FLAG_NONE = 0,

    /**
     * 表示应使用纹理数组资源分配帧数据。
     */
    AV_D3D12VA_FRAME_FLAG_TEXTURE_ARRAY = (1 << 1),
} AVD3D12VAFrameFlags;

/**
 * @brief 用于池分配的 D3D12VA 帧描述符。
 *
 */
typedef struct AVD3D12VAFrame {
    /**
     * 帧所在的纹理。引用计数由 AVBufferRef 管理，销毁引用会释放接口。
     */
    ID3D12Resource *texture;

    /**
     * 纹理内子资源的索引。
     *
     * 纹理数组模式下指定数组切片索引；不使用纹理数组模式时始终为 0。
     */
    int subresource_index;

    /**
     * 纹理的同步上下文
     *
     * @see: https://learn.microsoft.com/en-us/windows/win32/medfound/direct3d-12-video-overview#directx-12-fences
     */
    AVD3D12VASyncContext sync_ctx;

    /**
     * AVD3D12VAFrameFlags 的组合。由 AVD3D12VAFramesContext 设置。
     */
    AVD3D12VAFrameFlags flags;
} AVD3D12VAFrame;

/**
 * @brief 此结构体作为 AVHWFramesContext.hwctx 分配
 *
 */
typedef struct AVD3D12VAFramesContext {
    /**
     * DXGI_FORMAT 格式。必须与像素格式兼容；未设置时自动设置。
     */
    DXGI_FORMAT format;

    /**
     * 处理资源的选项。未设置时为 D3D12_RESOURCE_FLAG_NONE。
     *
     * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_resource_flags
     */
    D3D12_RESOURCE_FLAGS resource_flags;

    /**
     * 创建资源时处理堆分配的选项。未设置时为 D3D12_HEAP_FLAG_NONE。
     *
     * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_heap_flags
     */
    D3D12_HEAP_FLAGS heap_flags;

    /**
     * 纹理数组模式下，D3D12 对所有图像使用同一个纹理数组（资源）。
     */
    ID3D12Resource *texture_array;

    /**
     * AVD3D12VAFrameFlags 的组合。除非设置 AV_D3D12VA_FRAME_FLAG_NONE，
     * av_hwframe_ctx_init() 期间会根据设备和帧特征按位 OR 自动检测的标志。
     */
    AVD3D12VAFrameFlags flags;
} AVD3D12VAFramesContext;

#endif /* AVUTIL_HWCONTEXT_D3D12VA_H */
