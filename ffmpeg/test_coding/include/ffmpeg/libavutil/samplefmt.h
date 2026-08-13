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

#ifndef AVUTIL_SAMPLEFMT_H
#define AVUTIL_SAMPLEFMT_H

#include <stdint.h>

/**
 * @addtogroup lavu_音频
 * @{
 *
 * @defgroup lavu_sampfmts 音频 采样格式s
 *
 * 音频 采样格式 enumeration 和 related convenience functions.
 * @{
 */

/**
 * 音频 采样格式s
 *
 * - data described by the 采样格式 is always 中 native-endian order.
 *   采样 值 can be expressed by native C types, hence the lack 的 a signed
 *   24-bit 采样格式 even though it is a common raw 音频 data 格式.
 *
 * - floating-point 格式s are based 上 full volume being 中 the range
 *   [-1.0, 1.0]. Any 值 outside this range are beyond full volume level.
 *
 * - data layout as used 中 av_采样s_fill_数组s() 和 elsewhere 中 FFmpeg
 *   (such as AV帧 中 libavcodec) is as follows:
 *
 * @par
 * For planar 采样格式s, each 音频 声道 is 中 a separate data plane,
 * 和 line大小 is the 缓冲区 大小, 中 bytes, 用于 a single plane. All data
 * planes must be the same 大小. For packed 采样格式s, only the first data
 * plane is used, 和 采样s 用于 each 声道 are interleaved. In this case,
 * line大小 is the 缓冲区 大小, 中 bytes, 用于 the 1 plane.
 *
 */
enum AVSampleFormat {
    AV_SAMPLE_FMT_NONE = -1,
    AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
    AV_SAMPLE_FMT_S32,         ///< signed 32 bits
    AV_SAMPLE_FMT_FLT,         ///< float
    AV_SAMPLE_FMT_DBL,         ///< double

    AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    AV_SAMPLE_FMT_FLTP,        ///< float, planar
    AV_SAMPLE_FMT_DBLP,        ///< double, planar
    AV_SAMPLE_FMT_S64,         ///< signed 64 bits
    AV_SAMPLE_FMT_S64P,        ///< signed 64 bits, planar

    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};

/**
 * 返回 the name 的 采样_fmt, 或 NULL 如果 采样_fmt is not
 * recognized.
 */
const char *av_get_sample_fmt_name(enum AVSampleFormat sample_fmt);

/**
 * 返回 a 采样格式 corresponding 到 name, 或 AV_采样_FMT_NONE
 * 上 error.
 */
enum AVSampleFormat av_get_sample_fmt(const char *name);

/**
 * 返回 the planar<->packed alternative form 的 the given 采样格式, or
 * AV_采样_FMT_NONE 上 error. 如果 the passed 采样_fmt is already 中 the
 * requested planar/packed 格式, the 格式 返回ed is the same as the
 * 输入.
 */
enum AVSampleFormat av_get_alt_sample_fmt(enum AVSampleFormat sample_fmt, int planar);

/**
 * 获取 the packed alternative form 的 the given 采样格式.
 *
 * 如果 the passed 采样_fmt is already 中 packed 格式, the 格式 返回ed is
 * the same as the 输入.
 *
 * @返回  the packed alternative form 的 the given 采样格式 or
            AV_SAMPLE_FMT_NONE on error.
 */
enum AVSampleFormat av_get_packed_sample_fmt(enum AVSampleFormat sample_fmt);

/**
 * 获取 the planar alternative form 的 the given 采样格式.
 *
 * 如果 the passed 采样_fmt is already 中 planar 格式, the 格式 返回ed is
 * the same as the 输入.
 *
 * @返回  the planar alternative form 的 the given 采样格式 or
            AV_SAMPLE_FMT_NONE on error.
 */
enum AVSampleFormat av_get_planar_sample_fmt(enum AVSampleFormat sample_fmt);

/**
 * Generate a string corresponding 到 the 采样格式 with
 * 采样_fmt, 或 a header 如果 采样_fmt is negative.
 *
 * @param buf the 缓冲区 where 到 write the string
 * @param buf_大小 the 大小 的 buf
 * @param 采样_fmt the 数量 的 the 采样格式 到 print the
 * corresponding info string, 或 a negative 值 到 print the
 * corresponding header.
 * @返回 the 指针 到 the filled 缓冲区 或 NULL 中 case 的 other errors
 */
char *av_get_sample_fmt_string(char *buf, int buf_size, enum AVSampleFormat sample_fmt);

/**
 * 返回 数量 的 bytes per 采样.
 *
 * @param 采样_fmt the 采样格式
 * @返回 数量 的 bytes per 采样 或 zero 如果 unknown 用于 the given
 * 采样格式
 */
int av_get_bytes_per_sample(enum AVSampleFormat sample_fmt);

/**
 * 检查 如果 the 采样格式 is planar.
 *
 * @param 采样_fmt the 采样格式 到 inspect
 * @返回 1 如果 the 采样格式 is planar, 0 如果 it is interleaved
 */
int av_sample_fmt_is_planar(enum AVSampleFormat sample_fmt);

/**
 * 获取 the required 缓冲区 大小 用于 the given 音频 parameters.
 *
 * @param[out] line大小 计算d line大小, may be NULL
 * @param nb_声道数   the 数量 的 声道数
 * @param nb_采样s    the 数量 的 采样s 中 a single 声道
 * @param 采样_fmt    the 采样格式
 * @param align         缓冲区 大小 alignment (0 = 默认, 1 = no alignment)
 * @返回              required 缓冲区 大小, 或 negative error code 上 failure
 */
