/*
 * 像素格式 描述符
 * 复制right (c) 2009 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef AVUTIL_PIXDESC_H
#define AVUTIL_PIXDESC_H

#include <inttypes.h>

#include "attributes.h"
#include "pixfmt.h"

typedef struct AVComponentDescriptor {
    /**
     * Which 的 the 4 planes 包含 component.
     */
    int plane;

    /**
     * 数量 的 elements between 2 horizontally consecutive 像素s.
     * Elements are bits 用于 bitstream 格式s, bytes otherwise.
     */
    int step;

    /**
     * 数量 的 elements before the component 的 the first 像素.
     * Elements are bits 用于 bitstream 格式s, bytes otherwise.
     */
    int offset;

    /**
     * 数量 的 least sign如果icant bits that must be sh如果ted away
     * 到 get the 值.
     */
    int shift;

    /**
     * 数量 的 bits 中 the component.
     */
    int depth;
} AVComponentDescriptor;

/**
 * 描述符 that unambiguously describes how the bits 的 a 像素 are
 * stored 中 the up 到 4 data planes 的 an image. It also stores the
 * subsampling factors 和 数量 的 components.
 *
 * @note This is separate 的 the colorspace (RGB, YCbCr, YPbPr, JPEG-style YUV
 *       和 all the YUV variants) AVPixFmt描述符 just stores how 值
 *       are stored not what these 值 represent.
 */
typedef struct AVPixFmtDescriptor {
    const char *name;
    uint8_t nb_components;  ///< The number of components each pixel has, (1-4)

    /**
     * Amount 到 sh如果t the luma 宽度 right 到 find the chroma 宽度.
     * For YV12 this is 1 用于 example.
     * chroma_宽度 = AV_CEIL_RSH如果T(luma_宽度, log2_chroma_w)
     * note above is needed 到 ensure rounding up.
     * This 值 only refers 到 the chroma components.
     */
    uint8_t log2_chroma_w;

    /**
     * Amount 到 sh如果t the luma 高度 right 到 find the chroma 高度.
     * For YV12 this is 1 用于 example.
     * chroma_高度= AV_CEIL_RSH如果T(luma_高度, log2_chroma_h)
     * note above is needed 到 ensure rounding up.
     * This 值 only refers 到 the chroma components.
     */
    uint8_t log2_chroma_h;

    /**
     * Combination 的 AV_PIX_FMT_标志_... 标志.
     */
    uint64_t flags;

    /**
     * Parameters that describe how 像素s are packed.
     * 如果 the 格式 has 1 或 2 components, then luma is 0.
     * 如果 the 格式 has 3 或 4 components:
     *   如果 the RGB 标志 is 设置 then 0 is red, 1 is green 和 2 is blue;
     *   otherwise 0 is luma, 1 is chroma-U 和 2 is chroma-V.
     *
     * 如果 present, the Alpha 声道 is always the last component.
     */
    AVComponentDescriptor comp[4];

    /**
     * Alternative comma-separated names.
     */
    const char *alias;
} AVPixFmtDescriptor;

/**
 * 像素格式 is big-endian.
 */
#define AV_PIX_FMT_FLAG_BE           (1 << 0)
/**
 * 像素格式 has a palette 中 data[1], 值 are indexes 中 this palette.
 */
#define AV_PIX_FMT_FLAG_PAL          (1 << 1)
/**
 * All 值 的 a component are bit-wise packed end 到 end.
 */
#define AV_PIX_FMT_FLAG_BITSTREAM    (1 << 2)
/**
 * 像素格式 is an HW accelerated 格式.
 */
#define AV_PIX_FMT_FLAG_HWACCEL      (1 << 3)
/**
 * At least one 像素 component is not 中 the first data plane.
 */
#define AV_PIX_FMT_FLAG_PLANAR       (1 << 4)
/**
 * 像素格式 包含 RGB-like data (as opposed 到 YUV/grayscale).
 */
#define AV_PIX_FMT_FLAG_RGB          (1 << 5)

/**
 * 像素格式 has an alpha 声道. This is 设置 上 all 格式s that
 * support alpha 中 some way, including AV_PIX_FMT_PAL8. alpha is always
 * straight, never pre-multiplied.
 *
 * 如果 a codec 或 a filter does not support alpha, it should 设置 all alpha to
 * opaque, 或 use the equivalent 像素格式s without alpha component, e.g.
 * AV_PIX_FMT_RGB0 (or AV_PIX_FMT_RGB24 etc.) instead 的 AV_PIX_FMT_RGBA.
 */
#define AV_PIX_FMT_FLAG_ALPHA        (1 << 7)

