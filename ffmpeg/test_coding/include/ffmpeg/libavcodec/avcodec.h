/*
 * copyright (c) 2001 Fabrice Bellard
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

#ifndef AVCODEC_AVCODEC_H
#define AVCODEC_AVCODEC_H

/**
 * @file
 * @ingroup libavc
 * Libavcodec 外部 API 头文件
 */

#include "libavutil/samplefmt.h"
#include "libavutil/attributes.h"
#include "libavutil/avutil.h"
#include "libavutil/buffer.h"
#include "libavutil/channel_layout.h"
#include "libavutil/dict.h"
#include "libavutil/frame.h"
#include "libavutil/log.h"
#include "libavutil/pixfmt.h"
#include "libavutil/rational.h"

#include "codec.h"
#include "codec_id.h"
#include "defs.h"
#include "packet.h"
#include "version_major.h"
#ifndef HAVE_AV_CONFIG_H
/* 作为 ffmpeg 构建的一部分包含时，仅包含主版本以避免不必要的重新构建。
 * 从外部包含时，仍包含完整版本信息。 */
#include "version.h"

#include "codec_desc.h"
#include "codec_par.h"
#endif

struct AVCodecParameters;

/**
 * @defgroup libavc libavcodec
 * 编码/解码库
 *
 * @{
 *
 * @defgroup lavc_decoding 解码
 * @{
 * @}
 *
 * @defgroup lavc_encoding 编码
 * @{
 * @}
 *
 * @defgroup lavc_codec 编解码器
 * @{
 * @defgroup lavc_codec_native 原生编解码器
 * @{
 * @}
 * @defgroup lavc_codec_wrappers 外部库包装器
 * @{
 * @}
 * @defgroup lavc_codec_hwaccel 硬件加速桥接
 * @{
 * @}
 * @}
 * @defgroup lavc_internal 内部接口
 * @{
 * @}
 * @}
 */

/**
 * @ingroup libavc
 * @defgroup lavc_encdec 发送/接收式编码和解码 API 概述
 * @{
 *
 * avcodec_send_packet()/avcodec_receive_frame()/avcodec_send_frame()/
 * avcodec_receive_packet() 提供将输入与输出解耦的编码/解码 API。
 *
 * 编码/解码以及音频/视频使用此 API 的方式非常相似，流程如下：
 * - 按通常方式设置并打开 AVCodecContext。
 * - 发送有效输入：
 *   - 解码时，调用 avcodec_send_packet()，通过 AVPacket 向解码器提供原始压缩数据。
 *   - 编码时，调用 avcodec_send_frame()，通过 AVFrame 向编码器提供未压缩音频或视频。
 *
 *   两种情况下都建议 AVPacket 和 AVFrame 使用引用计数，否则 libavcodec 可能需要
 *   复制输入数据。（libavformat 始终返回引用计数 AVPacket，av_frame_get_buffer()
 *   分配引用计数 AVFrame。）
 * - 循环接收输出。定期调用 avcodec_receive_*() 函数并处理输出：
 *   - 解码时调用 avcodec_receive_frame()，成功时返回包含未压缩音视频的 AVFrame。
 *   - 编码时调用 avcodec_receive_packet()，成功时返回包含压缩帧的 AVPacket。
 *
 *   重复调用直到返回 AVERROR(EAGAIN) 或错误。AVERROR(EAGAIN) 表示需要新输入
 *   才能产生新输出，此时继续发送输入。每个输入帧/包通常产生 1 个输出帧/包，
 *   但也可能为 0 个或多个。
 *
 * 编解码开始时，编解码器可能接受多个输入帧/包而不返回帧，直到内部缓冲区填满。
 * 按上述步骤操作即可透明处理这种情况。
 *
 * 理论上发送输入可能返回 EAGAIN，这只应发生在尚未取完全部输出时。
 * 可利用此特性组织不同于上述建议的编解码循环。例如每次迭代尝试发送新输入，
 * 返回 EAGAIN 时再尝试接收输出。
 *
 * 流结束时需要“冲刷”（即排空）编解码器，因为出于性能或必要性（如 B 帧），
 * 编解码器内部可能缓冲多个帧或包。处理方式如下：
 * - 不发送有效输入，而是向 avcodec_send_packet()（解码）或
 *   avcodec_send_frame()（编码）发送 NULL，进入排空模式。
 * - 循环调用 avcodec_receive_frame()（解码）或 avcodec_receive_packet()（编码），
 *   直到返回 AVERROR_EOF。除非忘记进入排空模式，否则不会返回 AVERROR(EAGAIN)。
 * - 再次恢复解码前，必须使用 avcodec_flush_buffers() 重置编解码器。
 *
 * 强烈建议按上述方式使用 API，但也可在此固定模式之外调用函数。例如可连续调用
 * avcodec_send_packet() 而不调用 avcodec_receive_frame()。在内部缓冲区填满前
 * avcodec_send_packet() 会成功，随后以 AVERROR(EAGAIN) 拒绝输入。
 * 一旦开始拒绝输入，就必须读取至少一部分输出。
 *
 * 并非所有编解码器都遵循严格可预测的数据流。唯一保证是：一端发送/接收调用返回
 * AVERROR(EAGAIN) 时，另一端的接收/发送调用会成功，或至少不会也以
 * AVERROR(EAGAIN) 失败。一般而言，编解码器不允许无限缓冲输入或输出。
 *
 * 编解码器不允许发送和接收同时返回 AVERROR(EAGAIN)，否则会形成可能使用户陷入
 * 无限循环的无效状态。API 也没有时间概念：不能在 avcodec_send_packet() 返回
 * AVERROR(EAGAIN) 后，不进行其他接收/冲刷调用，仅等待一秒再调用就接受数据包。
 * 此 API 是严格状态机，时间流逝不应影响它。某些情况下可接受依赖时序的行为，
 * 但绝不能使发送/接收在任何时刻同时返回 EAGAIN，也必须避免状态“不稳定”并在
 * 发送/接收 API 之间来回切换才可推进。例如 avcodec_send_packet() 刚返回
 * AVERROR(EAGAIN) 后，编解码器不能随机改为想消费数据包而不是返回帧。
 * @}
 */

/**
 * @defgroup lavc_core 核心函数/结构体
 * @ingroup libavc
 *
 * 基本定义、查询 libavcodec 能力的函数、核心结构体的分配等。
 * @{
 */

/**
 * @ingroup lavc_encoding
 */
typedef struct RcOverride{
    int start_frame;
    int end_frame;
    int qscale; // 若为 0，则改用 quality_factor。
    float quality_factor;
} RcOverride;

/* 编码支持
   可在初始化前通过 AVCodecContext.flags 传入这些标志。
   注意：目前尚未支持所有功能。
*/

/**
 * 允许解码器生成数据平面未按 CPU 要求对齐的帧（例如因裁剪所致）。
 */
#define AV_CODEC_FLAG_UNALIGNED       (1 <<  0)
/**
 * 使用固定 qscale。
 */
#define AV_CODEC_FLAG_QSCALE          (1 <<  1)
/**
 * 允许每个 MB 使用 4 个 MV / H.263 高级预测。
 */
#define AV_CODEC_FLAG_4MV             (1 <<  2)
/**
 * 即使帧可能损坏也将其输出。
 */
#define AV_CODEC_FLAG_OUTPUT_CORRUPT  (1 <<  3)
/**
 * 使用四分之一像素运动补偿。
 */
#define AV_CODEC_FLAG_QPEL            (1 <<  4)
/**
 * 请求编码器输出重建帧，即解码已编码比特流时会产生的帧。成功调用
 * avcodec_receive_packet() 后，可立即调用 avcodec_receive_frame() 获取这些帧。
 *
 * 只能用于带有 @ref AV_CODEC_CAP_ENCODER_RECON_FRAME 能力标志的编码器。
 *
 * @note
 * 编码器返回的每个重建帧都对应最后一个已编码数据包，即帧按编码顺序而非显示顺序返回。
 *
 * @note
 * 帧参数（如像素格式或尺寸）不一定与 AVCodecContext 中的值一致。请务必使用返回帧中的值。
 */
#define AV_CODEC_FLAG_RECON_FRAME     (1 <<  6)
/**
 * @par 解码
 * 请求解码器将每个数据包的 AVPacket.opaque 和 AVPacket.opaque_ref 传播到其对应的输出 AVFrame。
 *
 * @par 编码：
 * 请求编码器将每个帧的 AVFrame.opaque 和 AVFrame.opaque_ref 值传播到其对应的输出 AVPacket。
 *
 * @par
 * 只能在带有 @ref AV_CODEC_CAP_ENCODER_REORDERED_OPAQUE 能力标志的编码器上设置。
 *
 * @note
 * 典型情况下，一个输入帧恰好产生一个输出数据包（可能有延迟），但一般而言，帧到数据包
 * 的映射是 M 对 N，因此：
 * - 任意数量的输入帧都可与某个输出数据包关联，包括零个；例如，某些编码器可能输出
 *   仅携带整个流元数据的数据包。
 * - 某个输入帧可与任意数量的输出数据包关联，同样包括零个；例如，某些编码器可能在
 *   特定条件下丢帧。
 * .
 * 这意味着使用此标志时，调用方不得假定：
 * - 某个输入帧的 opaque 一定会出现在某个输出数据包上；
 * - 每个输出数据包都会有非 NULL 的 opaque 值。
 * .
 * 当一个输出数据包包含多个帧时，opaque 值取自其中第一个帧。
 *
 * @note
 * 对解码器则相反，只需将帧与数据包互换。
 */
#define AV_CODEC_FLAG_COPY_OPAQUE     (1 <<  7)
/**
 * 告知编码器 AVFrame.duration 的值有效且应被使用（通常用于传递到输出数据包）。
 *
 * 若未设置此标志，则忽略帧时长。
 */
#define AV_CODEC_FLAG_FRAME_DURATION  (1 <<  8)
/**
 * 在第一遍模式下使用内部两遍码率控制。
 */
#define AV_CODEC_FLAG_PASS1           (1 <<  9)
/**
 * 在第二遍模式下使用内部两遍码率控制。
 */
#define AV_CODEC_FLAG_PASS2           (1 << 10)
/**
 * 环路滤波器。
 */
#define AV_CODEC_FLAG_LOOP_FILTER     (1 << 11)
/**
 * 仅解码/编码灰度图像。
 */
#define AV_CODEC_FLAG_GRAY            (1 << 13)
/**
 * 编码期间将设置 error[?] 变量。
 */
#define AV_CODEC_FLAG_PSNR            (1 << 15)
/**
 * 使用隔行 DCT。
 */
#define AV_CODEC_FLAG_INTERLACED_DCT  (1 << 18)
/**
 * 强制低延迟。
 */
#define AV_CODEC_FLAG_LOW_DELAY       (1 << 19)
/**
 * 将全局头放入 extradata，而不是放入每个关键帧。
 */
#define AV_CODEC_FLAG_GLOBAL_HEADER   (1 << 22)
/**
 * 仅使用位精确处理（(I)DCT 除外）。
 */
#define AV_CODEC_FLAG_BITEXACT        (1 << 23)
/* Fx：H.263+ 额外选项的标志 */
/**
 * H.263 高级帧内编码 / MPEG-4 AC 预测
 */
#define AV_CODEC_FLAG_AC_PRED         (1 << 24)
/**
 * 隔行运动估计
 */
#define AV_CODEC_FLAG_INTERLACED_ME   (1 << 29)
#define AV_CODEC_FLAG_CLOSED_GOP      (1U << 31)

/**
 * 允许不符合规范的加速技巧。
 */
#define AV_CODEC_FLAG2_FAST           (1 <<  0)
/**
 * 跳过比特流编码。
 */
#define AV_CODEC_FLAG2_NO_OUTPUT      (1 <<  2)
/**
 * 将全局头放入每个关键帧，而不是放入 extradata。
 */
#define AV_CODEC_FLAG2_LOCAL_HEADER   (1 <<  3)

/**
 * 输入比特流可能在数据包边界处截断，而不只是在帧边界处截断。
 */
#define AV_CODEC_FLAG2_CHUNKS         (1 << 15)
/**
 * 丢弃 SPS 中的裁剪信息。
 */
#define AV_CODEC_FLAG2_IGNORE_CROP    (1 << 16)
/**
 * 强制音频编码器使用固定帧大小。
 */
#define AV_CODEC_FLAG2_FIXED_FRAME_SIZE (1 << 17)

/**
 * 显示第一个关键帧之前的所有帧
 */
#define AV_CODEC_FLAG2_SHOW_ALL       (1 << 22)
/**
 * 通过帧侧数据导出运动矢量
 */
