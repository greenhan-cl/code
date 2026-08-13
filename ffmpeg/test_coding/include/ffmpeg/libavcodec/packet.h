/*
 * AVPacket public API
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

#ifndef AVCODEC_PACKET_H
#define AVCODEC_PACKET_H

#include <stddef.h>
#include <stdint.h>

#include "libavutil/attributes.h"
#include "libavutil/buffer.h"
#include "libavutil/dict.h"
#include "libavutil/rational.h"
#include "libavutil/version.h"

#include "libavcodec/version_major.h"

/**
 * @defgroup lavc_packet_side_data AVPacketSideData
 *
 * 用于处理 AVPacketSideData 的类型和函数。
 * @{
 */
enum AVPacketSideDataType {
    /**
     * AV_PKT_DATA_PALETTE 侧数据包恰好包含 AVPALETTE_SIZE 字节的调色板。
     * 此侧数据表示存在新调色板。
     */
    AV_PKT_DATA_PALETTE,

    /**
     * AV_PKT_DATA_NEW_EXTRADATA 用于通知编解码器或格式 extradata 缓冲区已更改，
     * 接收方应作相应处理。新的 extradata 嵌入侧数据缓冲区，
     * 应立即用于处理当前帧或数据包。
     */
    AV_PKT_DATA_NEW_EXTRADATA,

    /**
     * AV_PKT_DATA_PARAM_CHANGE 侧数据包的布局如下：
     * @code
     * u32le param_flags
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_SAMPLE_RATE)
     *     s32le sample_rate
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_DIMENSIONS)
     *     s32le width
     *     s32le height
     * @endcode
     */
    AV_PKT_DATA_PARAM_CHANGE,

    /**
     * AV_PKT_DATA_H263_MB_INFO 侧数据包包含若干宏块信息结构体，
     * 用于在宏块边界将数据包拆成更小的包（例如 RFC 2190）。
     * 它不一定包含所有宏块的信息，只要信息中宏块间距离小于目标有效载荷大小即可。
     * 每个 MB 信息结构体为 12 字节，布局如下：
     * @code
     * u32le bit offset from the start of the packet
     * u8    current quantizer at the start of the macroblock
     * u8    GOB number
     * u16le macroblock address within the GOB
     * u8    horizontal MV predictor
     * u8    vertical MV predictor
     * u8    horizontal MV predictor for block number 3
     * u8    vertical MV predictor for block number 3
     * @endcode
     */
    AV_PKT_DATA_H263_MB_INFO,

    /**
     * 此侧数据应与音频流关联，并以 AVReplayGain 结构体形式包含 ReplayGain 信息。
     */
    AV_PKT_DATA_REPLAYGAIN,

    /**
     * 此侧数据包含描述仿射变换的 3x3 变换矩阵，必须将其应用于解码后的视频帧
     * 才能正确呈现。
     *
     * 数据的详细说明参见 libavutil/display.h。
     */
    AV_PKT_DATA_DISPLAYMATRIX,

    /**
     * 此侧数据应与视频流关联，并以 AVStereo3D 结构体形式包含立体 3D 信息。
     */
    AV_PKT_DATA_STEREO3D,

    /**
     * 此侧数据应与音频流关联，并对应枚举 AVAudioServiceType。
     */
    AV_PKT_DATA_AUDIO_SERVICE_TYPE,

    /**
     * 此侧数据包含来自编码器的质量相关信息。
     * @code
     * u32le quality factor of the compressed frame. Allowed range is between 1 (good) and FF_LAMBDA_MAX (bad).
     * u8    picture type
     * u8    error count
     * u16   reserved
     * u64le[error count] sum of squared differences between encoder in and output
     * @endcode
     */
    AV_PKT_DATA_QUALITY_STATS,

    /**
     * 此侧数据包含表示“回退”轨道流索引的整数。当当前轨道因某种原因无法解码时，
     * 回退轨道表示可使用的替代轨道，例如没有适用于编解码器的解码器。
     */
    AV_PKT_DATA_FALLBACK_TRACK,

