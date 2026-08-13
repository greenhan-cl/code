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

#ifndef AVFILTER_BUFFERSINK_H
#define AVFILTER_BUFFERSINK_H

/**
 * @file
 * @ingroup lavfi_buffersink 用于音频和视频的内存缓冲区接收器 API
 */

#include "avfilter.h"

/**
 * @defgroup lavfi_buffersink 缓冲区接收器 API
 * @ingroup lavfi
 * @{
 *
 * buffersink 和 abuffersink 过滤器用于将过滤器图连接到应用程序。它们只有一个输入，连接到图表，没有输出。必须使用 av_buffersink_get_frame() 或 av_buffersink_get_samples() 提取帧。
 *
 * 配置期间图形协商的格式可以使用访问器函数获取： - av_buffersink_get_time_base()、 - av_buffersink_get_format()、 - av_buffersink_get_frame_rate()、 - av_buffersink_get_w()、 - av_buffersink_get_h()、 - av_buffersink_get_sample_aspect_ratio()、- av_buffersink_get_channels()、- av_buffersink_get_ch_layout()、- av_buffersink_get_sample_rate()。 - av_buffersink_get_side_data()。
 *
 * av_buffersink_get_ch_layout() 返回的布局必须由调用者取消初始化。
 *
 * 可以通过设置选项、使用 av_opt_set() 和带有 AV_OPT_SEARCH_CHILDREN 标志的相关函数来限制格式。 - Pixel_formats（像素格式数组）， - colorspaces（整数数组）， - colorranges（整数数组）， - alphamodes（整数数组）， - Sample_formats（样本格式数组）， - Samplerates（整数数组）， - Channel_layouts（通道布局数组） 如果未设置选项，则接受所有相应的格式。
 */

/**
 * 从接收器中获取包含过滤数据的框架并将其放入框架中。
 *
 * @param ctx 指向 buffersink 或 buffersink 过滤器上下文的指针。
 * @param frame 指向将填充数据的已分配帧的指针。必须使用 av_frame_unref() / av_frame_free()
 * @param flags 释放数据，AV_BUFFERSINK_FLAG_* 标志的组合
 *
 * @return >= 0 表示成功，负 AVERROR 代码表示失败。
 */
int av_buffersink_get_frame_flags(AVFilterContext *ctx, AVFrame *frame, int flags);

/**
 * 告诉 av_buffersink_get_buffer_ref() 读取视频/样本缓冲区引用，但不将其从缓冲区中删除。如果您只需要读取视频/样本缓冲区而不需要获取它，这非常有用。
 */
#define AV_BUFFERSINK_FLAG_PEEK 1

/**
 * 告诉 av_buffersink_get_buffer_ref() 不要从其输入请求帧。如果帧已被缓冲，则读取该帧（并从缓冲区中删除），但如果不存在帧，则返回 AVERROR(EAGAIN)。
 */
#define AV_BUFFERSINK_FLAG_NO_REQUEST 2

/**
 * 设置音频缓冲区接收器的帧大小。
 *
 * 对 av_buffersink_get_buffer_ref 的所有调用都将返回一个具有指定数量样本的缓冲区，如果样本数量不够，则返回 AVERROR(EAGAIN)。 EOF 处的最后一个缓冲区将用 0 填充。
 */
void av_buffersink_set_frame_size(AVFilterContext *ctx, unsigned frame_size);

/**
 * @defgroup lavfi_buffersink_accessors 缓冲区接收器访问器 获取流的属性
 * @{
 */

enum AVMediaType av_buffersink_get_type                (const AVFilterContext *ctx);
AVRational       av_buffersink_get_time_base           (const AVFilterContext *ctx);
int              av_buffersink_get_format              (const AVFilterContext *ctx);

AVRational       av_buffersink_get_frame_rate          (const AVFilterContext *ctx);
int              av_buffersink_get_w                   (const AVFilterContext *ctx);
int              av_buffersink_get_h                   (const AVFilterContext *ctx);
AVRational       av_buffersink_get_sample_aspect_ratio (const AVFilterContext *ctx);
enum AVColorSpace av_buffersink_get_colorspace         (const AVFilterContext *ctx);
enum AVColorRange av_buffersink_get_color_range        (const AVFilterContext *ctx);
enum AVAlphaMode  av_buffersink_get_alpha_mode         (const AVFilterContext *ctx);

int              av_buffersink_get_channels            (const AVFilterContext *ctx);
int              av_buffersink_get_ch_layout           (const AVFilterContext *ctx,
                                                        AVChannelLayout *ch_layout);
int              av_buffersink_get_sample_rate         (const AVFilterContext *ctx);

AVBufferRef *    av_buffersink_get_hw_frames_ctx       (const AVFilterContext *ctx);

const AVFrameSideData *const *av_buffersink_get_side_data(const AVFilterContext *ctx,
                                                          int *nb_side_data);

/** @} */

/**
 * 获取包含来自接收器的已过滤数据的帧并将其放入帧中。
 *
 * @param ctx 指向 buffersink 或 buffersink AVFilter 上下文的指针。
 * @param frame 指向将填充数据的已分配帧的指针。如果成功返回帧，则必须使用 av_frame_unref() / av_frame_free() 释放数据
 *
 * @return - >= 0。 - AVERROR(EAGAIN) 如果此时没有可用的帧；必须将更多输入帧添加到过滤器图中才能获得更多输出。 - AVERROR_EOF 如果此接收器上不再有输出帧。 - 其他失败情况下的不同负 AVERROR 代码。
 */
int av_buffersink_get_frame(AVFilterContext *ctx, AVFrame *frame);

/**
 * 与 av_buffersink_get_frame() 相同，但能够指定读取的样本数。该函数的效率低于 av_buffersink_get_frame()，因为它会复制数据。
 *
 * @param ctx 指向缓冲区接收器 AVFilter 上下文的指针。
 * @param frame 指向将填充数据的已分配帧的指针。必须使用 av_frame_unref() 释放数据 / av_frame_free() 帧将恰好包含 nb_samples 个音频样本，但在流末尾除外，此时它可以包含少于 nb_samples 的音频样本。
 *
 * @return 返回码的含义与 av_buffersink_get_frame() 相同。
 *
 * @warning 请勿将此函数与 av_buffersink_get_frame() 混合使用。单个水槽只能使用其中之一，而不是同时使用。
 */
int av_buffersink_get_samples(AVFilterContext *ctx, AVFrame *frame, int nb_samples);

/**
 * @}
 */

#endif /* AVFILTER_BUFFERSINK_H */