#define AV_CODEC_FLAG2_EXPORT_MVS     (1 << 28)
/**
 * 不跳过采样，并将跳过信息导出为帧侧数据
 */
#define AV_CODEC_FLAG2_SKIP_MANUAL    (1 << 29)
/**
 * 冲刷时不重置 ASS ReadOrder 字段（字幕解码）
 */
#define AV_CODEC_FLAG2_RO_FLUSH_NOOP  (1 << 30)
/**
 * 编码/解码时根据文件类型生成/解析 ICC 配置文件。对无法包含嵌入式 ICC 配置文件的
 * 编解码器，或编译时未启用 lcms2 支持时无效。
 */
#define AV_CODEC_FLAG2_ICC_PROFILES   (1U << 31)

/* 导出的侧数据。
   可在初始化前通过 AVCodecContext.export_side_data 传入这些标志。
*/
/**
 * 通过帧侧数据导出运动矢量
 */
#define AV_CODEC_EXPORT_DATA_MVS         (1 << 0)
/**
 * 通过数据包侧数据导出编码器生产者参考时间
 */
#define AV_CODEC_EXPORT_DATA_PRFT        (1 << 1)
/**
 * 仅用于解码。
 * 通过帧侧数据导出 AVVideoEncParams 结构体。
 */
#define AV_CODEC_EXPORT_DATA_VIDEO_ENC_PARAMS (1 << 2)
/**
 * 仅用于解码。
 * 不应用胶片颗粒，而是将其导出。
 */
#define AV_CODEC_EXPORT_DATA_FILM_GRAIN (1 << 3)

/**
 * 仅用于解码。
 * 不应用图像增强层，而是将其导出。
 */
#define AV_CODEC_EXPORT_DATA_ENHANCEMENTS (1 << 4)

/**
 * 解码器将保留对该帧的引用，并可能稍后复用它。
 */
#define AV_GET_BUFFER_FLAG_REF (1 << 0)

/**
 * 编码器将保留对该数据包的引用，并可能稍后复用它。
 */
#define AV_GET_ENCODE_BUFFER_FLAG_REF (1 << 0)

/**
 * 解码器将绕过帧级线程，并尽快返回下一帧。注意，这可能会比公布的
 * `AVCodecContext.delay` 更早交付帧。帧级线程被禁用时或编码时无效。
 */
#define AV_CODEC_RECEIVE_FRAME_FLAG_SYNCHRONOUS (1 << 0)

/**
 * 主要的外部 API 结构体。
 * 小版本升级时可以在末尾添加新字段。
 * 删除、重新排序或更改现有字段需要升级主版本。
 * 用户应用程序可使用 AVOptions（av_opt* / av_set/get*()）访问这些字段。
 * AVOptions 选项的名称字符串与对应命令行参数名一致，可在
 * libavcodec/options_table.h 中找到。
 * 由于历史原因或为简洁起见，AVOption/命令行参数名有时与 C 结构体字段名不同。
 * 不得在 libav* 外部使用 sizeof(AVCodecContext)。
 */
