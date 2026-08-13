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

#ifndef AVCODEC_AVDCT_H
#define AVCODEC_AVDCT_H

#include <stddef.h>
#include <stdint.h>

#include "libavutil/opt.h"

/**
 * AVDCT 上下文。
 * @note 如果特定功能在构建时被禁用，相应函数指针可能为 NULL。
 */
typedef struct AVDCT {
    const AVClass *av_class;

    void (*idct)(int16_t *block /* align 16 */);

    /**
     * IDCT 输入排列。
     * 一些优化的 IDCT 需要经过排列的输入（相对于参考 IDCT 的正常顺序）。
     * 此排列必须在 idct_put/add 之前执行。
     * 注意，通常可将它与 zigzag/alternate 扫描合并。<br>
     * 为避免混淆，示例如下：
     * - (->decode coeffs -> zigzag reorder -> dequant -> reference IDCT -> ...)
     * - (x -> reference DCT -> reference IDCT -> x)
     * - (x -> reference DCT -> simple_mmx_perm = idct_permutation
     *    -> simple_idct_mmx -> x)
     * - (-> decode coeffs -> zigzag reorder -> simple_mmx_perm -> dequant
     *    -> simple_idct_mmx -> ...)
     */
    uint8_t idct_permutation[64];

    void (*fdct)(int16_t *block /* align 16 */);


    /**
     * DCT 算法。
     * 必须使用 AVOptions 设置此字段。
     */
    int dct_algo;

    /**
     * IDCT 算法。
     * 必须使用 AVOptions 设置此字段。
     */
    int idct_algo;

    void (*get_pixels)(int16_t *block /* align 16 */,
                       const uint8_t *pixels /* align 8 */,
                       ptrdiff_t line_size);

    int bits_per_sample;

    void (*get_pixels_unaligned)(int16_t *block /* align 16 */,
                       const uint8_t *pixels,
                       ptrdiff_t line_size);
} AVDCT;

/**
 * 分配 AVDCT 上下文。
 * 可先使用 AVOptions 对其进行配置，然后需要调用 avcodec_dct_init() 初始化。
 *
 * 使用 av_free() 释放。
 */
AVDCT *avcodec_dct_alloc(void);
int avcodec_dct_init(AVDCT *);

const AVClass *avcodec_dct_get_class(void);

#endif /* AVCODEC_AVDCT_H */