    /**
     * 此侧数据对应 AVCPBProperties 结构体。
     */
    AV_PKT_DATA_CPB_PROPERTIES,

    /**
     * 建议跳过指定数量的采样。
     * @code
     * u32le number of samples to skip from start of this packet
     * u32le number of samples to skip from end of this packet
     * u8    reason for start skip
     * u8    reason for end   skip (0=padding silence, 1=convergence)
     * @endcode
     */
    AV_PKT_DATA_SKIP_SAMPLES,

    /**
     * AV_PKT_DATA_JP_DUALMONO 侧数据包表示该包可能包含日本 DTV 特有的“双单声道”音频；
     * 如果确实如此，建议仅使用所选声道。
     * @code
     * u8    selected channels (0=main/left, 1=sub/right, 2=both)
     * @endcode
     */
    AV_PKT_DATA_JP_DUALMONO,

    /**
     * 以零结尾的键/值字符串列表。列表没有结束标记，必须依靠侧数据大小确定结尾。
     */
    AV_PKT_DATA_STRINGS_METADATA,

    /**
     * 字幕事件位置
     * @code
     * u32le x1
     * u32le y1
     * u32le x2
     * u32le y2
     * @endcode
     */
    AV_PKT_DATA_SUBTITLE_POSITION,

    /**
     * Matroska 容器 BlockAdditional 元素中的数据。数据没有结束标记，
     * 必须依靠侧数据大小识别结尾。内容为 8 字节 ID（见 BlockAddId）后跟数据。
     */
    AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL,

    /**
     * WebVTT cue 可选的第一行标识符。
     */
    AV_PKT_DATA_WEBVTT_IDENTIFIER,

    /**
     * 紧跟 WebVTT cue 时间戳说明符的可选设置（渲染指令）。
     */
    AV_PKT_DATA_WEBVTT_SETTINGS,

    /**
     * 以零结尾的键/值字符串列表。列表没有结束标记，必须依靠侧数据大小确定结尾。
     * 此侧数据包含流中出现的更新元数据。
     */
    AV_PKT_DATA_METADATA_UPDATE,

    /**
     * uint8_t 形式的 MPEGTS 流 ID，用于将流 ID 信息从解封装器传给对应封装器。
     */
    AV_PKT_DATA_MPEGTS_STREAM_ID,

    /**
     * 母版显示元数据（基于 SMPTE-2086:2014）。此元数据应与视频流关联，
     * 并以 AVMasteringDisplayMetadata 结构体形式包含数据。
     */
    AV_PKT_DATA_MASTERING_DISPLAY_METADATA,

    /**
     * 此侧数据应与视频流关联，并对应 AVSphericalMapping 结构体。
     */
    AV_PKT_DATA_SPHERICAL,

    /**
     * 内容亮度级别（基于 CTA-861.3）。此元数据应与视频流关联，
     * 并以 AVContentLightMetadata 结构体形式包含数据。
     */
    AV_PKT_DATA_CONTENT_LIGHT_LEVEL,

    /**
     * ATSC A53 第 4 部分隐藏字幕。此元数据应与视频流关联。A53 CC 比特流以 uint8_t
     * 存储在 AVPacketSideData.data 中，CC 数据字节数为 AVPacketSideData.size。
     */
    AV_PKT_DATA_A53_CC,

    /**
     * 此侧数据是加密初始化数据。其格式不属于 ABI，请使用
     * av_encryption_init_info_* 方法访问。
     */
    AV_PKT_DATA_ENCRYPTION_INIT_INFO,

    /**
     * 此侧数据包含如何解密数据包的加密信息。其格式不属于 ABI，
     * 请使用 av_encryption_info_* 方法访问。
     */
    AV_PKT_DATA_ENCRYPTION_INFO,

    /**
     * ETSI TS 101 154 规定的活动格式描述数据，由使用 AVActiveFormatDescription
     * 枚举的单个字节组成。
     */
    AV_PKT_DATA_AFD,

