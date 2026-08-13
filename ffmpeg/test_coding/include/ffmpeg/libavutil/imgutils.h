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

#ifndef AVUTIL_IMGUTILS_H
#define AVUTIL_IMGUTILS_H

/**
 * @file
 * misc image utilities
 *
 * @addtogroup lavu_picture
 * @{
 */

#include <stddef.h>
#include <stdint.h>
#include "pixdesc.h"
#include "pixfmt.h"
#include "rational.h"

/**
 * Compute the max 像素 step 用于 each plane 的 an image，使用 a
 * 格式 described by pixdesc.
 *
 * 像素 step is the distance 中 bytes between the first byte of
 * the group 的 bytes which describe a 像素 component 和 the first
 * byte 的 the successive group 中 the same plane 用于 the same
 * component.
 *
 * @param max_pixsteps an 数组 which is filled，使用 the max 像素 step
 * 用于 each plane. Since a plane may contain d如果ferent 像素
 * components, the computed max_pixsteps[plane] is relative 到 the
 * component 中 the plane，使用 the max 像素 step.
 * @param max_pixstep_comps an 数组 which is filled，使用 the component
 * 用于 each plane which has the max 像素 step. May be NULL.
 * @param pixdesc the AVPixFmt描述符 用于 the image, describing its 格式
 */
void av_image_fill_max_pixsteps(int max_pixsteps[4], int max_pixstep_comps[4],
                                const AVPixFmtDescriptor *pixdesc);

/**
 * Compute the 大小 的 an image line，使用 格式 pix_fmt 和 宽度
 * 宽度 用于 the plane plane.
 *
 * @返回 the computed 大小 中 bytes
 */
int av_image_get_linesize(enum AVPixelFormat pix_fmt, int width, int plane);

/**
 * Fill plane line大小s 用于 an image，使用 像素格式 pix_fmt and
 * 宽度 宽度.
 *
 * @param line大小s 数组 到 be filled，使用 the line大小 用于 each plane
 * @param pix_fmt the AV像素格式 的 the image
 * @param 宽度 宽度 的 the image 中 像素s
 * @返回 >= 0 中 case 的 success, a negative error code otherwise
 */
int av_image_fill_linesizes(int linesizes[4], enum AVPixelFormat pix_fmt, int width);

/**
 * Fill plane 大小s 用于 an image，使用 像素格式 pix_fmt 和 高度 高度.
 *
 * @param 大小 the 数组 到 be filled，使用 the 大小 的 each image plane
 * @param pix_fmt the AV像素格式 的 the image
 * @param 高度 高度 的 the image 中 像素s
 * @param line大小s the 数组 containing the line大小 用于 each
 *        plane, should be filled by av_image_fill_line大小s()
 * @返回 >= 0 中 case 的 success, a negative error code otherwise
 *
 * @note line大小 parameters have the type ptrd如果f_t here, while they are
 *       int 用于 av_image_fill_line大小s().
 */
int av_image_fill_plane_sizes(size_t size[4], enum AVPixelFormat pix_fmt,
                              int height, const ptrdiff_t linesizes[4]);

/**
 * Fill plane data 指针s 用于 an image，使用 像素格式 pix_fmt and
 * 高度 高度.
 *
 * @param data 指针s 数组 到 be filled，使用 the 指针 用于 each image plane
 * @param pix_fmt the AV像素格式 的 the image
 * @param 高度 高度 的 the image 中 像素s
 * @param ptr the 指针 到 a 缓冲区 which will contain the image
 * @param line大小s the 数组 containing the line大小 用于 each
 * plane, should be filled by av_image_fill_line大小s()
 * @返回 the 大小 中 bytes required 用于 the image 缓冲区, a negative
 * error code 中 case 的 failure
 */
int av_image_fill_pointers(uint8_t *data[4], enum AVPixelFormat pix_fmt, int height,
                           uint8_t *ptr, const int linesizes[4]);

/**
 * 分配 an image，使用 大小 w 和 h 和 像素格式 pix_fmt, and
 * fill 指针s 和 line大小s accordingly.
 * 分配d image 缓冲区 has 到 be 释放d by using
 * av_释放p(&指针s[0]).
 *
 * @param 指针s 数组 到 be filled，使用 the 指针 用于 each image plane
 * @param line大小s the 数组 filled，使用 the line大小 用于 each plane
 * @param w 宽度 的 the image 中 像素s
 * @param h 高度 的 the image 中 像素s
 * @param pix_fmt the AV像素格式 的 the image
 * @param align the 值 到 use 用于 缓冲区 大小 alignment
 * @返回 the 大小 中 bytes required 用于 the image 缓冲区, a negative
 * error code 中 case 的 failure
 */
