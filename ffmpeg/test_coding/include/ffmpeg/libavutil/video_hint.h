/**
 * Copyright 2023 Elias Carotti <eliascrt at amazon dot it>
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

#ifndef AVUTIL_VIDEO_HINT_H
#define AVUTIL_VIDEO_HINT_H

#include <stddef.h>
#include <stdint.h>
#include "libavutil/avassert.h"
#include "libavutil/frame.h"

typedef struct AVVideoRect {
    uint32_t x, y;
    uint32_t width, height;
} AVVideoRect;

typedef enum AVVideoHintType {
    /* 矩形界定恒定（未改变）区域，默认区域为已改变 */
    AV_VIDEO_HINT_TYPE_CONSTANT,

    /* 矩形界定已改变区域，默认区域为未改变 */
    AV_VIDEO_HINT_TYPE_CHANGED,
} AVVideoHintType;

typedef struct AVVideoHint {
    /**
     * 当前 AVVideoRect 的数量。
     *
     * 可以为 0，此时不存在逐矩形信息；rect_offset / rect_size 的值未指定，不应访问。
     */
    size_t nb_rects;

    /**
     * AVVideoRect 数组相对此结构体起始位置的字节偏移。
     */
    size_t rect_offset;

    /**
     * AVVideoRect 的大小，单位为字节。
     */
    size_t rect_size;

    AVVideoHintType type;
} AVVideoHint;

static av_always_inline AVVideoRect *
av_video_hint_rects(const AVVideoHint *hints) {
    return (AVVideoRect *)((uint8_t *)hints + hints->rect_offset);
}

static av_always_inline AVVideoRect *
av_video_hint_get_rect(const AVVideoHint *hints, size_t idx) {
    return (AVVideoRect *)((uint8_t *)hints + hints->rect_offset + idx * hints->rect_size);
}

/**
 * 为 AVVideoHint 结构体及包含 nb_rects 个元素的 AVVideoRect 数组分配内存。
 *
 * 侧数据包含矩形列表，表示相对上一编码帧已改变或未改变的区域（取决于 type 参数），
 * 其余区域则具有相反状态。宏块据此被提示使用 P_SKIP 或执行常规编码过程。
 *
 * 调用方负责相应填充 AVRect，并设置正确的 AVVideoHintType 字段。
 *
 * @param out_size 非 NULL 时，在此写入结果数据数组的字节大小
 *
 * @return 成功返回新分配的 AVVideoHint（调用方必须使用 av_free() 释放），
 *         内存分配失败返回 NULL
 */
AVVideoHint *av_video_hint_alloc(size_t nb_rects,
                                 size_t *out_size);

/**
 * 与 av_video_hint_alloc() 相同，但新分配的 AVVideoHint 会作为
 * AV_FRAME_DATA_VIDEO_HINT_INFO 类型侧数据附加到 frame。
 */
AVVideoHint *av_video_hint_create_side_data(AVFrame *frame,
                                            size_t nb_rects);


#endif /* AVUTIL_VIDEO_HINT_H */