    /**
     * 对应 AVProducerReferenceTime 结构体的生产者参考时间数据，通常由某些编码器导出
     * （通过 AVCodecContext export_side_data 字段中设置的 prft 标志按需导出）。
     */
    AV_PKT_DATA_PRFT,

    /**
     * ICC 配置文件数据，由遵循 ISO 15076-1 所述格式的不透明八位组缓冲区组成。
     */
    AV_PKT_DATA_ICC_PROFILE,

    /**
     * DOVI 配置
     * 参考：
     * dolby-vision-bitstreams-within-the-iso-base-media-file-format-v2.1.2, section 2.2
     * dolby-vision-bitstreams-in-mpeg-2-transport-stream-multiplex-v1.2, section 3.3
     * 标签存储在结构体 AVDOVIDecoderConfigurationRecord 中。
     */
    AV_PKT_DATA_DOVI_CONF,

    /**
     * 符合 SMPTE ST 12-1:2014 的时间码。数据是包含 4 个 uint32_t 的数组，
     * 第一个 uint32_t 描述其余时间码中使用了几个（1-3 个）。时间码格式见
     * libavutil/timecode.h 中 av_timecode_get_smpte_from_framenum() 的文档。
     */
    AV_PKT_DATA_S12M_TIMECODE,

    /**
     * 与视频帧关联的 HDR10+ 动态元数据。元数据采用 AVDynamicHDRPlus 结构体形式，
     * 包含 SMPTE 2094-40:2016 标准应用 4 的色彩体积变换信息。
     */
    AV_PKT_DATA_DYNAMIC_HDR10_PLUS,

    /**
     * 与音频帧关联的 IAMF 混音增益参数数据。此元数据采用 AVIAMFParamDefinition
     * 结构体形式，包含沉浸式音频模型和格式标准第 3.6.1、3.8.1 节定义的信息。
     */
    AV_PKT_DATA_IAMF_MIX_GAIN_PARAM,

    /**
     * 与音频帧关联的 IAMF 解混信息参数数据。此元数据采用 AVIAMFParamDefinition
     * 结构体形式，包含沉浸式音频模型和格式标准第 3.6.1、3.8.2 节定义的信息。
     */
    AV_PKT_DATA_IAMF_DEMIXING_INFO_PARAM,

    /**
     * 与音频帧关联的 IAMF 重建增益信息参数数据。此元数据采用 AVIAMFParamDefinition
     * 结构体形式，包含沉浸式音频模型和格式标准第 3.6.1、3.8.3 节定义的信息。
     */
    AV_PKT_DATA_IAMF_RECON_GAIN_INFO_PARAM,

    /**
     * H.274 定义的环境观看条件元数据。此元数据应与视频流关联，
     * 并以 AVAmbientViewingEnvironment 结构体形式包含数据。
    */
    AV_PKT_DATA_AMBIENT_VIEWING_ENVIRONMENT,

    /**
     * 为获得预期呈现的子矩形，需要从解码帧上/下/左/右边界丢弃的像素数。
     *
     * @code
     * u32le crop_top
     * u32le crop_bottom
     * u32le crop_left
     * u32le crop_right
     * @endcode
     */
    AV_PKT_DATA_FRAME_CROPPING,

    /**
     * uint8_t 数组形式的原始 LCEVC 有效载荷数据，保留 NAL 防竞争字节。
     */
    AV_PKT_DATA_LCEVC,

    /**
     * 此侧数据包含参考显示宽度、参考观看距离及对应参考立体视图对的信息，
     * 即在参考观看距离下，参考显示器上为观看者左右眼显示的视图对。
     * 有效载荷是 libavutil/tdrdi.h 中定义的 AV3DReferenceDisplaysInfo 结构体。
     */
    AV_PKT_DATA_3D_REFERENCE_DISPLAYS,

    /**
     * 以 AVRTCPSenderReport 结构体形式包含最后收到的 RTCP SR（发送方报告）信息。
     */
    AV_PKT_DATA_RTCP_SR,

