/*
 * Copyright (C) 2011-2013 Michael Niedermayer (michaelni@gmx.at)
 *
 * This file is part of libswresample
 *
 * libswresample is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libswresample is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libswresample; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SWRESAMPLE_SWRESAMPLE_H
#define SWRESAMPLE_SWRESAMPLE_H

/**
 * @file
 * @ingroup lswr
 * libswresample 公共头文件
 */

/**
 * @defgroup lswr libswresample
 * @{
 *
 * 音频重采样、采样格式转换和混音库。
 *
 * 通过 SwrContext 与 lswr 交互，该上下文使用 swr_alloc() 或
 * swr_alloc_set_opts2() 分配。它是不透明结构，因此所有参数都必须通过
 * @ref avoptions API 设置。
 *
 * 使用 lswr 时，首先需要分配 SwrContext。可以使用 swr_alloc() 或
 * swr_alloc_set_opts2() 完成。如果使用前者，则必须通过 @ref avoptions API
 * 设置选项。后一个函数提供相同功能，但允许在同一条语句中设置一些常用选项。
 *
 * 例如，以下代码会配置从平面浮点采样格式到交错有符号 16 位整数格式的转换，
 * 将采样率从 48kHz 降至 44.1kHz，并将 5.1 声道下混为立体声
 * （使用默认混音矩阵）。这里使用 swr_alloc() 函数。
 * @code
 * SwrContext *swr = swr_alloc();
 * av_opt_set_chlayout(swr, "in_chlayout", &(AVChannelLayout)AV_CHANNEL_LAYOUT_5POINT1, 0);
 * av_opt_set_chlayout(swr, "out_chlayout", &(AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO, 0);
 * av_opt_set_int(swr, "in_sample_rate",     48000,                0);
 * av_opt_set_int(swr, "out_sample_rate",    44100,                0);
 * av_opt_set_sample_fmt(swr, "in_sample_fmt",  AV_SAMPLE_FMT_FLTP, 0);
 * av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16,  0);
 * @endcode
 *
 * 使用 swr_alloc_set_opts2() 也可以完成相同工作：
 * @code
 * SwrContext *swr = NULL;
 * int ret = swr_alloc_set_opts2(&swr,         // we're allocating a new context
 *                       &(AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO, // out_ch_layout
 *                       AV_SAMPLE_FMT_S16,    // out_sample_fmt
 *                       44100,                // out_sample_rate
 *                       &(AVChannelLayout)AV_CHANNEL_LAYOUT_5POINT1, // in_ch_layout
 *                       AV_SAMPLE_FMT_FLTP,   // in_sample_fmt
 *                       48000,                // in_sample_rate
 *                       0,                    // log_offset
 *                       NULL);                // log_ctx
 * @endcode
 *
 * 设置好所有值后，必须使用 swr_init() 初始化。如果需要更改转换参数，可以像
 * 上面的第一个示例那样通过 @ref avoptions 修改参数；也可以调用
 * swr_alloc_set_opts2()，并将已分配的上下文作为第一个参数传入。
 * 随后必须再次调用 swr_init()。
 *
 * 转换本身通过反复调用 swr_convert() 完成。请注意，如果提供的输出空间不足，
 * 或正在进行需要“未来”样本的采样率转换，样本可能会缓存在 swr 中。
 * 不需要未来输入的样本可随时通过 swr_convert() 取出（可将 in_count 设为 0）。
 * 转换结束时，可通过调用 swr_convert()，并传入 NULL 的 in 和 0 的 in_count，
 * 来冲刷重采样缓冲区。
 *
 * 转换过程中使用的样本可以通过 libavutil 的
 * @ref lavu_sampmanip "样本操作" API 管理，其中包括下例使用的
 * av_samples_alloc() 函数。
 *
 * 可以随时使用 swr_get_delay() 获取输入与输出之间的延迟。
 *
 * 以下代码使用上面的参数以及调用者定义的 get_input() 和 handle_output()
 * 函数来演示转换循环：
 * @code
 * uint8_t **input;
 * int in_samples;
 *
 * while (get_input(&input, &in_samples)) {
 *     uint8_t *output;
 *     int out_samples = av_rescale_rnd(swr_get_delay(swr, 48000) +
 *                                      in_samples, 44100, 48000, AV_ROUND_UP);
 *     av_samples_alloc(&output, NULL, 2, out_samples,
 *                      AV_SAMPLE_FMT_S16, 0);
 *     out_samples = swr_convert(swr, &output, out_samples,
 *                                      input, in_samples);
 *     handle_output(output, out_samples);
 *     av_freep(&output);
 * }
 * @endcode
 *
 * 转换完成后，必须使用 swr_free() 释放转换上下文及其关联的所有内容。
 * 此外还提供 swr_close()，但它主要用于兼容 libavresample，并非必须调用。
 *
 * 即使在 swr_free() 之前没有完全冲刷数据，也不会发生内存泄漏。
 */

