/*
 * Codec descriptors public API
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

#ifndef AVCODEC_CODEC_DESC_H
#define AVCODEC_CODEC_DESC_H

#include "libavutil/avutil.h"

#include "codec_id.h"

/**
 * @addtogroup lavc_core
 * @{
 */

/**
 * 此结构体描述由 AVCodecID 标识的单个编解码器的属性。
 * @see avcodec_descriptor_get()
 */
typedef struct AVCodecDescriptor {
    enum AVCodecID     id;
    enum AVMediaType type;
    /**
     * 此描述符所描述编解码器的名称。名称非空，且对每个编解码器描述符唯一。
     * 名称只能包含字母、数字和 '_'。
     */
    const char      *name;
    /**
     * 此编解码器更具描述性的名称，可以为 NULL。
     */
    const char *long_name;
    /**
     * 编解码器属性，由 AV_CODEC_PROP_* 标志组合而成。
     */
    int             props;
    /**
     * 与编解码器关联的 MIME 类型。
     * 可以为 NULL；非 NULL 时，是以 NULL 结尾的 MIME 类型数组。
     * 第一项始终非 NULL，且是首选 MIME 类型。
     */
    const char *const *mime_types;
    /**
     * 非 NULL 时，是此编解码器可识别的配置文件数组。
     * 以 AV_PROFILE_UNKNOWN 结尾。
     */
    const struct AVProfile *profiles;
} AVCodecDescriptor;

/**
 * 编解码器仅使用帧内压缩。
 * 仅适用于视频和音频编解码器。
 */
#define AV_CODEC_PROP_INTRA_ONLY    (1 << 0)
/**
 * 编解码器支持有损压缩。仅适用于音频和视频编解码器。
 * @note 编解码器可以同时支持有损和无损压缩模式
 */
#define AV_CODEC_PROP_LOSSY         (1 << 1)
/**
 * 编解码器支持无损压缩。仅适用于音频和视频编解码器。
 */
#define AV_CODEC_PROP_LOSSLESS      (1 << 2)
/**
 * 编解码器支持帧重排。也就是说，编码顺序（编码器输出数据包、存储数据包或
 * 向解码器输入数据包的顺序）可能不同于对应帧的呈现顺序。
 *
 * 对于未设置此属性的编解码器，PTS 和 DTS 应始终相等。
 */
#define AV_CODEC_PROP_REORDER       (1 << 3)

/**
 * 视频编解码器支持分别编码隔行帧中的场。
 */
#define AV_CODEC_PROP_FIELDS        (1 << 4)

/**
 * 视频编解码器包含要应用于其他现有帧的增强信息，自身无法生成可用图像数据。
 * 通常不会提供相应的独立解码器，也不应对此有所预期。
 */
#define AV_CODEC_PROP_ENHANCEMENT   (1 << 5)

/**
 * 字幕编解码器基于位图。
 * 可从 AVSubtitleRect->pict 字段读取解码后的 AVSubtitle 数据。
 */
#define AV_CODEC_PROP_BITMAP_SUB    (1 << 16)
/**
 * 字幕编解码器基于文本。
 * 可从 AVSubtitleRect->ass 字段读取解码后的 AVSubtitle 数据。
 */
#define AV_CODEC_PROP_TEXT_SUB      (1 << 17)

/**
 * @return 给定编解码器 ID 的描述符；不存在时返回 NULL。
 */
const AVCodecDescriptor *avcodec_descriptor_get(enum AVCodecID id);

/**
 * 遍历 libavcodec 已知的所有编解码器描述符。
 *
 * @param prev 上一个描述符。传入 NULL 可获取第一个描述符。
 *
 * @return 下一个描述符；最后一个描述符之后返回 NULL
 */
const AVCodecDescriptor *avcodec_descriptor_next(const AVCodecDescriptor *prev);

/**
 * @return 具有给定名称的编解码器描述符；不存在时返回 NULL。
 */
const AVCodecDescriptor *avcodec_descriptor_get_by_name(const char *name);

/**
 * @}
 */

#endif // AVCODEC_CODEC_DESC_H
