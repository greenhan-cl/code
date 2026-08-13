/*
 * 复制right (c) 2006 Michael Niedermayer <michaelni@gmx.at>
 * 复制right (c) 2008 Peter Ross
 *
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

#ifndef AVUTIL_CHANNEL_LAYOUT_H
#define AVUTIL_CHANNEL_LAYOUT_H

#include <stdint.h>
#include <stdlib.h>

#include "version.h"
#include "attributes.h"

/**
 * @file
 * @ingroup lavu_音频_声道数
 * 公共 libavutil 声道布局 APIs header.
 */


/**
 * @defgroup lavu_音频_声道数 音频 声道数
 * @ingroup lavu_音频
 *
 * 音频 声道布局 utility functions
 *
 * @{
 */

enum AVChannel {
    /// Invalid 声道 index
    AV_CHAN_NONE = -1,
    AV_CHAN_FRONT_LEFT,
    AV_CHAN_FRONT_RIGHT,
    AV_CHAN_FRONT_CENTER,
    AV_CHAN_LOW_FREQUENCY,
    AV_CHAN_BACK_LEFT,
    AV_CHAN_BACK_RIGHT,
    AV_CHAN_FRONT_LEFT_OF_CENTER,
    AV_CHAN_FRONT_RIGHT_OF_CENTER,
    AV_CHAN_BACK_CENTER,
    AV_CHAN_SIDE_LEFT,
    AV_CHAN_SIDE_RIGHT,
    AV_CHAN_TOP_CENTER,
    AV_CHAN_TOP_FRONT_LEFT,
    AV_CHAN_TOP_FRONT_CENTER,
    AV_CHAN_TOP_FRONT_RIGHT,
    AV_CHAN_TOP_BACK_LEFT,
    AV_CHAN_TOP_BACK_CENTER,
    AV_CHAN_TOP_BACK_RIGHT,
    /** Stereo downmix. */
    AV_CHAN_STEREO_LEFT = 29,
    /** 参见 above. */
    AV_CHAN_STEREO_RIGHT,
    AV_CHAN_WIDE_LEFT,
    AV_CHAN_WIDE_RIGHT,
    AV_CHAN_SURROUND_DIRECT_LEFT,
    AV_CHAN_SURROUND_DIRECT_RIGHT,
    AV_CHAN_LOW_FREQUENCY_2,
    AV_CHAN_TOP_SIDE_LEFT,
    AV_CHAN_TOP_SIDE_RIGHT,
    AV_CHAN_BOTTOM_FRONT_CENTER,
    AV_CHAN_BOTTOM_FRONT_LEFT,
    AV_CHAN_BOTTOM_FRONT_RIGHT,
    AV_CHAN_SIDE_SURROUND_LEFT,     ///<  +90 degrees, Lss, SiL
    AV_CHAN_SIDE_SURROUND_RIGHT,    ///<  -90 degrees, Rss, SiR
    AV_CHAN_TOP_SURROUND_LEFT,      ///< +110 degrees, Lvs, TpLS
    AV_CHAN_TOP_SURROUND_RIGHT,     ///< -110 degrees, Rvs, TpRS

    AV_CHAN_BINAURAL_LEFT = 61,
    AV_CHAN_BINAURAL_RIGHT,

    /** 声道 is empty can be safely skipped. */
    AV_CHAN_UNUSED = 0x200,

    /** 声道 包含 data, but its position is unknown. */
    AV_CHAN_UNKNOWN = 0x300,

    /**
     * Range 的 声道数 between AV_CHAN_AMBISONIC_BASE and
     * AV_CHAN_AMBISONIC_END represent Ambisonic components using the ACN system.
     *
     * Given a 声道 id `<i>` between AV_CHAN_AMBISONIC_BASE and
     * AV_CHAN_AMBISONIC_END (inclusive), the ACN index 的 the 声道 `<n>` is
     * `<n> = <i> - AV_CHAN_AMBISONIC_BASE`.
     *
     * @note these 值 are only 用于 AV_声道_ORDER_CUSTOM 声道
     * orderings, the AV_声道_ORDER_AMBISONIC ordering orders the 声道数
     * implicitly by their position 中 the stream.
     */
    AV_CHAN_AMBISONIC_BASE = 0x400,
    // leave space 用于 1024 ids, which correspond 到 maximum order-32 harmonics,
    // which should be enough 用于 the foreseeable use cases
    AV_CHAN_AMBISONIC_END  = 0x7ff,
};

enum AVChannelOrder {
    /**
     * Only the 声道 count is spec如果ied, without any further in格式ion
     * about the 声道 order.
     */
    AV_CHANNEL_ORDER_UNSPEC,
    /**
     * native 声道 order, i.e. the 声道数 are 中 the same order in
     * which they are defined 中 the AV声道 enum. This supports up 到 63
     * d如果ferent 声道数.
     */
    AV_CHANNEL_ORDER_NATIVE,
    /**
     * 声道 order does not correspond 到 any other predefined order and
     * is stored as an explicit map. For example, this could be 用于 support
     * layouts，使用 64 或 more 声道数, or，使用 empty/skipped (AV_CHAN_UNUSED)
     * 声道数 at arbitrary positions.
     */
    AV_CHANNEL_ORDER_CUSTOM,
    /**
     * 音频 is represented as the decomposition 的 the sound field into
     * spherical harmonics. Each 声道 corresponds 到 a single expansion
     * component. 声道数 are ordered according 到 ACN (Ambisonic 声道
     * 数量).
     *
     * 声道，使用 the index n 中 the stream 包含 spherical
     * harmonic 的 degree l 和 order m given by
     * @code{.un解析d}
     *   l   = floor(sqrt(n)),
     *   m   = n - l * (l + 1).
     * @endcode
     *
     * Conversely given a spherical harmonic 的 degree l 和 order m, the
     * corresponding 声道 index n is given by
     * @code{.un解析d}
     *   n = l * (l + 1) + m.
     * @endcode
     *
     * Normalization is assumed 到 be SN3D (Schmidt Semi-Normalization)
     * as defined 中 AmbiX 格式 $ 2.1.
     */
    AV_CHANNEL_ORDER_AMBISONIC,
    /**
     * 数量 的 声道 orders, not part 的 ABI/API
     */
    FF_CHANNEL_ORDER_NB
};