typedef struct AVCodecContext {
    /**
     * 用于 av_log 的结构体信息
     * - 由 avcodec_alloc_context3 设置
     */
    const AVClass *av_class;
    int log_level_offset;

    enum AVMediaType codec_type; /* see AVMEDIA_TYPE_xxx */
    const struct AVCodec  *codec;
    enum AVCodecID     codec_id; /* see AV_CODEC_ID_xxx */

    /**
     * fourcc（最低有效字节在前，因此 "ABCD" -> ('D'<<24) + ('C'<<16) + ('B'<<8) + 'A'）。
     * 用于规避某些编码器缺陷。
     * 解复用器应将其设置为用于标识编解码器的字段中存储的值。
     * 如果容器中存在多个此类字段，解复用器应选择最能描述所用编解码器的字段。
     * 如果容器中的编解码器标签字段大于 32 位，解复用器应通过表或其他结构将较长 ID
     * 映射为 32 位。也可以添加 extra_codec_tag + size，但必须先证明这样做有明显优势。
     * - 编码：由用户设置；否则使用基于 codec_id 的默认值。
     * - 解码：由用户设置，libavcodec 初始化期间会将其转换为大写。
     */
    unsigned int codec_tag;

    void *priv_data;

    /**
     * 用于内部数据的私有上下文。
     *
     * 与 priv_data 不同，它并非特定于编解码器，而由通用 libavcodec 函数使用。
     */
    struct AVCodecInternal *internal;

    /**
     * 用户私有数据，可用于携带应用程序特有内容。
     * - 编码：由用户设置。
     * - 解码：由用户设置。
     */
    void *opaque;

    /**
     * 平均码率
     * - 编码：由用户设置；恒定量化器编码不使用。
     * - 解码：由用户设置；若流中有此信息，可能被 libavcodec 覆盖。
     */
    int64_t bit_rate;

    /**
     * AV_CODEC_FLAG_*.
     * - 编码：由用户设置。
     * - 解码：由用户设置。
     */
    int flags;

    /**
     * AV_CODEC_FLAG2_*
     * - 编码：由用户设置。
     * - 解码：由用户设置。
     */
    int flags2;

    /**
     * 某些编解码器可能使用的带外全局头。
     *
     * - 解码：若可用（通常来自解复用器），调用方应在打开解码器前设置；某些解码器
     *   要求设置此项，否则初始化会失败。
     *
     *   数组必须使用 av_malloc() 系列函数分配；分配大小必须至少比 extradata_size
     *   多 AV_INPUT_BUFFER_PADDING_SIZE 字节。
     *
     * - 编码：可能由编码器在 avcodec_open2() 中设置（可能取决于是否设置
     *   AV_CODEC_FLAG_GLOBAL_HEADER 标志）。
     *
     * 设置后，该数组归编解码器所有，并在 avcodec_free_context() 中释放。
     */
    uint8_t *extradata;
    int extradata_size;

    /**
     * 这是表示帧时间戳的基本时间单位（秒）。对于固定帧率内容，timebase 应为
     * 1/framerate，时间戳增量应始终为 1。
     * 对视频而言，它通常但不总是帧率或场率的倒数。若帧率不恒定，1/time_base
     * 并非平均帧率。
     *
     * 与容器一样，基本流也可存储时间戳，1/time_base 是这些时间戳采用的单位。
     * 此类编解码器时基的示例见 ISO/IEC 14496-2:2001(E) 中的
     * vop_time_increment_resolution 和 fixed_vop_rate
     *（fixed_vop_rate == 0 表示它与帧率不同）。
     *
     * - 编码：必须由用户设置。
     * - 解码：不使用。
     */
    AVRational time_base;

    /**
     * pkt_dts/pts 和 AVPacket.dts/pts 所使用的时基。
     * - 编码：不使用。
     * - 解码：由用户设置。
     */
    AVRational pkt_timebase;

    /**
     * - 解码：对于在压缩比特流中存储帧率值的编解码器，解码器可在此导出该值。
     *         未知时为 { 0, 1}。
     * - 编码：可用于向编码器指示 CFR 内容的帧率。
     */
    AVRational framerate;

    /**
     * 编解码器延迟。
     *
     * 编码：从编码器输入到解码器输出的帧延迟数（假定解码器符合规范）。
     * 解码：在规范所定义标准解码器产生的延迟之外增加的帧延迟数。
     *
     * 视频：
     *   解码输出相对于编码输入延迟的帧数。
     *
     * 音频：
     *   编码时不使用此字段（见 initial_padding）。
     *
     *   解码时，这是解码器输出有效前需要输出的采样数。定位时，应从目标定位点之前
     *   这么多个采样处开始解码。
     *
     * - 编码：由 libavcodec 设置。
     * - 解码：由 libavcodec 设置。
     */
    int delay;


    /* 仅视频 */
    /**
     * 图像宽度/高度。
     *
     * @note 由于帧重排序，这些字段可能与 avcodec_receive_frame() 最后输出的
     * AVFrame 中的值不一致。
     *
     * - 编码：必须由用户设置。
     * - 解码：若已知（例如来自容器），用户可在打开解码器前设置。某些解码器要求
     *         调用方设置尺寸。解码期间，解码器可能在解析数据时按需覆盖这些值。
     */
    int width, height;

    /**
     * 比特流宽度/高度，可能与 width/height 不同，例如解码帧在输出前被裁剪或启用了 lowres。
     *
     * @note 由于帧重排序，这些字段可能与 avcodec_receive_frame() 最后输出的
     * AVFrame 中的值不一致。
     *
     * - 编码：不使用。
     * - 解码：若已知（例如来自容器），用户可在打开解码器前设置。解码期间，解码器
     *         可能在解析数据时按需覆盖这些值。
     */
    int coded_width, coded_height;

    /**
     * 采样宽高比（未知时为 0），即像素宽度除以像素高度。
     * 对某些视频标准，分子和分母必须互质且小于 256。
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    AVRational sample_aspect_ratio;

    /**
     * 像素格式，见 AV_PIX_FMT_xxx。
     * 若可从头信息获知，可由解复用器设置；解码器掌握更准确信息时可覆盖它。
     *
     * @note 由于帧重排序，此字段可能与 avcodec_receive_frame() 最后输出的
     * AVFrame 中的值不一致。
     *
     * - 编码：由用户设置。
     * - 解码：若已知则由用户设置，解析数据时由 libavcodec 覆盖。
     */
    enum AVPixelFormat pix_fmt;

    /**
     * 名义上的非加速像素格式，见 AV_PIX_FMT_xxx。
     * - 编码：不使用。
     * - 解码：由 libavcodec 在调用 get_format() 前设置。
     */
    enum AVPixelFormat sw_pix_fmt;

    /**
     * 源基色的色度坐标。
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    enum AVColorPrimaries color_primaries;

    /**
     * 颜色传递特性。
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    enum AVColorTransferCharacteristic color_trc;

    /**
     * YUV 色彩空间类型。
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    enum AVColorSpace colorspace;

    /**
     * MPEG 与 JPEG 的 YUV 范围。
     * - 编码：由用户设置以覆盖默认输出颜色范围；若未指定，libavcodec 根据输出格式设置。
     * - 解码：由 libavcodec 设置；用户也可设置，以将颜色范围传播给读取解码器上下文的组件。
     */
    enum AVColorRange color_range;

    /**
     * 定义色度采样的位置。
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    enum AVChromaLocation chroma_sample_location;

    /** 场顺序
     * - 编码：由 libavcodec 设置。
     * - 解码：由用户设置。
     */
    enum AVFieldOrder field_order;

    /**
     * 参考帧数量
     * - 编码：由用户设置。
     * - 解码：由 lavc 设置。
     */
    int refs;

    /**
     * 解码器中帧重排序缓冲区的大小。
     * 对 MPEG-2，IPB 为 1，低延迟 IP 为 0。
     * - 编码：由 libavcodec 设置。
     * - 解码：由 libavcodec 设置。
     */
    int has_b_frames;

    /**
     * 切片标志
     * - 编码：不使用。
     * - 解码：由用户设置。
     */
    int slice_flags;
#define SLICE_FLAG_CODED_ORDER    0x0001 ///< 按编码顺序而非显示顺序调用 draw_horiz_band()
#define SLICE_FLAG_ALLOW_FIELD    0x0002 ///< 允许 draw_horiz_band() 使用场切片（MPEG-2 场图像）
#define SLICE_FLAG_ALLOW_PLANE    0x0004 ///< 允许 draw_horiz_band() 每次处理一个分量（SVQ1）

    /**
     * 若非 NULL，libavcodec 解码器会调用 'draw_horiz_band' 绘制水平带，以提高缓存利用率。
     * 并非所有编解码器都支持，必须事先检查编解码器能力。
     * 使用多线程时，可能由多个线程同时调用；线程可能绘制同一 AVFrame 的不同部分，
     * 或绘制多个 AVFrame，且不保证切片按顺序绘制。
     * 此函数也供硬件加速 API 使用。帧解码期间至少调用一次，以传递硬件渲染所需数据。
     * 在该模式下，AVFrame 不指向像素数据，而指向加速 API 特有的结构体。应用程序读取
     * 此结构体，并可更改某些字段以指示进度或标记状态。
     * - 编码：不使用。
     * - 解码：由用户设置。
     * @param height 切片高度
     * @param y 切片的 y 位置
     * @param type 1->顶场，2->底场，3->帧
     * @param offset 应从中读取切片的 AVFrame.data 偏移量
     */
    void (*draw_horiz_band)(struct AVCodecContext *s,
                            const AVFrame *src, int offset[AV_NUM_DATA_POINTERS],
                            int y, int type, int height);

    /**
     * 协商像素格式的回调。仅用于解码，可由调用方在 avcodec_open2() 前设置。
     *
     * 某些解码器调用它来选择输出帧使用的像素格式。主要用于设置硬件加速，此时提供的
     * 格式列表同时包含相应硬件加速像素格式和“软件”格式。也可从 \ref sw_pix_fmt
     * 获取软件像素格式。
     *
     * 当编码帧属性（如分辨率、像素格式等）发生变化，且新属性支持多个输出格式时，
     * 会调用此回调。若选择硬件像素格式但初始化失败，可能立即再次调用此回调。
     *
     * 若解码器使用多线程，此回调可能从不同线程调用，但不会同时由多个线程调用。
     *
     * @param fmt 当前配置可使用的格式列表，以 AV_PIX_FMT_NONE 结尾。
     * @warning 若回调返回 fmt 中格式和 AV_PIX_FMT_NONE 之外的值，则行为未定义。
     * @return 选中的格式或 AV_PIX_FMT_NONE
     */
    enum AVPixelFormat (*get_format)(struct AVCodecContext *s, const enum AVPixelFormat * fmt);

    /**
     * 非 B 帧之间允许的最大 B 帧数
     * 注意：输出相对输入将延迟 max_b_frames+1 帧。
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int max_b_frames;

    /**
     * IP 帧与 B 帧之间的 qscale 因子
     * 若 > 0，使用最后一个 P 帧量化器（q= lastp_q*factor+offset）。
     * 若 < 0，执行普通码率控制（q= -normal_q*factor+offset）。
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float b_quant_factor;

    /**
     * IP 帧与 B 帧之间的 qscale 偏移
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float b_quant_offset;

    /**
     * P 帧与 I 帧之间的 qscale 因子
     * 若 > 0，使用最后一个 P 帧量化器（q = lastp_q * factor + offset）。
     * 若 < 0，执行普通码率控制（q= -normal_q*factor+offset）。
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float i_quant_factor;

    /**
     * P 帧与 I 帧之间的 qscale 偏移
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float i_quant_offset;

    /**
     * 亮度掩蔽（0->禁用）
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float lumi_masking;

    /**
     * 时间复杂度掩蔽（0->禁用）
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float temporal_cplx_masking;

    /**
     * 空间复杂度掩蔽（0->禁用）
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float spatial_cplx_masking;

    /**
     * P 块掩蔽（0->禁用）
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float p_masking;

    /**
     * 暗部掩蔽（0->禁用）
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float dark_masking;

    /**
     * nsse 比较函数中噪声相对于 sse 的权重
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
     int nsse_weight;

    /**
     * 运动估计比较函数
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int me_cmp;
    /**
     * 亚像素运动估计比较函数
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int me_sub_cmp;
    /**
     * 宏块比较函数（尚不支持）
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int mb_cmp;
    /**
     * 隔行 DCT 比较函数
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int ildct_cmp;
#define FF_CMP_SAD          0
#define FF_CMP_SSE          1
#define FF_CMP_SATD         2
#define FF_CMP_DCT          3
#define FF_CMP_PSNR         4
#define FF_CMP_BIT          5
#define FF_CMP_RD           6
#define FF_CMP_ZERO         7
#define FF_CMP_VSAD         8
#define FF_CMP_VSSE         9
#define FF_CMP_NSSE         10
#define FF_CMP_W53          11
#define FF_CMP_W97          12
#define FF_CMP_DCTMAX       13
#define FF_CMP_DCT264       14
#define FF_CMP_MEDIAN_SAD   15
#define FF_CMP_CHROMA       256

    /**
     * 运动估计菱形搜索的大小和形状
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int dia_size;

    /**
     * 先前 MV 预测器的数量（2a+1 x 2a+1 方形）
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int last_predictor_count;

    /**
     * 运动估计预处理比较函数
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int me_pre_cmp;

    /**
     * 运动估计预处理菱形搜索的大小和形状
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int pre_dia_size;

    /**
     * 亚像素运动估计质量
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int me_subpel_quality;

    /**
     * 以亚像素为单位的最大运动估计搜索范围
     * 为 0 时不限制。
     *
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int me_range;

    /**
     * 宏块决策模式
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int mb_decision;
#define FF_MB_DECISION_SIMPLE 0        ///< 使用 mb_cmp
#define FF_MB_DECISION_BITS   1        ///< 选择需要比特数最少的方案
#define FF_MB_DECISION_RD     2        ///< 率失真

    /**
     * 自定义帧内量化矩阵
     * 必须使用 av_malloc() 系列函数分配，并将在
     * avcodec_free_context().
     * - 编码：由用户设置/分配，由 libavcodec 释放。可以为 NULL。
     * - 解码：由 libavcodec 设置/分配/释放。
     */
    uint16_t *intra_matrix;

    /**
     * 自定义帧间量化矩阵
     * 必须使用 av_malloc() 系列函数分配，并将在
     * avcodec_free_context().
     * - 编码：由用户设置/分配，由 libavcodec 释放。可以为 NULL。
     * - 解码：由 libavcodec 设置/分配/释放。
     */
    uint16_t *inter_matrix;

    /**
     * 自定义帧内量化矩阵
     * - encoding: 由用户设置, can be NULL.
     * - decoding: 不使用.
     */
    uint16_t *chroma_intra_matrix;

#if FF_API_INTRA_DC_PRECISION
    /**
     * 帧内 DC 系数精度减 8
     * - encoding: 由用户设置.
     * - decoding: 由 libavcodec 设置
     * @deprecated Use the MPEG-2 encoder's private option "intra_dc_precision" instead.
     */
    attribute_deprecated
    int intra_dc_precision;
#endif

    /**
     * 最小宏块拉格朗日乘数
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int mb_lmin;

    /**
     * 最大宏块拉格朗日乘数
     * - encoding: 由用户设置.
     * - decoding: 不使用
     */
    int mb_lmax;

    /**
     * - encoding: 由用户设置.
     * - decoding: 不使用
     */
    int bidir_refine;

    /**
     * 最小 GOP 大小
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int keyint_min;

    /**
     * 图像组中的图像数量，intra_only 时为 0
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int gop_size;

    /**
     * 注意：该值取决于全像素运动估计使用的比较函数。
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int mv0_threshold;

    /**
     * 切片数量。
     * 表示图像细分数量，用于并行解码。
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int slices;

    /* 仅音频 */
    int sample_rate; ///< 每秒采样数

    /**
     * 音频采样格式
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    enum AVSampleFormat sample_fmt;  ///< 采样格式

    /**
     * 音频声道布局。
     * - 编码：必须由调用方设置为 AVCodec.ch_layouts 中的一项。
     * - 解码：若已知（例如来自容器），可由调用方设置；解码器随后可在解码期间按需覆盖。
     */
    AVChannelLayout ch_layout;

    /**
     * 音频帧中每个声道的采样数。
     *
     * - 编码：可由用户在调用 avcodec_open2() 前设置，libavcodec 随后可按需覆盖。
     *   除最后一帧外，每个提交帧的每个声道必须恰好包含 frame_size 个采样。
     *   当编解码器设置 AV_CODEC_CAP_VARIABLE_FRAME_SIZE 时可以为 0；请求
     *   AV_CODEC_FLAG2_FIXED_FRAME_SIZE 时除外，此时帧大小不受限制。
     * - 解码：某些解码器可设置它以指示固定帧大小。
     */
    int frame_size;

    /* 以下数据不应初始化。 */
    /**
     * 若每个数据包的字节数固定且已知，则为该字节数，否则为 0。
     * 某些基于 WAV 的音频编解码器使用此字段。
     */
    int block_align;

    /**
     * 音频截止带宽（0 表示“自动”）
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int cutoff;

    /**
     * 音频流承载的服务类型。
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    enum AVAudioServiceType audio_service_type;

    /**
     * 期望的采样格式
     * - 编码：不使用。
     * - 解码：由用户设置。若可行，解码器将解码为此格式。
     */
    enum AVSampleFormat request_sample_fmt;

    /**
     * 仅音频。编码器在音频开头插入的“预热”采样（填充）数量。调用方必须丢弃这么多个
     * 开头的解码采样，才能得到没有前置填充的原始音频。
     *
     * - 解码：不使用。
     * - 编码：由 libavcodec 设置。编码器会调整输出数据包的时间戳，使其始终指向
     *   数据包实际包含数据的第一个采样（包括新增填充）。例如，若时基为 1/samplerate，
     *   第一个输入采样的时间戳为 0，则第一个输出数据包的时间戳为 -initial_padding。
     */
    int initial_padding;

    /**
     * 仅音频。编码器追加到音频末尾的填充量（以采样计）。调用方必须从流末尾丢弃
     * 这么多个解码采样，才能得到没有尾部填充的原始音频。
     *
     * - 解码：不使用。
     * - 编码：不使用。
     */
    int trailing_padding;

    /**
     * 发生不连续后要跳过的采样数
     * - 解码：不使用。
     * - 编码：由 libavcodec 设置。
     */
    int seek_preroll;

    /**
     * 每帧开始时调用此回调，为该帧获取数据缓冲区。所有数据可共用一个连续缓冲区，
     * 也可每个数据平面各用一个缓冲区，或采用介于两者之间的方式。因此可按需要设置
     * buf[] 中任意数量的条目。每个缓冲区必须使用 AVBuffer API 进行引用计数
     *（见下文 buf[] 的说明）。
     *
     * 调用此回调前，帧中的以下字段会被设置：
     * - format
     * - width、height（仅视频）
     * - sample_rate、channel_layout、nb_samples（仅音频）
     * 它们的值可能与 AVCodecContext 中对应值不同。此回调必须使用帧中的值而非
     * 编解码器上下文中的值来计算所需缓冲区大小。
     *
     * 此回调必须填充帧中的以下字段：
     * - data[]
     * - linesize[]
     * - extended_data:
     *   * 若数据是超过 8 个声道的平面音频，此回调必须分配并填充 extended_data，
     *     使其包含所有数据平面的全部指针。data[] 应容纳尽可能多的指针。
     *     extended_data 必须用 av_malloc() 分配，并在 av_frame_unref() 中释放。
     *   * 否则 extended_data 必须指向 data。
     * - buf[] 必须包含一个或多个指向 AVBufferRef 结构体的指针。帧的每个 data 和
     *   extended_data 指针都必须包含在这些缓冲区中。即每个已分配内存块对应一个
     *   AVBufferRef，不一定每个 data[] 条目对应一个。见 av_buffer_create()、
     *   av_buffer_alloc() 和 av_buffer_ref()。
     * - 若缓冲区数量超过 buf[] 容量，此回调必须用 av_malloc() 分配 extended_buf 和
     *   nb_extended_buf，并用额外缓冲区填充。extended_buf 将在 av_frame_unref() 中释放。
     *   解码器通常会在输出前初始化整个缓冲区，但极少数错误情况下可能传出未初始化数据。
     *   \important 因此，get_buffer* 返回的缓冲区不应包含敏感数据。
     *
     * 若未设置 AV_CODEC_CAP_DR1，则 get_buffer2() 必须调用 avcodec_default_get_buffer2()，
     * 而不能提供以其他方式分配的缓冲区。
     *
     * 每个数据平面必须按目标 CPU 所需的最大对齐方式对齐。
     *
     * @see avcodec_default_get_buffer2()
     *
     * 视频：
     *
     * 若 flags 中设置 AV_GET_BUFFER_FLAG_REF，该帧稍后可能被 libavcodec 复用
     *（读取；若可写，也可能写入）。
     *
     * 应使用 avcodec_align_dimensions2() 获取所需宽高，因为通常需向上取整到 16 的倍数。
     *
     * 某些解码器不支持帧间 linesize 改变。
     *
     * 使用帧级多线程时，此回调可能从不同线程调用，但不会同时由多个线程调用，
     * 因此无需可重入。
     *
     * @see avcodec_align_dimensions2()
     *
     * 音频：
     *
     * 解码器通过在调用 get_buffer2() 前设置 AVFrame.nb_samples 请求特定大小的缓冲区。
     * 但解码器可能仅使用缓冲区的一部分，即在输出帧中将 AVFrame.nb_samples 设为较小值。
     *
     * 为方便起见，自定义 get_buffer2() 函数可使用 libavutil 中的
     * av_samples_get_buffer_size() 和 av_samples_fill_arrays() 查找所需数据大小，
     * 并填充数据指针和 linesize。对于音频，AVFrame.linesize 中只能设置 linesize[0]，
     * 因为所有平面大小必须相同。
     *
     * @see av_samples_get_buffer_size(), av_samples_fill_arrays()
     *
     * - 编码：不使用。
     * - 解码：由 libavcodec 设置，用户可覆盖。
     */
    int (*get_buffer2)(struct AVCodecContext *s, AVFrame *frame, int flags);

    /* - 编码参数 */
    /**
     * 允许比特流偏离参考值的比特数。参考值可以是 CBR（用于 CBR 第一遍）或 VBR（用于第二遍）。
     * - 编码：由用户设置；恒定量化器编码不使用。
     * - 解码：不使用。
     */
    int bit_rate_tolerance;

    /**
     * 无法逐帧更改质量的编解码器所使用的全局质量。
     * 该值应与 MPEG-1/2/4 qscale 成正比。
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int global_quality;

    /**
     * - encoding: 由用户设置.
     * - decoding: 不使用
     */
    int compression_level;
#define FF_COMPRESSION_DEFAULT -1

    float qcompress;  ///< 简单与复杂场景之间的 qscale 变化量（0.0-1.0）
    float qblur;      ///< qscale 随时间的平滑程度（0.0-1.0）

    /**
     * 最小量化器
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int qmin;

    /**
     * 最大量化器
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int qmax;

    /**
     * 帧间最大量化器差值
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int max_qdiff;

    /**
     * 解码器比特流缓冲区大小
     * - 编码：由用户设置。
     * - 解码：可能由 libavcodec 设置。
     */
    int rc_buffer_size;

    /**
     * 码率控制覆盖，见 RcOverride
     * - 编码：由用户分配/设置/释放。
     * - 解码：不使用。
     */
    int rc_override_count;
    RcOverride *rc_override;

    /**
     * 最大码率
     * - 编码：由用户设置。
     * - 解码：由用户设置，可能被 libavcodec 覆盖。
     */
    int64_t rc_max_rate;

    /**
     * 最小码率
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int64_t rc_min_rate;

    /**
     * 码率控制尝试最多使用在不发生下溢情况下可用量的 <value>。
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float rc_max_available_vbv_use;

    /**
     * 码率控制尝试至少使用防止 VBV 上溢所需量的 <value> 倍。
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    float rc_min_vbv_overflow_use;

    /**
     * 解码开始前应载入 rc 缓冲区的比特数。
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int rc_initial_buffer_occupancy;

    /**
     * 网格 RD 量化
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int trellis;

    /**
     * 第一遍编码统计信息输出缓冲区
     * - 编码：由 libavcodec 设置。
     * - 解码：不使用。
     */
    char *stats_out;

    /**
     * 第二遍编码统计信息输入缓冲区
     * 应在此放置第一遍 stats_out 拼接后的内容。
     * - 编码：由用户分配/设置/释放。
     * - 解码：不使用。
     */
    char *stats_in;

    /**
     * 规避编码器中有时无法自动检测的缺陷。
     * - 编码：由用户设置。
     * - 解码：由用户设置。
     */
    int workaround_bugs;
#define FF_BUG_AUTODETECT       1  ///< 自动检测
#define FF_BUG_XVID_ILACE       4
#define FF_BUG_UMP4             8
#define FF_BUG_NO_PADDING       16
#define FF_BUG_AMV              32
#define FF_BUG_QPEL_CHROMA      64
#define FF_BUG_STD_QPEL         128
#define FF_BUG_QPEL_CHROMA2     256
#define FF_BUG_DIRECT_BLOCKSIZE 512
#define FF_BUG_EDGE             1024
#define FF_BUG_HPEL_CHROMA      2048
#define FF_BUG_DC_CLIP          4096
#define FF_BUG_MS               8192 ///< 规避 Microsoft 有缺陷的解码器中的各种问题。
#define FF_BUG_TRUNCATED       16384
#define FF_BUG_IEDGE           32768

    /**
     * 严格遵循标准（MPEG-4 等）。
     * - 编码：由用户设置。
     * - 解码：由用户设置。
     * 设置为 STRICT 或更高时，编码器和解码器通常会采取较为刻板的行为；设置为
     * unofficial 或更低时，编码器可能产生并非所有符合规范的解码器都支持的输出。
     * 解码器通常不区分 normal、unofficial 和 experimental（即只要能解码就会尝试），
     * 除非明确要求严格遵循规范。
     * 只能设为 defs.h 中的 FF_COMPLIANCE_* 值之一。
     */
    int strict_std_compliance;

    /**
     * 错误隐藏标志
     * - 编码：不使用。
     * - 解码：由用户设置。
     */
    int error_concealment;
#define FF_EC_GUESS_MVS   1
#define FF_EC_DEBLOCK     2
#define FF_EC_FAVOR_INTER 256

    /**
     * 调试
     * - 编码：由用户设置。
     * - 解码：由用户设置。
     */
    int debug;
#define FF_DEBUG_PICT_INFO   1
#define FF_DEBUG_RC          2
#define FF_DEBUG_BITSTREAM   4
#define FF_DEBUG_MB_TYPE     8
#define FF_DEBUG_QP          16
#define FF_DEBUG_DCT_COEFF   0x00000040
#define FF_DEBUG_SKIP        0x00000080
#define FF_DEBUG_STARTCODE   0x00000100
#define FF_DEBUG_ER          0x00000400
#define FF_DEBUG_MMCO        0x00000800
#define FF_DEBUG_BUGS        0x00001000
#define FF_DEBUG_BUFFERS     0x00008000
#define FF_DEBUG_THREADS     0x00010000
#define FF_DEBUG_GREEN_MD    0x00800000
#define FF_DEBUG_NOMC        0x01000000

    /**
     * 错误识别；可能将某些基本有效的部分误判为错误。
     * 这是 defs.h 中定义的 AV_EF_* 值的位字段。
     *
     * - encoding: 由用户设置.
     * - decoding: 由用户设置.
     */
    int err_recognition;

    /**
     * 正在使用的硬件加速器
     * - 编码：不使用。
     * - 解码：由 libavcodec 设置。
     */
    const struct AVHWAccel *hwaccel;

    /**
     * Legacy hardware accelerator context.
     *
     * For some hardware acceleration methods, the caller may use this field to
     * signal hwaccel-specific data to the codec. The struct pointed to by this
     * pointer is hwaccel-dependent and defined in the respective header. Please
     * refer to the FFmpeg HW accelerator documentation to know how to fill
     * this.
     *
     * In most cases this field is optional - the necessary information may also
     * be provided to libavcodec through @ref hw_frames_ctx or @ref
     * hw_device_ctx (see avcodec_get_hw_config()). However, in some cases it
     * may be the only method of signalling some (optional) information.
     *
     * The struct and its contents are owned by the caller.
     *
     * - encoding: May be set by the caller before avcodec_open2(). Must remain
     *             valid until avcodec_free_context().
     * - decoding: May be set by the caller in the get_format() callback.
     *             Must remain valid until the next get_format() call,
     *             or avcodec_free_context() (whichever comes first).
     */
    void *hwaccel_context;

    /**
     * A reference to the AVHWFramesContext describing the input (for encoding)
     * or output (decoding) frames. The reference is set by the caller and
     * afterwards owned (and freed) by libavcodec - it should never be read by
     * the caller after being set.
     *
     * - decoding: This field should be set by the caller from the get_format()
     *             callback. The previous reference (if any) will always be
     *             unreffed by libavcodec before the get_format() call.
     *
     *             If the default get_buffer2() is used with a hwaccel pixel
     *             format, then this AVHWFramesContext will be used for
     *             allocating the frame buffers.
     *
     * - encoding: For hardware encoders configured to use a hwaccel pixel
     *             format, this field should be set by the caller to a reference
     *             to the AVHWFramesContext describing input frames.
     *             AVHWFramesContext.format must be equal to
     *             AVCodecContext.pix_fmt.
     *
     *             This field should be set before avcodec_open2() is called.
     */
    AVBufferRef *hw_frames_ctx;

    /**
     * 对描述硬件编码器/解码器所用设备的 AVHWDeviceContext 的引用。该引用由调用方设置，
     * 随后归 libavcodec 所有（并由其释放）。
     *
     * 若编解码器设备不需要硬件帧，或使用的硬件帧均由 libavcodec 内部分配，则应使用
     * 此字段。若用户希望提供编码器输入或解码器输出所用的任何帧，则应改用
     * hw_frames_ctx。对解码器在 get_format() 中设置 hw_frames_ctx 后，解码关联流段时
     * 会忽略此字段；再次调用 get_format() 后，它可用于后续流段。
     *
     * 对编码器和解码器，此字段都应在调用 avcodec_open2() 前设置，此后不得写入。
     *
     * 注意，某些解码器可能要求最初设置此字段才能支持 hw_frames_ctx；此时所有使用的
     * 帧上下文都必须在同一设备上创建。
     */
    AVBufferRef *hw_device_ctx;

    /**
     * AV_HWACCEL_FLAG_* 标志位集合，影响硬件加速解码（若启用）。
     * - 编码：不使用。
     * - 解码：由用户设置（在 avcodec_open2() 前或 AVCodecContext.get_format 回调中）。
     */
    int hwaccel_flags;

    /**
     * 仅视频解码。设置解码器为调用方分配的额外硬件帧数量。必须在调用 avcodec_open2() 前设置。
     *
     * 某些硬件解码器要求在解码开始前预先定义所有输出帧，因此其硬件帧池必须为固定大小。
     * 此处设置的额外帧是在解码器正常运行所需内部帧（例如用作参考图像的帧）之外增加的。
     */
    int extra_hw_frames;

    /**
     * 错误
     * - 编码：若 flags & AV_CODEC_FLAG_PSNR，则由 libavcodec 设置。
     * - 解码：不使用。
     */
    uint64_t error[AV_NUM_DATA_POINTERS];

    /**
     * DCT 算法，见下方 FF_DCT_*
     * - 编码：由用户设置。
     * - 解码：不使用。
     */
    int dct_algo;
#define FF_DCT_AUTO    0
#define FF_DCT_FASTINT 1
#define FF_DCT_INT     2
#define FF_DCT_MMX     3
#define FF_DCT_ALTIVEC 5
#define FF_DCT_FAAN    6
#define FF_DCT_NEON    7

    /**
     * IDCT 算法，见下方 FF_IDCT_*。
     * - 编码：由用户设置。
     * - 解码：由用户设置。
     */
    int idct_algo;
#define FF_IDCT_AUTO          0
#define FF_IDCT_INT           1
#define FF_IDCT_SIMPLE        2
#define FF_IDCT_SIMPLEMMX     3
#define FF_IDCT_ARM           7
#define FF_IDCT_ALTIVEC       8
#define FF_IDCT_SIMPLEARM     10
#define FF_IDCT_XVID          14
#define FF_IDCT_SIMPLEARMV5TE 16
#define FF_IDCT_SIMPLEARMV6   17
#define FF_IDCT_FAAN          20
#define FF_IDCT_SIMPLENEON    22
#define FF_IDCT_SIMPLEAUTO    128

    /**
     * 来自解复用器的每采样/像素位数（huffyuv 需要）。
     * - 编码：由 libavcodec 设置。
     * - 解码：由用户设置。
     */
     int bits_per_coded_sample;

    /**
     * libavcodec 内部像素/采样格式的每采样/像素位数。
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    int bits_per_raw_sample;

    /**
     * 线程数，用于决定应向 execute() 传递多少个独立任务。
     * - 编码：由用户设置。
     * - 解码：由用户设置。
     */
    int thread_count;

    /**
     * 使用哪些多线程方式。
     * 使用 FF_THREAD_FRAME 会使每个线程增加一帧解码延迟，因此无法提供后续帧的客户端不应使用。
     *
     * - 编码：由用户设置，否则使用默认值。
     * - 解码：由用户设置，否则使用默认值。
     */
    int thread_type;
#define FF_THREAD_FRAME   1 ///< 同时解码多个帧
#define FF_THREAD_SLICE   2 ///< 同时解码单个帧的多个部分

    /**
     * 编解码器正在使用的多线程方式。
     * - 编码：由 libavcodec 设置。
     * - 解码：由 libavcodec 设置。
     */
    int active_thread_type;

    /**
     * 编解码器可调用它执行若干独立任务。只有完成全部任务后才返回。
     * 用户可将其替换为某种多线程实现；默认实现串行执行各部分。
     * @param count 要执行的任务数量
     * - 编码：由 libavcodec 设置，用户可覆盖。
     * - 解码：由 libavcodec 设置，用户可覆盖。
     */
    int (*execute)(struct AVCodecContext *c, int (*func)(struct AVCodecContext *c2, void *arg), void *arg2, int *ret, int count, int size);

    /**
     * 编解码器可调用它执行若干独立任务。只有完成全部任务后才返回。
     * 用户可将其替换为某种多线程实现；默认实现串行执行各部分。
     * @param c 同样传递给 func 的上下文
     * @param count 要执行的任务数量
     * @param arg2 原样传递给 func 的参数
     * @param ret 已执行函数的返回值，必须能容纳 "count" 个值，可以为 NULL。
     * @param func 将调用 count 次的函数，jobnr 范围为 0 到 count-1。
     *             threadnr 范围为 0 到 c->thread_count-1 < MAX_THREADS，并保证同时执行的
     *             两个 func 实例不会具有相同的 threadnr。
     * @return 当前始终为 0，但代码应兼容未来改进：任一 func 调用返回 < 0 时，
     *         不再继续调用 func，并返回 < 0。
     * - 编码：由 libavcodec 设置，用户可覆盖。
     * - 解码：由 libavcodec 设置，用户可覆盖。
     */
    int (*execute2)(struct AVCodecContext *c, int (*func)(struct AVCodecContext *c2, void *arg, int jobnr, int threadnr), void *arg2, int *ret, int count);

    /**
     * 配置档次
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     * 见 defs.h 中的 AV_PROFILE_* 定义。
     */
     int profile;

    /**
     * 编码级别描述符。
     * - 编码：由用户设置，对应编解码器定义的特定级别，通常对应配置档次级别；
     *   若未指定，则设为 AV_LEVEL_UNKNOWN。
     * - 解码：由 libavcodec 设置。
     * 见 defs.h 中的 AV_LEVEL_*。
     */
     int level;

    /**
     * 对选定帧跳过环路滤波。
     * - 编码：不使用。
     * - 解码：由用户设置。
     */
    enum AVDiscard skip_loop_filter;

    /**
     * 对选定帧跳过 IDCT/反量化。
     * - 编码：不使用。
     * - 解码：由用户设置。
     */
    enum AVDiscard skip_idct;

    /**
     * 对选定帧跳过解码。
     * - 编码：不使用。
     * - 解码：由用户设置。
     */
    enum AVDiscard skip_frame;

    /**
     * 若编解码器支持，则跳过 Alpha 处理。
     * 注意，若格式使用预乘 Alpha（VP6 中很常见，且因视频质量/压缩效果更好而推荐），
     * 图像会像是以 Alpha 混合到黑色背景上。但对不使用预乘 Alpha 的格式，可能出现
     * 严重伪影（不过例如 libswscale 当前仍假定使用预乘 Alpha）。
     *
     * - 解码：由用户设置。
     * - 编码：不使用。
     */
    int skip_alpha;

    /**
     * 顶部要跳过的宏块行数。
     * - 编码：不使用。
     * - 解码：由用户设置。
     */
    int skip_top;

    /**
     * 底部要跳过的宏块行数。
     * - 编码：不使用。
     * - 解码：由用户设置。
     */
    int skip_bottom;

    /**
     * 低分辨率解码，1->1/2 大小，2->1/4 大小
     * - 编码：不使用。
     * - 解码：由用户设置。
     */
     int lowres;

    /**
     * AVCodecDescriptor
     * - 编码：不使用。
     * - 解码：由 libavcodec 设置。
     */
    const struct AVCodecDescriptor *codec_descriptor;

    /**
     * 输入字幕文件的字符编码。
     * - 解码：由用户设置。
     * - 编码：不使用。
     */
    char *sub_charenc;

    /**
     * 字幕字符编码模式。格式或编解码器可能会调整此设置（例如自行执行转换时）。
     * - 解码：由 libavcodec 设置。
     * - 编码：不使用。
     */
    int sub_charenc_mode;
#define FF_SUB_CHARENC_MODE_DO_NOTHING  -1  ///< 不执行操作（例如解复用器输出假定已是 UTF-8 的流，或编解码器为位图）
#define FF_SUB_CHARENC_MODE_AUTOMATIC    0  ///< libavcodec 自行选择模式
#define FF_SUB_CHARENC_MODE_PRE_DECODER  1  ///< AVPacket 数据送入解码器前需重新编码为 UTF-8，需要 iconv
#define FF_SUB_CHARENC_MODE_IGNORE       2  ///< 既不转换字幕，也不检查其是否为有效 UTF-8

    /**
     * Header containing style information for text subtitles.
     * For SUBTITLE_ASS subtitle type, it should contain the whole ASS
     * [Script Info] and [V4+ Styles] section, plus the [Events] line and
     * the Format line following. It shouldn't include any Dialogue line.
     *
     * - encoding: May be set by the caller before avcodec_open2() to an array
     *   allocated with the av_malloc() family of functions.
     * - decoding: May be set by libavcodec in avcodec_open2().
     *
     * After being set, the array is owned by the codec and freed in
     * avcodec_free_context().
     */
    int subtitle_header_size;
    uint8_t *subtitle_header;

    /**
     * dump format separator.
     * can be ", " or "\n      " or anything else
     * - encoding: 由用户设置.
     * - decoding: 由用户设置.
     */
    uint8_t *dump_separator;

    /**
     * ',' separated list of allowed decoders.
     * If NULL then all are allowed
     * - encoding: 不使用
     * - decoding: set by user
     */
    char *codec_whitelist;

    /**
     * Additional data associated with the entire coded stream.
     *
     * - decoding: may be set by user before calling avcodec_open2().
     * - encoding: may be set by libavcodec after avcodec_open2().
     */
    AVPacketSideData *coded_side_data;
    int            nb_coded_side_data;

    /**
     * Bit set of AV_CODEC_EXPORT_DATA_* flags, which affects the kind of
     * metadata exported in frame, packet, or coded stream side data by
     * decoders and encoders.
     *
     * - decoding: set by user
     * - encoding: set by user
     */
    int export_side_data;

    /**
     * The number of pixels per image to maximally accept.
     *
     * - decoding: set by user
     * - encoding: set by user
     */
    int64_t max_pixels;

    /**
     * Video decoding only. Certain video codecs support cropping, meaning that
     * only a sub-rectangle of the decoded frame is intended for display.  This
     * option controls how cropping is handled by libavcodec.
     *
     * When set to 1 (the default), libavcodec will apply cropping internally.
     * I.e. it will modify the output frame width/height fields and offset the
     * data pointers (only by as much as possible while preserving alignment, or
     * by the full amount if the AV_CODEC_FLAG_UNALIGNED flag is set) so that
     * the frames output by the decoder refer only to the cropped area. The
     * crop_* fields of the output frames will be zero.
     *
     * When set to 0, the width/height fields of the output frames will be set
     * to the coded dimensions and the crop_* fields will describe the cropping
     * rectangle. Applying the cropping is left to the caller.
     *
     * @warning When hardware acceleration with opaque output frames is used,
     * libavcodec is unable to apply cropping from the top/left border.
     *
     * @note when this option is set to zero, the width/height fields of the
     * AVCodecContext and output AVFrames have different meanings. The codec
     * context fields store display dimensions (with the coded dimensions in
     * coded_width/height), while the frame fields store the coded dimensions
     * (with the display dimensions being determined by the crop_* fields).
     */
    int apply_cropping;

    /**
     * The percentage of damaged samples to discard a frame.
     *
     * - decoding: set by user
     * - encoding: 不使用
     */
    int discard_damaged_percentage;

    /**
     * The number of samples per frame to maximally accept.
     *
     * - decoding: set by user
     * - encoding: set by user
     */
    int64_t max_samples;

    /**
     * This callback is called at the beginning of each packet to get a data
     * buffer for it.
     *
     * The following field will be set in the packet before this callback is
     * called:
     * - size
     * This callback must use the above value to calculate the required buffer size,
     * which must padded by at least AV_INPUT_BUFFER_PADDING_SIZE bytes.
     *
     * In some specific cases, the encoder may not use the entire buffer allocated by this
     * callback. This will be reflected in the size value in the packet once returned by
     * avcodec_receive_packet().
     *
     * This callback must fill the following fields in the packet:
     * - data: alignment requirements for AVPacket apply, if any. Some architectures and
     *   encoders may benefit from having aligned data.
     * - buf: must contain a pointer to an AVBufferRef structure. The packet's
     *   data pointer must be contained in it. See: av_buffer_create(), av_buffer_alloc(),
     *   and av_buffer_ref().
     *
     * If AV_CODEC_CAP_DR1 is not set then get_encode_buffer() must call
     * avcodec_default_get_encode_buffer() instead of providing a buffer allocated by
     * some other means.
     *
     * The flags field may contain a combination of AV_GET_ENCODE_BUFFER_FLAG_ flags.
     * They may be used for example to hint what use the buffer may get after being
     * created.
     * Implementations of this callback may ignore flags they don't understand.
     * If AV_GET_ENCODE_BUFFER_FLAG_REF is set in flags then the packet may be reused
     * (read and/or written to if it is writable) later by libavcodec.
     *
     * This callback must be thread-safe, as when frame threading is used, it may
     * be called from multiple threads simultaneously.
     *
     * @see avcodec_default_get_encode_buffer()
     *
     * - encoding: 由 libavcodec 设置, user can override.
     * - decoding: 不使用
     */
    int (*get_encode_buffer)(struct AVCodecContext *s, AVPacket *pkt, int flags);

    /**
     * Frame counter, set by libavcodec.
     *
     * - decoding: total number of frames returned from the decoder so far.
     * - encoding: total number of frames passed to the encoder so far.
     *
     *   @note the counter is not incremented if encoding/decoding resulted in
     *   an error.
     */
    int64_t frame_num;

    /**
     * Decoding only. May be set by the caller before avcodec_open2() to an
     * av_malloc()'ed array (or via AVOptions). Owned and freed by the decoder
     * afterwards.
     *
     * Side data attached to decoded frames may come from several sources:
     * 1. coded_side_data, which the decoder will for certain types translate
     *    from packet-type to frame-type and attach to frames;
     * 2. side data attached to an AVPacket sent for decoding (same
     *    considerations as above);
     * 3. extracted from the coded bytestream.
     * The first two cases are supplied by the caller and typically come from a
     * container.
     *
     * This array configures decoder behaviour in cases when side data of the
     * same type is present both in the coded bytestream and in the
     * user-supplied side data (items 1. and 2. above). In all cases, at most
     * one instance of each side data type will be attached to output frames. By
     * default it will be the bytestream side data. Adding an
     * AVPacketSideDataType value to this array will flip the preference for
     * this type, thus making the decoder prefer user-supplied side data over
     * bytestream. In case side data of the same type is present both in
     * coded_data and attacked to a packet, the packet instance always has
     * priority.
     *
     * The array may also contain a single -1, in which case the preference is
     * switched for all side data types.
     */
    int        *side_data_prefer_packet;
    /**
     * Number of entries in side_data_prefer_packet.
     */
    unsigned nb_side_data_prefer_packet;

    /**
     * Array containing static side data, such as HDR10 CLL / MDCV structures.
     * Side data entries should be allocated by usage of helpers defined in
     * libavutil/frame.h.
     *
     * - encoding: may be set by user before calling avcodec_open2() for
     *             encoder configuration. Afterwards owned and freed by the
     *             encoder.
     * - decoding: may be set by libavcodec in avcodec_open2().
     */
    AVFrameSideData  **decoded_side_data;
    int             nb_decoded_side_data;

    /**
     * Indicates how the alpha channel of the video is represented.
     * - encoding: 由用户设置
     * - decoding: 由 libavcodec 设置
     */
    enum AVAlphaMode alpha_mode;
} AVCodecContext;

