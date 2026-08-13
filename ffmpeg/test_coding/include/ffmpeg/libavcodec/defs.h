/*
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

#ifndef AVCODEC_DEFS_H
#define AVCODEC_DEFS_H

/**
 * @file
 * @ingroup libavc
 * 不适合归入其他位置的杂项类型和常量。
 */

#include <stdint.h>
#include <stdlib.h>

/**
 * @ingroup lavc_decoding
 * 解码时必须在输入比特流末尾额外分配的字节数。
 * 主要原因是某些优化的比特流读取器一次读取 32 或 64 位，可能越过末尾。<br>
 * 注意：如果额外字节的前 23 位不为 0，损坏的 MPEG 比特流可能导致越界读取和段错误。
 */
#define AV_INPUT_BUFFER_PADDING_SIZE 64

/**
 * 验证嵌入比特流的校验和（取决于格式，可能属于编码或解码数据），
 * 不匹配时打印错误消息。如果同时设置 AV_EF_EXPLODE，校验和不匹配会使
 * 解码器/解封装器返回错误。
 */
#define AV_EF_CRCCHECK       (1<<0)
#define AV_EF_BITSTREAM      (1<<1)   ///< 检测比特流规范偏差
#define AV_EF_BUFFER         (1<<2)   ///< 检测不正确的比特流长度
#define AV_EF_EXPLODE        (1<<3)   ///< 检测到轻微错误时中止解码

#define AV_EF_IGNORE_ERR     (1<<15)  ///< 忽略错误并继续
#define AV_EF_CAREFUL        (1<<16)  ///< 将违反规范、易于计算且现实中尚未出现的情况视为错误
#define AV_EF_COMPLIANT      (1<<17)  ///< 将所有不符合规范的情况视为错误
#define AV_EF_AGGRESSIVE     (1<<18)  ///< 将正常编码器/封装器不应执行的操作视为错误

#define FF_COMPLIANCE_VERY_STRICT   2 ///< 严格遵循较旧且更严格的规范版本或参考软件。
#define FF_COMPLIANCE_STRICT        1 ///< 不计后果地严格遵循规范中的所有要求。
#define FF_COMPLIANCE_NORMAL        0
#define FF_COMPLIANCE_UNOFFICIAL   -1 ///< 允许非官方扩展
#define FF_COMPLIANCE_EXPERIMENTAL -2 ///< 允许未标准化的实验性内容。


#define AV_PROFILE_UNKNOWN        -99
#define AV_PROFILE_RESERVED      -100

#define AV_PROFILE_AAC_MAIN         0
#define AV_PROFILE_AAC_LOW          1
#define AV_PROFILE_AAC_SSR          2
#define AV_PROFILE_AAC_LTP          3
#define AV_PROFILE_AAC_HE           4
#define AV_PROFILE_AAC_HE_V2       28
#define AV_PROFILE_AAC_LD          22
#define AV_PROFILE_AAC_ELD         38
#define AV_PROFILE_AAC_USAC        41
#define AV_PROFILE_MPEG2_AAC_LOW  128
#define AV_PROFILE_MPEG2_AAC_HE   131

#define AV_PROFILE_DNXHD         0
#define AV_PROFILE_DNXHR_LB      1
#define AV_PROFILE_DNXHR_SQ      2
#define AV_PROFILE_DNXHR_HQ      3
#define AV_PROFILE_DNXHR_HQX     4
#define AV_PROFILE_DNXHR_444     5

#define AV_PROFILE_DTS                20
#define AV_PROFILE_DTS_ES             30
#define AV_PROFILE_DTS_96_24          40
#define AV_PROFILE_DTS_HD_HRA         50
#define AV_PROFILE_DTS_HD_MA          60
#define AV_PROFILE_DTS_EXPRESS        70
#define AV_PROFILE_DTS_HD_MA_X        61
#define AV_PROFILE_DTS_HD_MA_X_IMAX   62

#define AV_PROFILE_EAC3_DDP_ATMOS         30

#define AV_PROFILE_TRUEHD_ATMOS           30