#include <stdint.h>
#include "libavutil/channel_layout.h"
#include "libavutil/frame.h"
#include "libavutil/samplefmt.h"

#include "libswresample/version_major.h"
#ifndef HAVE_AV_CONFIG_H
/* 作为 ffmpeg 构建的一部分被包含时，仅包含主版本号，
 * 以避免不必要的重新构建。从外部包含时，仍包含完整的版本信息。 */
#include "libswresample/version.h"
#endif

/**
 * @name 选项常量
 * 这些常量用于 lswr 的 @ref avoptions 接口。
 * @{
 *
 */

#define SWR_FLAG_RESAMPLE 1 ///< 即使采样率相同也强制重采样
//TODO 是否使用 int resample？
//长期 TODO：能否动态启用此功能？

/** 抖动算法 */
enum SwrDitherType {
    SWR_DITHER_NONE = 0,
    SWR_DITHER_RECTANGULAR,
    SWR_DITHER_TRIANGULAR,
    SWR_DITHER_TRIANGULAR_HIGHPASS,

    SWR_DITHER_NS = 64,         ///< 不属于 API/ABI
    SWR_DITHER_NS_LIPSHITZ,
    SWR_DITHER_NS_F_WEIGHTED,
    SWR_DITHER_NS_MODIFIED_E_WEIGHTED,
    SWR_DITHER_NS_IMPROVED_E_WEIGHTED,
    SWR_DITHER_NS_SHIBATA,
    SWR_DITHER_NS_LOW_SHIBATA,
    SWR_DITHER_NS_HIGH_SHIBATA,
    SWR_DITHER_NB,              ///< 不属于 API/ABI
};

/** 重采样引擎 */
enum SwrEngine {
    SWR_ENGINE_SWR,             /**< SW 重采样器 */
    SWR_ENGINE_SOXR,            /**< SoX 重采样器 */
    SWR_ENGINE_NB,              ///< 不属于 API/ABI
};

/** 重采样滤波器类型 */
enum SwrFilterType {
    SWR_FILTER_TYPE_CUBIC,              /**< 三次 */
    SWR_FILTER_TYPE_BLACKMAN_NUTTALL,   /**< Blackman-Nuttall 窗 sinc */
    SWR_FILTER_TYPE_KAISER,             /**< Kaiser 窗 sinc */
};

/**
 * @}
 */

/**
 * libswresample 上下文。与 libavcodec 和 libavformat 不同，此结构是不透明的。
 * 这意味着设置选项时必须使用 @ref avoptions API，不能直接给结构体成员赋值。
 */
typedef struct SwrContext SwrContext;

/**
 * 获取 SwrContext 的 AVClass。它可以与 AV_OPT_SEARCH_FAKE_OBJ 结合使用，
 * 以检查选项。
 *
 * @see av_opt_find().
 * @return SwrContext 的 AVClass
 */
const AVClass *swr_get_class(void);

/**
 * @name SwrContext 构造函数
 * @{
 */

/**
 * 分配 SwrContext。
 *
 * 使用此函数时，需要在调用 swr_init() 之前设置参数（手动设置或使用
 * swr_alloc_set_opts2()）。
 *
 * @see swr_alloc_set_opts2(), swr_init(), swr_free()
 * @return 出错时返回 NULL，否则返回已分配的上下文
 */
struct SwrContext *swr_alloc(void);

/**
 * 在用户参数设置完成后初始化上下文。
 * @note 必须使用 AVOption API 配置上下文。
 *
 * @see av_opt_set_int()
 * @see av_opt_set_dict()
 *
 * @param[in,out]   s 要初始化的 Swr 上下文
 * @return 失败时返回 AVERROR 错误码。
 */
int swr_init(struct SwrContext *s);

/**
 * 检查 swr 上下文是否已经初始化。
 *
 * @param[in]       s 要检查的 Swr 上下文
 * @see swr_init()
 * @return 已初始化时返回正值，未初始化时返回 0
 */
int swr_is_initialized(struct SwrContext *s);