/**
 * @defgroup 声道_masks 音频 声道 masks
 *
 * 声道布局 is a 64-bits integer，使用 a bit 设置 用于 every 声道.
 * 数量 的 bits 设置 must be equal 到 the 数量 的 声道数.
 * 值 0 means that the 声道布局 is not known.
 * @note this data 结构体 is not powerful enough 到 handle 声道数
 * combinations that have the same 声道 multiple times, such as
 * dual-mono.
 *
 * @{
 */
#define AV_CH_FRONT_LEFT             (1ULL << AV_CHAN_FRONT_LEFT           )
#define AV_CH_FRONT_RIGHT            (1ULL << AV_CHAN_FRONT_RIGHT          )
#define AV_CH_FRONT_CENTER           (1ULL << AV_CHAN_FRONT_CENTER         )
#define AV_CH_LOW_FREQUENCY          (1ULL << AV_CHAN_LOW_FREQUENCY        )
#define AV_CH_BACK_LEFT              (1ULL << AV_CHAN_BACK_LEFT            )
#define AV_CH_BACK_RIGHT             (1ULL << AV_CHAN_BACK_RIGHT           )
#define AV_CH_FRONT_LEFT_OF_CENTER   (1ULL << AV_CHAN_FRONT_LEFT_OF_CENTER )
#define AV_CH_FRONT_RIGHT_OF_CENTER  (1ULL << AV_CHAN_FRONT_RIGHT_OF_CENTER)
#define AV_CH_BACK_CENTER            (1ULL << AV_CHAN_BACK_CENTER          )
#define AV_CH_SIDE_LEFT              (1ULL << AV_CHAN_SIDE_LEFT            )
#define AV_CH_SIDE_RIGHT             (1ULL << AV_CHAN_SIDE_RIGHT           )
#define AV_CH_TOP_CENTER             (1ULL << AV_CHAN_TOP_CENTER           )
#define AV_CH_TOP_FRONT_LEFT         (1ULL << AV_CHAN_TOP_FRONT_LEFT       )
#define AV_CH_TOP_FRONT_CENTER       (1ULL << AV_CHAN_TOP_FRONT_CENTER     )
#define AV_CH_TOP_FRONT_RIGHT        (1ULL << AV_CHAN_TOP_FRONT_RIGHT      )
#define AV_CH_TOP_BACK_LEFT          (1ULL << AV_CHAN_TOP_BACK_LEFT        )
#define AV_CH_TOP_BACK_CENTER        (1ULL << AV_CHAN_TOP_BACK_CENTER      )
#define AV_CH_TOP_BACK_RIGHT         (1ULL << AV_CHAN_TOP_BACK_RIGHT       )
#define AV_CH_STEREO_LEFT            (1ULL << AV_CHAN_STEREO_LEFT          )
#define AV_CH_STEREO_RIGHT           (1ULL << AV_CHAN_STEREO_RIGHT         )
#define AV_CH_WIDE_LEFT              (1ULL << AV_CHAN_WIDE_LEFT            )
#define AV_CH_WIDE_RIGHT             (1ULL << AV_CHAN_WIDE_RIGHT           )
#define AV_CH_SURROUND_DIRECT_LEFT   (1ULL << AV_CHAN_SURROUND_DIRECT_LEFT )
#define AV_CH_SURROUND_DIRECT_RIGHT  (1ULL << AV_CHAN_SURROUND_DIRECT_RIGHT)
#define AV_CH_LOW_FREQUENCY_2        (1ULL << AV_CHAN_LOW_FREQUENCY_2      )
#define AV_CH_TOP_SIDE_LEFT          (1ULL << AV_CHAN_TOP_SIDE_LEFT        )
#define AV_CH_TOP_SIDE_RIGHT         (1ULL << AV_CHAN_TOP_SIDE_RIGHT       )
#define AV_CH_BOTTOM_FRONT_CENTER    (1ULL << AV_CHAN_BOTTOM_FRONT_CENTER  )
#define AV_CH_BOTTOM_FRONT_LEFT      (1ULL << AV_CHAN_BOTTOM_FRONT_LEFT    )
#define AV_CH_BOTTOM_FRONT_RIGHT     (1ULL << AV_CHAN_BOTTOM_FRONT_RIGHT   )
#define AV_CH_SIDE_SURROUND_LEFT     (1ULL << AV_CHAN_SIDE_SURROUND_LEFT   )
#define AV_CH_SIDE_SURROUND_RIGHT    (1ULL << AV_CHAN_SIDE_SURROUND_RIGHT  )
#define AV_CH_TOP_SURROUND_LEFT      (1ULL << AV_CHAN_TOP_SURROUND_LEFT    )
#define AV_CH_TOP_SURROUND_RIGHT     (1ULL << AV_CHAN_TOP_SURROUND_RIGHT   )
#define AV_CH_BINAURAL_LEFT          (1ULL << AV_CHAN_BINAURAL_LEFT        )
#define AV_CH_BINAURAL_RIGHT         (1ULL << AV_CHAN_BINAURAL_RIGHT       )