/**
 * @defgroup lavc_hwaccel AVHWAccel
 *
 * @note  Nothing in this structure should be accessed by the user.  At some
 *        point in future it will not be externally visible at all.
 *
 * @{
 */
typedef struct AVHWAccel {
    /**
     * Name of the hardware accelerated codec.
     * The name is globally unique among encoders and among decoders (but an
     * encoder and a decoder can share the same name).
     */
    const char *name;

    /**
     * Type of codec implemented by the hardware accelerator.
     *
     * See AVMEDIA_TYPE_xxx
     */
    enum AVMediaType type;

    /**
     * Codec implemented by the hardware accelerator.
     *
     * See AV_CODEC_ID_xxx
     */
    enum AVCodecID id;

    /**
     * Supported pixel format.
     *
     * Only hardware accelerated formats are supported here.
     */
    enum AVPixelFormat pix_fmt;

    /**
     * Hardware accelerated codec capabilities.
     * see AV_HWACCEL_CODEC_CAP_*
     */
    int capabilities;
} AVHWAccel;

/**
 * HWAccel is experimental and is thus avoided in favor of non experimental
 * codecs
 */
#define AV_HWACCEL_CODEC_CAP_EXPERIMENTAL 0x0200

/**
 * Hardware acceleration should be used for decoding even if the codec level
 * used is 未知 or higher than the maximum supported level reported by the
 * hardware driver.
 *
 * It's generally a good idea to pass this flag unless you have a specific
 * reason not to, as hardware tends to under-report supported levels.
 */
