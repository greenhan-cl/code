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

#ifndef AVUTIL_HWCONTEXT_VAAPI_H
#define AVUTIL_HWCONTEXT_VAAPI_H

#include <va/va.h>

/**
 * @file
 * AV_HWDEVICE_TYPE_VAAPI 专用 API 头文件。
 *
 * 支持动态帧池，但请注意，用作渲染目标的池必须具有固定大小，才能作为
 * vaCreateContext() 的参数使用。
 *
 * 对于用户分配的池，AVHWFramesContext.pool 必须返回数据指针设为 VASurfaceID
 * 的 AVBufferRef。
 */

enum {
    /**
     * quirks 字段已由用户设置，不应由 av_hwdevice_ctx_init() 自动检测。
     */
    AV_VAAPI_DRIVER_QUIRK_USER_SET = (1 << 0),
    /**
     * 驱动程序不会销毁 vaRenderPicture() 使用过的参数缓冲区，之后需要使用额外
     * 代码单独销毁它们。
     */
    AV_VAAPI_DRIVER_QUIRK_RENDER_PARAM_BUFFERS = (1 << 1),

    /**
     * 驱动程序不支持 VASurfaceAttribMemoryType 属性，因此表面分配代码不会尝试
     * 使用它。
     */
    AV_VAAPI_DRIVER_QUIRK_ATTRIB_MEMTYPE = (1 << 2),

    /**
     * 驱动程序完全不支持表面属性。表面分配代码绝不会将它们传给表面分配，
     * vaQuerySurfaceAttributes() 调用的结果将被模拟。
     */
    AV_VAAPI_DRIVER_QUIRK_SURFACE_ATTRIBUTES = (1 << 3),
};

/**
 * VAAPI 连接详情。
 *
 * 被分配为 AVHWDeviceContext.hwctx。
 */
typedef struct AVVAAPIDeviceContext {
    /**
     * VADisplay 句柄，由用户填充。
     */
    VADisplay display;
    /**
     * 要应用的驱动程序特殊处理——除非已经设置 AV_VAAPI_DRIVER_QUIRK_USER_SET
     * 位，否则由 av_hwdevice_ctx_init() 参考已知驱动程序表填充。之后使用相同
     * VADisplay 执行任何 VAAPI 操作时，用户可能需要参考此字段。
     */
    unsigned int driver_quirks;
} AVVAAPIDeviceContext;

/**
 * 与帧池关联的 VAAPI 专用数据。
 *
 * 被分配为 AVHWFramesContext.hwctx。
 */
typedef struct AVVAAPIFramesContext {
    /**
     * 由用户设置，以将表面属性应用于帧池中的所有表面。为 null 时使用默认设置。
     */
    VASurfaceAttrib *attributes;
    int           nb_attributes;
    /**
     * 创建后池中所有表面的表面 ID。仅当 AVHWFramesContext.initial_pool_size
     * 为正时有效。这些 ID 用作 vaCreateContext() 的 render_targets 参数。
     */
    VASurfaceID     *surface_ids;
    int           nb_surfaces;
} AVVAAPIFramesContext;

/**
 * VAAPI 硬件管线配置详情。
 *
 * 使用 av_hwdevice_hwconfig_alloc() 分配。
 */
typedef struct AVVAAPIHWConfig {
    /**
     * VAAPI 管线配置的 ID。
     */
    VAConfigID config_id;
} AVVAAPIHWConfig;

#endif /* AVUTIL_HWCONTEXT_VAAPI_H */