/**
 * 按需分配 SwrContext，并设置/重置常用参数。
 *
 * 此函数不要求事先使用 swr_alloc() 分配 *ps。另一方面，通过 swr_alloc()
 * 分配上下文后，也可以使用 swr_alloc_set_opts2() 设置其参数。
 *
 * @param ps              如果已有 Swr 上下文，则指向该上下文；否则指向 NULL。
 *                        成功时，*ps 会被设置为已分配的上下文。
 * @param out_ch_layout   输出声道布局（例如 AV_CHANNEL_LAYOUT_*）
 * @param out_sample_fmt  输出采样格式（AV_SAMPLE_FMT_*）。
 * @param out_sample_rate 输出采样率（单位 Hz）
 * @param in_ch_layout    输入声道布局（例如 AV_CHANNEL_LAYOUT_*）
 * @param in_sample_fmt   输入采样格式（AV_SAMPLE_FMT_*）。
 * @param in_sample_rate  输入采样率（单位 Hz）
 * @param log_offset      日志级别偏移量
 * @param log_ctx         父日志上下文，可以为 NULL
 *
 * @see swr_init(), swr_free()
 * @return 成功时返回 0，出错时返回负的 AVERROR 错误码。
 *         出错时会释放 Swr 上下文，并将 *ps 设为 NULL。
 */
int swr_alloc_set_opts2(struct SwrContext **ps,
                        const AVChannelLayout *out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
                        const AVChannelLayout *in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
                        int log_offset, void *log_ctx);
/**
 * @}
 *
 * @name SwrContext 析构函数
 * @{
 */

/**
 * 释放给定的 SwrContext，并将指针设为 NULL。
 *
 * @param[in] s 指向 Swr 上下文指针的指针
 */
void swr_free(struct SwrContext **s);

/**
 * 关闭上下文，使 swr_is_initialized() 返回 0。
 *
 * 可以通过运行 swr_init() 重新启用该上下文；也可以不调用 swr_close()，
 * 直接使用 swr_init()。此函数主要用于简化同时支持 libavresample 和
 * libswresample 的使用场景。
 *
 * @param[in,out] s 要关闭的 Swr 上下文
 */
void swr_close(struct SwrContext *s);

/**
 * @}
 *
 * @name 核心转换函数
 * @{
 */

/** 转换音频。
 *
 * 最后可将 in 和 in_count 设为 0，以冲刷出剩余的少量样本。
 *
 * 如果提供的输入量超过输出空间，输入将被缓冲。可以使用
 * swr_get_out_samples() 获取给定输入样本数所需输出样本数的上限，从而避免
 * 这种缓冲。只要可能，转换会直接进行而不复制数据。
 *
 * @param s         已分配且已设置参数的 Swr 上下文
 * @param out       输出缓冲区；对于打包音频，只需设置第一个缓冲区
 * @param out_count 每个声道可供输出的空间，以样本数计
 * @param in        输入缓冲区；对于打包音频，只需设置第一个缓冲区
 * @param in_count  单个声道中可用的输入样本数
 *
 * @return 每个声道输出的样本数；出错时返回负值
 */
int swr_convert(struct SwrContext *s, uint8_t * const *out, int out_count,
                                const uint8_t * const *in , int in_count);

/**
 * 将下一个时间戳从输入时间戳转换为输出时间戳。
 * 时间戳的单位是 1/(in_sample_rate * out_sample_rate)。
 *
 * @note 有两种行为略有不同的模式。
 *       @li 不使用自动时间戳补偿时（min_compensation >= FLT_MAX），
 *           时间戳会在补偿延迟后直接传递。
 *       @li 使用自动时间戳补偿时（min_compensation < FLT_MAX），
 *           输出时间戳将与输出样本编号匹配。
 *           两种补偿模式请参见 ffmpeg-resampler(1)。
 *
 * @param[in] s     已初始化的 Swr 上下文
 * @param[in] pts   下一个输入样本的时间戳；未知时为 INT64_MIN
 * @see swr_set_compensation()、swr_drop_output() 和 swr_inject_silence()
 *      是内部用于时间戳补偿的函数。
 * @return 下一个输出样本的输出时间戳
 */
int64_t swr_next_pts(struct SwrContext *s, int64_t pts);

/**
 * @}
 *
 * @name 低级选项设置函数
 * 这些函数提供一种设置 AVOption API 无法设置的低级选项的方法。
 * @{
 */

