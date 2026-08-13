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

#ifndef AVUTIL_TX_H
#define AVUTIL_TX_H

#include <stdint.h>
#include <stddef.h>

typedef struct AVTXContext AVTXContext;

typedef struct AVComplexFloat {
    float re, im;
} AVComplexFloat;

typedef struct AVComplexDouble {
    double re, im;
} AVComplexDouble;

typedef struct AVComplexInt32 {
    int32_t re, im;
} AVComplexInt32;

enum AVTXType {
    /**
     * Standard complex 到 complex FFT，使用 采样 data type 的 AVComplexFloat,
     * AVComplexDouble 或 AVComplexInt32, 用于 each respective variant.
     *
     * 输出 is not 1/len normalized. Scaling currently unsupported.
     * stride parameter must be 设置 到 the 大小 的 a single 采样 中 bytes.
     */
    AV_TX_FLOAT_FFT  = 0,
    AV_TX_DOUBLE_FFT = 2,
    AV_TX_INT32_FFT  = 4,

    /**
     * Standard MDCT，使用 a 采样 data type 的 float, double 或 int32_t,
     * respectively. For the float 和 int32 variants, the scale type is
     * 'float', while 用于 the double variant, it's 'double'.
     * 如果 scale is NULL, 1.0 will be used as a 默认.
     *
     * Length is the 帧 大小, not the window 大小 (which is 2x 帧).
     * For forward transforms, the stride spec如果ies the spacing between each
     * 采样 中 the 输出 数组 中 bytes. 输入 must be a flat 数组.
     *
     * For inverse transforms, the stride spec如果ies the spacing between each
     * 采样 中 the 输入 数组 中 bytes. 输出 must be a flat 数组.
     *
     * NOTE: the inverse transform is half-length, meaning the 输出 will not
     * contain redundant data. This is what most codecs work with. To do a full
     * inverse transform, 设置 the AV_TX_FULL_IMDCT 标志 上 init.
     */
    AV_TX_FLOAT_MDCT  = 1,
    AV_TX_DOUBLE_MDCT = 3,
    AV_TX_INT32_MDCT  = 5,

    /**
     * Real 到 complex 和 complex 到 real DFTs.
     * For the float 和 int32 variants, the scale type is 'float', while for
     * the double variant, it's a 'double'. 如果 scale is NULL, 1.0 will be used
     * as a 默认.
     *
     * For forward transforms (R2C), stride must be the spacing between two
     * 采样s 中 bytes. For inverse transforms, the stride must be 设置
     * 到 the spacing between two complex 值 中 bytes.
     *
     * forward transform performs a real-to-complex DFT 的 N 采样s to
     * N/2+1 complex 值.
     *
     * inverse transform performs a complex-to-real DFT 的 N/2+1 complex
     * 值 到 N real 采样s. 输出 is not normalized, but can be
     * made so by 设置ting the scale 值 到 1.0/len.
     * NOTE: the inverse transform always overwrites the 输入.
     */
    AV_TX_FLOAT_RDFT  = 6,
    AV_TX_DOUBLE_RDFT = 7,
    AV_TX_INT32_RDFT  = 8,

    /**
     * Real 到 real (DCT) transforms.
     *
     * forward transform is a DCT-II.
     * inverse transform is a DCT-III.
     *
     * 输入 数组 is always overwritten. DCT-III requires that the
     * 输入 be padded，使用 2 extra 采样s. Stride must be 设置 到 the
     * spacing between two 采样s 中 bytes.
     */
    AV_TX_FLOAT_DCT  = 9,
    AV_TX_DOUBLE_DCT = 10,
    AV_TX_INT32_DCT  = 11,

    /**
     * Discrete Cosine Transform I
     *
     * forward transform is a DCT-I.
     * inverse transform is a DCT-I multiplied by 2/(N + 1).
     *
     * 输入 数组 is always overwritten.
     */
    AV_TX_FLOAT_DCT_I  = 12,
    AV_TX_DOUBLE_DCT_I = 13,
    AV_TX_INT32_DCT_I  = 14,

    /**
     * Discrete Sine Transform I
     *
     * forward transform is a DST-I.
     * inverse transform is a DST-I multiplied by 2/(N + 1).
     *
     * 输入 数组 is always overwritten.
     */
    AV_TX_FLOAT_DST_I  = 15,
    AV_TX_DOUBLE_DST_I = 16,
    AV_TX_INT32_DST_I  = 17,

    /* Not part 的 the API, do not use */
    AV_TX_NB,
};

/**
 * Function 指针 到 a function 到 perform the transform.
 *
 * @note Using a d如果ferent 上下文 than the one 分配d during av_tx_init()
 * is not allowed.
 *
 * @param s the transform 上下文
 * @param out the 输出 数组
 * @param 中 the 输入 数组
 * @param stride the 输入 或 输出 stride 中 bytes
 *
 * out 和 中 数组s must be aligned 到 the maximum required by the CPU
 * architecture unless the AV_TX_UNALIGNED 标志 was 设置 中 av_tx_init().
 * stride must follow the constraints the transform type has spec如果ied.
 */
typedef void (*av_tx_fn)(AVTXContext *s, void *out, void *in, ptrdiff_t stride);

/**
 * 标志 用于 av_tx_init()
 */
enum AVTXFlags {
    /**
     * Allows 用于 in-place trans格式ions, where 输入 == 输出.
     * May be unsupported 或 slower 用于 some transform types.
     */
    AV_TX_INPLACE = 1ULL << 0,

    /**
     * Relaxes alignment requirement 用于 the 中 和 out 数组s 的 av_tx_fn().
     * May be slower，使用 certain transform types.
     */
    AV_TX_UNALIGNED = 1ULL << 1,

    /**
     * Performs a full inverse MDCT rather than leaving out 采样s that can be
     * derived through symmetry. Requires an 输出 数组 的 'len' floats,
     * rather than the usual 'len/2' floats.
     * Ignored 用于 all transforms but inverse MDCTs.
     */
    AV_TX_FULL_IMDCT = 1ULL << 2,

    /**
     * Perform a real 到 half-complex RDFT.
     * Only the real, 或 imaginary coefficients will
     * be 输出, depending 上 the 标志 used. Only available 用于 forward RDFTs.
     * 输出 数组 must have enough space 到 hold N complex 值
     * (regular 大小 用于 a real 到 complex transform).
     */
    AV_TX_REAL_TO_REAL      = 1ULL << 3,
    AV_TX_REAL_TO_IMAGINARY = 1ULL << 4,
};

/**
 * 初始化 a transform 上下文，使用 the given configuration
 * (i)MDCTs，使用 an odd length are currently not supported.
 *
 * @param ctx the 上下文 到 分配, will be NULL 上 error
 * @param tx 指针 到 the transform function 指针 到 设置
 * @param type type the type 的 transform
 * @param inv 是否 到 do an inverse 或 a forward transform
 * @param len the 大小 的 the transform 中 采样s
 * @param scale 指针 到 the 值 到 scale the 输出 如果 supported by type
 * @param 标志 a bitmask 的 AVTX标志 或 0
 *
 * @返回 0 上 success, negative error code 上 failure
 */
int av_tx_init(AVTXContext **ctx, av_tx_fn *tx, enum AVTXType type,
               int inv, int len, const void *scale, uint64_t flags);

/**
 * 释放s a 上下文 和 设置s *ctx 到 NULL, does nothing 当 *ctx == NULL.
 */
void av_tx_uninit(AVTXContext **ctx);

#endif /* AVUTIL_TX_H */
