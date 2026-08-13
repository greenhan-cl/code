/*
 * The Video Decode and Presentation API for UNIX (VDPAU) is used for
 * hardware-accelerated decoding of MPEG-1/2, H.264 and VC-1.
 *
 * Copyright (C) 2008 NVIDIA
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

#ifndef AVCODEC_VDPAU_H
#define AVCODEC_VDPAU_H

/**
 * @file
 * @ingroup lavc_codec_hwaccel_vdpau
 * libavcodec 的公共 VDPAU 头文件。
 */


/**
 * @defgroup lavc_codec_hwaccel_vdpau VDPAU Decoder and Renderer
 * @ingroup lavc_codec_hwaccel
 *
 * VDPAU 硬件加速包含两个模块：
 * - VDPAU 解码
 * - VDPAU 呈现
 *
 * VDPAU 解码模块使用 FFmpeg 解析机制解析所有头信息，
 * 并使用 VDPAU 执行实际解码。
 *
 * 按照当前实现，实际解码和渲染（API 调用）作为 VDPAU 呈现模块
 * （vo_vdpau.c）的一部分执行。
 *
 * @{
 */

#include <vdpau/vdpau.h>

#include "libavutil/avconfig.h"
#include "libavutil/attributes.h"

#include "avcodec.h"

struct AVCodecContext;
struct AVFrame;

typedef int (*AVVDPAU_Render2)(struct AVCodecContext *, struct AVFrame *,
                               const VdpPictureInfo *, uint32_t,
                               const VdpBitstreamBuffer *);

/**
 * 此结构体用于在 libavcodec 库与客户端视频应用程序之间共享数据。
 * av_vdpau_bind_context() 会分配此结构体并将其存储在
 * AVCodecContext.hwaccel_context 中。用户可以在初始化期间设置一次成员，
 * 或在每次调用 AVCodecContext.get_buffer() 时设置。无论哪种方式，
 * 调用解码函数前这些成员都必须有效。
 *
 * 此结构体的大小不属于公共 ABI，不得在 libavcodec 外部使用。
 */
typedef struct AVVDPAUContext {
    /**
     * VDPAU 解码器句柄
     *
     * 由用户设置。
     */
    VdpDecoder decoder;

    /**
     * VDPAU 解码器渲染回调
     *
     * 由用户设置。
     */
    VdpDecoderRender *render;

    AVVDPAU_Render2 render2;
} AVVDPAUContext;

/**
 * 将 VDPAU 设备与编解码器上下文关联，以进行硬件加速。
 * 此函数应从 get_format() 编解码器回调中调用，或更早调用。
 * 也可在 avcodec_flush_buffers() 后调用，以便在流处理中途更换底层 VDPAU 设备
 * （例如从非透明显示抢占中恢复）。
 *
 * @note 如果此函数成功完成，get_format() 必须返回 AV_PIX_FMT_VDPAU。
 *
 * @param avctx 调用了 get_format() 回调的解码上下文
 * @param device 用于硬件加速的 VDPAU 设备句柄
 * @param get_proc_address VDPAU 设备驱动程序
 * @param flags 零个或多个按位 OR 组合的 AV_HWACCEL_FLAG_* 标志
 *
 * @return 成功返回 0，失败返回 AVERROR 错误码。
 */
int av_vdpau_bind_context(AVCodecContext *avctx, VdpDevice device,
                          VdpGetProcAddress *get_proc_address, unsigned flags);

/**
 * 获取相关参数，以便为使用 VDPAU 硬件解码加速的编解码器上下文
 * 创建合适的 VDPAU 视频表面。
 *
 * @note 如果没有使用 av_vdpau_bind_context() 将上下文成功绑定到 VDPAU 设备，
 * 则行为未定义。
 *
 * @param avctx 用于解码流的编解码器上下文
 * @param type 用于保存 VDPAU 视频表面色度类型的空间（传入 NULL 可忽略）
 * @param width 用于保存 VDPAU 视频表面像素宽度的空间（传入 NULL 可忽略）
 * @param height 用于保存 VDPAU 视频表面像素高度的空间（传入 NULL 可忽略）
 *
 * @return 成功返回 0，失败返回负的 AVERROR 错误码。
 */
int av_vdpau_get_surface_parameters(AVCodecContext *avctx, VdpChromaType *type,
                                    uint32_t *width, uint32_t *height);

/** @} */

#endif /* AVCODEC_VDPAU_H */
