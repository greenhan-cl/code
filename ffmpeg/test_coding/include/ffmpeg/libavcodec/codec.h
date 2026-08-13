/*
 * AVCodec public API
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

#ifndef AVCODEC_CODEC_H
#define AVCODEC_CODEC_H

#include <stdint.h>

#include "libavutil/avutil.h"
#include "libavutil/hwcontext.h"
#include "libavutil/log.h"
#include "libavutil/pixfmt.h"

#include "libavcodec/codec_id.h"

/**
 * @addtogroup lavc_core
 * @{
 */

/**
 * 解码器可以使用 draw_horiz_band 回调。
 */
#define AV_CODEC_CAP_DRAW_HORIZ_BAND     (1 <<  0)
/**
 * 编解码器使用 get_buffer() 或 get_encode_buffer() 分配缓冲区，并支持自定义分配器。
 * 如果未设置，编解码器可能完全不使用这两个函数，或者执行假定缓冲区由
 * avcodec_default_get_buffer2 或 avcodec_default_get_encode_buffer 分配的操作。
 */
#define AV_CODEC_CAP_DR1                 (1 <<  1)
/**
 * 编码器或解码器需要在末尾使用 NULL 输入进行冲刷，才能得到完整且正确的输出。
 *
 * 注意：如果未设置此标志，保证不会向编解码器传入 NULL 数据。用户仍可向公共编码
 *       或解码函数发送 NULL 数据，但除非设置此标志，否则 libavcodec 不会将其传给编解码器。
 *
 * 解码器：
 * 解码器具有非零延迟，末尾需要持续输入 avpkt->data=NULL、avpkt->size=0，
 * 以获取延迟数据，直到解码器不再返回帧。
 *
 * 编码器：
 * 编码结束时需要持续向编码器输入 NULL 数据，直到编码器不再返回数据。
 *
 * 注意：对于实现 AVCodec.encode2() 的编码器，设置此标志还意味着编码器必须为
 *       每个输出包设置 pts 和 duration。如果未设置此标志，libavcodec 将根据输入帧
 *       确定 pts 和 duration。
 */
#define AV_CODEC_CAP_DELAY               (1 <<  5)
/**
 * 可以向编解码器输入尺寸较小的最后一帧。
 * 这可用于防止最后的音频采样被截断。
 */
#define AV_CODEC_CAP_SMALL_LAST_FRAME    (1 <<  6)

/**
 * 编解码器是实验性的，因此会优先选择非实验性编码器而避开它。
 */
#define AV_CODEC_CAP_EXPERIMENTAL        (1 <<  9)
/**
 * 应由编解码器而不是容器填充声道配置和采样率。
 */
#define AV_CODEC_CAP_CHANNEL_CONF        (1 << 10)
/**
 * 编解码器支持帧级多线程。
 */
#define AV_CODEC_CAP_FRAME_THREADS       (1 << 12)
/**
 * 编解码器支持基于切片（或分区）的多线程。
 */
#define AV_CODEC_CAP_SLICE_THREADS       (1 << 13)
/**
 * 编解码器支持随时更改参数。
 */
#define AV_CODEC_CAP_PARAM_CHANGE        (1 << 14)
/**
 * 编解码器通过切片级或帧级多线程之外的方法支持多线程。
 * 通常用于标记对支持多线程的外部库的包装器。
 */
#define AV_CODEC_CAP_OTHER_THREADS       (1 << 15)
/**
 * 音频编码器支持每次调用接收不同数量的采样。
 */
#define AV_CODEC_CAP_VARIABLE_FRAME_SIZE (1 << 16)
/**
 * 解码器不是探测时的首选。
 * 这表示该解码器不适合用于探测。例如，它可能是启动成本很高的硬件解码器，
 * 或者无法提供很多有用的流信息。带有此标志的解码器只能作为探测的最后选择。
 */
#define AV_CODEC_CAP_AVOID_PROBING       (1 << 17)

/**
 * 编解码器由硬件实现支持。通常用于标识非 hwaccel 硬件解码器。
 * 如需 hwaccel 信息，请改用 avcodec_get_hw_config()。
 */
#define AV_CODEC_CAP_HARDWARE            (1 << 18)

/**
 * 编解码器可能由硬件实现支持，但并非必然如此。如果实现提供某种内部回退机制，
 * 则使用此标志而不是 AV_CODEC_CAP_HARDWARE。
 */
#define AV_CODEC_CAP_HYBRID              (1 << 19)

/**
 * 此编码器可以重排输入 AVFrame 中的用户 opaque 值，并随对应输出包返回这些值。
 * @see AV_CODEC_FLAG_COPY_OPAQUE
 */
#define AV_CODEC_CAP_ENCODER_REORDERED_OPAQUE (1 << 20)

/**
 * 可以使用 avcodec_flush_buffers() 冲刷此编码器。如果未设置此标志，
 * 必须关闭并重新打开编码器，以确保没有帧仍处于待处理状态。
 */
#define AV_CODEC_CAP_ENCODER_FLUSH   (1 << 21)

/**
 * 编码器能够输出重建帧数据，即解码已编码比特流后会产生的原始帧。
 *
 * 通过 AV_CODEC_FLAG_RECON_FRAME 标志启用重建帧输出。
 */
#define AV_CODEC_CAP_ENCODER_RECON_FRAME (1 << 22)

/**
 * AVProfile.
 */
typedef struct AVProfile {
    int profile;
    const char *name; ///< 配置文件的短名称
} AVProfile;

/**
 * AVCodec.
 */