/**
 * @}
 * @defgroup 声道_mask_c 音频 声道布局s
 * @{
 * */
#define AV_CH_LAYOUT_MONO              (AV_CH_FRONT_CENTER)
#define AV_CH_LAYOUT_STEREO            (AV_CH_FRONT_LEFT|AV_CH_FRONT_RIGHT)
#define AV_CH_LAYOUT_2POINT1           (AV_CH_LAYOUT_STEREO|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_2_1               (AV_CH_LAYOUT_STEREO|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_SURROUND          (AV_CH_LAYOUT_STEREO|AV_CH_FRONT_CENTER)
#define AV_CH_LAYOUT_3POINT1           (AV_CH_LAYOUT_SURROUND|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_4POINT0           (AV_CH_LAYOUT_SURROUND|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_4POINT1           (AV_CH_LAYOUT_4POINT0|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_2_2               (AV_CH_LAYOUT_STEREO|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT)
#define AV_CH_LAYOUT_QUAD              (AV_CH_LAYOUT_STEREO|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_5POINT0           (AV_CH_LAYOUT_SURROUND|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT)
#define AV_CH_LAYOUT_5POINT1           (AV_CH_LAYOUT_5POINT0|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_5POINT0_BACK      (AV_CH_LAYOUT_SURROUND|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_5POINT1_BACK      (AV_CH_LAYOUT_5POINT0_BACK|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_6POINT0           (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT0_FRONT     (AV_CH_LAYOUT_2_2|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_HEXAGONAL         (AV_CH_LAYOUT_5POINT0_BACK|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_3POINT1POINT2     (AV_CH_LAYOUT_3POINT1|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT)
#define AV_CH_LAYOUT_6POINT1           (AV_CH_LAYOUT_5POINT1|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT1_BACK      (AV_CH_LAYOUT_5POINT1_BACK|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT1_FRONT     (AV_CH_LAYOUT_6POINT0_FRONT|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_7POINT0           (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_7POINT0_FRONT     (AV_CH_LAYOUT_5POINT0|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_7POINT1           (AV_CH_LAYOUT_5POINT1|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_7POINT1_WIDE      (AV_CH_LAYOUT_5POINT1|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_7POINT1_WIDE_BACK (AV_CH_LAYOUT_5POINT1_BACK|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_5POINT1POINT2     (AV_CH_LAYOUT_5POINT1|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT)
#define AV_CH_LAYOUT_5POINT1POINT2_BACK (AV_CH_LAYOUT_5POINT1_BACK|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT)
#define AV_CH_LAYOUT_OCTAGONAL         (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_CENTER|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_CUBE              (AV_CH_LAYOUT_QUAD|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT|AV_CH_TOP_BACK_LEFT|AV_CH_TOP_BACK_RIGHT)
#define AV_CH_LAYOUT_5POINT1POINT4_BACK (AV_CH_LAYOUT_5POINT1POINT2|AV_CH_TOP_BACK_LEFT|AV_CH_TOP_BACK_RIGHT)
#define AV_CH_LAYOUT_7POINT1POINT2     (AV_CH_LAYOUT_7POINT1|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT)
#define AV_CH_LAYOUT_7POINT1POINT4_BACK (AV_CH_LAYOUT_7POINT1POINT2|AV_CH_TOP_BACK_LEFT|AV_CH_TOP_BACK_RIGHT)
#define AV_CH_LAYOUT_7POINT2POINT3     (AV_CH_LAYOUT_7POINT1POINT2|AV_CH_TOP_BACK_CENTER|AV_CH_LOW_FREQUENCY_2)
#define AV_CH_LAYOUT_9POINT1POINT4_BACK (AV_CH_LAYOUT_7POINT1POINT4_BACK|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_9POINT1POINT6     (AV_CH_LAYOUT_9POINT1POINT4_BACK|AV_CH_TOP_SIDE_LEFT|AV_CH_TOP_SIDE_RIGHT)
#define AV_CH_LAYOUT_HEXADECAGONAL     (AV_CH_LAYOUT_OCTAGONAL|AV_CH_WIDE_LEFT|AV_CH_WIDE_RIGHT|AV_CH_TOP_BACK_LEFT|AV_CH_TOP_BACK_RIGHT|AV_CH_TOP_BACK_CENTER|AV_CH_TOP_FRONT_CENTER|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT)
#define AV_CH_LAYOUT_BINAURAL          (AV_CH_BINAURAL_LEFT|AV_CH_BINAURAL_RIGHT)
#define AV_CH_LAYOUT_STEREO_DOWNMIX    (AV_CH_STEREO_LEFT|AV_CH_STEREO_RIGHT)
#define AV_CH_LAYOUT_22POINT2          (AV_CH_LAYOUT_9POINT1POINT6|AV_CH_BACK_CENTER|AV_CH_LOW_FREQUENCY_2|AV_CH_TOP_FRONT_CENTER|AV_CH_TOP_CENTER|AV_CH_TOP_BACK_CENTER|AV_CH_BOTTOM_FRONT_CENTER|AV_CH_BOTTOM_FRONT_LEFT|AV_CH_BOTTOM_FRONT_RIGHT)