/**
 * 像素格式 is following a Bayer pattern
 */
#define AV_PIX_FMT_FLAG_BAYER        (1 << 8)

/**
 * 像素格式 包含 IEEE-754 floating point 值. Precision (double,
 * single, 或 half) should be determined by the 像素 大小 (64, 32, 或 16 bits).
 */
#define AV_PIX_FMT_FLAG_FLOAT        (1 << 9)

/**
 * 像素格式 包含 XYZ-like data (as opposed 到 YUV/RGB/grayscale).
 */
#define AV_PIX_FMT_FLAG_XYZ          (1 << 10)

/**
 * 返回 the 数量 的 bits per 像素 used by the 像素格式
 * described by pixdesc. Note that this is not the same as the 数量
 * 的 bits per 采样.
 *
 * 返回ed 数量 的 bits refers 到 the 数量 的 bits actually
 * 用于 storing the 像素 in格式ion, that is padding bits are
 * not counted.
 */
int av_get_bits_per_pixel(const AVPixFmtDescriptor *pixdesc);

/**
 * 返回 the 数量 的 bits per 像素 用于 the 像素格式
 * described by pixdesc, including any padding 或 unused bits.
 */
int av_get_padded_bits_per_pixel(const AVPixFmtDescriptor *pixdesc);

/**
 * @返回 a 像素格式 描述符 用于 provided 像素格式 或 NULL 如果
 * this 像素格式 is unknown.
 */
const AVPixFmtDescriptor *av_pix_fmt_desc_get(enum AVPixelFormat pix_fmt);

/**
 * Iterate over all 像素格式 描述符s known 到 libavutil.
 *
 * @param prev previous 描述符. NULL 到 get the first 描述符.
 *
 * @返回 next 描述符 或 NULL after the last 描述符
 */
const AVPixFmtDescriptor *av_pix_fmt_desc_next(const AVPixFmtDescriptor *prev);

/**
 * @返回 an AV像素格式 id described by desc, 或 AV_PIX_FMT_NONE 如果 desc
 * is not a valid 指针 到 a 像素格式 描述符.
 */
enum AVPixelFormat av_pix_fmt_desc_get_id(const AVPixFmtDescriptor *desc);

/**
 * Utility function 到 access log2_chroma_w log2_chroma_h from
 * the 像素格式 AVPixFmt描述符.
 *
 * @param[in]  pix_fmt the 像素格式
 * @param[out] h_sh如果t store log2_chroma_w (horizontal/宽度 sh如果t)
 * @param[out] v_sh如果t store log2_chroma_h (vertical/高度 sh如果t)
 *
 * @返回 0 上 success, AVERROR(ENOSYS) 上 invalid 或 unknown 像素格式
 */
int av_pix_fmt_get_chroma_sub_sample(enum AVPixelFormat pix_fmt,
                                     int *h_shift, int *v_shift);

/**
 * @返回 数量 的 planes 中 pix_fmt, a negative AVERROR 如果 pix_fmt is not a
 * valid 像素格式.
 */
int av_pix_fmt_count_planes(enum AVPixelFormat pix_fmt);

/**
 * @返回 the name 用于 provided color range 或 NULL 如果 unknown.
 */
const char *av_color_range_name(enum AVColorRange range);

/**
 * @返回 the AVColorRange 值 用于 name 或 an AVError 如果 not found.
 */
int av_color_range_from_name(const char *name);

/**
 * @返回 the name 用于 provided color primaries 或 NULL 如果 unknown.
 */
const char *av_color_primaries_name(enum AVColorPrimaries primaries);

/**
 * @返回 the AVColorPrimaries 值 用于 name 或 an AVError 如果 not found.
 */
int av_color_primaries_from_name(const char *name);

/**
 * @返回 the name 用于 provided color transfer 或 NULL 如果 unknown.
 */
const char *av_color_transfer_name(enum AVColorTransferCharacteristic transfer);

/**
 * @返回 the AVColorTransferCharacteristic 值 用于 name 或 an AVError 如果 not found.
 */
int av_color_transfer_from_name(const char *name);

/**
 * @返回 the name 用于 provided color space 或 NULL 如果 unknown.
 */
const char *av_color_space_name(enum AVColorSpace space);

/**
 * @返回 the AVColorSpace 值 用于 name 或 an AVError 如果 not found.
 */
int av_color_space_from_name(const char *name);

/**
 * @返回 the name 用于 provided chroma location 或 NULL 如果 unknown.
 */
const char *av_chroma_location_name(enum AVChromaLocation location);