    /**
     * 可交换图像文件格式元数据。有效载荷是包含 EXIF 元数据的缓冲区，
     * 以 49 49 2a 00 或 4d 4d 00 2a 开始。
     */
     AV_PKT_DATA_EXIF,

    /**
     * 与视频帧关联的 HDR 动态元数据。有效载荷类型为 AVDynamicHDRSmpte2094App5，
     * 包含 SMPTE 2094-50 标准规定的色彩体积变换信息。
     */
    AV_PKT_DATA_DYNAMIC_HDR_SMPTE_2094_APP5,

    /**
     * Dolby Vision 增强层 HEVC 解码器配置。从基于 ISOM 的容器中的 @c hvcE box，
     * 或 Matroska 中对应的 BlockAdditionMapping 解析。数据是 ISO 14496-15
     * 定义的原始 HEVCDecoderConfigurationRecord。
     */
    AV_PKT_DATA_HEVC_CONF,

    /**
     * 侧数据类型的数量。它不属于公共 API/ABI，因为添加新侧数据类型时可能改变。
     * 此值必须始终是最后一个枚举值。如果其值变得很大，需要更新某些使用它的代码，
     * 因为这些代码假定它小于其他限制。
     */
    AV_PKT_DATA_NB
};

/**
 * 此结构体存储用于解码、呈现或以其他方式处理编码流的辅助信息。
 * 通常由解封装器和编码器导出，可按包传给解码器和封装器，
 * 也可作为适用于整个编码流的全局侧数据传入。
 *
 * 全局侧数据按如下方式处理：
 * - 解封装时，可通过 @ref AVCodecParameters.coded_side_data "AVStream 的编解码器参数"
 *   导出，再通过 @ref AVCodecContext.coded_side_data "解码器上下文的侧数据"
 *   作为输入传给解码器，用于初始化。
 * - 封装时，可通过 @ref AVCodecParameters.coded_side_data "AVStream 的编解码器参数"
 *   输入，通常来自编码器通过 @ref AVCodecContext.coded_side_data
 *   "编码器上下文的侧数据" 产生的输出，用于初始化。
 *
 * 数据包专用侧数据按如下方式处理：
 * - 解封装时，可通过 @ref AVPacket.side_data "AVPacket 的侧数据" 导出，
 *   再作为输入传给解码器。
 * - 封装时，可通过 @ref AVPacket.side_data "AVPacket 的侧数据" 输入，
 *   通常是编码器的输出。
 *
 * 不同模块可能根据媒体类型和编解码器接受或导出不同类型的侧数据。
 * 已定义类型及其出现或使用位置参见 @ref AVPacketSideDataType 。
 */
typedef struct AVPacketSideData {
    uint8_t *data;
    size_t   size;
    enum AVPacketSideDataType type;
} AVPacketSideData;

/**
 * 分配新的数据包侧数据。
 *
 * @param sd    指向要添加侧数据的侧数据数组。*sd 可以为 NULL，此时会初始化数组
 * @param nb_sd 指向包含数组条目数的整数。成功时该整数值增加 1
 * @param type  侧数据类型
 * @param size  所需侧数据大小
 * @param flags 当前未使用，必须为 0
 *
 * @return 成功返回指向新分配侧数据的指针，否则返回 NULL。
 */
AVPacketSideData *av_packet_side_data_new(AVPacketSideData **psd, int *pnb_sd,
                                          enum AVPacketSideDataType type,
                                          size_t size, int flags);

/**
 * 将现有数据包装为数据包侧数据。
 *
 * @param sd    指向要添加侧数据的侧数据数组。*sd 可以为 NULL，此时会初始化数组
 * @param nb_sd 指向包含数组条目数的整数。成功时该整数值增加 1
 * @param type  侧数据类型
 * @param data  数据数组。必须使用 av_malloc() 系列函数分配。
 *              成功时数据所有权转移给侧数据数组
 * @param size  数据数组大小
 * @param flags 当前未使用，必须为 0
 *
 * @return 成功返回指向新分配侧数据的指针，否则返回 NULL。
 *         失败时侧数据数组不变，数据仍归调用方所有。
 */