#define AV_HWACCEL_FLAG_IGNORE_LEVEL (1 << 0)

/**
 * Hardware acceleration can output YUV pixel formats with a different chroma
 * sampling than 4:2:0 and/or other than 8 bits per component.
 */
#define AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH (1 << 1)

/**
 * Hardware acceleration should still be attempted for decoding when the
 * codec profile does not match the reported capabilities of the hardware.
 *
 * For example, this can be used to try to decode baseline profile H.264
 * streams in hardware - it will often succeed, because many streams marked
 * as baseline profile actually conform to constrained baseline profile.
 *
 * @warning If the stream is actually not supported then the behaviour is
 *          undefined, and may include returning entirely incorrect output
 *          while indicating 成功.
 */
#define AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH (1 << 2)

/**
 * Some hardware decoders (namely nvdec) can either output direct decoder
 * surfaces, or make an on-device copy and return said copy.
 * There is a hard limit on how many decoder surfaces there can be, and it
 * cannot be accurately guessed ahead of time.
 * For some processing chains, this can be okay, but others will run into the
 * limit and in turn produce very confusing errors that require fine tuning of
 * more or less obscure options by the user, or in extreme cases cannot be
 * resolved at all without inserting an avfilter that forces a copy.
 *
 * Thus, the hwaccel will by default make a copy for safety and resilience.
 * If a users really wants to minimize the amount of copies, they can set this
 * flag and ensure their processing chain does not exhaust the surface pool.
 */