/**
 * @返回 the AVChromaLocation 值 用于 name 或 an AVError 如果 not found.
 */
int av_chroma_location_from_name(const char *name);

/**
 * 转换s AVChromaLocation 到 swscale x/y chroma position.
 *
 * positions represent the chroma (0,0) position 中 a coordinates system
 *，使用 luma (0,0) representing the origin 和 luma(1,1) representing 256,256
 *
 * @param xpos  horizontal chroma 采样 position
 * @param ypos  vertical   chroma 采样 position
 */
int av_chroma_location_enum_to_pos(int *xpos, int *ypos, enum AVChromaLocation pos);

/**
 * 转换s swscale x/y chroma position 到 AVChromaLocation.
 *
 * positions represent the chroma (0,0) position 中 a coordinates system
 *，使用 luma (0,0) representing the origin 和 luma(1,1) representing 256,256
 *
 * @param xpos  horizontal chroma 采样 position
 * @param ypos  vertical   chroma 采样 position
 */
enum AVChromaLocation av_chroma_location_pos_to_enum(int xpos, int ypos);

/**
 * @返回 the name 用于 provided alpha mode 或 NULL 如果 unknown.
 */
const char *av_alpha_mode_name(enum AVAlphaMode mode);

/**
 * @返回 the AVAlphaMode 值 用于 name 或 an AVError 如果 not found.
 */
enum AVAlphaMode av_alpha_mode_from_name(const char *name);

/**
 * 返回 the 像素格式 corresponding 到 name.
 *
 * 如果 there is no 像素格式，使用 name name, then looks 用于 a
 * 像素格式，使用 the name corresponding 到 the native endian
 * 格式 的 name.
 * For example 中 a little-endian system, first looks 用于 "gray16",
 * then 用于 "gray16le".
 *
 * Finally 如果 no 像素格式 has been found, 返回s AV_PIX_FMT_NONE.
 */
enum AVPixelFormat av_get_pix_fmt(const char *name);

/**
 * 返回 the short name 用于 a 像素格式, NULL 中 case pix_fmt is
 * unknown.
 *
 * @参见 av_get_pix_fmt(), av_get_pix_fmt_string()
 */
const char *av_get_pix_fmt_name(enum AVPixelFormat pix_fmt);

/**
 * Print 中 buf the string corresponding 到 the 像素格式 with
 * 数量 pix_fmt, 或 a header 如果 pix_fmt is negative.
 *
 * @param buf the 缓冲区 where 到 write the string
 * @param buf_大小 the 大小 的 buf
 * @param pix_fmt the 数量 的 the 像素格式 到 print the
 * corresponding info string, 或 a negative 值 到 print the
 * corresponding header.
 */
char *av_get_pix_fmt_string(char *buf, int buf_size,
                            enum AVPixelFormat pix_fmt);

/**
 * Read a line，来自 an image, 和 write the 值 的 the
 * 像素格式 component c 到 dst.
 *
 * @param data the 数组 containing the 指针s 到 the planes 的 the image
 * @param line大小 the 数组 containing the line大小s 的 the image
 * @param desc the 像素格式 描述符 用于 the image
 * @param x the horizontal coordinate 的 the first 像素 到 read
 * @param y the vertical coordinate 的 the first 像素 到 read
 * @param w the 宽度 的 the line 到 read, that is the 数量 of
 * 值 到 write 到 dst
 * @param read_pal_component 如果 not zero 和 the 格式 is a paletted
 * 格式 writes the 值 corresponding 到 the palette
 * component c 中 data[1] 到 dst, rather than the palette indexes in
 * data[0]. behavior is undefined 如果 the 格式 is not paletted.
 * @param dst_element_大小 大小 的 elements 中 dst 数组 (2 或 4 byte)
 */
void av_read_image_line2(void *dst, const uint8_t *data[4],
                        const int linesize[4], const AVPixFmtDescriptor *desc,
                        int x, int y, int c, int w, int read_pal_component,
                        int dst_element_size);

void av_read_image_line(uint16_t *dst, const uint8_t *data[4],
                        const int linesize[4], const AVPixFmtDescriptor *desc,
                        int x, int y, int c, int w, int read_pal_component);

/**
 * Write the 值，来自 src 到 the 像素格式 component c 的 an
 * image line.
 *
 * @param src 数组 containing the 值 到 write
 * @param data the 数组 containing the 指针s 到 the planes 的 the
 * image 到 write into. It is supposed 到 be zeroed.
 * @param line大小 the 数组 containing the line大小s 的 the image
 * @param desc the 像素格式 描述符 用于 the image
 * @param x the horizontal coordinate 的 the first 像素 到 write
 * @param y the vertical coordinate 的 the first 像素 到 write
 * @param w the 宽度 的 the line 到 write, that is the 数量 of
 * 值 到 write 到 the image line
 * @param src_element_大小 大小 的 elements 中 src 数组 (2 或 4 byte)
 */