AVPacketSideData *av_packet_side_data_add(AVPacketSideData **sd, int *nb_sd,
                                          enum AVPacketSideDataType type,
                                          void *data, size_t size, int flags);

/**
 * 从侧数据数组获取侧信息。
 *
 * @param sd    要从中获取侧数据的数组
 * @param nb_sd 数组条目数
 * @param type  所需侧信息类型
 *
 * @return 存在时返回侧数据指针，否则返回 NULL
 */
const AVPacketSideData *av_packet_side_data_get(const AVPacketSideData *sd,
                                                int nb_sd,
                                                enum AVPacketSideDataType type);

/**
 * 从侧数据数组移除给定类型的侧数据。
 *
 * @param sd    要从中移除侧数据的数组
 * @param nb_sd 指向包含数组条目数的整数。返回时减去已移除的条目数
 * @param type  侧信息类型
 */
void av_packet_side_data_remove(AVPacketSideData *sd, int *nb_sd,
                                enum AVPacketSideDataType type);

/**
 * 释放数组中所有侧数据及数组本身的便捷函数。
 *
 * @param sd    指向要释放的侧数据数组，返回时设为 NULL
 * @param nb_sd 指向包含数组条目数的整数，返回时设为 0
 */
void av_packet_side_data_free(AVPacketSideData **sd, int *nb_sd);

struct AVFrameSideData;

/**
 * 如果存在匹配的数据包侧数据类型，则根据现有帧侧数据向数组添加新条目。
 *
 * @param flags              当前未使用，必须为 0
 * @retval >= 0              成功
 * @retval AVERROR(EINVAL)   帧侧数据类型没有匹配的数据包侧数据类型
 * @retval AVERROR(ENOMEM)   向数组添加侧数据条目失败或类似错误
 */
int av_packet_side_data_from_frame(AVPacketSideData **sd, int *nb_sd,
                                   const struct AVFrameSideData *src, unsigned int flags);
/**
 * 如果存在匹配的帧侧数据类型，则根据现有数据包侧数据向数组添加新条目。
 *
 * @param flags              AV_FRAME_SIDE_DATA_FLAG_* 标志的某种组合，或 0
 * @retval >= 0              成功
 * @retval AVERROR(EINVAL)   数据包侧数据类型没有匹配的帧侧数据类型
 * @retval AVERROR(ENOMEM)   向数组添加侧数据条目失败或类似错误
 */
int av_packet_side_data_to_frame(struct AVFrameSideData ***sd, int *nb_sd,
                                 const AVPacketSideData *src, unsigned int flags);

const char *av_packet_side_data_name(enum AVPacketSideDataType type);

/**
 * @}
 */

/**
 * @defgroup lavc_packet AVPacket
 *
 * 用于处理 AVPacket 的类型和函数。
 * @{
 */

/**
 * 此结构体存储压缩数据。通常由解封装器导出并作为输入传给解码器，
 * 或作为编码器输出接收后传给封装器。
 *
 * 对视频而言通常包含一个压缩帧；对音频而言可能包含多个压缩帧。
 * 允许编码器输出不含压缩数据、仅含侧数据的空包
 * （例如在编码结束时更新某些流参数）。
 *
 * 数据所有权语义取决于 buf 字段。设置该字段时，数据包数据动态分配，
 * 在 av_packet_unref() 将引用计数减为 0 前一直有效。
 *
 * 如果未设置 buf 字段，av_packet_ref() 会创建副本而不是增加引用计数。
 *
 * 侧数据始终使用 av_malloc() 分配，由 av_packet_ref() 复制，
 * 并由 av_packet_unref() 释放。
 *
 * 不再建议将 sizeof(AVPacket) 作为公共 ABI 的一部分。av_init_packet() 移除后，
 * 新数据包将只能使用 av_packet_alloc() 分配，并可随次版本升级在结构体末尾添加字段。
 *
 * @see av_packet_alloc
 * @see av_packet_ref
 * @see av_packet_unref
 */