/**
 * 激活重采样补偿（“软”补偿）。swr_next_pts() 会在需要时从内部调用此函数。
 *
 * @param[in,out] s             已分配的 Swr 上下文。如果它尚未初始化，或者未设置
 *                              SWR_FLAG_RESAMPLE，则会在设置该标志后调用 swr_init()。
 * @param[in]     sample_delta  每个样本的 PTS 差值
 * @param[in]     compensation_distance 要进行补偿的样本数
 * @return    成功时 >= 0；在以下情况下返回 AVERROR 错误码：
 *            @li @c s 为 NULL；
 *            @li @c compensation_distance 小于 0；
 *            @li @c compensation_distance 为 0，但 sample_delta 不为 0；
 *            @li 重采样器不支持补偿；或者
 *            @li 调用 swr_init() 失败。
 */
int swr_set_compensation(struct SwrContext *s, int sample_delta, int compensation_distance);

/**
 * 设置自定义输入声道映射。
 *
 * @param[in,out] s           已分配但尚未初始化的 Swr 上下文
 * @param[in]     channel_map 自定义输入声道映射（声道索引数组，-1 表示静音声道）
 * @return 成功时 >= 0，失败时返回 AVERROR 错误码。
 */
int swr_set_channel_mapping(struct SwrContext *s, const int *channel_map);

/**
 * 生成声道混音矩阵。
 *
 * libswresample 内部使用此函数构建默认混音矩阵。将它公开只是为了作为构建
 * 自定义矩阵的工具函数。
 *
 * @param in_layout           输入声道布局
 * @param out_layout          输出声道布局
 * @param center_mix_level    中置声道的混音级别
 * @param surround_mix_level  环绕声道的混音级别
 * @param lfe_mix_level       低频效果声道的混音级别
 * @param rematrix_maxval     若为 1.0，将归一化系数以防止溢出；若为 INT_MAX，
 *                            则不归一化系数。
 * @param[out] matrix         混音系数；matrix[i + stride * o] 表示输入声道 i 在
 *                            输出声道 o 中的权重。
 * @param stride              矩阵数组中相邻输入声道之间的距离
 * @param matrix_encoding     矩阵立体声下混模式（例如 dplii）
 * @param log_ctx             父日志上下文，可以为 NULL
 * @return                    成功时返回 0，失败时返回负的 AVERROR 错误码
 */
int swr_build_matrix2(const AVChannelLayout *in_layout, const AVChannelLayout *out_layout,
                      double center_mix_level, double surround_mix_level,
                      double lfe_mix_level, double maxval,
                      double rematrix_volume, double *matrix,
                      ptrdiff_t stride, enum AVMatrixEncoding matrix_encoding,
                      void *log_context);

/**
 * 设置自定义重混矩阵。
 *
 * @param s       已分配但尚未初始化的 Swr 上下文
 * @param matrix  重混系数；matrix[i + stride * o] 表示输入声道 i 在输出声道 o
 *                中的权重
 * @param stride  矩阵各行之间的偏移量
 * @return 成功时 >= 0，失败时返回 AVERROR 错误码。
 */
int swr_set_matrix(struct SwrContext *s, const double *matrix, int stride);

/**
 * @}
 *
 * @name 样本处理函数
 * @{
 */

/**
 * 丢弃指定数量的输出样本。
 *
 * 需要进行“硬”补偿时，swr_next_pts() 会调用此函数和
 * swr_inject_silence()。
 *
 * @param s     已分配的 Swr 上下文
 * @param count 要丢弃的样本数
 *
 * @return 成功时 >= 0，失败时返回负的 AVERROR 错误码
 */
int swr_drop_output(struct SwrContext *s, int count);

/**
 * 注入指定数量的静音样本。
 *
 * 需要进行“硬”补偿时，swr_next_pts() 会调用此函数和 swr_drop_output()。
 *
 * @param s     已分配的 Swr 上下文
 * @param count 要注入的样本数
 *
 * @return 成功时 >= 0，失败时返回负的 AVERROR 错误码
 */
int swr_inject_silence(struct SwrContext *s, int count);

