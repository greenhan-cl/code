/*
 * MXF SMPTE-436M VBI/ANC parsing functions
 * Copyright (c) 2025 Jacob Lifshay
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

#ifndef AVCODEC_SMPTE_436M_H
#define AVCODEC_SMPTE_436M_H

#include <stdint.h>

/**
 * 用于遍历单个 AV_CODEC_ID_SMPTE_436M_ANC AVPacket 数据中 ANC 数据包的迭代器
 */
typedef struct AVSmpte436mAncIterator {
    uint16_t       anc_packets_left;
    int            size_left;
    const uint8_t *data_left;
} AVSmpte436mAncIterator;

/**
 * 封装类型，取自下列文档的表 7（第 13 页）：
 * https://pub.smpte.org/latest/st436/s436m-2006.pdf
 */
typedef enum AVSmpte436mWrappingType
{
    AV_SMPTE_436M_WRAPPING_TYPE_VANC_FRAME             = 1,
    AV_SMPTE_436M_WRAPPING_TYPE_VANC_FIELD_1           = 2,
    AV_SMPTE_436M_WRAPPING_TYPE_VANC_FIELD_2           = 3,
    AV_SMPTE_436M_WRAPPING_TYPE_VANC_PROGRESSIVE_FRAME = 4,
    AV_SMPTE_436M_WRAPPING_TYPE_HANC_FRAME             = 0x11,
    AV_SMPTE_436M_WRAPPING_TYPE_HANC_FIELD_1           = 0x12,
    AV_SMPTE_436M_WRAPPING_TYPE_HANC_FIELD_2           = 0x13,
    AV_SMPTE_436M_WRAPPING_TYPE_HANC_PROGRESSIVE_FRAME = 0x14,
    /** 不是真实的封装类型，仅用于确保枚举具有足够大小 */
    AV_SMPTE_436M_WRAPPING_TYPE_MAX = 0xFF,
} AVSmpte436mWrappingType;

/**
 * 有效载荷采样编码，取自下列文档的表 4（第 10 页）和表 7（第 13 页）：
 * https://pub.smpte.org/latest/st436/s436m-2006.pdf
 */
typedef enum AVSmpte436mPayloadSampleCoding
{
    /** 仅用于 VBI */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_1BIT_LUMA = 1,
    /** 仅用于 VBI */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_1BIT_COLOR_DIFF = 2,
    /** 仅用于 VBI */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_1BIT_LUMA_AND_COLOR_DIFF = 3,
    /** 用于 VBI 和 ANC */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_8BIT_LUMA = 4,
    /** 用于 VBI 和 ANC */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_8BIT_COLOR_DIFF = 5,
    /** 用于 VBI 和 ANC */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_8BIT_LUMA_AND_COLOR_DIFF = 6,
    /** 用于 VBI 和 ANC */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_10BIT_LUMA = 7,
    /** 用于 VBI 和 ANC */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_10BIT_COLOR_DIFF = 8,
    /** 用于 VBI 和 ANC */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_10BIT_LUMA_AND_COLOR_DIFF = 9,
    /** 仅用于 ANC */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_8BIT_LUMA_WITH_PARITY_ERROR = 10,
    /** 仅用于 ANC */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_8BIT_COLOR_DIFF_WITH_PARITY_ERROR = 11,
    /** 仅用于 ANC */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_8BIT_LUMA_AND_COLOR_DIFF_WITH_PARITY_ERROR = 12,
    /** 不是真实的采样编码，仅用于确保枚举具有足够大小 */
    AV_SMPTE_436M_PAYLOAD_SAMPLE_CODING_MAX = 0xFF,
} AVSmpte436mPayloadSampleCoding;

/** AVSmpte291mAnc8bit 的有效载荷容量（将来加入 AVSmpte291mAnc10bit 时也适用） */
#define AV_SMPTE_291M_ANC_PAYLOAD_CAPACITY 0xFF

/**
 * 具有 8 位有效载荷的 ANC 数据包。
 * 可从 AVSmpte436mCodedAnc::payload 解码得到。
 *
 * 注意：某些 ANC 数据包需要 10 位有效载荷；如果存储在此结构体中，
 * 每个采样的最高 2 位会被丢弃。
 */
typedef struct AVSmpte291mAnc8bit {
    uint8_t did;
    uint8_t sdid_or_dbn;
    uint8_t data_count;
    uint8_t payload[AV_SMPTE_291M_ANC_PAYLOAD_CAPACITY];
    uint8_t checksum;
} AVSmpte291mAnc8bit;