#define AV_CH_LAYOUT_7POINT1_TOP_BACK AV_CH_LAYOUT_5POINT1POINT2_BACK

enum AVMatrixEncoding {
    AV_MATRIX_ENCODING_NONE,
    AV_MATRIX_ENCODING_DOLBY,
    AV_MATRIX_ENCODING_DPLII,
    AV_MATRIX_ENCODING_DPLIIX,
    AV_MATRIX_ENCODING_DPLIIZ,
    AV_MATRIX_ENCODING_DOLBYEX,
    AV_MATRIX_ENCODING_DOLBYHEADPHONE,
    AV_MATRIX_ENCODING_NB
};

/**
 * @}
 */

/**
 * AV声道Custom defines a single 声道 within a custom order layout
 *
 * Unlike most 结构体s 中 FFmpeg, 大小of(AV声道Custom) is a part 的 the
 * 公共 ABI.
 *
 * No new fields may be added 到 it without a major version bump.
 */
typedef struct AVChannelCustom {
    enum AVChannel id;
    char name[16];
    void *opaque;
} AVChannelCustom;

/**
 * AV声道Layout holds in格式ion about the 声道布局 的 音频 data.
 *
 * 声道布局 here is defined as a 设置 的 声道数 ordered 中 a spec如果ic
 * way (unless the 声道 order is AV_声道_ORDER_UNSPEC, 中 which case an
 * AV声道Layout carries only the 声道 count).
 * All orders may be treated as 如果 they were AV_声道_ORDER_UNSPEC by
 * ignoring everything but the 声道 count, as long as av_声道_layout_检查()
 * considers they are valid.
 *
 * Unlike most 结构体s 中 FFmpeg, 大小of(AV声道Layout) is a part 的 the
 * 公共 ABI 和 may be used by the caller. E.g. it may be 分配d 上 stack
 * 或 embedded 中 caller-defined structs.
 *
 * AV声道Layout can be 初始化d as follows:
 * - 默认 initialization，使用 {0}, followed by 设置ting all used fields
 *   correctly;
 * - by assigning one 的 the predefined AV_声道_LAYOUT_* 初始化rs;
 * -，使用 a constructor function, such as av_声道_layout_默认(),
 *   av_声道_layout_from_mask() 或 av_声道_layout_from_string().
 *
 * 声道布局 must be un初始化d，使用 av_声道_layout_uninit()
 *
 * 复制ing an AV声道Layout via assigning is forbidden,
 * av_声道_layout_复制() must be used instead (and its 返回 值 should
 * be 检查ed)
 *
 * No new fields may be added 到 it without a major version bump, except for
 * new elements 的 the union fitting 中 大小of(uint64_t).
 */
typedef struct AVChannelLayout {
    /**
     * 声道 order used 中 this layout.
     * This is a mandatory field.
     */
    enum AVChannelOrder order;

    /**
     * 数量 的 声道数 中 this layout. Mandatory field.
     */
    int nb_channels;

    /**
     * Details about which 声道数 are present 中 this layout.
     * For AV_声道_ORDER_UNSPEC, this field is undefined 和 must not be
     * used.
     */
    union {
        /**
         * This member must be 用于 AV_声道_ORDER_NATIVE, 和 may be used
         * 用于 AV_声道_ORDER_AMBISONIC 到 signal non-diegetic 声道数.
         * It is a bitmask, where the position 的 each 设置 bit means that the
         * AV声道，使用 the corresponding 值 is present.
         *
         * I.e. 当 (mask & (1 << AV_CHAN_FOO)) is non-zero, then AV_CHAN_FOO
         * is present 中 the layout. Otherwise it is not present.
         *
         * @note 当 a 声道布局 using a bitmask is constructed or
         * mod如果ied manually (i.e.  not using any 的 the av_声道_layout_*
         * functions), the code doing it must ensure that the 数量 的 设置 bits
         * is equal 到 nb_声道数.
         */
        uint64_t mask;
        /**
         * This member must be used 当 the 声道 order is
         * AV_声道_ORDER_CUSTOM. It is a nb_声道数-大小d 数组,，使用 each
         * element signalling the presence 的 the AV声道，使用 the
         * corresponding 值 中 map[i].id.
         *
         * I.e. 当 map[i].id is equal 到 AV_CHAN_FOO, then AV_CH_FOO is the
         * i-th 声道 中 the 音频 data.
         *
         * 当 map[i].id is 中 the range between AV_CHAN_AMBISONIC_BASE and
         * AV_CHAN_AMBISONIC_END (inclusive), the 声道 包含 an ambisonic
         * component，使用 ACN index (as defined above)
         * n = map[i].id - AV_CHAN_AMBISONIC_BASE.
         *
         * map[i].name may be filled，使用 a 0-terminated string, 中 which case
         * it will be 用于 the purpose 的 ident如果ying the 声道，使用 the
         * convenience functions below. Otherwise it must be zeroed.
         */
        AVChannelCustom *map;
    } u;

    /**
     * For some 私有 data 的 the user.
     */
    void *opaque;
} AVChannelLayout;

/**
 * Macro 到 define native 声道布局s
 *
 * @note This doesn't use designated 初始化rs 用于 compatibility，使用 C++ 17 和 older.
 */