#define AV_PROFILE_MPEG2_422           0
#define AV_PROFILE_MPEG2_HIGH          1
#define AV_PROFILE_MPEG2_SS            2
#define AV_PROFILE_MPEG2_SNR_SCALABLE  3
#define AV_PROFILE_MPEG2_MAIN          4
#define AV_PROFILE_MPEG2_SIMPLE        5

#define AV_PROFILE_H264_CONSTRAINED  (1<<9)  // 8+1；constraint_set1_flag
#define AV_PROFILE_H264_INTRA        (1<<11) // 8+3；constraint_set3_flag

#define AV_PROFILE_H264_BASELINE             66
#define AV_PROFILE_H264_CONSTRAINED_BASELINE (66|AV_PROFILE_H264_CONSTRAINED)
#define AV_PROFILE_H264_MAIN                 77
#define AV_PROFILE_H264_EXTENDED             88
#define AV_PROFILE_H264_HIGH                 100
#define AV_PROFILE_H264_HIGH_10              110
#define AV_PROFILE_H264_HIGH_10_INTRA        (110|AV_PROFILE_H264_INTRA)
#define AV_PROFILE_H264_MULTIVIEW_HIGH       118
#define AV_PROFILE_H264_HIGH_422             122
#define AV_PROFILE_H264_HIGH_422_INTRA       (122|AV_PROFILE_H264_INTRA)
#define AV_PROFILE_H264_STEREO_HIGH          128
#define AV_PROFILE_H264_HIGH_444             144
#define AV_PROFILE_H264_HIGH_444_PREDICTIVE  244
#define AV_PROFILE_H264_HIGH_444_INTRA       (244|AV_PROFILE_H264_INTRA)
#define AV_PROFILE_H264_CAVLC_444            44

#define AV_PROFILE_VC1_SIMPLE   0
#define AV_PROFILE_VC1_MAIN     1
#define AV_PROFILE_VC1_COMPLEX  2
#define AV_PROFILE_VC1_ADVANCED 3

#define AV_PROFILE_MPEG4_SIMPLE                     0
#define AV_PROFILE_MPEG4_SIMPLE_SCALABLE            1
#define AV_PROFILE_MPEG4_CORE                       2
#define AV_PROFILE_MPEG4_MAIN                       3
#define AV_PROFILE_MPEG4_N_BIT                      4
#define AV_PROFILE_MPEG4_SCALABLE_TEXTURE           5
#define AV_PROFILE_MPEG4_SIMPLE_FACE_ANIMATION      6
#define AV_PROFILE_MPEG4_BASIC_ANIMATED_TEXTURE     7
#define AV_PROFILE_MPEG4_HYBRID                     8
#define AV_PROFILE_MPEG4_ADVANCED_REAL_TIME         9
#define AV_PROFILE_MPEG4_CORE_SCALABLE             10
#define AV_PROFILE_MPEG4_ADVANCED_CODING           11
#define AV_PROFILE_MPEG4_ADVANCED_CORE             12
#define AV_PROFILE_MPEG4_ADVANCED_SCALABLE_TEXTURE 13
#define AV_PROFILE_MPEG4_SIMPLE_STUDIO             14
#define AV_PROFILE_MPEG4_ADVANCED_SIMPLE           15

#define AV_PROFILE_JPEG2000_CSTREAM_RESTRICTION_0   1
#define AV_PROFILE_JPEG2000_CSTREAM_RESTRICTION_1   2
#define AV_PROFILE_JPEG2000_CSTREAM_NO_RESTRICTION  32768
#define AV_PROFILE_JPEG2000_DCINEMA_2K              3
#define AV_PROFILE_JPEG2000_DCINEMA_4K              4

#define AV_PROFILE_VP9_0                            0
#define AV_PROFILE_VP9_1                            1
#define AV_PROFILE_VP9_2                            2
#define AV_PROFILE_VP9_3                            3

