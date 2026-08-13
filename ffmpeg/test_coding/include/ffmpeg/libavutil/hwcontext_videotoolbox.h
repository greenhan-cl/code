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

#ifndef AVUTIL_HWCONTEXT_VIDEOTOOLBOX_H
#define AVUTIL_HWCONTEXT_VIDEOTOOLBOX_H

#include <stdint.h>

#include <VideoToolbox/VideoToolbox.h>

#include "frame.h"
#include "pixfmt.h"

/**
 * @file
 * AV_HWDEVICE_TYPE_VIDEOTOOLBOX 专用 API 头文件。
 *
 * 此 API 支持使用原生 CVPixelBufferPool（而非 AVBufferPool）分配帧。
 *
 * 如果 API 用户设置自定义池，AVHWFramesContext.pool 必须返回数据指针为
 * CVImageBufferRef 或 CVPixelBufferRef 的 AVBufferRef。请注意，根据应用程序的
 * 使用方式，底层 CVPixelBuffer 可能由操作系统框架保留，因此最好让 CoreVideo
 * 使用默认实现来管理该池。
 *
 * 当前 AVHWDeviceContext.hwctx 始终为 NULL。
 */

typedef struct AVVTFramesContext {
    enum AVColorRange color_range;
} AVVTFramesContext;

/**
 * 将 VideoToolbox（实际为 CoreVideo）格式转换为 AVPixelFormat。
 * 找不到已知等效格式时返回 AV_PIX_FMT_NONE。
 */
enum AVPixelFormat av_map_videotoolbox_format_to_pixfmt(uint32_t cv_fmt);

/**
 * 将 AVPixelFormat 转换为 VideoToolbox（实际为 CoreVideo）格式。
 * 找不到已知等效格式时返回 0。
 */
uint32_t av_map_videotoolbox_format_from_pixfmt(enum AVPixelFormat pix_fmt);

/**
 * 与 av_map_videotoolbox_format_from_pixfmt 函数相同，但可通过标志映射并返回
 * 全范围像素格式。
 */
uint32_t av_map_videotoolbox_format_from_pixfmt2(enum AVPixelFormat pix_fmt, bool full_range);

/**
 * 将 AVChromaLocation 转换为 VideoToolbox/CoreVideo 色度位置字符串。
 * 找不到已知等效值时返回 0。
 */
CFStringRef av_map_videotoolbox_chroma_loc_from_av(enum AVChromaLocation loc);

/**
 * 将 AVColorSpace 转换为 VideoToolbox/CoreVideo 颜色矩阵字符串。
 * 找不到已知等效值时返回 0。
 */
CFStringRef av_map_videotoolbox_color_matrix_from_av(enum AVColorSpace space);

/**
 * 将 AVColorPrimaries 转换为 VideoToolbox/CoreVideo 颜色原色字符串。
 * 找不到已知等效值时返回 0。
 */
CFStringRef av_map_videotoolbox_color_primaries_from_av(enum AVColorPrimaries pri);

/**
 * 将 AVColorTransferCharacteristic 转换为 VideoToolbox/CoreVideo 颜色传递函数
 * 字符串。找不到已知等效值时返回 0。
 */
CFStringRef av_map_videotoolbox_color_trc_from_av(enum AVColorTransferCharacteristic trc);

/**
 * 根据 AVFrame 设置 CVPixelBufferRef 的元数据。
 *
 * 设置/取消设置 CVPixelBuffer 附件，使其尽可能匹配 AVFrame 元数据。为了防止
 * 附件不一致，会取消设置无法匹配或未在给定 AVFrame 中指定的属性附件。因此，
 * 如果 AVFrame 元数据已涵盖的任何附件需要设为特定值，应在调用此函数后进行。
 *
 * 出错时返回 < 0。
 */
int av_vt_pixbuf_set_attachments(void *log_ctx,
                                 CVPixelBufferRef pixbuf, const struct AVFrame *src);

#endif /* AVUTIL_HWCONTEXT_VIDEOTOOLBOX_H */