#define AV_HWACCEL_FLAG_UNSAFE_OUTPUT (1 << 3)

/**
 * @}
 */

enum AVSubtitleType {
    SUBTITLE_NONE,

    SUBTITLE_BITMAP,                ///< A bitmap, pict will be set

    /**
     * Plain text, the text field must be set by the decoder and is
     * authoritative. ass and pict fields may contain approximations.
     */
    SUBTITLE_TEXT,

    /**
     * Formatted text, the ass field must be set by the decoder and is
     * authoritative. pict and text fields may contain approximations.
     */
    SUBTITLE_ASS,
};

#define AV_SUBTITLE_FLAG_FORCED 0x00000001

typedef struct AVSubtitleRect {
    int x;         ///< top left corner  of pict, undefined when pict is not set
    int y;         ///< top left corner  of pict, undefined when pict is not set
    int w;         ///< width            of pict, undefined when pict is not set
    int h;         ///< height           of pict, undefined when pict is not set
    int nb_colors; ///< number of colors in pict, undefined when pict is not set

    /**
     * data+linesize for the bitmap of this subtitle.
     * Can be set for text/ass as well once they are rendered.
     */
    uint8_t *data[4];
    int linesize[4];

    int flags;
    enum AVSubtitleType type;

    char *text;                     ///< 0 terminated plain UTF-8 text

    /**
     * 0 terminated ASS/SSA compatible event line.
     * The presentation of this is unaffected by the other values in this
     * struct.
     */
    char *ass;
} AVSubtitleRect;

typedef struct AVSubtitle {
    uint16_t format; /* 0 = graphics */
    uint32_t start_display_time; /* relative to packet pts, in ms */
    uint32_t end_display_time; /* relative to packet pts, in ms */
    unsigned num_rects;
    AVSubtitleRect **rects;
    int64_t pts;    ///< Same as packet pts, in AV_TIME_BASE
} AVSubtitle;

/**
 * Return the LIBAVCODEC_VERSION_INT constant.
 */
unsigned avcodec_version(void);

/**
 * Return the libavcodec build-time configuration.
 */
const char *avcodec_configuration(void);

/**
 * Return the libavcodec license.
 */
const char *avcodec_license(void);

/**
 * Allocate an AVCodecContext and set its fields to default values. The
 * resulting struct should be freed with avcodec_free_context().
 *
 * @param codec if non-NULL, allocate 私有数据 and initialize defaults
 *              for the given codec. It is illegal to then call avcodec_open2()
 *              with a different codec.
 *              If NULL, then the codec-specific defaults won't be initialized,
 *              which may result in suboptimal default settings (this is
 *              important mainly for encoders, e.g. libx264).
 *
 * @return An AVCodecContext filled with default values or NULL on failure.
 */
AVCodecContext *avcodec_alloc_context3(const AVCodec *codec);

/**
 * Free the 编解码器上下文 and everything associated with it and write NULL to
 * the provided pointer.
 */
void avcodec_free_context(AVCodecContext **avctx);

/**
 * Get the AVClass for AVCodecContext. It can be used in combination with
 * AV_OPT_SEARCH_FAKE_OBJ for examining options.
 *
 * @see av_opt_find().
 */
const AVClass *avcodec_get_class(void);

/**
 * Get the AVClass for AVSubtitleRect. It can be used in combination with
 * AV_OPT_SEARCH_FAKE_OBJ for examining options.
 *
 * @see av_opt_find().
 */
const AVClass *avcodec_get_subtitle_rect_class(void);

/**
 * Fill the parameters struct based on the values from the supplied codec
 * context. Any allocated fields in par are freed and replaced with duplicates
 * of the corresponding fields in codec.
 *
 * @return >= 0 on 成功, a negative AVERROR code on failure
 *
 * @relates AVCodecParameters
 */
int avcodec_parameters_from_context(struct AVCodecParameters *par,
                                    const AVCodecContext *codec);

/**
 * Fill the 编解码器上下文 based on the values from the supplied codec
 * parameters. Any allocated fields in codec that have a corresponding field in
 * par are freed and replaced with duplicates of the corresponding field in par.
 * Fields in codec that do not have a counterpart in par are not touched.
 *
 * @return >= 0 on 成功, a negative AVERROR code on failure.
 *
 * @relates AVCodecParameters
 */
int avcodec_parameters_to_context(AVCodecContext *codec,
                                  const struct AVCodecParameters *par);

/**
 * Initialize the AVCodecContext to use the given AVCodec. Prior to using this
 * function the context has to be allocated with avcodec_alloc_context3().
 *
 * The functions avcodec_find_decoder_by_name(), avcodec_find_encoder_by_name(),
 * avcodec_find_decoder() and avcodec_find_encoder() provide an easy way for
 * retrieving a codec.
 *
 * Depending on the codec, you might need to set options in the 编解码器上下文
 * also for decoding (e.g. width, height, or the pixel or audio sample format in
 * the case the information is not available in the bitstream, as when decoding
 * raw audio or video).
 *
 * Options in the 编解码器上下文 can be set either by setting them in the options
 * AVDictionary, or by setting the values in the context itself, directly or by
 * using the av_opt_set() API before calling this function.
 *
 * Example:
 * @code
 * av_dict_set(&opts, "b", "2.5M", 0);
 * codec = avcodec_find_decoder(AV_CODEC_ID_H264);
 * if (!codec)
 *     exit(1);
 *
 * context = avcodec_alloc_context3(codec);
 *
 * if (avcodec_open2(context, codec, opts) < 0)
 *     exit(1);
 * @endcode
 *
 * In the case AVCodecParameters are available (e.g. when demuxing a stream
 * using libavformat, and accessing the AVStream contained in the demuxer), the
 * codec parameters can be copied to the 编解码器上下文 using
 * avcodec_parameters_to_context(), as in the following example:
 *
 * @code
 * AVStream *stream = ...;
 * context = avcodec_alloc_context3(codec);
 * if (avcodec_parameters_to_context(context, stream->codecpar) < 0)
 *     exit(1);
 * if (avcodec_open2(context, codec, NULL) < 0)
 *     exit(1);
 * @endcode
 *
 * @note Always call this function before using decoding routines (such as
 * @ref avcodec_receive_frame()).
 *
 * @param avctx The context to initialize.
 * @param codec The codec to open this context for. If a non-NULL codec has been
 *              previously passed to avcodec_alloc_context3() or
 *              for this context, then this parameter MUST be either NULL or
 *              equal to the previously passed codec.
 * @param options A dictionary filled with AVCodecContext and codec-private
 *                options, which are set on top of the options already set in
 *                avctx, can be NULL. On return this object will be filled with
 *                options that were not found in the avctx 编解码器上下文.
 *
 * @return zero on 成功, a negative value on error
 * @see avcodec_alloc_context3(), avcodec_find_decoder(), avcodec_find_encoder(),
 *      av_dict_set(), av_opt_set(), av_opt_find(), avcodec_parameters_to_context()
 */
int avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);

/**
 * Free all allocated data in the given subtitle struct.
 *
 * @param sub AVSubtitle to free.
 */
void avsubtitle_free(AVSubtitle *sub);

/**
 * @}
 */

/**
 * @addtogroup lavc_decoding
 * @{
 */

/**
 * The default callback for AVCodecContext.get_buffer2(). It is made public so
 * it can be called by custom get_buffer2() implementations for decoders without
 * AV_CODEC_CAP_DR1 set.
 */
int avcodec_default_get_buffer2(AVCodecContext *s, AVFrame *frame, int flags);

/**
 * The default callback for AVCodecContext.get_encode_buffer(). It is made public so
 * it can be called by custom get_encode_buffer() implementations for encoders without
 * AV_CODEC_CAP_DR1 set.
 */
int avcodec_default_get_encode_buffer(AVCodecContext *s, AVPacket *pkt, int flags);

/**
 * Modify width and height values so that they will result in a memory
 * buffer that is acceptable for the codec if you do not use any horizontal
 * padding.
 *
 * May only be used if a codec with AV_CODEC_CAP_DR1 has been opened.
 */
void avcodec_align_dimensions(AVCodecContext *s, int *width, int *height);

/**
 * Modify width and height values so that they will result in a memory
 * buffer that is acceptable for the codec if you also ensure that all
 * line sizes are a multiple of the respective linesize_align[i].
 *
 * May only be used if a codec with AV_CODEC_CAP_DR1 has been opened.
 */
void avcodec_align_dimensions2(AVCodecContext *s, int *width, int *height,
                               int linesize_align[AV_NUM_DATA_POINTERS]);

/**
 * Decode a subtitle message.
 * Return a negative value on error, otherwise return the number of bytes used.
 * If no subtitle could be decompressed, got_sub_ptr is zero.
 * Otherwise, the subtitle is stored in *sub.
 * Note that AV_CODEC_CAP_DR1 is not available for subtitle codecs. This is for
 * simplicity, because the performance difference is expected to be negligible
 * and reusing a get_buffer written for video codecs would probably perform badly
 * due to a potentially very different allocation pattern.
 *
 * Some decoders (those marked with AV_CODEC_CAP_DELAY) have a delay between input
 * and output. This means that for some packets they will not immediately
 * produce decoded output and need to be flushed at the end of decoding to get
 * all the decoded data. Flushing is done by calling this function with packets
 * with avpkt->data set to NULL and avpkt->size set to 0 until it stops
 * returning subtitles. It is safe to flush even those decoders that are not
 * marked with AV_CODEC_CAP_DELAY, then no subtitles will be returned.
 *
 * @note The AVCodecContext MUST have been opened with @ref avcodec_open2()
 * before packets may be fed to the decoder.
 *
 * @param avctx the 编解码器上下文
 * @param[out] sub The preallocated AVSubtitle in which the decoded subtitle will be stored,
 *                 must be freed with avsubtitle_free if *got_sub_ptr is set.
 * @param[in,out] got_sub_ptr Zero if no subtitle could be decompressed, otherwise, it is nonzero.
 * @param[in] avpkt The input AVPacket containing the input buffer.
 */
int avcodec_decode_subtitle2(AVCodecContext *avctx, AVSubtitle *sub,
                             int *got_sub_ptr, const AVPacket *avpkt);