/** AVSmpte436mCodedAnc 有效载荷中可存储的最大采样数 */
#define AV_SMPTE_436M_CODED_ANC_SAMPLE_CAPACITY                                                     \
    (AV_SMPTE_291M_ANC_PAYLOAD_CAPACITY + 4) /* 4 个字段：did、sdid_or_dbn、data_count 和 checksum */
/** AVSmpte436mCodedAnc 有效载荷中可存储的最大字节数 */
#define AV_SMPTE_436M_CODED_ANC_PAYLOAD_CAPACITY (((AV_SMPTE_436M_CODED_ANC_SAMPLE_CAPACITY + 2) / 3) * 4)

/**
 * 单个 AV_CODEC_ID_SMPTE_436M_ANC AVPacket 数据中的编码 ANC 数据包。
 * 对应下列文档表 7（第 13 页）的重复部分：
 * https://pub.smpte.org/latest/st436/s436m-2006.pdf
 */
typedef struct AVSmpte436mCodedAnc {
    uint16_t                       line_number;
    AVSmpte436mWrappingType        wrapping_type;
    AVSmpte436mPayloadSampleCoding payload_sample_coding;
    uint16_t                       payload_sample_count;
    uint32_t                       payload_array_length;
    /** 有效载荷，大小为 payload_array_length。
     * 可解码为 AVSmpte291mAnc8bit
     */
    uint8_t payload[AV_SMPTE_436M_CODED_ANC_PAYLOAD_CAPACITY];
} AVSmpte436mCodedAnc;

/**
 * 验证 AVSmpte436mCodedAnc 结构体。不检查有效载荷是否有效。
 * @param[in]  anc 要验证的 ANC 数据包
 * @return 成功返回 0，否则返回 AVERROR 错误码。
 */
int av_smpte_436m_coded_anc_validate(const AVSmpte436mCodedAnc *anc);

/**
 * 将 ANC 数据包编码到单个 AV_CODEC_ID_SMPTE_436M_ANC AVPacket 的数据中。
 * @param[in]  anc_packet_count 要编码的 ANC 数据包数量
 * @param[in]  anc_packets      要编码的 ANC 数据包
 * @param[in]  size             out 的大小；out 为 NULL 时忽略
 * @param[out] out              输出字节；out 为 NULL 时不写入任何内容
 * @return 成功时返回写入的字节数，否则返回 AVERROR 错误码。
 *         out 为 NULL 时，返回原本会写入的字节数。
 */
int av_smpte_436m_anc_encode(uint8_t *out, int size, int anc_packet_count, const AVSmpte436mCodedAnc *anc_packets);

struct AVPacket;

/**
 * 向单个 AV_CODEC_ID_SMPTE_436M_ANC AVPacket 的数据追加更多 ANC 数据包。
 * @param[in]  anc_packet_count 要编码的 ANC 数据包数量
 * @param[in]  anc_packets      要编码的 ANC 数据包
 * @param      pkt              要追加到的 AVPacket；其大小必须为 0，
 *                              或者必须包含有效的 SMPTE_436M_ANC 数据
 * @return 成功返回 0，否则返回 AVERROR 错误码。
 */
int av_smpte_436m_anc_append(struct AVPacket *pkt, int anc_packet_count, const AVSmpte436mCodedAnc *anc_packets);

/**
 * 设置对单个 AV_CODEC_ID_SMPTE_436M_ANC AVPacket 数据中 ANC 数据包的遍历。
 * @param[in]  buf      指向 AV_CODEC_ID_SMPTE_436M_ANC AVPacket 数据的指针
 * @param[in]  buf_size AV_CODEC_ID_SMPTE_436M_ANC AVPacket 数据的大小
 * @param[out] iter     指向迭代器的指针
 * @return 成功返回 0，否则返回 AVERROR 错误码。
 */
int av_smpte_436m_anc_iter_init(AVSmpte436mAncIterator *iter, const uint8_t *buf, int buf_size);

/**
 * 从迭代器获取下一个 ANC 数据包，并推进迭代器。
 * @param[in,out] iter 指向迭代器的指针
 * @param[out]    anc  返回的 ANC 数据包
 * @return 成功返回 0；迭代器到达末尾时返回 AVERROR_EOF；否则返回 AVERROR 错误码。
 */
int av_smpte_436m_anc_iter_next(AVSmpte436mAncIterator *iter, AVSmpte436mCodedAnc *anc);

