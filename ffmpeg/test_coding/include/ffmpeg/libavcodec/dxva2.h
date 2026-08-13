/*
 * DXVA2 HW acceleration
 *
 * copyright (c) 2009 Laurent Aimar
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

#ifndef AVCODEC_DXVA2_H
#define AVCODEC_DXVA2_H

/**
 * @file
 * @ingroup lavc_codec_hwaccel_dxva2
 * libavcodec 的公共 DXVA2 头文件。
 */

#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0602
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif

#include <stdint.h>
#include <d3d9.h>
#include <dxva2api.h>

/**
 * @defgroup lavc_codec_hwaccel_dxva2 DXVA2
 * @ingroup lavc_codec_hwaccel
 *
 * @{
 */

/**
 * 此结构体用于向 FFmpeg 的 DXVA2 HWAccel 实现提供必要的配置和数据。
 *
 * 应用程序必须通过 AVCodecContext.hwaccel_context 提供此结构体。
 */
struct dxva_context {
    /**
     * DXVA2 解码器对象
     */
    IDirectXVideoDecoder *decoder;

    /**
     * 用于创建解码器的 DXVA2 配置
     */
    const DXVA2_ConfigPictureDecode *cfg;

    /**
     * surface 数组中的表面数量
     */
    unsigned surface_count;

    /**
     * 用于创建解码器的 Direct3D 表面数组
     */
    LPDIRECT3DSURFACE9 *surface;

    /**
     * 配置使用解码器所需规避措施的位字段
     */
    uint64_t workaround;

    /**
     * FFmpeg AVHWAccel 实现的私有字段
     */
    unsigned report_id;
};

/**
 * @}
 */

#endif /* AVCODEC_DXVA2_H */