int av_image_alloc(uint8_t *pointers[4], int linesizes[4],
                   int w, int h, enum AVPixelFormat pix_fmt, int align);

/**
 * 复制 image plane，来自 src 到 dst.
 * That is, 复制 "高度" 数量 的 lines 的 "byte宽度" bytes each.
 * first byte 的 each successive line is separated by *_line大小
 * bytes.
 *
 * byte宽度 must be contained by both absolute 值 的 dst_line大小
 * 和 src_line大小, otherwise the function behavior is undefined.
 *
 * @param dst          destination plane 到 复制 to
 * @param dst_line大小 line大小 用于 the image plane 中 dst
 * @param src          source plane 到 复制 from
 * @param src_line大小 line大小 用于 the image plane 中 src
 * @param 高度       高度 (数量 的 lines) 的 the plane
 */
void av_image_copy_plane(uint8_t       *dst, int dst_linesize,
                         const uint8_t *src, int src_linesize,
                         int bytewidth, int height);

/**
 * 复制 image data located 中 uncacheable (e.g. GPU mapped) 内存. Where
 * available, this function will use special functionality 用于 reading，来自 such
 * 内存, which may result 中 greatly improved performance compared 到 plain
 * av_image_复制_plane().
 *
 * byte宽度 must be contained by both absolute 值 的 dst_line大小
 * 和 src_line大小, otherwise the function behavior is undefined.
 *
 * @note line大小 parameters have the type ptrd如果f_t here, while they are
 *       int 用于 av_image_复制_plane().
 * @note On x86, the line大小s currently need 到 be aligned 到 the cacheline
 *       大小 (i.e. 64) 到 get improved performance.
 */
void av_image_copy_plane_uc_from(uint8_t       *dst, ptrdiff_t dst_linesize,
                                 const uint8_t *src, ptrdiff_t src_linesize,
                                 ptrdiff_t bytewidth, int height);

/**
 * 复制 image 中 src_data 到 dst_data.
 *
 * @param dst_data      destination image data 缓冲区 到 复制 to
 * @param dst_line大小s line大小s 用于 the image 中 dst_data
 * @param src_data      source image data 缓冲区 到 复制 from
 * @param src_line大小s line大小s 用于 the image 中 src_data
 * @param pix_fmt       the AV像素格式 的 the image
 * @param 宽度         宽度 的 the image 中 像素s
 * @param 高度        高度 的 the image 中 像素s
 */
void av_image_copy(uint8_t * const dst_data[4], const int dst_linesizes[4],
                   const uint8_t * const src_data[4], const int src_linesizes[4],
                   enum AVPixelFormat pix_fmt, int width, int height);

/**
 * Wrapper around av_image_复制() 到 workaround the limitation
 * that the conversion，来自 uint8_t * const * 到 const uint8_t * const *
 * is not performed automatically 中 C.
 * @参见 av_image_复制()
 */
static inline
void av_image_copy2(uint8_t * const dst_data[4], const int dst_linesizes[4],
                    uint8_t * const src_data[4], const int src_linesizes[4],
                    enum AVPixelFormat pix_fmt, int width, int height)
{
    av_image_copy(dst_data, dst_linesizes,
                  (const uint8_t * const *)src_data, src_linesizes,
                  pix_fmt, width, height);
}

/**
 * 复制 image data located 中 uncacheable (e.g. GPU mapped) 内存. Where
 * available, this function will use special functionality 用于 reading，来自 such
 * 内存, which may result 中 greatly improved performance compared 到 plain
 * av_image_复制().
 *
 * data 指针s 和 the line大小s must be aligned 到 the maximum required
 * by the CPU architecture.
 *
 * @note line大小 parameters have the type ptrd如果f_t here, while they are
 *       int 用于 av_image_复制().
 * @note On x86, the line大小s currently need 到 be aligned 到 the cacheline
 *       大小 (i.e. 64) 到 get improved performance.
 */