int av_samples_get_buffer_size(int *linesize, int nb_channels, int nb_samples,
                               enum AVSampleFormat sample_fmt, int align);

/**
 * @}
 *
 * @defgroup lavu_sampmanip 采样s manipulation
 *
 * Functions that manipulate 音频 采样s
 * @{
 */

/**
 * Fill plane data 指针s 和 line大小 用于 采样s，使用 采样
 * 格式 采样_fmt.
 *
 * 音频_data 数组 is filled，使用 the 指针s 到 the 采样s data planes:
 * 用于 planar, 设置 the start point 的 each 声道's data within the 缓冲区,
 * 用于 packed, 设置 the start point 的 the entire 缓冲区 only.
 *
 * 值 pointed 到 by line大小 is 设置 到 the aligned 大小 的 each
 * 声道's data 缓冲区 用于 planar layout, 或 到 the aligned 大小 的 the
 * 缓冲区 用于 all 声道数 用于 packed layout.
 *
 * 缓冲区 中 buf must be big enough 到 contain all the 采样s
 * (use av_采样s_get_缓冲区_大小() 到 compute its minimum 大小),
 * otherwise the 音频_data 指针s will point 到 invalid data.
 *
 * @参见 enum AV采样格式
 * documentation 用于 AV采样格式 describes the data layout.
 *
 * @param[out] 音频_data  数组 到 be filled，使用 the 指针 用于 each 声道
 * @param[out] line大小    计算d line大小, may be NULL
 * @param buf              the 指针 到 a 缓冲区 containing the 采样s
 * @param nb_声道数      the 数量 的 声道数
 * @param nb_采样s       the 数量 的 采样s 中 a single 声道
 * @param 采样_fmt       the 采样格式
 * @param align            缓冲区 大小 alignment (0 = 默认, 1 = no alignment)
 * @返回                 minimum 大小 中 bytes required 用于 the 缓冲区 上 success,
 *                         或 a negative error code 上 failure
 */
int av_samples_fill_arrays(uint8_t **audio_data, int *linesize,
                           const uint8_t *buf,
                           int nb_channels, int nb_samples,
                           enum AVSampleFormat sample_fmt, int align);

/**
 * 分配 a 采样s 缓冲区 用于 nb_采样s 采样s, 和 fill data 指针s and
 * line大小 accordingly.
 * 分配d 采样s 缓冲区 can be 释放d by using av_释放p(&音频_data[0])
 * 分配d data will be 初始化d 到 silence.
 *
 * @参见 enum AV采样格式
 * documentation 用于 AV采样格式 describes the data layout.
 *
 * @param[out] 音频_data  数组 到 be filled，使用 the 指针 用于 each 声道
 * @param[out] line大小    aligned 大小 用于 音频 缓冲区(s), may be NULL
 * @param nb_声道数      数量 的 音频 声道数
 * @param nb_采样s       数量 的 采样s per 声道
 * @param 采样_fmt       the 采样格式
 * @param align            缓冲区 大小 alignment (0 = 默认, 1 = no alignment)
 * @返回                 >=0 上 success 或 a negative error code 上 failure
 * @todo 返回 the 大小 的 the 分配d 缓冲区 中 case 的 success at the next bump
 * @参见 av_采样s_fill_数组s()
 * @参见 av_采样s_alloc_数组_and_采样s()
 */
int av_samples_alloc(uint8_t **audio_data, int *linesize, int nb_channels,
                     int nb_samples, enum AVSampleFormat sample_fmt, int align);

/**
 * 分配 a data 指针s 数组, 采样s 缓冲区 用于 nb_采样s
 * 采样s, 和 fill data 指针s 和 line大小 accordingly.
 *
 * This is the same as av_采样s_alloc(), but also 分配s the data
 * 指针s 数组.
 *
 * @参见 av_采样s_alloc()
 */
int av_samples_alloc_array_and_samples(uint8_t ***audio_data, int *linesize, int nb_channels,
                                       int nb_samples, enum AVSampleFormat sample_fmt, int align);

/**
 * 复制 采样s，来自 src 到 dst.
 *
 * @param dst destination 数组 的 指针s 到 data planes
 * @param src source 数组 的 指针s 到 data planes
 * @param dst_off设置 off设置 中 采样s at which the data will be written 到 dst
 * @param src_off设置 off设置 中 采样s at which the data will be read，来自 src
 * @param nb_采样s 数量 的 采样s 到 be copied
 * @param nb_声道数 数量 的 音频 声道数
 * @param 采样_fmt 音频 采样格式
 */
int av_samples_copy(uint8_t * const *dst, uint8_t * const *src, int dst_offset,
                    int src_offset, int nb_samples, int nb_channels,
                    enum AVSampleFormat sample_fmt);

/**
 * Fill an 音频 缓冲区，使用 silence.
 *
 * @param 音频_data  数组 的 指针s 到 data planes
 * @param off设置      off设置 中 采样s at which 到 start filling
 * @param nb_采样s  数量 的 采样s 到 fill
 * @param nb_声道数 数量 的 音频 声道数
 * @param 采样_fmt  音频 采样格式
 */
int av_samples_set_silence(uint8_t * const *audio_data, int offset, int nb_samples,
                           int nb_channels, enum AVSampleFormat sample_fmt);

/**
 * @}
 * @}
 */
#endif /* AVUTIL_SAMPLEFMT_H */
