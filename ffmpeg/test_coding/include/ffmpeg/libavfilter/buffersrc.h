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

#ifndef AVFILTER_BUFFERSRC_H
#define AVFILTER_BUFFERSRC_H

/**
 * @file
 * @ingroup lavfi_buffersrc 内存缓冲区源 API。
 */

#include "avfilter.h"

/**
 * @defgroup lavfi_buffersrc 缓冲区源 API
 * @ingroup lavfi
 * @{
 */

enum {

    /**
 * 不检查格式更改。
 */
    AV_BUFFERSRC_FLAG_NO_CHECK_FORMAT = 1 << 0,

    /**
 * 立即将框架推至输出。
 */
    AV_BUFFERSRC_FLAG_PUSH = 1 << 2,

    /**
 * 保留对框架的引用。如果该框架已引用计数，则创建一个新引用；否则复制帧数据。
 */
    AV_BUFFERSRC_FLAG_KEEP_REF = 1 << 3,

};

/**
 * 获取失败请求数。
 *
 * 请求失败是指在缓冲区中不存在帧的情况下调用 request_frame 方法。添加帧时，该数字会重置。
 */
unsigned av_buffersrc_get_nb_failed_requests(AVFilterContext *buffer_src);

/**
 * 该结构包含描述将传递给该过滤器的帧的参数。
 *
 * 它应该用 av_buffersrc_parameters_alloc() 分配并用 av_free() 释放。其中所有分配的字段仍归调用者所有。
 */
typedef struct AVBufferSrcParameters {
    /**
 * 视频：像素格式，值对应于枚举 AVPixelFormat 音频：样本格式，值对应于枚举 AVSampleFormat
 */
    int format;
    /**
 * 用于输入帧上的时间戳的时基。
 */
    AVRational time_base;

    /**
 * 仅视频，输入帧的显示尺寸。
 */
    int width, height;

    /**
 * 仅视频，样本（像素）宽高比。
 */
    AVRational sample_aspect_ratio;

    /**
 * 仅视频，输入视频的帧速率。如果输入流具有已知的恒定帧速率，则该字段只能设置为非零值；如果帧速率可变或未知，则应保留其初始值。
 */
    AVRational frame_rate;

    /**
 * 仅具有 hwaccel 像素格式的视频。这应该是对描述输入帧的 AVHWFramesContext 实例的引用。
 */
    AVBufferRef *hw_frames_ctx;

    /**
 * 仅音频，音频采样率（以每秒样本数为单位）。
 */
    int sample_rate;

    /**
 * 仅音频，音频通道布局
 */
    AVChannelLayout ch_layout;

    /**
 * 仅视频，YUV 色彩空间和范围。
 */
    enum AVColorSpace color_space;
    enum AVColorRange color_range;

    AVFrameSideData **side_data;
    int nb_side_data;

    /**
 * 仅视频，alpha 模式。
 */
    enum AVAlphaMode alpha_mode;
} AVBufferSrcParameters;

/**
 * 分配一个新的 AVBufferSrcParameters 实例。它应该由调用者使用 av_free() 释放。
 */
AVBufferSrcParameters *av_buffersrc_parameters_alloc(void);

/**
 * 使用提供的参数初始化 buffersrc 或 abuffersrc 过滤器。该函数可以被多次调用，后面的调用会覆盖前面的调用。有些参数也可以通过AVOptions设置，那么最后使用的方法优先。
 *
 * @param ctx buffersrc 或 abuffersrc 过滤器的实例
 * @param param 流参数。随后传递给该过滤器的帧必须符合这些参数。 param 中所有分配的字段仍归调用者所有，libavfilter 将在必要时进行内部复制或引用。
 * @return 成功时为 0，失败时为负 AVERROR 代码。
 */
int av_buffersrc_parameters_set(AVFilterContext *ctx, AVBufferSrcParameters *param);

/**
 * 将帧添加到缓冲区源。
 *
 * @param ctx 要添加的 buffersrc 过滤器
 * @param frame 帧的实例。如果该帧是引用计数的，则该函数将对其进行新的引用。否则帧数据将被复制。
 *
 * @return 成功时为 0，错误时为负 AVERROR
 *
 * 此函数相当于带有 AV_BUFFERSRC_FLAG_KEEP_REF 标志的 av_buffersrc_add_frame_flags()。
 */
av_warn_unused_result
int av_buffersrc_write_frame(AVFilterContext *ctx, const AVFrame *frame);

/**
 * 将帧添加到缓冲区源。
 *
 * @param ctx 要添加的 buffersrc 过滤器
 * @param frame 帧的实例。如果帧是引用计数的，则该函数将取得引用的所有权并重置帧。否则帧数据将被复制。如果此函数返回错误，则不会触摸输入框。
 *
 * @return 成功时为 0，错误时为负 AVERROR。
 *
 * @note 此函数和 av_buffersrc_write_frame() 之间的区别在于 av_buffersrc_write_frame() 创建对输入帧的新引用，而此函数获取传递给它的引用的所有权。
 *
 * 该函数相当于没有 AV_BUFFERSRC_FLAG_KEEP_REF 标志的 av_buffersrc_add_frame_flags()。
 */
av_warn_unused_result
int av_buffersrc_add_frame(AVFilterContext *ctx, AVFrame *frame);

/**
 * 将帧添加到缓冲区源。
 *
 * 默认情况下，如果帧是引用计数的，则此函数将取得引用的所有权并重置帧。这可以使用标志来控制。
 *
 * 如果此函数返回错误，则不会触摸输入框。
 *
 * @param buffer_src 指向缓冲区源上下文的指针
 * @param frame 一个帧，或 NULL 来标记 EOF
 * @param flags AV_BUFFERSRC_FLAG_* 的组合
 * @return >= 0 如果成功，则为负数失败时的 AVERROR 代码
 */
av_warn_unused_result
int av_buffersrc_add_frame_flags(AVFilterContext *buffer_src,
                                 AVFrame *frame, int flags);

/**
 * EOF 后关闭缓冲源。
 *
 * 这与将 NULL 传递给 av_buffersrc_add_frame_flags() 类似，只不过它采用 EOF 的时间戳，即最后一帧结束的时间戳。
 */
int av_buffersrc_close(AVFilterContext *ctx, int64_t pts, unsigned flags);

/**
 * 返回 0 或负 AVERROR 代码。目前，这只会返回 AVERROR(EOF)，以指示缓冲区源已关闭，无论是由于 av_buffersrc_close() 的结果，还是因为下游过滤器不再接受新数据。
 */
int av_buffersrc_get_status(AVFilterContext *ctx);

/**
 * @}
 */

#endif /* AVFILTER_BUFFERSRC_H */