void av_image_copy_uc_from(uint8_t * const dst_data[4],       const ptrdiff_t dst_linesizes[4],
                           const uint8_t * const src_data[4], const ptrdiff_t src_linesizes[4],
                           enum AVPixelFormat pix_fmt, int width, int height);

/**
 * 设置up the data 指针s 和 line大小s based 上 the spec如果ied image
 * parameters 和 the provided 数组.
 *
 * fields 的 the given image are filled 中 by using the src
 * address which points 到 the image data 缓冲区. Depending 上 the
 * spec如果ied 像素格式, one 或 multiple image data 指针s and
 * line 大小s will be 设置.  如果 a planar 格式 is spec如果ied, several
 * 指针s will be 设置 pointing 到 the d如果ferent picture planes and
 * the line 大小s 的 the d如果ferent planes will be stored 中 the
 * lines_大小s 数组. Call，使用 src == NULL 到 get the required
 * 大小 用于 the src 缓冲区.
 *
 * To 分配 the 缓冲区 和 fill 中 the dst_data 和 dst_line大小 in
 * one call, use av_image_alloc().
 *
 * @param dst_data      data 指针s 到 be filled in
 * @param dst_line大小  line大小s 用于 the image 中 dst_data 到 be filled in
 * @param src           缓冲区 which will contain 或 包含 actual image data, can be NULL
 * @param pix_fmt       the 像素格式 的 the image
 * @param 宽度         the 宽度 的 the image 中 像素s
 * @param 高度        the 高度 的 the image 中 像素s
 * @param align         the 值 used 中 src 用于 line大小 alignment
 * @返回 the 大小 中 bytes required 用于 src, a negative error code
 * 中 case 的 failure
 */
int av_image_fill_arrays(uint8_t *dst_data[4], int dst_linesize[4],
                         const uint8_t *src,
                         enum AVPixelFormat pix_fmt, int width, int height, int align);

/**
 * 返回 the 大小 中 bytes 的 the amount 的 data required 到 store an
 * image，使用 the given parameters.
 *
 * @param pix_fmt  the 像素格式 的 the image
 * @param 宽度    the 宽度 的 the image 中 像素s
 * @param 高度   the 高度 的 the image 中 像素s
 * @param align    the assumed line大小 alignment
 * @返回 the 缓冲区 大小 中 bytes, a negative error code 中 case 的 failure
 */
int av_image_get_buffer_size(enum AVPixelFormat pix_fmt, int width, int height, int align);

/**
 * 复制 image data，来自 an image into a 缓冲区.
 *
 * av_image_get_缓冲区_大小() 可用于 到 compute the required 大小
 * 用于 the 缓冲区 到 fill.
 *
 * @param dst           a 缓冲区 into which picture data will be copied
 * @param dst_大小      the 大小 中 bytes 的 dst
 * @param src_data      指针s containing the source image data
 * @param src_line大小  line大小s 用于 the image 中 src_data
 * @param pix_fmt       the 像素格式 的 the source image
 * @param 宽度         the 宽度 的 the source image 中 像素s
 * @param 高度        the 高度 的 the source image 中 像素s
 * @param align         the assumed line大小 alignment 用于 dst
 * @返回 the 数量 的 bytes written 到 dst, 或 a negative 值
 * (error code) 上 error
 */
int av_image_copy_to_buffer(uint8_t *dst, int dst_size,
                            const uint8_t * const src_data[4], const int src_linesize[4],
                            enum AVPixelFormat pix_fmt, int width, int height, int align);

/**
 * 检查 如果 the given dimension 的 an image is valid, meaning that all
 * bytes 的 the image can be addressed，使用 a signed int.
 *
 * @param w the 宽度 的 the picture
 * @param h the 高度 的 the picture
 * @param log_off设置 the off设置 到 sum 到 the log level 用于 logging，使用 log_ctx
 * @param log_ctx the parent logging 上下文, it may be NULL
 * @返回 >= 0 如果 valid, a negative error code otherwise
 */
int av_image_check_size(unsigned int w, unsigned int h, int log_offset, void *log_ctx);