typedef struct AVCodec {
    /**
     * 编解码器实现的名称。
     * 该名称在编码器之间及解码器之间全局唯一（但编码器和解码器可以同名）。
     * 从用户角度看，这是查找编解码器的主要方式。
     */
    const char *name;
    /**
     * 编解码器的描述性名称，旨在比 name 更易读。
     * 应使用 NULL_IF_CONFIG_SMALL() 宏定义它。
     */
    const char *long_name;
    enum AVMediaType type;
    enum AVCodecID id;
    /**
     * 编解码器能力。参见 AV_CODEC_CAP_*。
     */
    int capabilities;
    uint8_t max_lowres;                     ///< 解码器支持的 lowres 最大值

    const AVClass *priv_class;              ///< 私有上下文的 AVClass
    const AVProfile *profiles;              ///< 可识别配置文件的数组；未知时为 NULL；数组以 {AV_PROFILE_UNKNOWN} 结尾

    /**
     * 编解码器实现的组名称。
     * 这是支持此编解码器的包装器的简短符号名称。包装器使用某种外部实现，
     * 例如外部库，或操作系统/硬件提供的编解码器实现。
     * 此字段为 NULL 时，表示内置的 libavcodec 原生编解码器。
     * 非 NULL 时，大多数情况下它是 AVCodec.name 的后缀
     * （AVCodec.name 通常形如 "<codec_name>_<wrapper_name>"）。
     */
    const char *wrapper_name;
} AVCodec;

/**
 * 遍历所有已注册的编解码器。
 *
 * @param opaque libavcodec 用于存储迭代状态的指针。开始迭代时必须指向 NULL。
 *
 * @return 下一个已注册的编解码器；迭代结束时返回 NULL
 */
const AVCodec *av_codec_iterate(void **opaque);

/**
 * 查找具有匹配编解码器 ID 的已注册解码器。
 *
 * @param id 所请求解码器的 AVCodecID
 * @return 找到时返回解码器，否则返回 NULL。
 */
const AVCodec *avcodec_find_decoder(enum AVCodecID id);

/**
 * 查找具有指定名称的已注册解码器。
 *
 * @param name 所请求解码器的名称
 * @return 找到时返回解码器，否则返回 NULL。
 */
const AVCodec *avcodec_find_decoder_by_name(const char *name);

/**
 * 查找具有匹配编解码器 ID 的已注册编码器。
 *
 * @param id 所请求编码器的 AVCodecID
 * @return 找到时返回编码器，否则返回 NULL。
 */
const AVCodec *avcodec_find_encoder(enum AVCodecID id);

/**
 * 查找具有指定名称的已注册编码器。
 *
 * @param name 所请求编码器的名称
 * @return 找到时返回编码器，否则返回 NULL。
 */
const AVCodec *avcodec_find_encoder_by_name(const char *name);
/**
 * @return codec 是编码器时返回非零值，否则返回 0
 */
int av_codec_is_encoder(const AVCodec *codec);

/**
 * @return codec 是解码器时返回非零值，否则返回 0
 */
int av_codec_is_decoder(const AVCodec *codec);

/**
 * 如果可用，返回指定配置文件的名称。
 *
 * @param codec 在其中搜索给定配置文件的编解码器
 * @param profile 要获取名称的配置文件值
 * @return 找到时返回配置文件名称，否则返回 NULL。
 */
const char *av_get_profile_name(const AVCodec *codec, int profile);

enum {
    /**
     * 编解码器通过 hw_device_ctx 接口支持此格式。
     *
     * 选择此格式时，应在调用 avcodec_open2() 前将 AVCodecContext.hw_device_ctx
     * 设置为指定类型的设备。
     */
    AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX = 0x01,
    /**
     * 编解码器通过 hw_frames_ctx 接口支持此格式。
     *
     * 为解码器选择此格式时，应在 get_format() 回调内将
     * AVCodecContext.hw_frames_ctx 设为适当的帧上下文。
     * 该帧上下文必须在指定类型的设备上创建。
     *
     * 为编码器选择此格式时，应在调用 avcodec_open2() 前将
     * AVCodecContext.hw_frames_ctx 设为输入帧所使用的上下文。
     */
    AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX = 0x02,
    /**
     * 编解码器通过某种内部方法支持此格式。
     *
     * 无需任何额外配置即可选择此格式，不需要设备或帧上下文。
     */
    AV_CODEC_HW_CONFIG_METHOD_INTERNAL      = 0x04,
    /**
     * 编解码器通过某种临时专用方法支持此格式。
     *
     * 需要额外设置和/或函数调用。详情参见编解码器专用文档。
     * （需要此类配置的方法已弃用，应优先使用其他方法。）
     */
    AV_CODEC_HW_CONFIG_METHOD_AD_HOC        = 0x08,
};

typedef struct AVCodecHWConfig {
    /**
     * 对解码器而言，表示在具有合适硬件时可能解码到的硬件像素格式。
     *
     * 对编码器而言，表示编码器可能接受的像素格式。如果设为 AV_PIX_FMT_NONE，
     * 则适用于编解码器支持的所有像素格式。
     */
    enum AVPixelFormat pix_fmt;
    /**
     * AV_CODEC_HW_CONFIG_METHOD_* 标志的位集合，描述此配置可使用的设置方法。
     */
    int methods;
    /**
     * 与配置关联的设备类型。
     *
     * 使用 AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX 和
     * AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX 时必须设置，否则不使用。
     */
    enum AVHWDeviceType device_type;
} AVCodecHWConfig;

/**
 * 获取编解码器支持的硬件配置。
 *
 * index 从 0 到某个最大值时返回对应索引的配置描述符，其他值返回 NULL。
 * 如果编解码器不支持任何硬件配置，则始终返回 NULL。
 */
const AVCodecHWConfig *avcodec_get_hw_config(const AVCodec *codec, int index);

/**
 * @}
 */

#endif /* AVCODEC_CODEC_H */
