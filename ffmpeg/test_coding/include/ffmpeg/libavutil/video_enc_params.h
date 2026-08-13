/*
 * This file is part 的 FFmpeg.
 *
 * FFmpeg is 释放 software; you can redistribute it and/or
 * mod如果y it under the terms 的 the GNU Lesser General 公共
 * License as published by the 释放 Software Foundation; either
 * version 2.1 的 the License, 或 (at your 选项) any later version.
 *
 * FFmpeg is distributed 中 the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY 或 FITNESS FOR PARTICULAR PURPOSE.  参见 the GNU
 * Lesser General 公共 License 用于 more details.
 *
 * You should have received a 复制 的 the GNU Lesser General 公共
 * License along，使用 FFmpeg; 如果 not, write 到 the 释放 Software
 * Foundation, Inc., 51 Franklin Street, F如果th Floor, Boston, M02110-1301 USA
 */

#ifndef AVUTIL_VIDEO_ENC_PARAMS_H
#define AVUTIL_VIDEO_ENC_PARAMS_H

#include <stddef.h>
#include <stdint.h>

#include "libavutil/avassert.h"
#include "libavutil/frame.h"

enum AVVideoEncParamsType {
    AV_VIDEO_ENC_PARAMS_NONE = -1,
    /**
     * VP9 stores:
     * - per-帧 base (luma AC) quantizer index, exported as AV视频EncParams.qp
     * - deltas 用于 luma DC, chroma AC 和 chroma DC, exported 中 the
     *   corresponding entries 中 AV视频EncParams.delta_qp
     * - per-segment delta, exported as 用于 each block as AV视频BlockParams.delta_qp
     *
     * To compute the resulting quantizer index 用于 a block:
     * - 用于 luma AC, add the base qp 和 the per-block delta_qp, saturating to
     *   unsigned 8-bit.
     * - 用于 luma DC 和 chroma AC/DC, add the corresponding
     *   AV视频BlockParams.delta_qp 到 the luma AC index, again saturating to
     *   unsigned 8-bit.
     */
    AV_VIDEO_ENC_PARAMS_VP9,

    /**
     * H.264 stores:
     * - 中 PPS (per-picture):
     *   * initial QP_Y (luma) 值, exported as AV视频EncParams.qp
     *   * delta(s) 用于 chroma QP 值 (same 用于 both, 或 each separately),
     *     exported as 中 the corresponding entries 中 AV视频EncParams.delta_qp
     * - per-slice QP delta, not exported directly, added 到 the per-MB 值
     * - per-MB delta; not exported directly; the final per-MB quantizer
     *   parameter - QP_Y - minus the 值 中 AV视频EncParams.qp is exported
     *   as AV视频BlockParams.qp_delta.
     */
    AV_VIDEO_ENC_PARAMS_H264,

    /*
     * MPEG-2-compatible quantizer.
     *
     * Summing the 帧-level qp，使用 the per-block delta_qp gives the
     * resulting quantizer 用于 the block.
     */
    AV_VIDEO_ENC_PARAMS_MPEG2,
};

/**
 * 视频 编码 parameters 用于 a given 帧. This struct is 分配d along
 *，使用 an 可选 数组 的 per-block AV视频BlockParams 描述符s.
 * Must be 分配d，使用 av_视频_enc_params_alloc().
 */
typedef struct AVVideoEncParams {
    /**
     * 数量 的 blocks 中 the 数组.
     *
     * May be 0, 中 which case no per-block in格式ion is present. In this case
     * the 值 的 blocks_off设置 / block_大小 are unspec如果ied 和 should not
     * be accessed.
     */
    unsigned int nb_blocks;
    /**
     * Off设置 中 bytes，来自 the beginning 的 this 结构体 at which the 数组
     * 的 blocks starts.
     */
    size_t blocks_offset;
    /*
     * 大小 的 each block 中 bytes. May not match 大小of(AV视频BlockParams).
     */
    size_t block_size;

    /**
     * Type 的 the parameters (the codec they are used with).
     */
    enum AVVideoEncParamsType type;

    /**
     * Base quantisation parameter 用于 the 帧. final quantiser 用于 a
     * given block 中 a given plane is obtained，来自 this 值, possibly
     * combined，使用 {@code delta_qp} 和 the per-block delta 中 a manner
     * documented 用于 each type.
     */
    int32_t qp;

    /**
     * Quantisation parameter off设置，来自 the base (per-帧) qp 用于 a given
     * plane (first index) 和 AC/DC coefficients (second index).
     */
    int32_t delta_qp[4][2];
} AVVideoEncParams;

/**
 * Data 结构体 用于 storing block-level 编码 in格式ion.
 * It is 分配d as a part 的 AV视频EncParams 和 should be retrieved with
 * av_视频_enc_params_block().
 *
 * 大小of(AV视频BlockParams) is not a part 的 the ABI 和 new fields may be
 * added 到 it.
 */
typedef struct AVVideoBlockParams {
    /**
     * Distance 中 luma 像素s，来自 the top-left corner 的 the visible 帧
     * 到 the top-left corner 的 the block.
     * Can be negative 如果 top/right padding is present 上 the coded 帧.
     */
    int src_x, src_y;
    /**
     * 宽度 和 高度 的 the block 中 luma 像素s.
     */
    int w, h;

    /**
     * D如果ference between this block's final quantization parameter 和 the
     * corresponding per-帧 值.
     */
    int32_t delta_qp;
} AVVideoBlockParams;

/**
 * 获取 the block at the spec如果ied {@code idx}. Must be between 0 和 nb_blocks - 1.
 */
static av_always_inline AVVideoBlockParams*
av_video_enc_params_block(AVVideoEncParams *par, unsigned int idx)
{
    av_assert0(idx < par->nb_blocks);
    return (AVVideoBlockParams *)((uint8_t *)par + par->blocks_offset +
                                  idx * par->block_size);
}

/**
 * 分配s 内存 用于 AV视频EncParams 的 the given type, plus an 数组 of
 * {@code nb_blocks} AV视频BlockParams 和 初始化s the variables. Can be
 * 释放d，使用 a normal av_释放() call.
 *
 * @param out_大小 如果 non-NULL, the 大小 中 bytes 的 the resulting data 数组 is
 * written here.
 */
AVVideoEncParams *av_video_enc_params_alloc(enum AVVideoEncParamsType type,
                                            unsigned int nb_blocks, size_t *out_size);

/**
 * 分配s 内存 用于 AVEncodeInfo帧 plus an 数组 of
 * {@code nb_blocks} AVEncodeInfoBlock 中 the given AV帧 {@code 帧}
 * as AV帧SideData 的 type AV_帧_DATA_视频_ENC_PARAMS
 * 和 初始化s the variables.
 */
AVVideoEncParams*
av_video_enc_params_create_side_data(AVFrame *frame, enum AVVideoEncParamsType type,
                                     unsigned int nb_blocks);

#endif /* AVUTIL_VIDEO_ENC_PARAMS_H */