#define AV_CHANNEL_LAYOUT_MASK(nb, m) \
    { /* .order */ AV_CHANNEL_ORDER_NATIVE, \
      /* .nb_声道数 */  (nb), \
      /* .u.mask */ { m }, \
      /* .opaque */ NULL }

/**
 * @name Common pre-defined 声道布局s
 * @{
 */
#define AV_CHANNEL_LAYOUT_MONO              AV_CHANNEL_LAYOUT_MASK(1,  AV_CH_LAYOUT_MONO)
#define AV_CHANNEL_LAYOUT_STEREO            AV_CHANNEL_LAYOUT_MASK(2,  AV_CH_LAYOUT_STEREO)
#define AV_CHANNEL_LAYOUT_2POINT1           AV_CHANNEL_LAYOUT_MASK(3,  AV_CH_LAYOUT_2POINT1)
#define AV_CHANNEL_LAYOUT_2_1               AV_CHANNEL_LAYOUT_MASK(3,  AV_CH_LAYOUT_2_1)
#define AV_CHANNEL_LAYOUT_SURROUND          AV_CHANNEL_LAYOUT_MASK(3,  AV_CH_LAYOUT_SURROUND)
#define AV_CHANNEL_LAYOUT_3POINT1           AV_CHANNEL_LAYOUT_MASK(4,  AV_CH_LAYOUT_3POINT1)
#define AV_CHANNEL_LAYOUT_4POINT0           AV_CHANNEL_LAYOUT_MASK(4,  AV_CH_LAYOUT_4POINT0)
#define AV_CHANNEL_LAYOUT_4POINT1           AV_CHANNEL_LAYOUT_MASK(5,  AV_CH_LAYOUT_4POINT1)
#define AV_CHANNEL_LAYOUT_2_2               AV_CHANNEL_LAYOUT_MASK(4,  AV_CH_LAYOUT_2_2)
#define AV_CHANNEL_LAYOUT_QUAD              AV_CHANNEL_LAYOUT_MASK(4,  AV_CH_LAYOUT_QUAD)
#define AV_CHANNEL_LAYOUT_5POINT0           AV_CHANNEL_LAYOUT_MASK(5,  AV_CH_LAYOUT_5POINT0)
#define AV_CHANNEL_LAYOUT_5POINT1           AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_5POINT1)
#define AV_CHANNEL_LAYOUT_5POINT0_BACK      AV_CHANNEL_LAYOUT_MASK(5,  AV_CH_LAYOUT_5POINT0_BACK)
#define AV_CHANNEL_LAYOUT_5POINT1_BACK      AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_5POINT1_BACK)
#define AV_CHANNEL_LAYOUT_6POINT0           AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_6POINT0)
#define AV_CHANNEL_LAYOUT_6POINT0_FRONT     AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_6POINT0_FRONT)
#define AV_CHANNEL_LAYOUT_3POINT1POINT2     AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_3POINT1POINT2)
#define AV_CHANNEL_LAYOUT_HEXAGONAL         AV_CHANNEL_LAYOUT_MASK(6,  AV_CH_LAYOUT_HEXAGONAL)
#define AV_CHANNEL_LAYOUT_6POINT1           AV_CHANNEL_LAYOUT_MASK(7,  AV_CH_LAYOUT_6POINT1)
#define AV_CHANNEL_LAYOUT_6POINT1_BACK      AV_CHANNEL_LAYOUT_MASK(7,  AV_CH_LAYOUT_6POINT1_BACK)
#define AV_CHANNEL_LAYOUT_6POINT1_FRONT     AV_CHANNEL_LAYOUT_MASK(7,  AV_CH_LAYOUT_6POINT1_FRONT)
#define AV_CHANNEL_LAYOUT_7POINT0           AV_CHANNEL_LAYOUT_MASK(7,  AV_CH_LAYOUT_7POINT0)
#define AV_CHANNEL_LAYOUT_7POINT0_FRONT     AV_CHANNEL_LAYOUT_MASK(7,  AV_CH_LAYOUT_7POINT0_FRONT)
#define AV_CHANNEL_LAYOUT_7POINT1           AV_CHANNEL_LAYOUT_MASK(8,  AV_CH_LAYOUT_7POINT1)
#define AV_CHANNEL_LAYOUT_7POINT1_WIDE      AV_CHANNEL_LAYOUT_MASK(8,  AV_CH_LAYOUT_7POINT1_WIDE)
#define AV_CHANNEL_LAYOUT_7POINT1_WIDE_BACK AV_CHANNEL_LAYOUT_MASK(8,  AV_CH_LAYOUT_7POINT1_WIDE_BACK)
#define AV_CHANNEL_LAYOUT_5POINT1POINT2     AV_CHANNEL_LAYOUT_MASK(8,  AV_CH_LAYOUT_5POINT1POINT2)
#define AV_CHANNEL_LAYOUT_5POINT1POINT2_BACK AV_CHANNEL_LAYOUT_MASK(8, AV_CH_LAYOUT_5POINT1POINT2_BACK)
#define AV_CHANNEL_LAYOUT_OCTAGONAL         AV_CHANNEL_LAYOUT_MASK(8,  AV_CH_LAYOUT_OCTAGONAL)
#define AV_CHANNEL_LAYOUT_CUBE              AV_CHANNEL_LAYOUT_MASK(8,  AV_CH_LAYOUT_CUBE)
#define AV_CHANNEL_LAYOUT_5POINT1POINT4_BACK AV_CHANNEL_LAYOUT_MASK(10, AV_CH_LAYOUT_5POINT1POINT4_BACK)
#define AV_CHANNEL_LAYOUT_7POINT1POINT2     AV_CHANNEL_LAYOUT_MASK(10, AV_CH_LAYOUT_7POINT1POINT2)
#define AV_CHANNEL_LAYOUT_7POINT1POINT4_BACK AV_CHANNEL_LAYOUT_MASK(12, AV_CH_LAYOUT_7POINT1POINT4_BACK)
#define AV_CHANNEL_LAYOUT_7POINT2POINT3     AV_CHANNEL_LAYOUT_MASK(12, AV_CH_LAYOUT_7POINT2POINT3)
#define AV_CHANNEL_LAYOUT_9POINT1POINT4_BACK AV_CHANNEL_LAYOUT_MASK(14, AV_CH_LAYOUT_9POINT1POINT4_BACK)
#define AV_CHANNEL_LAYOUT_9POINT1POINT6     AV_CHANNEL_LAYOUT_MASK(16, AV_CH_LAYOUT_9POINT1POINT6)
#define AV_CHANNEL_LAYOUT_HEXADECAGONAL     AV_CHANNEL_LAYOUT_MASK(16, AV_CH_LAYOUT_HEXADECAGONAL)
#define AV_CHANNEL_LAYOUT_BINAURAL          AV_CHANNEL_LAYOUT_MASK(2,  AV_CH_LAYOUT_BINAURAL)
#define AV_CHANNEL_LAYOUT_STEREO_DOWNMIX    AV_CHANNEL_LAYOUT_MASK(2,  AV_CH_LAYOUT_STEREO_DOWNMIX)
#define AV_CHANNEL_LAYOUT_22POINT2          AV_CHANNEL_LAYOUT_MASK(24, AV_CH_LAYOUT_22POINT2)