void av_write_image_line2(const void *src, uint8_t *data[4],
                         const int linesize[4], const AVPixFmtDescriptor *desc,
                         int x, int y, int c, int w, int src_element_size);

void av_write_image_line(const uint16_t *src, uint8_t *data[4],
                         const int linesize[4], const AVPixFmtDescriptor *desc,
                         int x, int y, int c, int w);

/**
 * Utility function 到 swap the endianness 的 a 像素格式.
 *
 * @param[in]  pix_fmt the 像素格式
 *
 * @返回 像素格式，使用 swapped endianness 如果 it exists,
 * otherwise AV_PIX_FMT_NONE
 */
enum AVPixelFormat av_pix_fmt_swap_endianness(enum AVPixelFormat pix_fmt);

#define FF_LOSS_RESOLUTION        0x0001 /**< loss due to resolution change */
#define FF_LOSS_DEPTH             0x0002 /**< loss due to color depth change */
#define FF_LOSS_COLORSPACE        0x0004 /**< loss due to color space conversion */
#define FF_LOSS_ALPHA             0x0008 /**< loss of alpha bits */
#define FF_LOSS_COLORQUANT        0x0010 /**< loss due to color quantization */
#define FF_LOSS_CHROMA            0x0020 /**< loss of chroma (e.g. RGB to gray conversion) */
#define FF_LOSS_EXCESS_RESOLUTION 0x0040 /**< loss due to unneeded extra resolution */
#define FF_LOSS_EXCESS_DEPTH      0x0080 /**< loss due to unneeded extra color depth */


/**
 * Compute what kind 的 losses will occur 当 转换ing，来自 one spec如果ic
 * 像素格式 到 another.
 * 当 转换ing，来自 one 像素格式 到 another, in格式ion loss may occur.
 * For example, 当 转换ing，来自 RGB24 到 GRAY, the color in格式ion will
 * be lost. Similarly, other losses occur 当 转换ing，来自 some 格式s to
 * other 格式s. These losses can involve loss 的 chroma, but also loss of
 * resolution, loss 的 color depth, loss due 到 the color space conversion, loss
 * 的 the alpha bits 或 loss due 到 color quantization.
 * av_get_fix_fmt_loss() informs you about the various types 的 losses
 * which will occur 当 转换ing，来自 one 像素格式 到 another.
 *
 * @param[in] dst_pix_fmt destination 像素格式
 * @param[in] src_pix_fmt source 像素格式
 * @param[in] has_alpha 是否 the source 像素格式 alpha 声道 is used.
 * @返回 Combination 的 标志 informing you what kind 的 losses will occur
 * (maximum loss 用于 an invalid dst_pix_fmt).
 */
int av_get_pix_fmt_loss(enum AVPixelFormat dst_pix_fmt,
                        enum AVPixelFormat src_pix_fmt,
                        int has_alpha);

/**
 * Compute what kind 的 losses will occur 当 转换ing，来自 one spec如果ic
 * 像素格式 到 another.
 * 当 转换ing，来自 one 像素格式 到 another, in格式ion loss may occur.
 * For example, 当 转换ing，来自 RGB24 到 GRAY, the color in格式ion will
 * be lost. Similarly, other losses occur 当 转换ing，来自 some 格式s to
 * other 格式s. These losses can involve loss 的 chroma, but also loss of
 * resolution, loss 的 color depth, loss due 到 the color space conversion, loss
 * 的 the alpha bits 或 loss due 到 color quantization.
 * av_get_fix_fmt_loss() informs you about the various types 的 losses
 * which will occur 当 转换ing，来自 one 像素格式 到 another.
 *
 * @param[in] dst_pix_fmt destination 像素格式
 * @param[in] src_pix_fmt source 像素格式
 * @param[in] has_alpha 是否 the source 像素格式 alpha 声道 is used.
 * @返回 Combination 的 标志 informing you what kind 的 losses will occur
 * (maximum loss 用于 an invalid dst_pix_fmt).
 */
enum AVPixelFormat av_find_best_pix_fmt_of_2(enum AVPixelFormat dst_pix_fmt1, enum AVPixelFormat dst_pix_fmt2,
                                             enum AVPixelFormat src_pix_fmt, int has_alpha, int *loss_ptr);

#endif /* AVUTIL_PIXDESC_H */