typedef struct AVPacket {
    /**
     * 对存储数据包数据的引用计数缓冲区的引用。
     * 可以为 NULL，此时数据包数据不使用引用计数。
     */
    AVBufferRef *buf;
    /**
     * 呈现时间戳，单位为 AVStream->time_base；表示解压后的包何时呈现给用户。
     * 文件未存储时可为 AV_NOPTS_VALUE。由于呈现不能早于解压，pts 必须大于或等于 dts
     * （除非只是查看十六进制转储）。某些格式对 dts 和 pts/cts 术语另有误用，
     * 此类时间戳必须在存入 AVPacket 前转换为真正的 pts/dts。
     */
    int64_t pts;
    /**
     * 解压时间戳，单位为 AVStream->time_base；表示数据包解压的时间。
     * 文件未存储时可为 AV_NOPTS_VALUE。
     */
    int64_t dts;
    uint8_t *data;
    int   size;
    int   stream_index;
    /**
     * AV_PKT_FLAG 值的组合
     */
    int   flags;
    /**
     * 容器可提供的附加数据包数据。数据包可包含多种侧信息。
     */
    AVPacketSideData *side_data;
    int side_data_elems;

    /**
     * 此数据包的时长，单位为 AVStream->time_base；未知时为 0。
     * 按呈现顺序等于 next_pts - this_pts。
     */
    int64_t duration;

    int64_t pos;                            ///< 在流中的字节位置，未知时为 -1

    /**
     * 用于用户的某些私有数据
     */
    void *opaque;

    /**
     * 供 API 用户自由使用的 AVBufferRef。FFmpeg 永远不会检查缓冲区引用的内容。
     * 数据包解引用时，FFmpeg 会对其调用 av_buffer_unref()。
     * av_packet_copy_props() 会用 av_buffer_ref() 为目标包的 opaque_ref 创建新引用。
     *
     * 尽管用途相似，但此字段与 opaque 字段无关。
     */
    AVBufferRef *opaque_ref;

    /**
     * 数据包时间戳的时间基。将来编码器或解封装器输出的包可能设置此字段，
     * 但输入解码器或封装器时默认忽略其值。
     */
    AVRational time_base;
} AVPacket;

#define AV_PKT_FLAG_KEY     0x0001 ///< 数据包包含关键帧
#define AV_PKT_FLAG_CORRUPT 0x0002 ///< 数据包内容已损坏
/**
 * 此标志用于丢弃维持有效解码器状态所需、但输出不需要且应在解码后丢弃的数据包。
 **/
#define AV_PKT_FLAG_DISCARD   0x0004
/**
 * 数据包来自可信来源。
 *
 * 可以跟随原本不安全的结构，例如指向数据包外部数据的任意指针。
 */
#define AV_PKT_FLAG_TRUSTED   0x0008
/**
 * 此标志表示数据包包含可被解码器丢弃的帧，即非参考帧。
 */
#define AV_PKT_FLAG_DISPOSABLE 0x0010

enum AVSideDataParamChangeFlags {
    AV_SIDE_DATA_PARAM_CHANGE_SAMPLE_RATE    = 0x0004,
    AV_SIDE_DATA_PARAM_CHANGE_DIMENSIONS     = 0x0008,
};

/**
 * 分配 AVPacket 并将字段设为默认值。得到的结构体必须使用 av_packet_free() 释放。
 *
 * @return 已填充默认值的 AVPacket，失败时返回 NULL。
 *
 * @note 此函数只分配 AVPacket 本身，不分配数据缓冲区。后者必须通过
 *       av_new_packet 等其他方式分配。
 *
 * @see av_new_packet
 */
AVPacket *av_packet_alloc(void);

/**
 * 创建引用与 src 相同数据的新数据包。
 *
 * 这是 av_packet_alloc()+av_packet_ref() 的快捷方式。
 *
 * @return 成功返回新创建的 AVPacket，出错返回 NULL。
 *
 * @see av_packet_alloc
 * @see av_packet_ref
 */