/**
 * Supply raw packet data as input to a decoder.
 *
 * Internally, this call will copy relevant AVCodecContext fields, which can
 * influence decoding per-packet, and apply them when the packet is actually
 * decoded. (For example AVCodecContext.skip_frame, which might direct the
 * decoder to drop the frame contained by the packet sent with this function.)
 *
 * @warning The input buffer, avpkt->data must be AV_INPUT_BUFFER_PADDING_SIZE
 *          larger than the actual read bytes because some optimized bitstream
 *          readers read 32 or 64 bits at once and could read over the end.
 *
 * @note The AVCodecContext MUST have been opened with @ref avcodec_open2()
 *       before packets may be fed to the decoder.
 *
 * @param avctx 编解码器上下文
 * @param[in] avpkt The input AVPacket. Usually, this will be a single video
 *                  frame, or several complete audio frames.
 *                  Ownership of the packet remains with the caller, and the
 *                  decoder will not write to the packet. The decoder may create
 *                  a reference to the packet data (or copy it if the packet is
 *                  not reference-counted).
 *                  Unlike with older APIs, the packet is always fully consumed,
 *                  and if it contains multiple frames (e.g. some audio codecs),
 *                  will require you to call avcodec_receive_frame() multiple
 *                  times afterwards before you can send a new packet.
 *                  It can be NULL (or an AVPacket with data set to NULL and
 *                  size set to 0); in this case, it is considered a flush
 *                  packet, which signals the end of the stream. Sending the
 *                  first flush packet will return 成功. Subsequent ones are
 *                  unnecessary and will return AVERROR_EOF. If the decoder
 *                  still has frames buffered, it will return them after sending
 *                  a flush packet.
 *
 * @retval 0                 成功
 * @retval AVERROR(EAGAIN)   input is not accepted in the current state - user
 *                           must read output with avcodec_receive_frame() (once
 *                           all output is read, the packet should be resent,
 *                           and the call will not fail with EAGAIN).
 * @retval AVERROR_EOF       the decoder has been flushed, and no new packets can be
 *                           sent to it (also returned if more than 1 flush
 *                           packet is sent)
 * @retval AVERROR(EINVAL)   codec not opened, it is an encoder, or requires flush
 * @retval AVERROR(ENOMEM)   failed to add packet to internal queue, or similar
 * @retval "another negative error code" legitimate decoding errors
 */
int avcodec_send_packet(AVCodecContext *avctx, const AVPacket *avpkt);

/**
 * Return decoded output data from a decoder or encoder (when the
 * @ref AV_CODEC_FLAG_RECON_FRAME flag is used).
 *
 * @param avctx 编解码器上下文
 * @param frame This will be set to a reference-counted video or audio
 *              frame (depending on the decoder type) allocated by the
 *              codec. Note that the function will always call
 *              av_frame_unref(frame) before doing anything else.
 * @param flags Combination of AV_CODEC_RECEIVE_FRAME_FLAG_* flags.
 *
 * @retval 0                成功, a frame was returned
 * @retval AVERROR(EAGAIN)  output is not available in this state - user must
 *                          try to send new input
 * @retval AVERROR_EOF      the codec has been fully flushed, and there will be
 *                          no more output frames
 * @retval AVERROR(EINVAL)  codec not opened, or it is an encoder without the
 *                          @ref AV_CODEC_FLAG_RECON_FRAME flag enabled
 * @retval "other negative error code" legitimate decoding errors
 */
int avcodec_receive_frame_flags(AVCodecContext *avctx, AVFrame *frame, unsigned flags);

/**
 * Alias for `avcodec_receive_frame_flags(avctx, frame, 0)`.
 */
int avcodec_receive_frame(AVCodecContext *avctx, AVFrame *frame);

/**
 * Supply a raw video or audio frame to the encoder. Use avcodec_receive_packet()
 * to retrieve buffered output packets.
 *
 * @param avctx     编解码器上下文
 * @param[in] frame AVFrame containing the raw audio or video frame to be encoded.
 *                  Ownership of the frame remains with the caller, and the
 *                  encoder will not write to the frame. The encoder may create
 *                  a reference to the frame data (or copy it if the frame is
 *                  not reference-counted).
 *                  It can be NULL, in which case it is considered a flush
 *                  packet.  This signals the end of the stream. If the encoder
 *                  still has packets buffered, it will return them after this
 *                  call. Once flushing mode has been entered, additional flush
 *                  packets are ignored, and sending frames will return
 *                  AVERROR_EOF.
 *
 *                  For audio:
 *                  If AV_CODEC_CAP_VARIABLE_FRAME_SIZE is set, then each frame
 *                  can have any number of samples.
 *                  If it is not set, or AV_CODEC_FLAG2_FIXED_FRAME_SIZE was
 *                  requested, then frame->nb_samples must be equal to
 *                  avctx->frame_size for all frames except the last.
 *                  The final frame may be smaller than avctx->frame_size.
 * @retval 0                 成功
 * @retval AVERROR(EAGAIN)   input is not accepted in the current state - user must
 *                           read output with avcodec_receive_packet() (once all
 *                           output is read, the packet should be resent, and the
 *                           call will not fail with EAGAIN).
 * @retval AVERROR_EOF       the encoder has been flushed, and no new frames can
 *                           be sent to it
 * @retval AVERROR(EINVAL)   codec not opened, it is a decoder, or requires flush
 * @retval AVERROR(ENOMEM)   failed to add packet to internal queue, or similar
 * @retval "another negative error code" 合法的编码错误
 */
int avcodec_send_frame(AVCodecContext *avctx, const AVFrame *frame);

/**
 * 从编码器读取已编码数据。
 *
 * @param avctx 编解码器上下文
 * @param avpkt This will be set to a reference-counted packet allocated by the
 *              encoder. Note that the function will always call
 *              av_packet_unref(avpkt) before doing anything else.
 * @retval 0               成功
 * @retval AVERROR(EAGAIN) 当前状态下没有可用输出——用户必须
 *                         尝试发送输入
 * @retval AVERROR_EOF     编码器已完全冲刷，不会再有
 *                         更多输出数据包
 * @retval AVERROR(EINVAL) 编解码器未打开，或当前是解码器
 * @retval "another negative error code" 合法的编码错误
 */
int avcodec_receive_packet(AVCodecContext *avctx, AVPacket *avpkt);

/**
 * 创建并返回适用于硬件
 * 解码的 AVHWFramesContext。此函数应从 get_format 回调中调用，用于
 * 为 AVCodecContext.hw_frames_ctx 准备 AVHWFramesContext。
 * 此 API 仅适用于使用特定硬件加速模式/API 的解码。
 *
 * 返回的 AVHWFramesContext 尚未初始化。调用方必须执行此操作
 * with av_hwframe_ctx_init().
 *
 * 调用此函数并非强制要求，但可简化操作，避免
 * 手动分配帧时处理编解码器或硬件 API 的具体细节。
 *
 * Alternatively to this, an API user can set AVCodecContext.hw_device_ctx,
 * which sets up AVCodecContext.hw_frames_ctx fully automatically, and makes
 * it unnecessary to call this function or having to care about
 * AVHWFramesContext initialization at all.
 *
 * 调用此函数有以下要求：
 *
 * - It must be called from get_format with the same avctx parameter that was
 *   passed to get_format. Calling it outside of get_format is not allowed, and
 *   can trigger undefined behavior.
 * - 此函数并非始终受支持（见返回值说明）。
 *   Even if this function returns 成功fully, hwaccel initialization could
 *   fail later. (The degree to which implementations check whether the stream
 *   is actually supported varies. Some do this check only after the user's
 *   get_format callback returns.)
 * - hw_pix_fmt 必须是 get_format 建议的选项之一。 If the
 *   user decides to use a AVHWFramesContext prepared with this API function,
 *   the user must return the same hw_pix_fmt from get_format.
 * - 传给此函数的 device_ref 必须支持给定的 hw_pix_fmt。
 * - After calling this API function, it is the user's responsibility to
 *   initialize the AVHWFramesContext (returned by the out_frames_ref parameter),
 *   and to set AVCodecContext.hw_frames_ctx to it. If done, this must be done
 *   before returning from get_format (this is implied by the normal
 *   AVCodecContext.hw_frames_ctx API rules).
 * - The AVHWFramesContext parameters may change every time time get_format is
 *   called. Also, AVCodecContext.hw_frames_ctx is reset before get_format. So
 *   you are inherently required to go through this process again on every
 *   get_format call.
 * - It is perfectly possible to call this function without actually using
 *   the resulting AVHWFramesContext. One use-case might be trying to reuse a
 *   previously initialized AVHWFramesContext, and calling this API function
 *   only to test whether the required frame parameters have changed.
 * - Fields that use dynamically allocated values of any kind must not be set
 *   by the user unless setting them is explicitly allowed by the documentation.
 *   If the user sets AVHWFramesContext.free and AVHWFramesContext.user_opaque,
 *   the new free callback must call the potentially set previous free callback.
 *   This API call may set any dynamically allocated fields, including the free
 *   callback.
 *
 * The function will set at least the following fields on AVHWFramesContext
 * (potentially more, depending on hwaccel API):
 *
 * - All fields set by av_hwframe_ctx_alloc().
 * - Set the format field to hw_pix_fmt.
 * - Set the sw_format field to the most suited and most versatile format. (An
 *   implication is that this will prefer generic formats over opaque formats
 *   with arbitrary restrictions, if possible.)
 * - Set the width/height fields to the coded frame size, rounded up to the
 *   API-specific minimum alignment.
 * - Only _if_ the hwaccel requires a pre-allocated pool: set the initial_pool_size
 *   field to the number of maximum reference surfaces possible with the codec,
 *   plus 1 surface for the user to work (meaning the user can safely reference
 *   at most 1 decoded surface at a time), plus additional buffering introduced
 *   by frame threading. If the hwaccel does not require pre-allocation, the
 *   field is left to 0, and the decoder will allocate new surfaces on demand
 *   during decoding.
 * - Possibly AVHWFramesContext.hwctx fields, depending on the underlying
 *   hardware API.
 *
 * Essentially, out_frames_ref returns the same as av_hwframe_ctx_alloc(), but
 * with basic frame parameters set.
 *
 * 此函数无状态，不会更改 AVCodecContext 或
 * device_ref 指向的 AVHWDeviceContext。
 *
 * @param avctx The context which is currently calling get_format, and which
 *              implicitly contains all state needed for filling the returned
 *              AVHWFramesContext properly.
 * @param device_ref A reference to the AVHWDeviceContext describing the device
 *                   which will be used by the hardware decoder.
 * @param hw_pix_fmt The hwaccel format you are going to return from get_format.
 * @param out_frames_ref On 成功, set to a reference to an _uninitialized_
 *                       AVHWFramesContext, created from the given device_ref.
 *                       Fields will be set to values required for decoding.
 *                       Not changed if an error is returned.
 * @return zero on 成功, a negative value on error. The following error codes
 *         have special semantics:
 *      AVERROR(ENOENT): the decoder does not support this functionality. Setup
 *                       is always manual, or it is a decoder which does not
 *                       support setting AVCodecContext.hw_frames_ctx at all,
 *                       or it is a software format.
 *      AVERROR(EINVAL): it is known that hardware decoding is not supported for
 *                       this configuration, or the device_ref is not supported
 *                       for the hwaccel referenced by hw_pix_fmt.
 */
int avcodec_get_hw_frames_parameters(AVCodecContext *avctx,
                                     AVBufferRef *device_ref,
                                     enum AVPixelFormat hw_pix_fmt,
                                     AVBufferRef **out_frames_ref);

enum AVCodecConfig {
    AV_CODEC_CONFIG_PIX_FORMAT,     ///< AVPixelFormat, terminated by AV_PIX_FMT_NONE
    AV_CODEC_CONFIG_FRAME_RATE,     ///< AVRational, terminated by {0, 0}
    AV_CODEC_CONFIG_SAMPLE_RATE,    ///< int, terminated by 0
    AV_CODEC_CONFIG_SAMPLE_FORMAT,  ///< AVSampleFormat, terminated by AV_SAMPLE_FMT_NONE
    AV_CODEC_CONFIG_CHANNEL_LAYOUT, ///< AVChannelLayout, terminated by {0}
    AV_CODEC_CONFIG_COLOR_RANGE,    ///< AVColorRange, terminated by AVCOL_RANGE_UNSPECIFIED
    AV_CODEC_CONFIG_COLOR_SPACE,    ///< AVColorSpace, terminated by AVCOL_SPC_UNSPECIFIED
    AV_CODEC_CONFIG_ALPHA_MODE,     ///< AVAlphaMode, terminated by AVALPHA_MODE_UNSPECIFIED
};

/**
 * 获取给定配置类型支持的所有值。
 *
 * @param avctx An optional context to use. Values such as
 *              `strict_std_compliance` may affect the result. If NULL,
 *              default values are used.
 * @param codec The codec to query, or NULL to use avctx->codec.
 * @param config The configuration to query.
 * @param flags Currently 不使用; should be set to zero.
 * @param out_configs On 成功, set to a list of configurations, terminated
 *                    by a config-specific terminator, or NULL if all
 *                    possible values are supported.
 * @param out_num_configs On 成功, set to the number of elements in
                          *out_configs, excluding the terminator. Optional.
 */
int avcodec_get_supported_config(const AVCodecContext *avctx,
                                 const AVCodec *codec, enum AVCodecConfig config,
                                 unsigned flags, const void **out_configs,
                                 int *out_num_configs);



/**
 * @defgroup lavc_parsing 帧解析
 * @{
 */

enum AVPictureStructure {
    AV_PICTURE_STRUCTURE_UNKNOWN,      ///< 未知
    AV_PICTURE_STRUCTURE_TOP_FIELD,    ///< 编码为顶场
    AV_PICTURE_STRUCTURE_BOTTOM_FIELD, ///< 编码为底场
    AV_PICTURE_STRUCTURE_FRAME,        ///< 编码为帧
};