#define AV_CHANNEL_LAYOUT_7POINT1_TOP_BACK  AV_CHANNEL_LAYOUT_5POINT1POINT2_BACK

#define AV_CHANNEL_LAYOUT_AMBISONIC_FIRST_ORDER \
    { /* .order */ AV_CHANNEL_ORDER_AMBISONIC, \
      /* .nb_声道数 */ 4, \
      /* .u.mask */ { 0 }, \
      /* .opaque */ NULL }
/** @} */

struct AVBPrint;

/**
 * 获取 a human readable string 中 an abbreviated form describing a given 声道.
 * This is the inverse function 的 @ref av_声道_from_string().
 *
 * @param buf pre-分配d 缓冲区 where 到 put the generated string
 * @param buf_大小 大小 中 bytes 的 the 缓冲区.
 * @param 声道 the AV声道 whose name 到 get
 * @返回 amount 的 bytes needed 到 hold the 输出 string, 或 a negative AVERROR
 *         上 failure. 如果 the 返回ed 值 is bigger than buf_大小, then the
 *         string was truncated.
 */
int av_channel_name(char *buf, size_t buf_size, enum AVChannel channel);

/**
 * bprint variant 的 av_声道_name().
 *
 * @note the string will be appended 到 the bprint 缓冲区.
 */
void av_channel_name_bprint(struct AVBPrint *bp, enum AVChannel channel_id);

/**
 * 获取 a human readable string describing a given 声道.
 *
 * @param buf pre-分配d 缓冲区 where 到 put the generated string
 * @param buf_大小 大小 中 bytes 的 the 缓冲区.
 * @param 声道 the AV声道 whose 描述 到 get
 * @返回 amount 的 bytes needed 到 hold the 输出 string, 或 a negative AVERROR
 *         上 failure. 如果 the 返回ed 值 is bigger than buf_大小, then the
 *         string was truncated.
 */
int av_channel_description(char *buf, size_t buf_size, enum AVChannel channel);

/**
 * bprint variant 的 av_声道_描述().
 *
 * @note the string will be appended 到 the bprint 缓冲区.
 */
void av_channel_description_bprint(struct AVBPrint *bp, enum AVChannel channel_id);

/**
 * This is the inverse function 的 @ref av_声道_name().
 *
 * @返回 the 声道，使用 the given name
 *         AV_CHAN_NONE 当 name does not ident如果y a known 声道
 */
enum AVChannel av_channel_from_string(const char *name);

/**
 * 初始化 a custom 声道布局，使用 the spec如果ied 数量 的 声道数.
 * 声道 map will be 分配d 和 the designation 的 all 声道数 will
 * be 设置 到 AV_CHAN_UNKNOWN.
 *
 * This is only a convenience helper function, a custom 声道布局 can also
 * be constructed without using this.
 *
 * @param 声道_layout the layout 结构体 到 be 初始化d
 * @param nb_声道数 the 数量 的 声道数
 *
 * @返回 0 上 success
 *         AVERROR(EINVAL) 如果 the 数量 的 声道数 <= 0
 *         AVERROR(ENOMEM) 如果 the 声道 map could not be 分配d
 */
int av_channel_layout_custom_init(AVChannelLayout *channel_layout, int nb_channels);

/**
 * 初始化 a native 声道布局，来自 a bitmask indicating which 声道数
 * are present.
 *
 * @param 声道_layout the layout 结构体 到 be 初始化d
 * @param mask bitmask describing the 声道布局
 *
 * @返回 0 上 success
 *         AVERROR(EINVAL) 用于 invalid mask 值
 */
