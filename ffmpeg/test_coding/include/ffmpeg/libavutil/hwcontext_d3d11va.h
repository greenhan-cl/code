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

#ifndef AVUTIL_HWCONTEXT_D3D11VA_H
#define AVUTIL_HWCONTEXT_D3D11VA_H

/**
 * @file
 * API-spec如果ic header 用于 AV_HW设备_TYPE_D3D11VA.
 *
 * 默认 pool implementation will be fixed-大小 如果 initial_pool_大小 is
 * 设置 (and 分配 elements，来自 an 数组 texture). Otherwise it will 分配
 * individual textures. Be aware that 解码 requires a single 数组 texture.
 *
 * Using sw_格式==AV_PIX_FMT_YUV420P has special semantics, 和 maps to
 * DXGI_格式_420_OPAQUE. av_hw帧_transfer_data() is not supported for
 * this 格式. Refer 到 MSDN 用于 details.
 *
 * av_hw设备_ctx_创建() 用于 this 设备 type supports a key named "debug"
 * 用于 the AVDictionary entry. 如果 this is 设置 到 any 值, the 设备 creation
 * code will try 到 load various supported D3D debugging layers.
 */

#include <d3d11.h>
#include <stdint.h>

/**
 * This struct is 分配d as AVHW设备上下文.hwctx
 */
typedef struct AVD3D11VADeviceContext {
    /**
     * 设备 用于 texture creation 和 access. This can also be 用于
     * 设置 the libavcodec 解码 设备.
     *
     * Must be 设置 by the user. This is the only mandatory field - the other
     * 设备 上下文 fields are 设置，来自 this 和 are available 用于 convenience.
     *
     * Deallocating the AVHW设备上下文 will always release this interface,
     * 和 it does not matter 是否 it was user-分配d.
     */
    ID3D11Device        *device;

    /**
     * 如果 un设置, this will be 设置，来自 the 设备 field 上 init.
     *
     * Deallocating the AVHW设备上下文 will always release this interface,
     * 和 it does not matter 是否 it was user-分配d.
     */
    ID3D11DeviceContext *device_context;

    /**
     * 如果 un设置, this will be 设置，来自 the 设备 field 上 init.
     *
     * Deallocating the AVHW设备上下文 will always release this interface,
     * 和 it does not matter 是否 it was user-分配d.
     */
    ID3D11VideoDevice   *video_device;

    /**
     * 如果 un设置, this will be 设置，来自 the 设备_上下文 field 上 init.
     *
     * Deallocating the AVHW设备上下文 will always release this interface,
     * 和 it does not matter 是否 it was user-分配d.
     */
    ID3D11VideoContext  *video_context;

    /**
     * Callbacks 用于 locking. They protect accesses 到 设备_上下文 and
     * 视频_上下文 calls. They also protect access 到 the internal staging
     * texture (for av_hw帧_transfer_data() calls). They do NOT protect
     * access 到 hw上下文 或 解码器 state 中 general.
     *
     * 如果 un设置 上 init, the hw上下文 implementation will 设置 them 到 use an
     * internal mutex.
     *
     * underlying lock must be recursive. lock_ctx is 用于 释放 use by the
     * locking implementation.
     */
    void (*lock)(void *lock_ctx);
    void (*unlock)(void *lock_ctx);
    void *lock_ctx;
    /**
     * D3D11_TEXTURE2D_DESC.Bind标志 到 be applied 到 D3D11 resources 分配d
     * 用于 帧s using this 设备 上下文.
     *
     * It applies globally 到 all AVD3D11VA帧s上下文 分配d，来自 this 设备 上下文.
     */
    UINT BindFlags;
    /**
     * D3D11_TEXTURE2D_DESC.Misc标志 到 be applied 到 D3D11 resources 分配d
     * 用于 帧s using this 设备 上下文.
     *
     * It applies globally 到 all AVD3D11VA帧s上下文 分配d，来自 this 设备 上下文.
     */
    UINT MiscFlags;
} AVD3D11VADeviceContext;

/**
 * D3D11 帧 描述符 用于 pool allocation.
 *
 * In user-分配d pools, AVHW帧s上下文.pool must 返回 AV缓冲区Refs
 *，使用 the data 指针 pointing at an object 的 this type describing the
 * planes 的 the 帧.
 *
 * This has no use outside 的 custom allocation, 和 AV帧 AV缓冲区Ref do not
 * necessarily point 到 an instance 的 this struct.
 */
typedef struct AVD3D11FrameDescriptor {
    /**
     * texture 中 which the 帧 is located. 引用 count is
     * managed by the AV缓冲区Ref, 和 destroying the 引用 will release
     * the interface.
     *
     * Normally stored 中 AV帧.data[0].
     */
    ID3D11Texture2D *texture;

    /**
     * index into the 数组 texture element representing the 帧, 或 0
     * 如果 the texture is not an 数组 texture.
     *
     * Normally stored 中 AV帧.data[1] (cast，来自 intptr_t).
     */
    intptr_t index;
} AVD3D11FrameDescriptor;

/**
 * This struct is 分配d as AVHW帧s上下文.hwctx
 */
typedef struct AVD3D11VAFramesContext {
    /**
     * canonical texture 用于 pool allocation. 如果 this is 设置 到 NULL
     * 上 init, the hw帧s implementation will 分配 和 设置 an 数组
     * texture 如果 initial_pool_大小 > 0.
     *
     * only situation 当 the API user should 设置 this is:
     * - the user wants 到 do manual pool allocation (设置ting
     *   AVHW帧s上下文.pool), instead 的 letting AVHW帧s上下文
     *   分配 the pool
     * - 的 an 数组 texture
     * - 和 wants it 到 use it 用于 解码
     * - this has 到 be done before calling av_hw帧_ctx_init()
     *
     * Deallocating the AVHW帧s上下文 will always release this interface,
     * 和 it does not matter 是否 it was user-分配d.
     *
     * This is 中 particular used by the libavcodec D3D11Vhwaccel, which
     * requires a single 数组 texture. It will 创建 ID3D11视频解码器输出View
     * objects 用于 each 数组 texture element 上 解码器 initialization.
     */
    ID3D11Texture2D *texture;

    /**
     * D3D11_TEXTURE2D_DESC.Bind标志 用于 texture creation. user must
     * at least 设置 D3D11_BIND_解码器 如果 the 帧s 上下文 is 到 be 用于
     * 视频 解码.
     * This field is ignored/invalid 如果 a user-分配d texture is provided.
     */
    UINT BindFlags;

    /**
     * D3D11_TEXTURE2D_DESC.Misc标志 用于 texture creation.
     * This field is ignored/invalid 如果 a user-分配d texture is provided.
     */
    UINT MiscFlags;

    /**
     * In case 如果 texture 结构体 member above is not NULL 包含 same texture
     * 指针 用于 all elements 和 d如果ferent indexes into the 数组 texture.
     * In case 如果 texture 结构体 member above is NULL, all elements 包含
     * 指针s 到 separate non-数组 textures 和 0 indexes.
     * This field is ignored/invalid 如果 a user-分配d texture is provided.
    */
    AVD3D11FrameDescriptor *texture_infos;
} AVD3D11VAFramesContext;

#endif /* AVUTIL_HWCONTEXT_D3D11VA_H */