typedef struct AVCodecParserContext {
    void *priv_data;
    const struct AVCodecParser *parser;
    int64_t frame_offset; /* offset of the current frame */
    int64_t cur_offset; /* current offset
                           (incremented by each av_parser_parse()) */
    int64_t next_frame_offset; /* offset of the next frame */
    /* video info */
    int pict_type; /* XXX: Put it back in AVCodecContext. */
    /**
     * This field is used for proper frame duration computation in lavf.
     * It signals, how much longer the frame duration of the current frame
     * is compared to normal frame duration.
     *
     * frame_duration = (1 + repeat_pict) * time_base
     *
     * It is used by codecs like H.264 to display telecined material.
     */
    int repeat_pict; /* XXX: Put it back in AVCodecContext. */
    int64_t pts;     /* pts of the current frame */
    int64_t dts;     /* dts of the current frame */

    /* 私有数据 */
    int64_t last_pts;
    int64_t last_dts;
    int fetch_timestamp;

#define AV_PARSER_PTS_NB 4
    int cur_frame_start_index;
    int64_t cur_frame_offset[AV_PARSER_PTS_NB];
    int64_t cur_frame_pts[AV_PARSER_PTS_NB];
    int64_t cur_frame_dts[AV_PARSER_PTS_NB];

    int flags;
#define PARSER_FLAG_COMPLETE_FRAMES           0x0001
#define PARSER_FLAG_ONCE                      0x0002
/// 解析器具有有效文件偏移时设置
#define PARSER_FLAG_FETCHED_OFFSET            0x0004
#define PARSER_FLAG_USE_CODEC_TS              0x1000

    int64_t offset;      ///< 相对于起始数据包起点的字节偏移
    int64_t cur_frame_end[AV_PARSER_PTS_NB];

    /**
     * Set by parser to 1 for key frames and 0 for non-key frames.
     * It is initialized to -1, so if the parser doesn't set this flag,
     * old-style fallback using AV_PICTURE_TYPE_I picture type as key frames
     * will be used.
     */
    int key_frame;

    // 时间戳生成支持：
    /**
     * Synchronization point for start of timestamp generation.
     *
     * Set to >0 for sync point, 0 for no sync point and <0 for undefined
     * (default).
     *
     * For example, this corresponds to presence of H.264 buffering period
     * SEI message.
     */
    int dts_sync_point;

    /**
     * Offset of the current timestamp against last timestamp sync point in
     * units of AVCodecContext.time_base.
     *
     * Set to INT_MIN when dts_sync_point 不使用. Otherwise, it must
     * contain a valid timestamp offset.
     *
     * Note that the timestamp of sync point has usually a nonzero
     * dts_ref_dts_delta, which refers to the previous sync point. Offset of
     * the next frame after timestamp sync point will be usually 1.
     *
     * For example, this corresponds to H.264 cpb_removal_delay.
     */
    int dts_ref_dts_delta;

    /**
     * Presentation delay of current frame in units of AVCodecContext.time_base.
     *
     * Set to INT_MIN when dts_sync_point 不使用. Otherwise, it must
     * contain valid non-negative timestamp delta (presentation time of a frame
     * must not lie in the past).
     *
     * This delay represents the difference between decoding and presentation
     * time of the frame.
     *
     * For example, this corresponds to H.264 dpb_output_delay.
     */
    int pts_dts_delta;

    /**
     * 数据包在文件中的位置。
     *
     * Analogous to cur_frame_pts/dts
     */
    int64_t cur_frame_pos[AV_PARSER_PTS_NB];

    /**
     * Byte position of currently parsed frame in stream.
     */
    int64_t pos;

    /**
     * 前一帧的字节位置。
     */
    int64_t last_pos;

    /**
     * 当前帧的时长。
     * For audio, this is in units of 1 / AVCodecContext.sample_rate.
     * For all other types, this is in units of AVCodecContext.time_base.
     */
    int duration;

    enum AVFieldOrder field_order;

    /**
     * Indicate whether a picture is coded as a frame, top field or bottom field.
     *
     * For example, H.264 field_pic_flag equal to 0 corresponds to
     * AV_PICTURE_STRUCTURE_FRAME. An H.264 picture with field_pic_flag
     * equal to 1 and bottom_field_flag equal to 0 corresponds to
     * AV_PICTURE_STRUCTURE_TOP_FIELD.
     */
    enum AVPictureStructure picture_structure;

    /**
     * Picture number incremented in presentation or output order.
     * This field may be reinitialized at the first picture of a new sequence.
     *
     * For example, this corresponds to H.264 PicOrderCnt.
     */
    int output_picture_number;

    /**
     * 用于显示的已解码视频尺寸。
     */
    int width;
    int height;

    /**
     * 已编码视频的尺寸。
     */
    int coded_width;
    int coded_height;

    /**
     * The format of the coded data, corresponds to enum AVPixelFormat for video
     * and for enum AVSampleFormat for audio.
     *
     * Note that a decoder can have considerable freedom in how exactly it
     * decodes the data, so the format reported here might be different from the
     * one returned by a decoder.
     */
    int format;
} AVCodecParserContext;

typedef struct AVCodecParser {
    enum AVCodecID codec_ids[7]; /* several codec IDs are permitted */
} AVCodecParser;

/**
 * 遍历所有已注册的编解码器解析器。
 *
 * @param opaque a pointer where libavcodec will store the iteration state. Must
 *               point to NULL to start the iteration.
 *
 * @return the next registered codec parser or NULL when the iteration is
 *         finished
 */
const AVCodecParser *av_parser_iterate(void **opaque);

AVCodecParserContext *av_parser_init(enum AVCodecID codec_id);

/**
 * 解析数据包。
 *
 * @param s             parser context.
 * @param avctx         编解码器上下文.
 * @param poutbuf       set to pointer to parsed buffer or NULL if not yet finished.
 * @param poutbuf_size  set to size of parsed buffer or zero if not yet finished.
 * @param buf           input buffer.
 * @param buf_size      buffer size in bytes without the padding. I.e. the full buffer
                        size is assumed to be buf_size + AV_INPUT_BUFFER_PADDING_SIZE.
                        To signal EOF, this should be 0 (so that the last frame
                        can be output).
 * @param pts           input presentation timestamp.
 * @param dts           input decoding timestamp.
 * @param pos           input byte position in stream.
 * @return the number of bytes of the input bitstream used.
 *
 * Example:
 * @code
 *   while(in_len){
 *       len = av_parser_parse2(myparser, AVCodecContext, &data, &size,
 *                                        in_data, in_len,
 *                                        pts, dts, pos);
 *       in_data += len;
 *       in_len  -= len;
 *
 *       if(size)
 *          decode_frame(data, size);
 *   }
 * @endcode
 */
int av_parser_parse2(AVCodecParserContext *s,
                     AVCodecContext *avctx,
                     uint8_t **poutbuf, int *poutbuf_size,
                     const uint8_t *buf, int buf_size,
                     int64_t pts, int64_t dts,
                     int64_t pos);

void av_parser_close(AVCodecParserContext *s);

/**
 * @}
 * @}
 */

/**
 * @addtogroup lavc_encoding
 * @{
 */

int avcodec_encode_subtitle(AVCodecContext *avctx, uint8_t *buf, int buf_size,
                            const AVSubtitle *sub);


/**
 * @}
 */

/**
 * @defgroup lavc_misc 实用函数
 * @ingroup libavc
 *
 * Miscellaneous utility functions related to both encoding and decoding
 * (or neither).
 * @{
 */

/**
 * @defgroup lavc_misc_pixfmt 像素格式
 *
 * 用于处理像素格式的函数。
 * @{
 */

/**
 * Return a value representing the fourCC code associated to the
 * pixel format pix_fmt, or 0 if no associated fourCC code can be
 * found.
 */
unsigned int avcodec_pix_fmt_to_codec_tag(enum AVPixelFormat pix_fmt);

/**
 * Find the best pixel format to convert to given a certain source pixel
 * format.  When converting from one pixel format to another, information loss
 * may occur.  For example, when converting from RGB24 to GRAY, the color
 * information will be lost. Similarly, other losses occur when converting from
 * some formats to other formats. avcodec_find_best_pix_fmt_of_2() searches which of
 * the given pixel formats should be used to suffer the least amount of loss.
 * The pixel formats from which it chooses one, are determined by the
 * pix_fmt_list parameter.
 *
 *
 * @param[in] pix_fmt_list AV_PIX_FMT_NONE terminated array of pixel formats to choose from
 * @param[in] src_pix_fmt source pixel format
 * @param[in] has_alpha Whether the source pixel format alpha channel is used.
 * @param[out] loss_ptr Combination of flags informing you what kind of losses will occur.
 * @return 要转换到的最佳像素格式；若未找到则返回 -1。
 */
enum AVPixelFormat avcodec_find_best_pix_fmt_of_list(const enum AVPixelFormat *pix_fmt_list,
                                            enum AVPixelFormat src_pix_fmt,
                                            int has_alpha, int *loss_ptr);

enum AVPixelFormat avcodec_default_get_format(struct AVCodecContext *s, const enum AVPixelFormat * fmt);

/**
 * @}
 */

void avcodec_string(char *buf, int buf_size, AVCodecContext *enc, int encode);

int avcodec_default_execute(AVCodecContext *c, int (*func)(AVCodecContext *c2, void *arg2),void *arg, int *ret, int count, int size);
int avcodec_default_execute2(AVCodecContext *c, int (*func)(AVCodecContext *c2, void *arg2, int, int),void *arg, int *ret, int count);
//FIXME func typedef

/**
 * 填充 AVFrame 音频数据和 linesize 指针。
 *
 * The buffer buf must be a preallocated buffer with a size big enough
 * to contain the specified samples amount. The filled AVFrame data
 * pointers will point to this buffer.
 *
 * AVFrame extended_data channel pointers are allocated if necessary for
 * planar audio.
 *
 * @param frame       the AVFrame
 *                    frame->nb_samples must be set prior to calling the
 *                    function. This function fills in frame->data,
 *                    frame->extended_data, frame->linesize[0].
 * @param nb_channels channel count
 * @param sample_fmt  sample format
 * @param buf         buffer to use for frame data
 * @param buf_size    size of buffer
 * @param align       plane size sample alignment (0 = default)
 * @return            >=0 on 成功, negative error code on failure
 * @todo return the size in bytes required to store the samples in
 * case of 成功, at the next libavutil bump
 */
int avcodec_fill_audio_frame(AVFrame *frame, int nb_channels,
                             enum AVSampleFormat sample_fmt, const uint8_t *buf,
                             int buf_size, int align);

/**
 * 重置内部编解码器状态/冲刷内部缓冲区。应在以下情况调用
 * 例如定位或切换到其他流时。
 *
 * @note for decoders, this function just releases any references the decoder
 * might keep internally, but the caller's references remain valid.
 *
 * @note for encoders, this function will only do something if the encoder
 * declares support for AV_CODEC_CAP_ENCODER_FLUSH. When called, the encoder
 * will drain any remaining packets, and can then be reused for a different
 * stream (as opposed to sending a null frame which will leave the encoder
 * in a permanent EOF state after draining). This can be desirable if the
 * cost of tearing down and replacing the encoder instance is high.
 */
void avcodec_flush_buffers(AVCodecContext *avctx);

/**
 * 返回音频帧时长。
 *
 * @param avctx        编解码器上下文
 * @param frame_bytes  size of the frame, or 0 if 未知
 * @return             frame duration, in samples, if known. 0 if not able to
 *                     determine.
 */
int av_get_audio_frame_duration(AVCodecContext *avctx, int frame_bytes);

/* memory */

/**
 * 行为与 av_fast_malloc 相同，但缓冲区末尾额外包含
 * AV_INPUT_BUFFER_PADDING_SIZE ，该区域始终为 0。
 *
 * In addition the whole buffer will initially and after resizes
 * be 0-initialized so that no uninitialized data will ever appear.
 */
void av_fast_padded_malloc(void *ptr, unsigned int *size, size_t min_size);

/**
 * 行为与 av_fast_padded_malloc 相同，但缓冲区调用后始终
 * 被初始化为 0。
 */
void av_fast_padded_mallocz(void *ptr, unsigned int *size, size_t min_size);

/**
 * @return 若 s 已打开则返回正值 (i.e. avcodec_open2() was called on it),
 * 否则返回 0。
 */
int avcodec_is_open(AVCodecContext *s);

/**
 * @}
 */

#endif /* AVCODEC_AVCODEC_H */


