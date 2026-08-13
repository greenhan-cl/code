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

#ifndef AVUTIL_HWCONTEXT_DRM_H
#define AVUTIL_HWCONTEXT_DRM_H

#include <stddef.h>
#include <stdint.h>

/**
 * @file
 * AV_HWDEVICE_TYPE_DRM 专用 API 头文件。
 *
 * 当前不支持内部帧分配，所有帧都必须由用户分配。因此 AVHWFramesContext
 * 始终为 NULL；未来加入帧分配支持后可能改变。
 */

enum {
    /**
     * DRM 帧中的最大层数/平面数。
     */
    AV_DRM_MAX_PLANES = 4
};

/**
 * DRM 对象描述符。
 *
 * 描述单个 DRM 对象，以 PRIME 文件描述符寻址。
 */
typedef struct AVDRMObjectDescriptor {
    /**
     * 对象的 DRM PRIME 文件描述符。
     */
    int fd;
    /**
     * 对象的总大小。
     *
     * （包括不含图像数据的部分。）
     */
    size_t size;
    /**
     * 应用于对象的格式修饰符（DRM_FORMAT_MOD_*）。
     *
     * 格式修饰符未知时应设为 DRM_FORMAT_MOD_INVALID。
     */
    uint64_t format_modifier;
} AVDRMObjectDescriptor;

/**
 * DRM 平面描述符。
 *
 * 描述层中的单个平面，该平面包含在单个对象内。
 */
typedef struct AVDRMPlaneDescriptor {
    /**
     * 包含此平面的对象在所属帧描述符 objects 数组中的索引。
     */
    int object_index;
    /**
     * 此平面在该对象内的偏移。
     */
    ptrdiff_t offset;
    /**
     * 此平面的 pitch（linesize）。
     */
    ptrdiff_t pitch;
} AVDRMPlaneDescriptor;

/**
 * DRM 层描述符。
 *
 * 描述帧中的单个层。其结构由格式定义，包含一个或多个平面。
 */
typedef struct AVDRMLayerDescriptor {
    /**
     * 层的格式（DRM_FORMAT_*）。
     */
    uint32_t format;
    /**
     * 层中的平面数。
     *
     * 必须与 format 要求的平面数匹配。
     */
    int nb_planes;
    /**
     * 此层中的平面数组。
     */
    AVDRMPlaneDescriptor planes[AV_DRM_MAX_PLANES];
} AVDRMLayerDescriptor;

/**
 * DRM 帧描述符。
 *
 * 用作 AV_PIX_FMT_DRM_PRIME 帧的 data 指针，也供用户分配的帧池使用；
 * AVHWFramesContext.pool 中的分配必须返回包含此类型对象的 AVBufferRef。
 *
 * 应设置此结构体字段，使 EGL 可使用 EGL_EXT_image_dma_buf_import 和
 * EGL_EXT_image_dma_buf_import_modifiers 扩展直接导入。
 * （注意，特定格式的确切布局可能因平台而异；这里只规定同一平台应能导入它。）
 *
 * 平面总数不得超过 AV_DRM_MAX_PLANES；按层索引递增、再按平面索引递增的顺序，
 * 必须与等效软件格式中 data 指针采用的顺序相同。
 */
typedef struct AVDRMFrameDescriptor {
    /**
     * 构成此帧的 DRM 对象数量。
     */
    int nb_objects;
    /**
     * 构成帧的对象数组。
     */
    AVDRMObjectDescriptor objects[AV_DRM_MAX_PLANES];
    /**
     * 帧中的层数。
     */
    int nb_layers;
    /**
     * 帧中的层数组。
     */
    AVDRMLayerDescriptor layers[AV_DRM_MAX_PLANES];
} AVDRMFrameDescriptor;

/**
 * DRM 设备。
 *
 * 作为 AVHWDeviceContext.hwctx 分配。
 */
typedef struct AVDRMDeviceContext {
    /**
     * DRM 设备的文件描述符。
     *
     * 用作创建帧的设备，也可能用于某些派生和映射操作。
     *
     * 不需要设备时设为 -1。
     */
    int fd;
} AVDRMDeviceContext;

#endif /* AVUTIL_HWCONTEXT_DRM_H */
