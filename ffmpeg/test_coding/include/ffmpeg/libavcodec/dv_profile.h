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

#ifndef AVCODEC_DV_PROFILE_H
#define AVCODEC_DV_PROFILE_H

#include <stdint.h>

#include "libavutil/pixfmt.h"
#include "libavutil/rational.h"

/* 为确定配置文件，需要从 DV 流中读取的最小字节数 */
#define DV_PROFILE_BYTES (6 * 80) /* 6 个 DIF 块 */


/*
 * AVDVProfile 用于表示各种 DV 类型之间的差异。目前主要用于区分
 * 525/60 与 625/50，未来也计划用于区分各种 DV 规范
 * （例如 SMPTE314M 与 IEC 61834）。
 */
typedef struct AVDVProfile {
    int              dsf;                   /* DV 头中的 dsf 值 */
    int              video_stype;           /* VAUX 源包的 stype */
    int              frame_size;            /* 一帧的总字节数 */
    int              difseg_size;           /* 每个 DIF 通道的 DIF 段数 */
    int              n_difchan;             /* 每帧的 DIF 通道数 */
    AVRational       time_base;             /* 1/帧率 */
    int              ltc_divisor;           /* 从 LTS 角度计算的 FPS */
    int              height;                /* 图像高度，单位为像素 */
    int              width;                 /* 图像宽度，单位为像素 */
    AVRational       sar[2];                /* 4:3 和 16:9 的采样宽高比 */
    enum AVPixelFormat pix_fmt;             /* 图像像素格式 */
    int              bpm;                   /* 每个宏块的块数 */
    const uint8_t   *block_sizes;           /* AC 块大小，单位为比特 */
    int              audio_stride;          /* audio_shuffle 表的大小 */
    int              audio_min_samples[3];  /* 最小音频采样数 */
                                            /* 对应 48kHz、44.1kHz 和 32kHz */
    int              audio_samples_dist[5]; /* 5 帧窗口内应包含的采样数 */
                                            /* 每帧的采样分布 */
    const uint8_t  (*audio_shuffle)[9];     /* PCM 重排表 */
} AVDVProfile;

/**
 * 获取所提供压缩帧的 DV 配置文件。
 *
 * @param sys 上一帧使用的配置文件，可以为 NULL
 * @param frame 压缩数据缓冲区
 * @param buf_size 缓冲区大小，单位为字节
 * @return 所提供数据对应的 DV 配置文件，失败时返回 NULL
 */
const AVDVProfile *av_dv_frame_profile(const AVDVProfile *sys,
                                       const uint8_t *frame, unsigned buf_size);

/**
 * 获取所提供流参数对应的 DV 配置文件。
 */
const AVDVProfile *av_dv_codec_profile(int width, int height, enum AVPixelFormat pix_fmt);

/**
 * 获取所提供流参数对应的 DV 配置文件。
 * 帧率作为尽力匹配参数使用。
 */
const AVDVProfile *av_dv_codec_profile2(int width, int height, enum AVPixelFormat pix_fmt, AVRational frame_rate);

#endif /* AVCODEC_DV_PROFILE_H */