#define AV_PROFILE_HEVC_MAIN                        1
#define AV_PROFILE_HEVC_MAIN_10                     2
#define AV_PROFILE_HEVC_MAIN_STILL_PICTURE          3
#define AV_PROFILE_HEVC_REXT                        4
#define AV_PROFILE_HEVC_MULTIVIEW_MAIN              6
#define AV_PROFILE_HEVC_SCC                         9

#define AV_PROFILE_VVC_MAIN_10                      1
#define AV_PROFILE_VVC_MAIN_10_444                 33

#define AV_PROFILE_AV1_MAIN                         0
#define AV_PROFILE_AV1_HIGH                         1
#define AV_PROFILE_AV1_PROFESSIONAL                 2

#define AV_PROFILE_MJPEG_HUFFMAN_BASELINE_DCT            0xc0
#define AV_PROFILE_MJPEG_HUFFMAN_EXTENDED_SEQUENTIAL_DCT 0xc1
#define AV_PROFILE_MJPEG_HUFFMAN_PROGRESSIVE_DCT         0xc2
#define AV_PROFILE_MJPEG_HUFFMAN_LOSSLESS                0xc3
#define AV_PROFILE_MJPEG_JPEG_LS                         0xf7

#define AV_PROFILE_SBC_MSBC                         1

#define AV_PROFILE_PRORES_PROXY     0
#define AV_PROFILE_PRORES_LT        1
#define AV_PROFILE_PRORES_STANDARD  2
#define AV_PROFILE_PRORES_HQ        3
#define AV_PROFILE_PRORES_4444      4
#define AV_PROFILE_PRORES_XQ        5

#define AV_PROFILE_PRORES_RAW       0
#define AV_PROFILE_PRORES_RAW_HQ    1

#define AV_PROFILE_ARIB_PROFILE_A 0
#define AV_PROFILE_ARIB_PROFILE_C 1

#define AV_PROFILE_KLVA_SYNC  0
#define AV_PROFILE_KLVA_ASYNC 1

#define AV_PROFILE_EVC_BASELINE             0
#define AV_PROFILE_EVC_MAIN                 1

#define AV_PROFILE_APV_422_10  33
#define AV_PROFILE_APV_422_12  44
#define AV_PROFILE_APV_444_10  55
#define AV_PROFILE_APV_444_12  66
#define AV_PROFILE_APV_4444_10 77
#define AV_PROFILE_APV_4444_12 88
#define AV_PROFILE_APV_400_10  99


#define AV_LEVEL_UNKNOWN                  -99

enum AVFieldOrder {
    AV_FIELD_UNKNOWN,
    AV_FIELD_PROGRESSIVE,
    AV_FIELD_TT,          ///< 顶场先编码，顶场先显示
    AV_FIELD_BB,          ///< 底场先编码，底场先显示
    AV_FIELD_TB,          ///< 顶场先编码，底场先显示
    AV_FIELD_BT,          ///< 底场先编码，顶场先显示
};

/**
 * @ingroup lavc_decoding
 */
enum AVDiscard{
    /* 在各值之间保留一些空间用于扩展（例如仅帧内编码时丢弃部分关键帧，
     * 或仅丢弃部分双向帧）。 */
    AVDISCARD_NONE    =-16, ///< 不丢弃任何内容
    AVDISCARD_DEFAULT =  0, ///< 丢弃无用数据包，例如 AVI 中大小为 0 的包
    AVDISCARD_NONREF  =  8, ///< 丢弃所有非参考帧
    AVDISCARD_BIDIR   = 16, ///< 丢弃所有双向帧
    AVDISCARD_NONINTRA= 24, ///< 丢弃所有非帧内帧
    AVDISCARD_NONKEY  = 32, ///< 丢弃除关键帧外的所有帧
    AVDISCARD_ALL     = 48, ///< 全部丢弃
};

