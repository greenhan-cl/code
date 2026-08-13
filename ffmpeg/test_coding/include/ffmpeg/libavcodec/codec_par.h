/*
 * Codec parameters public API
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

#ifndef AVCODEC_CODEC_PAR_H
#define AVCODEC_CODEC_PAR_H

#include <stdint.h>

#include "libavutil/avutil.h"
#include "libavutil/channel_layout.h"
#include "libavutil/rational.h"
#include "libavutil/pixfmt.h"

#include "codec_id.h"
#include "defs.h"
#include "packet.h"

/**
 * @defgroup lavc_codec_params 编解码器参数
 * @ingroup lavc_core
 * @{
 */

/**
 * 此结构体描述编码流的属性。
 *
 * @note
 * `sizeof(AVCodecParameters)` 不属于公共 ABI，因此必须使用
 * ::avcodec_parameters_alloc() 分配此结构体，并使用
 * ::avcodec_parameters_free() 释放。
 */
typedef struct AVCodecParameters {
    /**
     * 编码数据的通用类型。
     */
    enum AVMediaType codec_type;
    /**
     * 编码数据的具体类型（所使用的编解码器）。
     */
    enum AVCodecID   codec_id;
    /**
     * 编解码器的附加信息（对应 AVI FOURCC）。
     */
    uint32_t         codec_tag;

    /**
     * 初始化解码器所需的额外二进制数据，具体内容取决于编解码器。
     *
     * 必须使用 ::av_malloc() 分配，并由 ::avcodec_parameters_free() 释放。
     * extradata 的分配大小必须至少为 #extradata_size +
     * ::AV_INPUT_BUFFER_PADDING_SIZE，且填充字节必须清零。
     */
    uint8_t *extradata;
    /**
     * extradata 内容的大小，单位为字节。
     */
    int      extradata_size;

    /**
     * 与整个流关联的附加数据。
     *
     * 应使用 ::av_packet_side_data_new() 或 ::av_packet_side_data_add() 分配，
     * 并由 ::avcodec_parameters_free() 释放。
     */
    AVPacketSideData *coded_side_data;

    /**
     * #coded_side_data 中的条目数。
     */
    int nb_coded_side_data;

    /**
     * - 视频：像素格式，值对应枚举 ::AVPixelFormat。
     * - 音频：采样格式，值对应枚举 ::AVSampleFormat。
     */
    int format;

    /**
     * 编码数据的平均码率，单位为 bit/s。
     */
    int64_t bit_rate;

    /**
     * 码字中每个采样所占的位数。
     *
     * 这基本上是每个采样的比特率。许多格式必须提供此值才能真正解码。
     * 它表示实际编码比特流中一个采样所占的位数。
     *
     * 例如，对 ADPCM 而言此值可以为 4。
     * 对 PCM 格式而言，它与 #bits_per_raw_sample 相同。
     *
     * 可以为 0。
     */
    int bits_per_coded_sample;

    /**
     * 每个输出采样中的有效位数。
     *
     * 如果采样格式具有更多位，则最低有效位是额外的填充位，并且始终为 0。
     * 使用右移将采样缩减到其实际大小。
     *
     * 例如，使用 24 位采样的音频格式会将 #bits_per_raw_sample 设为 24，
     * 将 ::format 设为 ::AV_SAMPLE_FMT_S32。要获得原始采样，请使用：
     * `(int32_t)sample >> 8`。
     *
     * 对 ADPCM 而言，此值可能是 12、16 或类似值。
     *
     * 可以为 0。
     */
    int bits_per_raw_sample;

    /**
     * 该流遵循的编解码器特定比特流限制。
     */
    int profile;
    int level;

    /**
     * 视频帧宽度，单位为像素。
     *
     * 仅用于视频。
     */
    int width;

    /**
     * 视频帧高度，单位为像素。
     *
     * 仅用于视频。
     */
    int height;

    /**
     * 单个像素显示时应具有的宽高比（宽度/高度）。
     *
     * 宽高比未知或未定义时，分子应设为 0（分母可以为任意值）。
     *
     * 仅用于视频。
     */
    AVRational sample_aspect_ratio;

    /**
     * 对帧时长恒定的流，表示每秒帧数。部分帧时长不同或该值未知时，
     * 应设为 `{ 0, 1 }`。
     *
     * @note 此字段对应存储在编解码器级头信息中的值。存在容器/传输层时间戳时，
     * 通常会被后者覆盖。因此仅当没有更高层时间信息时，才应将其作为最后手段使用。
     *
     * 仅用于视频。
     */
    AVRational framerate;

    /**
     * 隔行视频中的场顺序。
     *
     * 仅用于视频。
     */
    enum AVFieldOrder                  field_order;

    /**
     * 额外的色彩空间特性。
     *
     * 仅用于视频。
     */
    enum AVColorRange                  color_range;
    enum AVColorPrimaries              color_primaries;
    enum AVColorTransferCharacteristic color_trc;
    enum AVColorSpace                  color_space;
    enum AVChromaLocation              chroma_location;

    /**
     * 延迟帧数量。
     *
     * 仅用于视频。
     */
    int video_delay;

    /**
     * 声道布局和声道数。
     *
     * 仅用于音频。
     */
    AVChannelLayout ch_layout;
    /**
     * 每秒音频采样数。
     *
     * 仅用于音频。
     */
    int      sample_rate;
    /**
     * 每个编码音频帧的字节数，某些格式需要此值。
     *
     * 仅用于音频。
     *
     * 对应 WAVEFORMATEX 中的 nBlockAlign。
     */
    int      block_align;
    /**
     * 音频帧大小（如果已知）。某些格式要求此值固定。
     *
     * 仅用于音频。
     */
    int      frame_size;

    /**
     * 开头的音频填充采样数。
     *
     * 编码器在音频开头插入的填充量（以采样数计）。也就是说，必须丢弃这么多
     * 位于开头的解码采样，才能获得不含前导填充的原始音频。
     *
     * 仅用于音频。
     */
    int initial_padding;
    /**
     * 末尾的音频填充采样数。
     *
     * 编码器附加到音频末尾的填充量（以采样数计）。也就是说，必须从流末尾
     * 丢弃这么多解码采样，才能获得不含尾随填充的原始音频。
     *
     * 仅用于音频。
     */
    int trailing_padding;
    /**
     * 发生不连续后要跳过的音频采样数。
     *
     * 仅用于音频。
     */
    int seek_preroll;

    /**
     * 仅适用于带 alpha 通道的视频。alpha 通道处理方式。
     */
    enum AVAlphaMode alpha_mode;
} AVCodecParameters;

/**
 * @relates AVCodecParameters
 * @{
 */

/**
 * 分配新的 AVCodecParameters，并将其字段设为默认值（未知/无效/0）。
 * 返回的结构体必须使用 ::avcodec_parameters_free() 释放。
 */
AVCodecParameters *avcodec_parameters_alloc(void);

/**
 * 释放 AVCodecParameters 实例及其关联的所有内容，并向提供的指针写入 `NULL`。
 */
void avcodec_parameters_free(AVCodecParameters **par);

/**
 * 将 \p src 的内容复制到 \p dst。dst 中已分配的字段会被释放，
 * 并替换为 src 对应字段的新分配副本。
 *
 * @return 成功返回 >= 0，失败返回负的 AVERROR 错误码。
 */
int avcodec_parameters_copy(AVCodecParameters *dst, const AVCodecParameters *src);

/**
 * 此函数与 ::av_get_audio_frame_duration() 相同，但它使用
 * ::AVCodecParameters 而不是 ::AVCodecContext。
 */
int av_get_audio_frame_duration2(AVCodecParameters *par, int frame_bytes);

/** @} */

/**
 * @}
 */

#endif // AVCODEC_CODEC_PAR_H