/**
 * 检查 如果 the given dimension 的 an image is valid, meaning that all
 * bytes 的 a plane 的 an image，使用 the spec如果ied pix_fmt can be addressed
 *，使用 a signed int.
 *
 * @param w the 宽度 的 the picture
 * @param h the 高度 的 the picture
 * @param max_像素s the maximum 数量 的 像素s the user wants 到 accept
 * @param pix_fmt the 像素格式, can be AV_PIX_FMT_NONE 如果 unknown.
 * @param log_off设置 the off设置 到 sum 到 the log level 用于 logging，使用 log_ctx
 * @param log_ctx the parent logging 上下文, it may be NULL
 * @返回 >= 0 如果 valid, a negative error code otherwise
 */
int av_image_check_size2(unsigned int w, unsigned int h, int64_t max_pixels, enum AVPixelFormat pix_fmt, int log_offset, void *log_ctx);

/**
 * 检查 如果 the given 采样 aspect ratio 的 an image is valid.
 *
 * It is considered invalid 如果 the denominator is 0 或 如果 applying the ratio
 * 到 the image 大小 would make the smaller dimension less than 1. 如果 the
 * sar numerator is 0, it is considered unknown 和 will 返回 as valid.
 *
 * @param w 宽度 的 the image
 * @param h 高度 的 the image
 * @param sar 采样 aspect ratio 的 the image
 * @返回 0 如果 valid, a negative AVERROR code otherwise
 */
int av_image_check_sar(unsigned int w, unsigned int h, AVRational sar);

/**
 * Overwrite the image data，使用 black. This is suitable 用于 filling a
 * sub-rectangle 的 an image, meaning the padding between the right most 像素
 * 和 the left most 像素 上 the next line will not be overwritten. For some
 * 格式s, the image 大小 might be rounded up due 到 inherent alignment.
 *
 * 如果 the 像素格式 has alpha, the alpha is cleared 到 opaque.
 *
 * This can 返回 an error 如果 the 像素格式 is not supported. Normally, all
 * non-hwaccel 像素格式s should be supported.
 *
 * Passing NULL 用于 dst_data is allowed. Then the function 返回s 是否 the
 * operation would have succeeded. (It can 返回 an error 如果 the pix_fmt is
 * not supported.)
 *
 * @param dst_data      data 指针s 到 destination image
 * @param dst_line大小  line大小s 用于 the destination image
 * @param pix_fmt       the 像素格式 的 the image
 * @param range         the color range 的 the image (important 用于 colorspaces such as YUV)
 * @param 宽度         the 宽度 的 the image 中 像素s
 * @param 高度        the 高度 的 the image 中 像素s
 * @返回 0 如果 the image data was cleared, a negative AVERROR code otherwise
 */
int av_image_fill_black(uint8_t * const dst_data[4], const ptrdiff_t dst_linesize[4],
                        enum AVPixelFormat pix_fmt, enum AVColorRange range,
                        int width, int height);

/**
 * Overwrite the image data，使用 a color. This is suitable 用于 filling a
 * sub-rectangle 的 an image, meaning the padding between the right most 像素
 * 和 the left most 像素 上 the next line will not be overwritten. For some
 * 格式s, the image 大小 might be rounded up due 到 inherent alignment.
 *
 * 如果 the 像素格式 has alpha, it is also replaced. Color component 值
 * are interpreted as native integers (or intfloats) regardless 的 actual 像素
 * 格式 endianness.
 *
 * This can 返回 an error 如果 the 像素格式 is not supported. Normally, all
 * non-hwaccel 像素格式s should be supported.
 *
 * Passing NULL 用于 dst_data is allowed. Then the function 返回s 是否 the
 * operation would have succeeded. (It can 返回 an error 如果 the pix_fmt is
 * not supported.)
 *
 * @param dst_data      data 指针s 到 destination image
 * @param dst_line大小  line大小s 用于 the destination image
 * @param pix_fmt       the 像素格式 的 the image
 * @param color         the color components 到 be 用于 the fill
 * @param 宽度         the 宽度 的 the image 中 像素s
 * @param 高度        the 高度 的 the image 中 像素s
 * @param 标志         currently unused
 * @返回 0 如果 the image data was filled, a negative AVERROR code otherwise
 */
int av_image_fill_color(uint8_t * const dst_data[4], const ptrdiff_t dst_linesize[4],
                        enum AVPixelFormat pix_fmt, const uint32_t color[4],
                        int width, int height, int flags);

/**
 * @}
 */


#endif /* AVUTIL_IMGUTILS_H */
