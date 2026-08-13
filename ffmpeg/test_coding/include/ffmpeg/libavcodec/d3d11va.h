/*
 * Direct3D11 HW acceleration
 *
 * copyright (c) 2009 Laurent Aimar
 * copyright (c) 2015 Steve Lhomme
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

#ifndef AVCODEC_D3D11VA_H
#define AVCODEC_D3D11VA_H

/**
 * @file
 * @ingroup lavc_codec_hwaccel_d3d11va
 * libavcodec 的公共 D3D11VA 头文件。
 */

#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0602
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif

#include <stdint.h>
#include <d3d11.h>

/**
 * @defgroup lavc_codec_hwaccel_d3d11va Direct3D11
 * @ingroup lavc_codec_hwaccel
 *
 * @{
 */

/**
 * 此结构体用于向 FFmpeg 的 Direct3D11 HWAccel 实现提供必要的配置和数据。
 *
 * 应用程序必须通过 AVCodecContext.hwaccel_context 提供此结构体。
 *
 * 必须仅使用 av_d3d11va_alloc_context() 分配 AVD3D11VAContext。
 */
typedef struct AVD3D11VAContext {
    /**
     * D3D11 解码器对象
     */
    ID3D11VideoDecoder *decoder;

    /**
      * D3D11 VideoContext
      */
    ID3D11VideoContext *video_context;

    /**
     * 用于创建解码器的 D3D11 配置
     */
    D3D11_VIDEO_DECODER_CONFIG *cfg;

    /**
     * surface 数组中的表面数量
     */
    unsigned surface_count;

    /**
     * 用于创建解码器的 Direct3D 表面数组
     */
    ID3D11VideoDecoderOutputView **surface;

    /**
     * 配置使用解码器所需规避措施的位字段
     */
    uint64_t workaround;

    /**
     * FFmpeg AVHWAccel 实现的私有字段
     */
    unsigned report_id;

    /**
      * 访问 video_context 的互斥锁
      */
    HANDLE  context_mutex;
} AVD3D11VAContext;

/**
 * 分配 AVD3D11VAContext。
 *
 * @return 新分配的 AVD3D11VAContext，失败时返回 NULL。
 */
AVD3D11VAContext *av_d3d11va_alloc_context(void);

/**
 * @}
 */

#endif /* AVCODEC_D3D11VA_H */
