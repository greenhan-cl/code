/*
 * Audio FIFO
 * Copyright (c) 2012 Justin Ruggles <justin.ruggles@gmail.com>
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

/**
 * @file
 * 音频 FIFO 缓冲区
 */

#ifndef AVUTIL_AUDIO_FIFO_H
#define AVUTIL_AUDIO_FIFO_H

#include "attributes.h"
#include "samplefmt.h"

/**
 * @addtogroup lavu_audio
 * @{
 *
 * @defgroup lavu_audiofifo 音频 FIFO 缓冲区
 * @{
 */

/**
 * 音频 FIFO 缓冲区的上下文。
 *
 * - 在样本级别而非字节级别操作。
 * - 支持采用平面或打包采样格式的多声道。
 * - 向已满缓冲区写入时自动重新分配。
 */
typedef struct AVAudioFifo AVAudioFifo;

/**
 * 释放 AVAudioFifo。
 *
 * @param af 要释放的 AVAudioFifo
 */
void av_audio_fifo_free(AVAudioFifo *af);

/**
 * 分配 AVAudioFifo。
 *
 * @param sample_fmt  采样格式
 * @param channels    声道数量
 * @param nb_samples  初始分配大小，以样本数计
 * @return            新分配的 AVAudioFifo；出错时返回 NULL
 */
AVAudioFifo *av_audio_fifo_alloc(enum AVSampleFormat sample_fmt, int channels,
                                 int nb_samples);

/**
 * 重新分配 AVAudioFifo。
 *
 * @param af          要重新分配的 AVAudioFifo
 * @param nb_samples  新的分配大小，以样本数计
 * @return            成功时返回 0，失败时返回负的 AVERROR 错误码
 */
av_warn_unused_result
int av_audio_fifo_realloc(AVAudioFifo *af, int nb_samples);

/**
 * 向 AVAudioFifo 写入数据。
 *
 * 如果可用空间少于 nb_samples，AVAudioFifo 会自动重新分配。
 *
 * @see enum AVSampleFormat
 * AVSampleFormat 的文档描述了数据布局。
 *
 * @param af          要写入的 AVAudioFifo
 * @param data        音频数据平面指针
 * @param nb_samples  要写入的样本数
 * @return            实际写入的样本数；失败时返回负的 AVERROR 错误码。成功时，
 *                    实际写入的样本数始终为 nb_samples。
 */
int av_audio_fifo_write(AVAudioFifo *af, void * const *data, int nb_samples);

/**
 * 查看 AVAudioFifo 中的数据但不移除。
 *
 * @see enum AVSampleFormat
 * AVSampleFormat 的文档描述了数据布局。
 *
 * @param af          要读取的 AVAudioFifo
 * @param data        音频数据平面指针
 * @param nb_samples  要查看的样本数
 * @return            实际查看的样本数；失败时返回负的 AVERROR 错误码。实际查看的
 *                    样本数不会大于 nb_samples，且仅当 av_audio_fifo_size 小于
 *                    nb_samples 时才会少于 nb_samples。
 */
int av_audio_fifo_peek(const AVAudioFifo *af, void * const *data, int nb_samples);

/**
 * 查看 AVAudioFifo 中的数据但不移除。
 *
 * @see enum AVSampleFormat
 * AVSampleFormat 的文档描述了数据布局。
 *
 * @param af          要读取的 AVAudioFifo
 * @param data        音频数据平面指针
 * @param nb_samples  要查看的样本数
 * @param offset      相对于当前读取位置的偏移量
 * @return            实际查看的样本数；失败时返回负的 AVERROR 错误码。实际查看的
 *                    样本数不会大于 nb_samples，且仅当 av_audio_fifo_size 小于
 *                    nb_samples 时才会少于 nb_samples。
 */
int av_audio_fifo_peek_at(const AVAudioFifo *af, void * const *data,
                          int nb_samples, int offset);

/**
 * 从 AVAudioFifo 读取数据。
 *
 * @see enum AVSampleFormat
 * AVSampleFormat 的文档描述了数据布局。
 *
 * @param af          要读取的 AVAudioFifo
 * @param data        音频数据平面指针
 * @param nb_samples  要读取的样本数
 * @return            实际读取的样本数；失败时返回负的 AVERROR 错误码。实际读取的
 *                    样本数不会大于 nb_samples，且仅当 av_audio_fifo_size 小于
 *                    nb_samples 时才会少于 nb_samples。
 */
int av_audio_fifo_read(AVAudioFifo *af, void * const *data, int nb_samples);

/**
 * 从 AVAudioFifo 中排出数据。
 *
 * 不读取数据而将其移除。
 *
 * @param af          要排出数据的 AVAudioFifo
 * @param nb_samples  要排出的样本数
 * @return            成功时返回 0，失败时返回负的 AVERROR 错误码
 */
int av_audio_fifo_drain(AVAudioFifo *af, int nb_samples);

/**
 * 重置 AVAudioFifo 缓冲区。
 *
 * 这会清空缓冲区中的所有数据。
 *
 * @param af 要重置的 AVAudioFifo
 */
void av_audio_fifo_reset(AVAudioFifo *af);

/**
 * 获取 AVAudioFifo 中当前可供读取的样本数。
 *
 * @param af 要查询的 AVAudioFifo
 * @return   可供读取的样本数
 */
int av_audio_fifo_size(AVAudioFifo *af);

/**
 * 获取 AVAudioFifo 中当前可供写入的样本数。
 *
 * @param af 要查询的 AVAudioFifo
 * @return   可供写入的样本数
 */
int av_audio_fifo_space(AVAudioFifo *af);

/**
 * @}
 * @}
 */

#endif /* AVUTIL_AUDIO_FIFO_H */