AVPacket *av_packet_clone(const AVPacket *src);

/**
 * 释放数据包；如果数据包使用引用计数，会先将其解引用。
 *
 * @param pkt 要释放的数据包。该指针会被设为 NULL。
 * @note 传入 NULL 不执行任何操作。
 */
void av_packet_free(AVPacket **pkt);

#if FF_API_INIT_PACKET
/**
 * 使用默认值初始化数据包的可选字段。
 *
 * 注意，此函数不修改 data 和 size 成员，必须单独初始化它们。
 *
 * @param pkt 数据包
 *
 * @see av_packet_alloc
 * @see av_packet_unref
 *
 * @deprecated 此函数已弃用。移除后，sizeof(AVPacket) 将不再属于 ABI。
 */
attribute_deprecated
void av_init_packet(AVPacket *pkt);
#endif

/**
 * 分配数据包的有效载荷，并使用默认值初始化字段。
 *
 * @param pkt 数据包
 * @param size 所需有效载荷大小
 * @return 成功返回 0，否则返回 AVERROR_xxx
 */
int av_new_packet(AVPacket *pkt, int size);

/**
 * 缩小数据包，并正确清零填充区域。
 *
 * @param pkt 数据包
 * @param size 新大小
 */
void av_shrink_packet(AVPacket *pkt, int size);

/**
 * 增大数据包，并正确清零填充区域。
 *
 * @param pkt 数据包
 * @param grow_by 数据包要增加的字节数
 */
int av_grow_packet(AVPacket *pkt, int grow_by);

/**
 * 使用 av_malloc() 分配的数据初始化引用计数数据包。
 *
 * @param pkt 要初始化的数据包。此函数会设置 data、size 和 buf 字段，其他字段不变。
 * @param data 由 av_malloc() 分配、用作数据包数据的数据。函数成功返回后，
 *        数据归底层 AVBuffer 所有，调用方不得通过其他方式访问。
 * @param size 不含填充的数据大小，单位为字节。完整缓冲区大小假定为
 *        size + AV_INPUT_BUFFER_PADDING_SIZE。
 *
 * @return 成功返回 0，出错返回负的 AVERROR
 */
int av_packet_from_data(AVPacket *pkt, uint8_t *data, int size);

/**
 * 分配新的数据包侧信息。
 *
 * @param pkt 数据包
 * @param type 侧信息类型
 * @param size 侧信息大小
 * @return 指向新分配数据的指针，否则返回 NULL
 */
uint8_t* av_packet_new_side_data(AVPacket *pkt, enum AVPacketSideDataType type,
                                 size_t size);

/**
 * 将现有数组包装为数据包侧数据。
 *
 * @param pkt 数据包
 * @param type 侧信息类型
 * @param data 侧数据数组。必须使用 av_malloc() 系列函数分配。
 *             数据所有权会转移给 pkt。
 * @param size 侧信息大小
 * @return 成功返回非负数，失败返回负的 AVERROR 错误码。
 *         失败时数据包不变，数据仍归调用方所有。
 */
int av_packet_add_side_data(AVPacket *pkt, enum AVPacketSideDataType type,
                            uint8_t *data, size_t size);

/**
 * 缩小已分配的侧数据缓冲区。
 *
 * @param pkt 数据包
 * @param type 侧信息类型
 * @param size 新的侧信息大小
 * @return 成功返回 0，失败返回 < 0
 */
int av_packet_shrink_side_data(AVPacket *pkt, enum AVPacketSideDataType type,
                               size_t size);

/**
 * 从数据包获取侧信息。
 *
 * @param pkt 数据包
 * @param type 所需侧信息类型
 * @param size 如果提供，则设为侧数据大小；所需侧数据不存在时设为 0
 * @return 数据存在时返回其指针，否则返回 NULL
 */
uint8_t* av_packet_get_side_data(const AVPacket *pkt, enum AVPacketSideDataType type,
                                 size_t *size);