int av_channel_layout_from_mask(AVChannelLayout *channel_layout, uint64_t mask);

/**
 * 初始化 a 声道布局，来自 a given string 描述.
 * 输入 string can be represented by:
 *  - the formal 声道布局 name (返回ed by av_声道_layout_describe())
 *  - single 或 multiple 声道 names (返回ed by av_声道_name(), eg. "FL",
 *    或 concatenated，使用 "+", each 可选ly containing a custom name after
 *    a "@", eg. "FL@Left+FR@Right+LFE")
 *  - a decimal 或 hexadecimal 值 的 a native 声道布局 (eg. "4" 或 "0x4")
 *  - the 数量 的 声道数，使用 默认 layout (eg. "4c")
 *  - the 数量 的 unordered 声道数 (eg. "4C" 或 "4 声道数")
 *  - the ambisonic order followed by 可选 non-diegetic 声道数 (eg.
 *    "ambisonic 2+stereo")
 * On error, the 声道布局 will remain un初始化d, but not necessarily
 * untouched.
 *
 * @param 声道_layout un初始化d 声道布局 用于 the result
 * @param str string describing the 声道布局
 * @返回 0 上 success parsing the 声道布局
 *         AVERROR(EINVAL) 如果 an invalid 声道布局 string was provided
 *         AVERROR(ENOMEM) 如果 there was not enough 内存
 */
int av_channel_layout_from_string(AVChannelLayout *channel_layout,
                                  const char *str);

/**
 * 获取 the 默认 声道布局 用于 a given 数量 的 声道数.
 *
 * @param ch_layout the layout 结构体 到 be 初始化d
 * @param nb_声道数 数量 的 声道数
 */
void av_channel_layout_default(AVChannelLayout *ch_layout, int nb_channels);

/**
 * Iterate over all standard 声道布局s.
 *
 * @param opaque a 指针 where libavutil will store the iteration state. Must
 *               point 到 NULL 到 start the iteration.
 *
 * @返回 the standard 声道布局 或 NULL 当 the iteration is
 *         finished
 */
const AVChannelLayout *av_channel_layout_standard(void **opaque);

/**
 * 释放 any 分配d data 中 the 声道布局 和 re设置 the 声道
 * count 到 0.
 *
 * @param 声道_layout the layout 结构体 到 be un初始化d
 */
void av_channel_layout_uninit(AVChannelLayout *channel_layout);

/**
 * Make a 复制 的 a 声道布局. This d如果fers，来自 just assigning src 到 dst
 * 中 that it 分配s 和 copies the map 用于 AV_声道_ORDER_CUSTOM.
 *
 * @note the destination 声道_layout will be always un初始化d before 复制.
 *
 * @param dst destination 声道布局
 * @param src source 声道布局
 * @返回 0 上 success, a negative AVERROR 上 error.
 */
int av_channel_layout_copy(AVChannelLayout *dst, const AVChannelLayout *src);

/**
 * 获取 a human-readable string describing the 声道布局 properties.
 * string will be 中 the same 格式 that is accepted by
 * @ref av_声道_layout_from_string(), allowing 到 rebuild the same
 * 声道布局, except 用于 opaque 指针s.
 *
 * @param 声道_layout 声道布局 到 be described
 * @param buf pre-分配d 缓冲区 where 到 put the generated string
 * @param buf_大小 大小 中 bytes 的 the 缓冲区.
 * @返回 amount 的 bytes needed 到 hold the 输出 string, 或 a negative AVERROR
 *         上 failure. 如果 the 返回ed 值 is bigger than buf_大小, then the
 *         string was truncated.
 */
int av_channel_layout_describe(const AVChannelLayout *channel_layout,
                               char *buf, size_t buf_size);

/**
 * bprint variant 的 av_声道_layout_describe().
 *
 * @note the string will be appended 到 the bprint 缓冲区.
 * @返回 0 上 success, 或 a negative AVERROR 值 上 failure.
 */
int av_channel_layout_describe_bprint(const AVChannelLayout *channel_layout,
                                      struct AVBPrint *bp);

/**
 * 获取 the 声道，使用 the given index 中 a 声道布局.
 *
 * @param 声道_layout 输入 声道布局
 * @param idx index 的 the 声道
 * @返回 声道，使用 the index idx 中 声道_layout 上 success or
 *         AV_CHAN_NONE 上 failure (如果 idx is not valid 或 the 声道 order is
 *         unspec如果ied)
 */
enum AVChannel
av_channel_layout_channel_from_index(const AVChannelLayout *channel_layout, unsigned int idx);

/**
 * 获取 the index 的 a given 声道 中 a 声道布局. In case multiple
 * 声道数 are found, only the first match will be 返回ed.
 *
 * @param 声道_layout 输入 声道布局
 * @param 声道 the 声道 whose index 到 obtain
 * @返回 index 的 声道 中 声道_layout 上 success 或 a negative 数量 如果
 *         声道 is not present 中 声道_layout.
 */
int av_channel_layout_index_from_channel(const AVChannelLayout *channel_layout,
                                         enum AVChannel channel);

/**
 * 获取 the index 中 a 声道布局 的 a 声道 described by the given string.
 * In case multiple 声道数 are found, only the first match will be 返回ed.
 *
 * 此函数 accepts 声道 names 中 the same 格式 as
 * @ref av_声道_from_string().
 *
 * @param 声道_layout 输入 声道布局
 * @param name string describing the 声道 whose index 到 obtain
 * @返回 a 声道 index described by the given string, 或 a negative AVERROR
 *         值.
 */