/**
 * 获取下一个输入样本相对于下一个输出样本将经历的延迟。
 *
 * 当提供的输入量超过可用输出空间时，Swresample 可能会缓冲数据；在采样率之间
 * 进行转换也需要延迟。此函数返回所有这些延迟的总和。
 * 精确延迟在输入或输出采样率下不一定是整数。尤其是在大幅降采样时，输出采样率
 * 可能不适合表示延迟；对于升采样和输入采样率，情况类似。
 *
 * @param s     swr 上下文
 * @param base  返回延迟所使用的时间基：
 *              @li 设为 1 时，返回的延迟以秒为单位
 *              @li 设为 1000 时，返回的延迟以毫秒为单位
 *              @li 设为输入采样率时，返回的延迟以输入样本数为单位
 *              @li 设为输出采样率时，返回的延迟以输出样本数为单位
 *              @li 设为 in_sample_rate 和 out_sample_rate 的最小公倍数时，
 *                  返回无需舍入的精确延迟
 * @returns     以 1 / @c base 为单位的延迟。
 */
int64_t swr_get_delay(struct SwrContext *s, int64_t base);

/**
 * 如果下一次调用 swr_convert 时传入 in_samples 个输入样本，则查找其输出样本数
 * 的上限。此值取决于内部状态，任何改变内部状态的操作（例如继续调用
 * swr_convert()）都可能改变 swr_get_out_samples() 针对相同输入样本数返回的值。
 *
 * @param in_samples    输入样本数。
 * @note 调用 swr_inject_silence()、swr_convert()、swr_next_pts() 或
 *       swr_set_compensation() 中的任何一个，都会使此上限失效
 * @note 即使 swr_get_out_samples() 表明实际使用量更少，仍建议向
 *       swr_convert() 等所有函数传入正确的可用缓冲区大小。
 * @returns 下一次 swr_convert 将输出的样本数上限；负值表示错误
 */
int swr_get_out_samples(struct SwrContext *s, int in_samples);

/**
 * @}
 *
 * @name 配置访问函数
 * @{
 */

/**
 * 返回 @ref LIBSWRESAMPLE_VERSION_INT 常量。
 *
 * 这可用于检查构建时使用的 libswresample 与运行时使用的版本是否相同。
 *
 * @returns     unsigned int 类型的版本号
 */
unsigned swresample_version(void);

/**
 * 返回 swr 的构建时配置。
 *
 * @returns     构建时使用的 @c ./configure 标志
 */
const char *swresample_configuration(void);

/**
 * 返回 swr 的许可证文本。
 *
 * @returns     构建时确定的 libswresample 许可证
 */
const char *swresample_license(void);

/**
 * @}
 *
 * @name 基于 AVFrame 的 API
 * @{
 */

/**
 * 转换输入 AVFrame 中的样本，并将其写入输出 AVFrame。
 *
 * 输入和输出 AVFrame 必须设置 channel_layout、sample_rate 和 format。
 *
 * 如果输出 AVFrame 尚未分配数据指针，则会设置 nb_samples 字段，并调用
 * av_frame_get_buffer() 分配帧。
 *
 * 输出 AVFrame 可以为 NULL，也可以分配了少于所需数量的样本。在这种情况下，
 * 所有未写入输出的剩余样本都会加入内部 FIFO 缓冲区，并在下次调用此函数或
 * swr_convert() 时返回。
 *
 * 如果正在转换采样率，内部重采样延迟缓冲区中可能仍有数据。
 * swr_get_delay() 会给出剩余样本数。要将这些数据作为输出取出，请以 NULL
 * 输入调用此函数或 swr_convert()。
 *
 * 如果 SwrContext 配置与输出或输入 AVFrame 的设置不匹配，则不会进行转换。
 * 根据不匹配的是哪个 AVFrame，会返回 AVERROR_OUTPUT_CHANGED、
 * AVERROR_INPUT_CHANGED，或二者按位或的结果。
 *
 * @see swr_delay()
 * @see swr_convert()
 * @see swr_get_delay()
 *
 * @param swr             音频重采样上下文
 * @param output          输出 AVFrame
 * @param input           输入 AVFrame
 * @return                成功时返回 0；失败或配置不匹配时返回 AVERROR。
 */
int swr_convert_frame(SwrContext *swr,
                      AVFrame *output, const AVFrame *input);

/**
 * 使用 AVFrame 提供的信息配置或重新配置 SwrContext。
 *
 * 即使失败，原重采样上下文也会被重置。如果上下文已打开，此函数会在内部调用
 * swr_close()。
 *
 * @see swr_close();
 *
 * @param swr             音频重采样上下文
 * @param out             输出 AVFrame
 * @param in              输入 AVFrame
 * @return                成功时返回 0，失败时返回 AVERROR。
 */
int swr_config_frame(SwrContext *swr, const AVFrame *out, const AVFrame *in);

/**
 * @}
 * @}
 */

#endif /* SWRESAMPLE_SWRESAMPLE_H */
