/*
 * Android MediaCodec public API
 *
 * Copyright (c) 2016 Matthieu Bouron <matthieu.bouron stupeflix.com>
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

#ifndef AVCODEC_MEDIACODEC_H
#define AVCODEC_MEDIACODEC_H

#include "libavcodec/avcodec.h"

/**
 * 此结构体保存对 android/view/Surface 对象的引用，解码器将其用作输出。
 *
 */
typedef struct AVMediaCodecContext {

    /**
     * android/view/Surface 对象引用。
     */
    void *surface;

} AVMediaCodecContext;

/**
 * 分配并初始化 MediaCodec 上下文。
 *
 * MediaCodec 解码结束后，调用方必须使用 av_mediacodec_default_free
 * 释放 MediaCodec 上下文。
 *
 * @return 成功时返回指向新分配 AVMediaCodecContext 的指针，否则返回 NULL
 */
AVMediaCodecContext *av_mediacodec_alloc_context(void);

/**
 * 用于设置 MediaCodec 上下文的便捷函数。
 *
 * @param avctx 编解码器上下文
 * @param ctx 要初始化的 MediaCodec 上下文
 * @param surface android/view/Surface 的引用
 * @return 成功返回 0，否则返回 < 0
 */
int av_mediacodec_default_init(AVCodecContext *avctx, AVMediaCodecContext *ctx, void *surface);

/**
 * 必须调用此函数释放由 av_mediacodec_default_init() 初始化的 MediaCodec 上下文。
 *
 * @param avctx 编解码器上下文
 */
void av_mediacodec_default_free(AVCodecContext *avctx);

/**
 * 表示待渲染 MediaCodec 缓冲区的不透明结构体。
 */
typedef struct MediaCodecBuffer AVMediaCodecBuffer;

/**
 * 释放 MediaCodec 缓冲区，并将其渲染到与解码器关联的 surface。
 * 对给定缓冲区只能调用此函数一次；释放后，底层缓冲区会返回编解码器，
 * 因此后续调用不会产生任何效果。
 *
 * @param buffer 要渲染的缓冲区
 * @param render 为 1 时释放缓冲区并渲染到 surface，为 0 时丢弃缓冲区
 * @return 成功返回 0，否则返回 < 0
 */
int av_mediacodec_release_buffer(AVMediaCodecBuffer *buffer, int render);

/**
 * 释放 MediaCodec 缓冲区，并在给定时间将其渲染到与解码器关联的 surface。
 * 时间戳与当前 `java/lang/System#nanoTime()` 的差值必须在一秒以内
 * （Android 上使用 `CLOCK_MONOTONIC` 实现）。详情参见 Android MediaCodec 文档
 * [`android/media/MediaCodec#releaseOutputBuffer(int,long)`][0]。
 *
 * @param buffer 要渲染的缓冲区
 * @param time 渲染缓冲区的时间戳，单位为纳秒
 * @return 成功返回 0，否则返回 < 0
 *
 * [0]: https://developer.android.com/reference/android/media/MediaCodec#releaseOutputBuffer(int,%20long)
 */
int av_mediacodec_render_buffer_at_time(AVMediaCodecBuffer *buffer, int64_t time);

#endif /* AVCODEC_MEDIACODEC_H */