int av_channel_layout_index_from_string(const AVChannelLayout *channel_layout,
                                        const char *name);

/**
 * 获取 a 声道 described by the given string.
 *
 * 此函数 accepts 声道 names 中 the same 格式 as
 * @ref av_声道_from_string().
 *
 * @param 声道_layout 输入 声道布局
 * @param name string describing the 声道 到 obtain
 * @返回 a 声道 described by the given string 中 声道_layout 上 success
 *         或 AV_CHAN_NONE 上 failure (如果 the string is not valid 或 the 声道
 *         order is unspec如果ied)
 */
enum AVChannel
av_channel_layout_channel_from_string(const AVChannelLayout *channel_layout,
                                      const char *name);

/**
 * Find out what 声道数，来自 a given 设置 are present 中 a 声道布局,
 * without regard 用于 their positions.
 *
 * @param 声道_layout 输入 声道布局
 * @param mask a combination 的 AV_CH_* representing a 设置 的 声道数
 * @返回 a bitfield representing all the 声道数，来自 mask that are present
 *         中 声道_layout
 */
uint64_t av_channel_layout_subset(const AVChannelLayout *channel_layout,
                                  uint64_t mask);

/**
 * 检查 是否 a 声道布局 is valid, i.e. can possibly describe 音频
 * data.
 *
 * @param 声道_layout 输入 声道布局
 * @返回 1 如果 声道_layout is valid, 0 otherwise.
 */
int av_channel_layout_check(const AVChannelLayout *channel_layout);

/**
 * 检查 是否 two 声道布局s are semantically the same, i.e. the same
 * 声道数 are present 上 the same positions 中 both.
 *
 * 如果 one 的 the 声道布局s is AV_声道_ORDER_UNSPEC, while the other is
 * not, they are considered 到 be unequal. 如果 both are AV_声道_ORDER_UNSPEC,
 * they are considered equal 如果f the 声道 counts are the same 中 both.
 *
 * @param chl 输入 声道布局
 * @param chl1 输入 声道布局
 * @返回 0 如果 chl 和 chl1 are equal, 1 如果 they are not equal. negative
 *         AVERROR code 如果 one 或 both are invalid.
 */
int av_channel_layout_compare(const AVChannelLayout *chl, const AVChannelLayout *chl1);

/**
 * 返回 the order 如果 the layout is n-th order standard-order ambisonic.
 * presence 的 可选 extra non-diegetic 声道数 at the end is not taken
 * into account.
 *
 * @param 声道_layout 输入 声道布局
 * @返回 the order 的 the layout, a negative error code otherwise.
 */
int av_channel_layout_ambisonic_order(const AVChannelLayout *channel_layout);

/**
 * conversion must be lossless.
 */
#define AV_CHANNEL_LAYOUT_RETYPE_FLAG_LOSSLESS (1 << 0)

/**
 * spec如果ied retype target order is ignored 和 the simplest possible
 * (canonical) order is 用于 which the 输入 layout can be losslessy
 * represented.
 */
#define AV_CHANNEL_LAYOUT_RETYPE_FLAG_CANONICAL (1 << 1)

/**
 * Change the AV声道Order 的 a 声道布局.
 *
 * Change 的 AV声道Order can be either lossless 或 lossy. In case 的 a
 * lossless conversion all the 声道 designations 和 the associated 声道
 * names (如果 any) are kept. On a lossy conversion the 声道 names 和 声道
 * designations might be lost depending 上 the capabilities 的 the desired
 * AV声道Order. Note that some conversions are simply not possible 中 which
 * case this function 返回s AVERROR(ENOSYS).
 *
 * following conversions are supported:
 *
 * Any       -> Custom     : Always possible, always lossless.
 * Any       -> Unspec如果ied: Always possible, lossless 如果 声道 designations
 *   are all unknown 和 声道 names are not used, lossy otherwise.
 * Custom    -> Ambisonic  : Possible 如果 it 包含 ambisonic 声道数 with
 *   可选 non-diegetic 声道数 中 the end. Lossy 如果 the 声道数 have
 *   custom names, lossless otherwise.
 * Custom    -> Native     : Possible 如果 it 包含 native 声道数 中 native
 *     order. Lossy 如果 the 声道数 have custom names, lossless otherwise.
 *
 * On error this function keeps the original 声道布局 untouched.
 *
 * @param 声道_layout 声道布局 which will be changed
 * @param order the desired 声道布局 order
 * @param 标志 a combination 的 AV_声道_LAYOUT_RETYPE_标志_* constants
 * @返回 0 如果 the conversion was successful 和 lossless 或 如果 the 声道
 *           layout was already 中 the desired order
 *         >0 如果 the conversion was successful but lossy
 *         AVERROR(ENOSYS) 如果 the conversion was not possible (or would be
 *           lossy 和 AV_声道_LAYOUT_RETYPE_标志_LOSSLESS was spec如果ied)
 *         AVERROR(EINVAL), AVERROR(ENOMEM) 上 error
 */
int av_channel_layout_retype(AVChannelLayout *channel_layout, enum AVChannelOrder order, int flags);

/**
 * @}
 */

#endif /* AVUTIL_CHANNEL_LAYOUT_H */
