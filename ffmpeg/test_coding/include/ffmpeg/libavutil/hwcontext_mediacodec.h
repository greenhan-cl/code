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

#ifndef AVUTIL_HWCONTEXT_MEDIACODEC_H
#define AVUTIL_HWCONTEXT_MEDIACODEC_H

/**
 * MediaCodec 详情。
 *
 * 被分配为 AVHWDeviceContext.hwctx。
 */
typedef struct AVMediaCodecDeviceContext {
    /**
     * android/view/Surface 句柄，由用户填充。
     *
     * 这是此设备上的解码器使用的默认表面。
     */
    void *surface;

    /**
     * 指向 ANativeWindow 的指针。
     *
     * 如果 surface 和 native_window 均为 NULL，且 create_window 为 true、
     * 操作系统支持 createPersistentInputSurface，则尝试自动创建它。
     *
     * 它可以用作解码器的输出表面和编码器的输入表面。
     */
    void *native_window;

    /**
     * 自动启用 createPersistentInputSurface。
     *
     * 默认禁用。
     *
     * 可以直接设置此标志启用，也可以设置 av_hwdevice_ctx_create() 的
     * AVDictionary，并使用 "create_window" 作为键。第二种方法适用于 ffmpeg
     * 命令行，例如可通过以下方式启用：
     *   -init_hw_device mediacodec=mediacodec,create_window=1
     */
    int create_window;
} AVMediaCodecDeviceContext;

#endif /* AVUTIL_HWCONTEXT_MEDIACODEC_H */
