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

#ifndef AVUTIL_DETECTION_BBOX_H
#define AVUTIL_DETECTION_BBOX_H

#include "rational.h"
#include "avassert.h"
#include "frame.h"

typedef struct AVDetectionBBox {
    /**
     * 与帧左边缘/上边缘的像素距离，与宽度和高度一起定义边界框。
     */
    int x;
    int y;
    int w;
    int h;

#define AV_DETECTION_BBOX_LABEL_NAME_MAX_SIZE 64

    /**
     * 带置信度的检测结果
     */
    char detect_label[AV_DETECTION_BBOX_LABEL_NAME_MAX_SIZE];
    AVRational detect_confidence;

    /**
     * 基于检测边界框的分类最多有 4 个。例如，可以在一个边界框上使用 4 个不同的
     * DNN 模型获得最多 4 个不同属性。没有分类时 classify_count 为零。
     */
#define AV_NUM_DETECTION_BBOX_CLASSIFY 4
    uint32_t classify_count;
    char classify_labels[AV_NUM_DETECTION_BBOX_CLASSIFY][AV_DETECTION_BBOX_LABEL_NAME_MAX_SIZE];
    AVRational classify_confidences[AV_NUM_DETECTION_BBOX_CLASSIFY];
} AVDetectionBBox;

typedef struct AVDetectionBBoxHeader {
    /**
     * 边界框生成方式的信息，例如 DNN 模型名称。
     */
    char source[256];

    /**
     * 数组中的边界框数量。
     */
    uint32_t nb_bboxes;

    /**
     * 边界框数组起始处相对于此结构开头的字节偏移量。
     */
    size_t bboxes_offset;

    /**
     * 每个边界框的字节大小。
     */
    size_t bbox_size;
} AVDetectionBBoxHeader;

/*
 * 获取指定 {@code idx} 处的边界框。idx 必须在 0 和 nb_bboxes 之间。
 */
static av_always_inline AVDetectionBBox *
av_get_detection_bbox(const AVDetectionBBoxHeader *header, unsigned int idx)
{
    av_assert0(idx < header->nb_bboxes);
    return (AVDetectionBBox *)((uint8_t *)header + header->bboxes_offset +
                               idx * header->bbox_size);
}

/**
 * 为 AVDetectionBBoxHeader 以及包含 {@code nb_bboxes} 个 AVDetectionBBox 的数组
 * 分配内存，并初始化变量。可以使用普通的 av_free() 调用释放。
 *
 * @param nb_bboxes 要分配的 AVDetectionBBox 结构数量
 * @param out_size 非 NULL 时，会在这里写入结果数据数组的字节大小。
 */
AVDetectionBBoxHeader *av_detection_bbox_alloc(uint32_t nb_bboxes, size_t *out_size);

/**
 * 在给定 AVFrame {@code frame} 中，以 AV_FRAME_DATA_DETECTION_BBOXES 类型的
 * AVFrameSideData 形式，为 AVDetectionBBoxHeader 以及包含 {@code nb_bboxes}
 * 个 AVDetectionBBox 的数组分配内存，并初始化变量。
 */
AVDetectionBBoxHeader *av_detection_bbox_create_side_data(AVFrame *frame, uint32_t nb_bboxes);
#endif