/**
 * 打包字典以供 side_data 使用。
 *
 * @param dict 要打包的字典
 * @param size 用于存储返回数据大小的指针
 * @return 成功返回数据指针，否则返回 NULL
 */
uint8_t *av_packet_pack_dictionary(const AVDictionary *dict, size_t *size);
/**
 * 从 side_data 解包字典。
 *
 * @param data 来自 side_data 的数据
 * @param size 数据大小
 * @param dict 元数据存储字典
 * @return 成功返回 0，失败返回 < 0
 */
int av_packet_unpack_dictionary(const uint8_t *data, size_t size,
                                AVDictionary **dict);

/**
 * 释放所有已存储侧数据的便捷函数。其他字段保持不变。
 *
 * @param pkt 数据包
 */
void av_packet_free_side_data(AVPacket *pkt);

/**
 * 为给定数据包描述的数据建立新引用。
 *
 * 如果 src 使用引用计数，则将 dst 建立为 src 中缓冲区的新引用；
 * 否则在 dst 中分配新缓冲区，并将 src 数据复制进去。
 *
 * 所有其他字段均从 src 复制。
 *
 * @see av_packet_unref
 *
 * @param dst 目标数据包，将被完全覆盖
 * @param src 源数据包
 *
 * @return 成功返回 0，出错返回负的 AVERROR。出错时 dst 为空，
 *         如同由 av_packet_alloc() 返回。
 */
int av_packet_ref(AVPacket *dst, const AVPacket *src);

/**
 * 清空数据包。
 *
 * 解除数据包所引用缓冲区的引用，并将其余字段重置为默认值。
 *
 * @param pkt 要解引用的数据包
 */
void av_packet_unref(AVPacket *pkt);

/**
 * 将 src 的所有字段移动到 dst，并重置 src。
 *
 * @see av_packet_unref
 *
 * @param src 源数据包，将被重置
 * @param dst 目标数据包
 */
void av_packet_move_ref(AVPacket *dst, AVPacket *src);

/**
 * 仅将“属性”字段从 src 复制到 dst。
 *
 * 对此函数而言，属性指除数据包数据相关字段（buf、data、size）外的所有字段。
 *
 * @param dst 目标数据包
 * @param src 源数据包
 *
 * @return 成功返回 0，失败返回 AVERROR。
 */
int av_packet_copy_props(AVPacket *dst, const AVPacket *src);

/**
 * 确保给定数据包描述的数据使用引用计数。
 *
 * @note 此函数不保证引用可写。如需可写引用，请改用 av_packet_make_writable。
 *
 * @see av_packet_ref
 * @see av_packet_make_writable
 *
 * @param pkt 需要使其数据使用引用计数的数据包
 *
 * @return 成功返回 0，出错返回负的 AVERROR。失败时数据包不变。
 */
int av_packet_make_refcounted(AVPacket *pkt);

/**
 * 为给定数据包描述的数据创建可写引用，并尽可能避免复制数据。
 *
 * @param pkt 需要使其数据可写的数据包
 *
 * @return 成功返回 0，失败返回负的 AVERROR。失败时数据包不变。
 */
int av_packet_make_writable(AVPacket *pkt);

/**
 * 将数据包中的有效时间字段（时间戳/时长）从一个时间基转换到另一个时间基。
 * 未知时间戳（AV_NOPTS_VALUE）会被忽略。
 *
 * @param pkt 要执行转换的数据包
 * @param tb_src pkt 中时间字段使用的源时间基
 * @param tb_dst 时间字段要转换到的目标时间基
 */
void av_packet_rescale_ts(AVPacket *pkt, AVRational tb_src, AVRational tb_dst);

/**
 * 为 AVPacket 分配 AVContainerFifo 实例。
 *
 * @param flags 当前未使用
 */
struct AVContainerFifo *av_container_fifo_alloc_avpacket(unsigned flags);

/**
 * @}
 */

#endif // AVCODEC_PACKET_H