enum AVAudioServiceType {
    AV_AUDIO_SERVICE_TYPE_MAIN              = 0,
    AV_AUDIO_SERVICE_TYPE_EFFECTS           = 1,
    AV_AUDIO_SERVICE_TYPE_VISUALLY_IMPAIRED = 2,
    AV_AUDIO_SERVICE_TYPE_HEARING_IMPAIRED  = 3,
    AV_AUDIO_SERVICE_TYPE_DIALOGUE          = 4,
    AV_AUDIO_SERVICE_TYPE_COMMENTARY        = 5,
    AV_AUDIO_SERVICE_TYPE_EMERGENCY         = 6,
    AV_AUDIO_SERVICE_TYPE_VOICE_OVER        = 7,
    AV_AUDIO_SERVICE_TYPE_KARAOKE           = 8,
    AV_AUDIO_SERVICE_TYPE_NB                   , ///< 不属于 ABI
};

/**
 * 平移扫描区域。
 * 指定应显示的区域。注意一帧可能包含多个此类区域。
 */
typedef struct AVPanScan {
    /**
     * id
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    int id;

    /**
     * 以 1/16 像素为单位的宽度和高度
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    int width;
    int height;

    /**
     * 最多 3 个场/帧的左上角位置，单位为 1/16 像素
     * - 编码：由用户设置。
     * - 解码：由 libavcodec 设置。
     */
    int16_t position[3][2];
} AVPanScan;

/**
 * 此结构体描述编码比特流的码率属性。它大致对应 MPEG-2 的部分 VBV 参数，
 * 或 H.264/HEVC 的部分 HRD 参数。
 */
typedef struct AVCPBProperties {
    /**
     * 流的最大码率，单位为 bit/s。未知或未指定时为 0。
     */
    int64_t max_bitrate;
    /**
     * 流的最小码率，单位为 bit/s。未知或未指定时为 0。
     */
    int64_t min_bitrate;
    /**
     * 流的平均码率，单位为 bit/s。未知或未指定时为 0。
     */
    int64_t avg_bitrate;

    /**
     * 应用码率控制的缓冲区大小，单位为 bit。未知或未指定时为 0。
     */
    int64_t buffer_size;

    /**
     * 从收到与此结构体关联的数据包，到应当解码该数据包之间的延迟，
     * 以 27MHz 时钟周期为单位。
     *
     * 未知或未指定时为 UINT64_MAX。
     */
    uint64_t vbv_delay;
} AVCPBProperties;

/**
 * 分配 CPB 属性结构体，并将字段初始化为默认值。
 *
 * @param size 非 NULL 时，将在此写入所分配结构体的大小。这对于将其嵌入侧数据很有用。
 *
 * @return 新分配的结构体，失败时返回 NULL
 */
AVCPBProperties *av_cpb_properties_alloc(size_t *size);

/**
 * 此结构体提供数据包时间戳与挂钟生成时间之间的对应关系。
 * 定义遵循 ISO/IEC 14496-12 中的生产者参考时间（'prft'）。
 */
typedef struct AVProducerReferenceTime {
    /**
     * 自 Unix 纪元以来的 UTC 时间戳，单位为微秒（例如 av_gettime()）。
     */
    int64_t wallclock;
    int flags;
} AVProducerReferenceTime;

/**
 * RTCP SR（发送方报告）信息
 *
 * 接收到的 RTSP 流发送方报告信息，以 AV_PKT_DATA_RTCP_SR 侧数据形式公开。
 */
typedef struct AVRTCPSenderReport {
    uint32_t ssrc; ///< 同步源标识符
    uint64_t ntp_timestamp; ///< 发送报告时的 NTP 时间
    uint32_t rtp_timestamp; ///< 发送报告时的 RTP 时间
    uint32_t sender_nb_packets; ///< 已发送的数据包总数
    uint32_t sender_nb_bytes; ///< 已发送的总字节数（不含头或填充）
} AVRTCPSenderReport;

/**
 * 将 extradata 长度编码到缓冲区。供 xiph 编解码器使用。
 *
 * @param s 要写入的缓冲区；长度必须至少为 (v/255+1) 字节
 * @param v extradata 大小，单位为字节
 * @return 写入缓冲区的字节数。
 */
unsigned int av_xiphlacing(unsigned char *s, unsigned int v);

#endif // AVCODEC_DEFS_H