/**
 * 获取存储 AVSmpte436mCodedAnc 有效载荷所需的最小字节数。
 * @param sample_coding 有效载荷采样编码
 * @param sample_count  有效载荷中存储的采样数
 * @return 返回所需的最小字节数，出错时返回 < 0；
 *         始终 <= SMPTE_436M_CODED_ANC_PAYLOAD_CAPACITY
 */
int av_smpte_436m_coded_anc_payload_size(AVSmpte436mPayloadSampleCoding sample_coding, uint16_t sample_count);

/**
 * 将 AVSmpte436mCodedAnc 有效载荷解码为 AVSmpte291mAnc8bit。
 * @param[in]  sample_coding 有效载荷采样编码
 * @param[in]  sample_count  有效载荷中存储的采样数
 * @param[in]  payload       存储有效载荷的字节；所需大小可通过
                             avpriv_smpte_436m_coded_anc_payload_size
 *                           获取
 * @param[in]  log_ctx       av_log 使用的上下文指针
 * @param[out] out           解码后的 ANC 数据包
 * @return 成功返回 0，否则返回 < 0。
 */
int av_smpte_291m_anc_8bit_decode(AVSmpte291mAnc8bit            *out,
                                  AVSmpte436mPayloadSampleCoding sample_coding,
                                  uint16_t                       sample_count,
                                  const uint8_t                 *payload,
                                  void                          *log_ctx);

/**
 * 为 AVSmpte291mAnc8bit 填入正确的校验和。
 * @param[in,out] anc ANC 数据包
 */
void av_smpte_291m_anc_8bit_fill_checksum(AVSmpte291mAnc8bit *anc);

/**
 * 计算将 AVSmpte291mAnc8bit 编码到 AVSmpte436mCodedAnc 有效载荷所需的采样数。
 * @param[in] anc           ANC 数据包
 * @param[in] sample_coding 采样编码
 * @param[in] log_ctx       av_log 使用的上下文指针
 * @return 成功时返回采样数，否则返回 < 0。
 */
int av_smpte_291m_anc_8bit_get_sample_count(const AVSmpte291mAnc8bit      *anc,
                                            AVSmpte436mPayloadSampleCoding sample_coding,
                                            void                          *log_ctx);

/**
 * 将 AVSmpte291mAnc8bit 编码为 AVSmpte436mCodedAnc。
 * @param[in]  line_number   ANC 数据包所在的行号
 * @param[in]  wrapping_type 封装类型
 * @param[in]  sample_coding 有效载荷采样编码
 * @param[in]  payload       要编码的 ANC 数据包
 * @param[in]  log_ctx       av_log 使用的上下文指针
 * @param[out] out           编码后的 ANC 数据包
 * @return 成功返回 0，否则返回 < 0。
 */
int av_smpte_291m_anc_8bit_encode(AVSmpte436mCodedAnc           *out,
                                  uint16_t                       line_number,
                                  AVSmpte436mWrappingType        wrapping_type,
                                  AVSmpte436mPayloadSampleCoding sample_coding,
                                  const AVSmpte291mAnc8bit      *payload,
                                  void                          *log_ctx);

/** 携带 CTA-708 数据（用于 AV_CODEC_ID_EIA_608）时的 AVSmpte291mAnc8bit::did */
#define AV_SMPTE_291M_ANC_DID_CTA_708 0x61

/** 携带 CTA-708 数据（用于 AV_CODEC_ID_EIA_608）时的 AVSmpte291mAnc8bit::sdid_or_dbn */
#define AV_SMPTE_291M_ANC_SDID_CTA_708 0x1

/**
 * 尝试将 ANC 数据包解码为 EIA-608/CTA-708 数据（AV_CODEC_ID_EIA_608）。
 * @param[in]  anc     ANC 数据包
 * @param[in]  log_ctx av_log 使用的上下文指针
 * @param[out] cc_data 用于存储提取出的 EIA-608/CTA-708 数据的缓冲区；
 *                     可传入 NULL 以不存储数据。所需大小为 3 * cc_count 字节，
 *                     SMPTE_291M_ANC_PAYLOAD_CAPACITY 始终足够。
 * @return 成功返回 cc_count（>= 0）；不是 CTA-708 ANC 数据包时返回 AVERROR(EAGAIN)；
 *         出错时返回 < 0。
 */
int av_smpte_291m_anc_8bit_extract_cta_708(const AVSmpte291mAnc8bit *anc, uint8_t *cc_data, void *log_ctx);

#endif /* AVCODEC_SMPTE_436M_H */
