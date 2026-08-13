/*
 * 复制right (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef AVUTIL_PIXFMT_H
#define AVUTIL_PIXFMT_H

/**
 * @file
 * 像素格式 definitions
 */

#include "libavutil/avconfig.h"
#include "version.h"

#define AVPALETTE_SIZE 1024
#define AVPALETTE_COUNT 256

/**
 * Maximum 数量 的 planes 中 any 像素格式.
 * This should be used 当 a maximum is needed, but code should not
 * be written 到 require a maximum 用于 no good reason.
 */
#define AV_VIDEO_MAX_PLANES 4

/**
 * 像素格式.
 *
 * @note
 * AV_PIX_FMT_RGB32 is handled 中 an endian-spec如果ic manner. RGBA
 * color is put together as:
 *  (<< 24) | (R << 16) | (G << 8) | B
 * This is stored as BGR上 little-endian CPU architectures 和 ARGB on
 * big-endian CPUs.
 *
 * @note
 * 如果 the resolution is not a multiple 的 the chroma subsampling factor
 * then the chroma plane resolution must be rounded up.
 *
 * @par
 * 当 the 像素格式 is palettized RGB32 (AV_PIX_FMT_PAL8), the palettized
 * image data is stored 中 AV帧.data[0]. palette is transported in
 * AV帧.data[1], is 1024 bytes long (256 4-byte entries) 和 is
 * 格式ted the same as 中 AV_PIX_FMT_RGB32 described above (i.e., it is
 * also endian-spec如果ic). Note also that the individual RGB32 palette
 * components stored 中 AV帧.data[1] should be 中 the range 0..255.
 * This is important as many custom PAL8 视频 codecs that were designed
 * 到 run 上 the IBM VGgraphics adapter use 6-bit palette components.
 *
 * @par
 * For all the 8 bits per 像素格式s, an RGB32 palette is 中 data[1] like
 * 用于 pal8. This palette is filled 中 automatically by the function
 * allocating the picture.
 */
enum AVPixelFormat {
    AV_PIX_FMT_NONE = -1,
    AV_PIX_FMT_YUV420P,   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    AV_PIX_FMT_YUYV422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    AV_PIX_FMT_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
    AV_PIX_FMT_BGR24,     ///< packed RGB 8:8:8, 24bpp, BGRBGR...
    AV_PIX_FMT_YUV422P,   ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    AV_PIX_FMT_YUV444P,   ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    AV_PIX_FMT_YUV410P,   ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
    AV_PIX_FMT_YUV411P,   ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
    AV_PIX_FMT_GRAY8,     ///<        Y        ,  8bpp
    AV_PIX_FMT_MONOWHITE, ///<        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
    AV_PIX_FMT_MONOBLACK, ///<        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
    AV_PIX_FMT_PAL8,      ///< 8 bits with AV_PIX_FMT_RGB32 palette
    AV_PIX_FMT_YUVJ420P,  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV420P and setting color_range
    AV_PIX_FMT_YUVJ422P,  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV422P and setting color_range
    AV_PIX_FMT_YUVJ444P,  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV444P and setting color_range
    AV_PIX_FMT_UYVY422,   ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
    AV_PIX_FMT_UYYVYY411, ///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
    AV_PIX_FMT_BGR8,      ///< packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
    AV_PIX_FMT_BGR4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    AV_PIX_FMT_BGR4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
    AV_PIX_FMT_RGB8,      ///< packed RGB 3:3:2,  8bpp, (msb)3R 3G 2B(lsb)
    AV_PIX_FMT_RGB4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    AV_PIX_FMT_RGB4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
    AV_PIX_FMT_NV12,      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    AV_PIX_FMT_NV21,      ///< as above, but U and V bytes are swapped

    AV_PIX_FMT_ARGB,      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    AV_PIX_FMT_RGBA,      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    AV_PIX_FMT_ABGR,      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    AV_PIX_FMT_BGRA,      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

    AV_PIX_FMT_GRAY16BE,  ///<        Y        , 16bpp, big-endian
    AV_PIX_FMT_GRAY16LE,  ///<        Y        , 16bpp, little-endian
    AV_PIX_FMT_YUV440P,   ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
    AV_PIX_FMT_YUVJ440P,  ///< planar YUV 4:4:0 full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV440P and setting color_range
    AV_PIX_FMT_YUVA420P,  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
    AV_PIX_FMT_RGB48BE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
    AV_PIX_FMT_RGB48LE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian

    AV_PIX_FMT_RGB565BE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
    AV_PIX_FMT_RGB565LE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
    AV_PIX_FMT_RGB555BE,  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), big-endian   , X=unused/undefined
    AV_PIX_FMT_RGB555LE,  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), little-endian, X=unused/undefined

    AV_PIX_FMT_BGR565BE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
    AV_PIX_FMT_BGR565LE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
    AV_PIX_FMT_BGR555BE,  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), big-endian   , X=unused/undefined
    AV_PIX_FMT_BGR555LE,  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), little-endian, X=unused/undefined

    /**
     *  硬件 acceleration through VA-API, data[3] 包含 a
     *  VASurfaceID.
     */
    AV_PIX_FMT_VAAPI,

    AV_PIX_FMT_YUV420P16LE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV420P16BE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV422P16LE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV422P16BE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P16LE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P16BE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_DXVA2_VLD,    ///< HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer

    AV_PIX_FMT_RGB444LE,  ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), little-endian, X=unused/undefined
    AV_PIX_FMT_RGB444BE,  ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), big-endian,    X=unused/undefined
    AV_PIX_FMT_BGR444LE,  ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), little-endian, X=unused/undefined
    AV_PIX_FMT_BGR444BE,  ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), big-endian,    X=unused/undefined
    AV_PIX_FMT_YA8,       ///< 8 bits gray, 8 bits alpha

    AV_PIX_FMT_Y400A = AV_PIX_FMT_YA8, ///< alias for AV_PIX_FMT_YA8
    AV_PIX_FMT_GRAY8A= AV_PIX_FMT_YA8, ///< alias for AV_PIX_FMT_YA8

    AV_PIX_FMT_BGR48BE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
    AV_PIX_FMT_BGR48LE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

    /**
     * following 12 格式s have the disadvantage 的 needing 1 格式 用于 each bit depth.
     * Notice that each 9/10 bits 采样 is stored 中 16 bits，使用 extra padding.
     * 如果 you want 到 support multiple bit depths, then using AV_PIX_FMT_YUV420P16*，使用 the bpp stored separately is better.
     */
    AV_PIX_FMT_YUV420P9BE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P9LE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV420P10BE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P10LE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV422P10BE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P10LE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P9BE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P9LE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P10BE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P10LE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV422P9BE, ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P9LE, ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_GBRP,      ///< planar GBR 4:4:4 24bpp
    AV_PIX_FMT_GBR24P = AV_PIX_FMT_GBRP, // alias for #AV_PIX_FMT_GBRP
    AV_PIX_FMT_GBRP9BE,   ///< planar GBR 4:4:4 27bpp, big-endian
    AV_PIX_FMT_GBRP9LE,   ///< planar GBR 4:4:4 27bpp, little-endian
    AV_PIX_FMT_GBRP10BE,  ///< planar GBR 4:4:4 30bpp, big-endian
    AV_PIX_FMT_GBRP10LE,  ///< planar GBR 4:4:4 30bpp, little-endian
    AV_PIX_FMT_GBRP16BE,  ///< planar GBR 4:4:4 48bpp, big-endian
    AV_PIX_FMT_GBRP16LE,  ///< planar GBR 4:4:4 48bpp, little-endian
    AV_PIX_FMT_YUVA422P,  ///< planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
    AV_PIX_FMT_YUVA444P,  ///< planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
    AV_PIX_FMT_YUVA420P9BE,  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian
    AV_PIX_FMT_YUVA420P9LE,  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian
    AV_PIX_FMT_YUVA422P9BE,  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian
    AV_PIX_FMT_YUVA422P9LE,  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian
    AV_PIX_FMT_YUVA444P9BE,  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
    AV_PIX_FMT_YUVA444P9LE,  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    AV_PIX_FMT_YUVA420P10BE, ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA420P10LE, ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA422P10BE, ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA422P10LE, ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA444P10BE, ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA444P10LE, ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA420P16BE, ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA420P16LE, ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA422P16BE, ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA422P16LE, ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA444P16BE, ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA444P16LE, ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)

    AV_PIX_FMT_VDPAU,     ///< HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface

    AV_PIX_FMT_XYZ12LE,      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0
    AV_PIX_FMT_XYZ12BE,      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0
    AV_PIX_FMT_NV16,         ///< interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    AV_PIX_FMT_NV20LE,       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_NV20BE,       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian

    AV_PIX_FMT_RGBA64BE,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    AV_PIX_FMT_RGBA64LE,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
    AV_PIX_FMT_BGRA64BE,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    AV_PIX_FMT_BGRA64LE,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian

    AV_PIX_FMT_YVYU422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb

    AV_PIX_FMT_YA16BE,       ///< 16 bits gray, 16 bits alpha (big-endian)
    AV_PIX_FMT_YA16LE,       ///< 16 bits gray, 16 bits alpha (little-endian)

    AV_PIX_FMT_GBRAP,        ///< planar GBRA 4:4:4:4 32bpp
    AV_PIX_FMT_GBRAP16BE,    ///< planar GBRA 4:4:4:4 64bpp, big-endian
    AV_PIX_FMT_GBRAP16LE,    ///< planar GBRA 4:4:4:4 64bpp, little-endian
    /**
     * HW acceleration through QSV, data[3] 包含 a 指针 到 the
     * mfx帧Surface1 结构体.
     *
     * Before FFmpeg 5.0:
     * mfx帧Surface1.Data.MemId 包含 a 指针 当 importing
     * the following 帧s as QSV 帧s:
     *
     * VAAPI:
     * mfx帧Surface1.Data.MemId 包含 a 指针 到 VASurfaceID
     *
     * DXVA2:
     * mfx帧Surface1.Data.MemId 包含 a 指针 到 IDirect3DSurface9
     *
     * FFmpeg 5.0 和 above:
     * mfx帧Surface1.Data.MemId 包含 a 指针 到 the mfxHDLPair
     * 结构体 当 importing the following 帧s as QSV 帧s:
     *
     * VAAPI:
     * mfxHDLPair.first 包含 a VASurfaceID 指针.
     * mfxHDLPair.second is always MFX_INFINITE.
     *
     * DXVA2:
     * mfxHDLPair.first 包含 IDirect3DSurface9 指针.
     * mfxHDLPair.second is always MFX_INFINITE.
     *
     * D3D11:
     * mfxHDLPair.first 包含 a ID3D11Texture2D 指针.
     * mfxHDLPair.second 包含 texture 数组 index 的 the 帧 如果 the
     * ID3D11Texture2D is an 数组 texture, 或 always MFX_INFINITE 如果 it is a
     * normal texture.
     */
    AV_PIX_FMT_QSV,
    /**
     * HW acceleration though MMAL, data[3] 包含 a 指针 到 the
     * MMAL_缓冲区_HEADER_T 结构体.
     */
    AV_PIX_FMT_MMAL,

    AV_PIX_FMT_D3D11VA_VLD,  ///< HW decoding through Direct3D11 via old API, Picture.data[3] contains a ID3D11VideoDecoderOutputView pointer

    /**
     * HW acceleration through CUDA. data[i] contain CU设备ptr 指针s
     * exactly as 用于 system 内存 帧s.
     */
    AV_PIX_FMT_CUDA,

    AV_PIX_FMT_0RGB,        ///< packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined
    AV_PIX_FMT_RGB0,        ///< packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined
    AV_PIX_FMT_0BGR,        ///< packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined
    AV_PIX_FMT_BGR0,        ///< packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined

    AV_PIX_FMT_YUV420P12BE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P12LE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV420P14BE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P14LE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV422P12BE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P12LE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV422P14BE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P14LE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P12BE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P12LE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P14BE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P14LE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_GBRP12BE,    ///< planar GBR 4:4:4 36bpp, big-endian
    AV_PIX_FMT_GBRP12LE,    ///< planar GBR 4:4:4 36bpp, little-endian
    AV_PIX_FMT_GBRP14BE,    ///< planar GBR 4:4:4 42bpp, big-endian
    AV_PIX_FMT_GBRP14LE,    ///< planar GBR 4:4:4 42bpp, little-endian
    AV_PIX_FMT_YUVJ411P,    ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV411P and setting color_range

    AV_PIX_FMT_BAYER_BGGR8,    ///< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples
    AV_PIX_FMT_BAYER_RGGB8,    ///< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples
    AV_PIX_FMT_BAYER_GBRG8,    ///< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples
    AV_PIX_FMT_BAYER_GRBG8,    ///< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples
    AV_PIX_FMT_BAYER_BGGR16LE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian
    AV_PIX_FMT_BAYER_BGGR16BE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian
    AV_PIX_FMT_BAYER_RGGB16LE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian
    AV_PIX_FMT_BAYER_RGGB16BE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian
    AV_PIX_FMT_BAYER_GBRG16LE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian
    AV_PIX_FMT_BAYER_GBRG16BE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian
    AV_PIX_FMT_BAYER_GRBG16LE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian
    AV_PIX_FMT_BAYER_GRBG16BE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian

    AV_PIX_FMT_YUV440P10LE, ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
    AV_PIX_FMT_YUV440P10BE, ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
    AV_PIX_FMT_YUV440P12LE, ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
    AV_PIX_FMT_YUV440P12BE, ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
    AV_PIX_FMT_AYUV64LE,    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    AV_PIX_FMT_AYUV64BE,    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), big-endian

    AV_PIX_FMT_VIDEOTOOLBOX, ///< hardware decoding through Videotoolbox

    AV_PIX_FMT_P010LE, ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, little-endian
    AV_PIX_FMT_P010BE, ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, big-endian

    AV_PIX_FMT_GBRAP12BE,  ///< planar GBR 4:4:4:4 48bpp, big-endian
    AV_PIX_FMT_GBRAP12LE,  ///< planar GBR 4:4:4:4 48bpp, little-endian

    AV_PIX_FMT_GBRAP10BE,  ///< planar GBR 4:4:4:4 40bpp, big-endian
    AV_PIX_FMT_GBRAP10LE,  ///< planar GBR 4:4:4:4 40bpp, little-endian

    AV_PIX_FMT_MEDIACODEC, ///< hardware decoding through MediaCodec

    AV_PIX_FMT_GRAY12BE,   ///<        Y        , 12bpp, big-endian
    AV_PIX_FMT_GRAY12LE,   ///<        Y        , 12bpp, little-endian
    AV_PIX_FMT_GRAY10BE,   ///<        Y        , 10bpp, big-endian
    AV_PIX_FMT_GRAY10LE,   ///<        Y        , 10bpp, little-endian

    AV_PIX_FMT_P016LE, ///< like NV12, with 16bpp per component, little-endian
    AV_PIX_FMT_P016BE, ///< like NV12, with 16bpp per component, big-endian

    /**
     * 硬件 surfaces 用于 Direct3D11.
     *
     * This is preferred over the legacy AV_PIX_FMT_D3D11VA_VLD. new D3D11
     * hwaccel API 和 filtering support AV_PIX_FMT_D3D11 only.
     *
     * data[0] 包含 a ID3D11Texture2D 指针, 和 data[1] 包含
     * texture 数组 index 的 the 帧 as intptr_t 如果 the ID3D11Texture2D is
     * an 数组 texture (or always 0 如果 it's a normal texture).
     */
    AV_PIX_FMT_D3D11,

    AV_PIX_FMT_GRAY9BE,   ///<        Y        , 9bpp, big-endian
    AV_PIX_FMT_GRAY9LE,   ///<        Y        , 9bpp, little-endian

    AV_PIX_FMT_GBRPF32BE,  ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, big-endian
    AV_PIX_FMT_GBRPF32LE,  ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, little-endian
    AV_PIX_FMT_GBRAPF32BE, ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, big-endian
    AV_PIX_FMT_GBRAPF32LE, ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, little-endian

    /**
     * DRM-managed 缓冲区 exposed through PRIME 缓冲区 sharing.
     *
     * data[0] points 到 an AVDRM帧描述符.
     */
    AV_PIX_FMT_DRM_PRIME,
    /**
     * 硬件 surfaces 用于 OpenCL.
     *
     * data[i] contain 2D image objects (typed 中 C as cl_mem, used
     * 中 OpenCL as image2d_t) 用于 each plane 的 the surface.
     */
    AV_PIX_FMT_OPENCL,

    AV_PIX_FMT_GRAY14BE,   ///<        Y        , 14bpp, big-endian
    AV_PIX_FMT_GRAY14LE,   ///<        Y        , 14bpp, little-endian

    AV_PIX_FMT_GRAYF32BE,  ///< IEEE-754 single precision Y, 32bpp, big-endian
    AV_PIX_FMT_GRAYF32LE,  ///< IEEE-754 single precision Y, 32bpp, little-endian

    AV_PIX_FMT_YUVA422P12BE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, big-endian
    AV_PIX_FMT_YUVA422P12LE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, little-endian
    AV_PIX_FMT_YUVA444P12BE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, big-endian
    AV_PIX_FMT_YUVA444P12LE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, little-endian

    AV_PIX_FMT_NV24,      ///< planar YUV 4:4:4, 24bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    AV_PIX_FMT_NV42,      ///< as above, but U and V bytes are swapped

    /**
     * Vulkan 硬件 images.
     *
     * data[0] points 到 an AVVk帧
     */
    AV_PIX_FMT_VULKAN,

    AV_PIX_FMT_Y210BE,    ///< packed YUV 4:2:2 like YUYV422, 20bpp, data in the high bits, big-endian
    AV_PIX_FMT_Y210LE,    ///< packed YUV 4:2:2 like YUYV422, 20bpp, data in the high bits, little-endian

    AV_PIX_FMT_X2RGB10LE, ///< packed RGB 10:10:10, 30bpp, (msb)2X 10R 10G 10B(lsb), little-endian, X=unused/undefined
    AV_PIX_FMT_X2RGB10BE, ///< packed RGB 10:10:10, 30bpp, (msb)2X 10R 10G 10B(lsb), big-endian, X=unused/undefined
    AV_PIX_FMT_X2BGR10LE, ///< packed BGR 10:10:10, 30bpp, (msb)2X 10B 10G 10R(lsb), little-endian, X=unused/undefined
    AV_PIX_FMT_X2BGR10BE, ///< packed BGR 10:10:10, 30bpp, (msb)2X 10B 10G 10R(lsb), big-endian, X=unused/undefined

    AV_PIX_FMT_P210BE,      ///< interleaved chroma YUV 4:2:2, 20bpp, data in the high bits, big-endian
    AV_PIX_FMT_P210LE,      ///< interleaved chroma YUV 4:2:2, 20bpp, data in the high bits, little-endian

    AV_PIX_FMT_P410BE,      ///< interleaved chroma YUV 4:4:4, 30bpp, data in the high bits, big-endian
    AV_PIX_FMT_P410LE,      ///< interleaved chroma YUV 4:4:4, 30bpp, data in the high bits, little-endian

    AV_PIX_FMT_P216BE,      ///< interleaved chroma YUV 4:2:2, 32bpp, big-endian
    AV_PIX_FMT_P216LE,      ///< interleaved chroma YUV 4:2:2, 32bpp, little-endian

    AV_PIX_FMT_P416BE,      ///< interleaved chroma YUV 4:4:4, 48bpp, big-endian
    AV_PIX_FMT_P416LE,      ///< interleaved chroma YUV 4:4:4, 48bpp, little-endian

    AV_PIX_FMT_VUYA,        ///< packed VUYA 4:4:4:4, 32bpp (1 Cr & Cb sample per 1x1 Y & A samples), VUYAVUYA...

    AV_PIX_FMT_RGBAF16BE,   ///< IEEE-754 half precision packed RGBA 16:16:16:16, 64bpp, RGBARGBA..., big-endian
    AV_PIX_FMT_RGBAF16LE,   ///< IEEE-754 half precision packed RGBA 16:16:16:16, 64bpp, RGBARGBA..., little-endian

    AV_PIX_FMT_VUYX,        ///< packed VUYX 4:4:4:4, 32bpp, Variant of VUYA where alpha channel is left undefined

    AV_PIX_FMT_P012LE,      ///< like NV12, with 12bpp per component, data in the high bits, zeros in the low bits, little-endian
    AV_PIX_FMT_P012BE,      ///< like NV12, with 12bpp per component, data in the high bits, zeros in the low bits, big-endian

    AV_PIX_FMT_Y212BE,      ///< packed YUV 4:2:2 like YUYV422, 24bpp, data in the high bits, zeros in the low bits, big-endian
    AV_PIX_FMT_Y212LE,      ///< packed YUV 4:2:2 like YUYV422, 24bpp, data in the high bits, zeros in the low bits, little-endian

    AV_PIX_FMT_XV30BE,      ///< packed XVYU 4:4:4, 32bpp, (msb)2X 10V 10Y 10U(lsb), big-endian, variant of Y410 where alpha channel is left undefined
    AV_PIX_FMT_XV30LE,      ///< packed XVYU 4:4:4, 32bpp, (msb)2X 10V 10Y 10U(lsb), little-endian, variant of Y410 where alpha channel is left undefined

    AV_PIX_FMT_XV36BE,      ///< packed XVYU 4:4:4, 48bpp, data in the high bits, zeros in the low bits, big-endian, variant of Y412 where alpha channel is left undefined
    AV_PIX_FMT_XV36LE,      ///< packed XVYU 4:4:4, 48bpp, data in the high bits, zeros in the low bits, little-endian, variant of Y412 where alpha channel is left undefined

    AV_PIX_FMT_RGBF32BE,    ///< IEEE-754 single precision packed RGB 32:32:32, 96bpp, RGBRGB..., big-endian
    AV_PIX_FMT_RGBF32LE,    ///< IEEE-754 single precision packed RGB 32:32:32, 96bpp, RGBRGB..., little-endian

    AV_PIX_FMT_RGBAF32BE,   ///< IEEE-754 single precision packed RGBA 32:32:32:32, 128bpp, RGBARGBA..., big-endian
    AV_PIX_FMT_RGBAF32LE,   ///< IEEE-754 single precision packed RGBA 32:32:32:32, 128bpp, RGBARGBA..., little-endian

    AV_PIX_FMT_P212BE,      ///< interleaved chroma YUV 4:2:2, 24bpp, data in the high bits, big-endian
    AV_PIX_FMT_P212LE,      ///< interleaved chroma YUV 4:2:2, 24bpp, data in the high bits, little-endian

    AV_PIX_FMT_P412BE,      ///< interleaved chroma YUV 4:4:4, 36bpp, data in the high bits, big-endian
    AV_PIX_FMT_P412LE,      ///< interleaved chroma YUV 4:4:4, 36bpp, data in the high bits, little-endian

    AV_PIX_FMT_GBRAP14BE,  ///< planar GBR 4:4:4:4 56bpp, big-endian
    AV_PIX_FMT_GBRAP14LE,  ///< planar GBR 4:4:4:4 56bpp, little-endian

    /**
     * 硬件 surfaces 用于 Direct3D 12.
     *
     * data[0] points 到 an AVD3D12VA帧
     */
    AV_PIX_FMT_D3D12,

    AV_PIX_FMT_AYUV,        ///< packed AYUV 4:4:4:4, 32bpp (1 Cr & Cb sample per 1x1 Y & A samples), AYUVAYUV...

    AV_PIX_FMT_UYVA,        ///< packed UYVA 4:4:4:4, 32bpp (1 Cr & Cb sample per 1x1 Y & A samples), UYVAUYVA...

    AV_PIX_FMT_VYU444,      ///< packed VYU 4:4:4, 24bpp (1 Cr & Cb sample per 1x1 Y), VYUVYU...

    AV_PIX_FMT_V30XBE,      ///< packed VYUX 4:4:4 like XV30, 32bpp, (msb)10V 10Y 10U 2X(lsb), big-endian
    AV_PIX_FMT_V30XLE,      ///< packed VYUX 4:4:4 like XV30, 32bpp, (msb)10V 10Y 10U 2X(lsb), little-endian

    AV_PIX_FMT_RGBF16BE,    ///< IEEE-754 half precision packed RGB 16:16:16, 48bpp, RGBRGB..., big-endian
    AV_PIX_FMT_RGBF16LE,    ///< IEEE-754 half precision packed RGB 16:16:16, 48bpp, RGBRGB..., little-endian

    AV_PIX_FMT_RGBA128BE,   ///< packed RGBA 32:32:32:32, 128bpp, RGBARGBA..., big-endian
    AV_PIX_FMT_RGBA128LE,   ///< packed RGBA 32:32:32:32, 128bpp, RGBARGBA..., little-endian

    AV_PIX_FMT_RGB96BE,     ///< packed RGBA 32:32:32, 96bpp, RGBRGB..., big-endian
    AV_PIX_FMT_RGB96LE,     ///< packed RGBA 32:32:32, 96bpp, RGBRGB..., little-endian

    AV_PIX_FMT_Y216BE,      ///< packed YUV 4:2:2 like YUYV422, 32bpp, big-endian
    AV_PIX_FMT_Y216LE,      ///< packed YUV 4:2:2 like YUYV422, 32bpp, little-endian

    AV_PIX_FMT_XV48BE,      ///< packed XVYU 4:4:4, 64bpp, big-endian, variant of Y416 where alpha channel is left undefined
    AV_PIX_FMT_XV48LE,      ///< packed XVYU 4:4:4, 64bpp, little-endian, variant of Y416 where alpha channel is left undefined

    AV_PIX_FMT_GBRPF16BE,  ///< IEEE-754 half precision planer GBR 4:4:4, 48bpp, big-endian
    AV_PIX_FMT_GBRPF16LE,  ///< IEEE-754 half precision planer GBR 4:4:4, 48bpp, little-endian
    AV_PIX_FMT_GBRAPF16BE, ///< IEEE-754 half precision planar GBRA 4:4:4:4, 64bpp, big-endian
    AV_PIX_FMT_GBRAPF16LE, ///< IEEE-754 half precision planar GBRA 4:4:4:4, 64bpp, little-endian

    AV_PIX_FMT_GRAYF16BE,  ///< IEEE-754 half precision Y, 16bpp, big-endian
    AV_PIX_FMT_GRAYF16LE,  ///< IEEE-754 half precision Y, 16bpp, little-endian

    /**
     * HW acceleration through AMF. data[0] contain AMFSurface 指针
     */
    AV_PIX_FMT_AMF_SURFACE,

    AV_PIX_FMT_GRAY32BE,    ///<         Y        , 32bpp, big-endian
    AV_PIX_FMT_GRAY32LE,    ///<         Y        , 32bpp, little-endian

    AV_PIX_FMT_YAF32BE,  ///< IEEE-754 single precision packed YA, 32 bits gray, 32 bits alpha, 64bpp, big-endian
    AV_PIX_FMT_YAF32LE,  ///< IEEE-754 single precision packed YA, 32 bits gray, 32 bits alpha, 64bpp, little-endian

    AV_PIX_FMT_YAF16BE,  ///< IEEE-754 half precision packed YA, 16 bits gray, 16 bits alpha, 32bpp, big-endian
    AV_PIX_FMT_YAF16LE,  ///< IEEE-754 half precision packed YA, 16 bits gray, 16 bits alpha, 32bpp, little-endian

    AV_PIX_FMT_GBRAP32BE,   ///< planar GBRA 4:4:4:4 128bpp, big-endian
    AV_PIX_FMT_GBRAP32LE,   ///< planar GBRA 4:4:4:4 128bpp, little-endian

    AV_PIX_FMT_YUV444P10MSBBE, ///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), lowest bits zero, big-endian
    AV_PIX_FMT_YUV444P10MSBLE, ///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), lowest bits zero, little-endian
    AV_PIX_FMT_YUV444P12MSBBE, ///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), lowest bits zero, big-endian
    AV_PIX_FMT_YUV444P12MSBLE, ///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), lowest bits zero, little-endian
    AV_PIX_FMT_GBRP10MSBBE,    ///< planar GBR 4:4:4 30bpp, lowest bits zero, big-endian
    AV_PIX_FMT_GBRP10MSBLE,    ///< planar GBR 4:4:4 30bpp, lowest bits zero, little-endian
    AV_PIX_FMT_GBRP12MSBBE,    ///< planar GBR 4:4:4 36bpp, lowest bits zero, big-endian
    AV_PIX_FMT_GBRP12MSBLE,    ///< planar GBR 4:4:4 36bpp, lowest bits zero, little-endian

    AV_PIX_FMT_OHCODEC, /// hardware decoding through openharmony

    /**
     * CUDblock-linear (opaque). data[0] is a CU数组 中 block-linear layout,
     * e.g.，来自 NVDEC opaque decode. Use 用于 zero-复制 到 NVENC (CUD数组 输入).
     */
    AV_PIX_FMT_CUARRAY,

    AV_PIX_FMT_NB         ///< number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions
};

#if AV_HAVE_BIGENDIAN
#   define AV_PIX_FMT_NE(be, le) AV_PIX_FMT_##be
#else
#   define AV_PIX_FMT_NE(be, le) AV_PIX_FMT_##le
#endif

#define AV_PIX_FMT_RGB32   AV_PIX_FMT_NE(ARGB, BGRA)
#define AV_PIX_FMT_RGB32_1 AV_PIX_FMT_NE(RGBA, ABGR)
#define AV_PIX_FMT_BGR32   AV_PIX_FMT_NE(ABGR, RGBA)
#define AV_PIX_FMT_BGR32_1 AV_PIX_FMT_NE(BGRA, ARGB)
#define AV_PIX_FMT_0RGB32  AV_PIX_FMT_NE(0RGB, BGR0)
#define AV_PIX_FMT_0BGR32  AV_PIX_FMT_NE(0BGR, RGB0)

#define AV_PIX_FMT_GRAY9  AV_PIX_FMT_NE(GRAY9BE,  GRAY9LE)
#define AV_PIX_FMT_GRAY10 AV_PIX_FMT_NE(GRAY10BE, GRAY10LE)
#define AV_PIX_FMT_GRAY12 AV_PIX_FMT_NE(GRAY12BE, GRAY12LE)
#define AV_PIX_FMT_GRAY14 AV_PIX_FMT_NE(GRAY14BE, GRAY14LE)
#define AV_PIX_FMT_GRAY16 AV_PIX_FMT_NE(GRAY16BE, GRAY16LE)
#define AV_PIX_FMT_GRAY32 AV_PIX_FMT_NE(GRAY32BE, GRAY32LE)
#define AV_PIX_FMT_YA16   AV_PIX_FMT_NE(YA16BE,   YA16LE)
#define AV_PIX_FMT_RGB48  AV_PIX_FMT_NE(RGB48BE,  RGB48LE)
#define AV_PIX_FMT_RGB565 AV_PIX_FMT_NE(RGB565BE, RGB565LE)
#define AV_PIX_FMT_RGB555 AV_PIX_FMT_NE(RGB555BE, RGB555LE)
#define AV_PIX_FMT_RGB444 AV_PIX_FMT_NE(RGB444BE, RGB444LE)
#define AV_PIX_FMT_RGBA64 AV_PIX_FMT_NE(RGBA64BE, RGBA64LE)
#define AV_PIX_FMT_BGR48  AV_PIX_FMT_NE(BGR48BE,  BGR48LE)
#define AV_PIX_FMT_BGR565 AV_PIX_FMT_NE(BGR565BE, BGR565LE)
#define AV_PIX_FMT_BGR555 AV_PIX_FMT_NE(BGR555BE, BGR555LE)
#define AV_PIX_FMT_BGR444 AV_PIX_FMT_NE(BGR444BE, BGR444LE)
#define AV_PIX_FMT_BGRA64 AV_PIX_FMT_NE(BGRA64BE, BGRA64LE)

#define AV_PIX_FMT_YUV420P9  AV_PIX_FMT_NE(YUV420P9BE , YUV420P9LE)
#define AV_PIX_FMT_YUV422P9  AV_PIX_FMT_NE(YUV422P9BE , YUV422P9LE)
#define AV_PIX_FMT_YUV444P9  AV_PIX_FMT_NE(YUV444P9BE , YUV444P9LE)
#define AV_PIX_FMT_YUV420P10 AV_PIX_FMT_NE(YUV420P10BE, YUV420P10LE)
#define AV_PIX_FMT_YUV422P10 AV_PIX_FMT_NE(YUV422P10BE, YUV422P10LE)
#define AV_PIX_FMT_YUV440P10 AV_PIX_FMT_NE(YUV440P10BE, YUV440P10LE)
#define AV_PIX_FMT_YUV444P10 AV_PIX_FMT_NE(YUV444P10BE, YUV444P10LE)
#define AV_PIX_FMT_YUV420P12 AV_PIX_FMT_NE(YUV420P12BE, YUV420P12LE)
#define AV_PIX_FMT_YUV422P12 AV_PIX_FMT_NE(YUV422P12BE, YUV422P12LE)
#define AV_PIX_FMT_YUV440P12 AV_PIX_FMT_NE(YUV440P12BE, YUV440P12LE)
#define AV_PIX_FMT_YUV444P12 AV_PIX_FMT_NE(YUV444P12BE, YUV444P12LE)
#define AV_PIX_FMT_YUV420P14 AV_PIX_FMT_NE(YUV420P14BE, YUV420P14LE)
#define AV_PIX_FMT_YUV422P14 AV_PIX_FMT_NE(YUV422P14BE, YUV422P14LE)
#define AV_PIX_FMT_YUV444P14 AV_PIX_FMT_NE(YUV444P14BE, YUV444P14LE)
#define AV_PIX_FMT_YUV420P16 AV_PIX_FMT_NE(YUV420P16BE, YUV420P16LE)
#define AV_PIX_FMT_YUV422P16 AV_PIX_FMT_NE(YUV422P16BE, YUV422P16LE)
#define AV_PIX_FMT_YUV444P16 AV_PIX_FMT_NE(YUV444P16BE, YUV444P16LE)

#define AV_PIX_FMT_YUV444P10MSB AV_PIX_FMT_NE(YUV444P10MSBBE, YUV444P10MSBLE)
#define AV_PIX_FMT_YUV444P12MSB AV_PIX_FMT_NE(YUV444P12MSBBE, YUV444P12MSBLE)

#define AV_PIX_FMT_GBRP9     AV_PIX_FMT_NE(GBRP9BE ,    GBRP9LE)
#define AV_PIX_FMT_GBRP10    AV_PIX_FMT_NE(GBRP10BE,    GBRP10LE)
#define AV_PIX_FMT_GBRP12    AV_PIX_FMT_NE(GBRP12BE,    GBRP12LE)
#define AV_PIX_FMT_GBRP14    AV_PIX_FMT_NE(GBRP14BE,    GBRP14LE)
#define AV_PIX_FMT_GBRP16    AV_PIX_FMT_NE(GBRP16BE,    GBRP16LE)
#define AV_PIX_FMT_GBRAP10   AV_PIX_FMT_NE(GBRAP10BE,   GBRAP10LE)
#define AV_PIX_FMT_GBRAP12   AV_PIX_FMT_NE(GBRAP12BE,   GBRAP12LE)
#define AV_PIX_FMT_GBRAP14   AV_PIX_FMT_NE(GBRAP14BE,   GBRAP14LE)
#define AV_PIX_FMT_GBRAP16   AV_PIX_FMT_NE(GBRAP16BE,   GBRAP16LE)
#define AV_PIX_FMT_GBRAP32   AV_PIX_FMT_NE(GBRAP32BE,   GBRAP32LE)

#define AV_PIX_FMT_GBRP10MSB AV_PIX_FMT_NE(GBRP10MSBBE, GBRP10MSBLE)
#define AV_PIX_FMT_GBRP12MSB AV_PIX_FMT_NE(GBRP12MSBBE, GBRP12MSBLE)

#define AV_PIX_FMT_BAYER_BGGR16 AV_PIX_FMT_NE(BAYER_BGGR16BE,    BAYER_BGGR16LE)
#define AV_PIX_FMT_BAYER_RGGB16 AV_PIX_FMT_NE(BAYER_RGGB16BE,    BAYER_RGGB16LE)
#define AV_PIX_FMT_BAYER_GBRG16 AV_PIX_FMT_NE(BAYER_GBRG16BE,    BAYER_GBRG16LE)
#define AV_PIX_FMT_BAYER_GRBG16 AV_PIX_FMT_NE(BAYER_GRBG16BE,    BAYER_GRBG16LE)

#define AV_PIX_FMT_GBRPF16    AV_PIX_FMT_NE(GBRPF16BE,  GBRPF16LE)
#define AV_PIX_FMT_GBRAPF16   AV_PIX_FMT_NE(GBRAPF16BE, GBRAPF16LE)
#define AV_PIX_FMT_GBRPF32    AV_PIX_FMT_NE(GBRPF32BE,  GBRPF32LE)
#define AV_PIX_FMT_GBRAPF32   AV_PIX_FMT_NE(GBRAPF32BE, GBRAPF32LE)

#define AV_PIX_FMT_GRAYF16    AV_PIX_FMT_NE(GRAYF16BE, GRAYF16LE)
#define AV_PIX_FMT_GRAYF32    AV_PIX_FMT_NE(GRAYF32BE, GRAYF32LE)

#define AV_PIX_FMT_YAF16      AV_PIX_FMT_NE(YAF16BE, YAF16LE)
#define AV_PIX_FMT_YAF32      AV_PIX_FMT_NE(YAF32BE, YAF32LE)

#define AV_PIX_FMT_YUVA420P9  AV_PIX_FMT_NE(YUVA420P9BE , YUVA420P9LE)
#define AV_PIX_FMT_YUVA422P9  AV_PIX_FMT_NE(YUVA422P9BE , YUVA422P9LE)
#define AV_PIX_FMT_YUVA444P9  AV_PIX_FMT_NE(YUVA444P9BE , YUVA444P9LE)
#define AV_PIX_FMT_YUVA420P10 AV_PIX_FMT_NE(YUVA420P10BE, YUVA420P10LE)
#define AV_PIX_FMT_YUVA422P10 AV_PIX_FMT_NE(YUVA422P10BE, YUVA422P10LE)
#define AV_PIX_FMT_YUVA444P10 AV_PIX_FMT_NE(YUVA444P10BE, YUVA444P10LE)
#define AV_PIX_FMT_YUVA422P12 AV_PIX_FMT_NE(YUVA422P12BE, YUVA422P12LE)
#define AV_PIX_FMT_YUVA444P12 AV_PIX_FMT_NE(YUVA444P12BE, YUVA444P12LE)
#define AV_PIX_FMT_YUVA420P16 AV_PIX_FMT_NE(YUVA420P16BE, YUVA420P16LE)
#define AV_PIX_FMT_YUVA422P16 AV_PIX_FMT_NE(YUVA422P16BE, YUVA422P16LE)
#define AV_PIX_FMT_YUVA444P16 AV_PIX_FMT_NE(YUVA444P16BE, YUVA444P16LE)

#define AV_PIX_FMT_XYZ12      AV_PIX_FMT_NE(XYZ12BE, XYZ12LE)
#define AV_PIX_FMT_NV20       AV_PIX_FMT_NE(NV20BE,  NV20LE)
#define AV_PIX_FMT_AYUV64     AV_PIX_FMT_NE(AYUV64BE, AYUV64LE)
#define AV_PIX_FMT_P010       AV_PIX_FMT_NE(P010BE,  P010LE)
#define AV_PIX_FMT_P012       AV_PIX_FMT_NE(P012BE,  P012LE)
#define AV_PIX_FMT_P016       AV_PIX_FMT_NE(P016BE,  P016LE)

#define AV_PIX_FMT_Y210       AV_PIX_FMT_NE(Y210BE,  Y210LE)
#define AV_PIX_FMT_Y212       AV_PIX_FMT_NE(Y212BE,  Y212LE)
#define AV_PIX_FMT_Y216       AV_PIX_FMT_NE(Y216BE,  Y216LE)
#define AV_PIX_FMT_XV30       AV_PIX_FMT_NE(XV30BE,  XV30LE)
#define AV_PIX_FMT_XV36       AV_PIX_FMT_NE(XV36BE,  XV36LE)
#define AV_PIX_FMT_XV48       AV_PIX_FMT_NE(XV48BE,  XV48LE)
#define AV_PIX_FMT_V30X       AV_PIX_FMT_NE(V30XBE,  V30XLE)
#define AV_PIX_FMT_X2RGB10    AV_PIX_FMT_NE(X2RGB10BE, X2RGB10LE)
#define AV_PIX_FMT_X2BGR10    AV_PIX_FMT_NE(X2BGR10BE, X2BGR10LE)

#define AV_PIX_FMT_P210       AV_PIX_FMT_NE(P210BE, P210LE)
#define AV_PIX_FMT_P410       AV_PIX_FMT_NE(P410BE, P410LE)
#define AV_PIX_FMT_P212       AV_PIX_FMT_NE(P212BE, P212LE)
#define AV_PIX_FMT_P412       AV_PIX_FMT_NE(P412BE, P412LE)
#define AV_PIX_FMT_P216       AV_PIX_FMT_NE(P216BE, P216LE)
#define AV_PIX_FMT_P416       AV_PIX_FMT_NE(P416BE, P416LE)

#define AV_PIX_FMT_RGBF16     AV_PIX_FMT_NE(RGBF16BE, RGBF16LE)
#define AV_PIX_FMT_RGBAF16    AV_PIX_FMT_NE(RGBAF16BE, RGBAF16LE)

#define AV_PIX_FMT_RGBF32     AV_PIX_FMT_NE(RGBF32BE, RGBF32LE)
#define AV_PIX_FMT_RGBAF32    AV_PIX_FMT_NE(RGBAF32BE, RGBAF32LE)

#define AV_PIX_FMT_RGB96      AV_PIX_FMT_NE(RGB96BE, RGB96LE)
#define AV_PIX_FMT_RGBA128    AV_PIX_FMT_NE(RGBA128BE, RGBA128LE)

/**
  * Chromaticity coordinates 的 the source primaries.
  * These 值 match the ones defined by ISO/IEC 23091-2_2019 subclause 8.1 和 ITU-T H.273.
  */
enum AVColorPrimaries {
    AVCOL_PRI_RESERVED0   = 0,
    AVCOL_PRI_BT709       = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP 177 Annex B
    AVCOL_PRI_UNSPECIFIED = 2,
    AVCOL_PRI_RESERVED    = 3,
    AVCOL_PRI_BT470M      = 4,  ///< also FCC Title 47 Code of Federal Regulations 73.682 (a)(20)

    AVCOL_PRI_BT470BG     = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
    AVCOL_PRI_SMPTE170M   = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    AVCOL_PRI_SMPTE240M   = 7,  ///< identical to above, also called "SMPTE C" even though it uses D65
    AVCOL_PRI_FILM        = 8,  ///< colour filters using Illuminant C
    AVCOL_PRI_BT2020      = 9,  ///< ITU-R BT2020
    AVCOL_PRI_SMPTE428    = 10, ///< SMPTE ST 428-1 (CIE 1931 XYZ)
    AVCOL_PRI_SMPTEST428_1 = AVCOL_PRI_SMPTE428,
    AVCOL_PRI_SMPTE431    = 11, ///< SMPTE ST 431-2 (2011) / DCI P3
    AVCOL_PRI_SMPTE432    = 12, ///< SMPTE ST 432-1 (2010) / P3 D65 / Display P3
    AVCOL_PRI_EBU3213     = 22, ///< EBU Tech. 3213-E (nothing there) / one of JEDEC P22 group phosphors
    AVCOL_PRI_JEDEC_P22   = AVCOL_PRI_EBU3213,
    AVCOL_PRI_NB,               ///< Not part of ABI

    /* following entries are not part 的 H.273, but custom extensions */
    AVCOL_PRI_EXT_BASE     = 256,
    AVCOL_PRI_V_GAMUT      = AVCOL_PRI_EXT_BASE,
    AVCOL_PRI_EXT_NB            ///< Not part of ABI
};

/**
 * Color Transfer Characteristic.
 * These 值 match the ones defined by ISO/IEC 23091-2_2019 subclause 8.2.
 */
enum AVColorTransferCharacteristic {
    AVCOL_TRC_RESERVED0    = 0,
    AVCOL_TRC_BT709        = 1,  ///< also ITU-R BT1361
    AVCOL_TRC_UNSPECIFIED  = 2,
    AVCOL_TRC_RESERVED     = 3,
    AVCOL_TRC_GAMMA22      = 4,  ///< also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
    AVCOL_TRC_GAMMA28      = 5,  ///< also ITU-R BT470BG
    AVCOL_TRC_SMPTE170M    = 6,  ///< also ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC
    AVCOL_TRC_SMPTE240M    = 7,
    AVCOL_TRC_LINEAR       = 8,  ///< "Linear transfer characteristics"
    AVCOL_TRC_LOG          = 9,  ///< "Logarithmic transfer characteristic (100:1 range)"
    AVCOL_TRC_LOG_SQRT     = 10, ///< "Logarithmic transfer characteristic (100 * Sqrt(10) : 1 range)"
    AVCOL_TRC_IEC61966_2_4 = 11, ///< IEC 61966-2-4
    AVCOL_TRC_BT1361_ECG   = 12, ///< ITU-R BT1361 Extended Colour Gamut
    AVCOL_TRC_IEC61966_2_1 = 13, ///< IEC 61966-2-1 (sRGB or sYCC)
    AVCOL_TRC_BT2020_10    = 14, ///< ITU-R BT2020 for 10-bit system
    AVCOL_TRC_BT2020_12    = 15, ///< ITU-R BT2020 for 12-bit system
    AVCOL_TRC_SMPTE2084    = 16, ///< SMPTE ST 2084 for 10-, 12-, 14- and 16-bit systems
    AVCOL_TRC_SMPTEST2084  = AVCOL_TRC_SMPTE2084,
    AVCOL_TRC_SMPTE428     = 17, ///< SMPTE ST 428-1
    AVCOL_TRC_SMPTEST428_1 = AVCOL_TRC_SMPTE428,
    AVCOL_TRC_ARIB_STD_B67 = 18, ///< ARIB STD-B67, known as "Hybrid log-gamma"
    AVCOL_TRC_NB,                ///< Not part of ABI

    /* following entries are not part 的 H.273, but custom extensions */
    AVCOL_TRC_EXT_BASE     = 256,
    AVCOL_TRC_V_LOG        = AVCOL_TRC_EXT_BASE,
    AVCOL_TRC_EXT_NB             ///< Not part of ABI
};

/**
 * YUV colorspace type.
 * These 值 match the ones defined by ISO/IEC 23091-2_2019 subclause 8.3.
 */
enum AVColorSpace {
    AVCOL_SPC_RGB         = 0,  ///< order of coefficients is actually GBR, also IEC 61966-2-1 (sRGB), YZX and ST 428-1
    AVCOL_SPC_BT709       = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / derived in SMPTE RP 177 Annex B
    AVCOL_SPC_UNSPECIFIED = 2,
    AVCOL_SPC_RESERVED    = 3,  ///< reserved for future use by ITU-T and ISO/IEC just like 15-255 are
    AVCOL_SPC_FCC         = 4,  ///< FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
    AVCOL_SPC_BT470BG     = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
    AVCOL_SPC_SMPTE170M   = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC / functionally identical to above
    AVCOL_SPC_SMPTE240M   = 7,  ///< derived from 170M primaries and D65 white point, 170M is derived from BT470 System M's primaries
    AVCOL_SPC_YCGCO       = 8,  ///< used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
    AVCOL_SPC_YCOCG       = AVCOL_SPC_YCGCO,
    AVCOL_SPC_BT2020_NCL  = 9,  ///< ITU-R BT2020 non-constant luminance system
    AVCOL_SPC_BT2020_CL   = 10, ///< ITU-R BT2020 constant luminance system
    AVCOL_SPC_SMPTE2085   = 11, ///< SMPTE 2085, Y'D'zD'x
    AVCOL_SPC_CHROMA_DERIVED_NCL = 12, ///< Chromaticity-derived non-constant luminance system
    AVCOL_SPC_CHROMA_DERIVED_CL = 13, ///< Chromaticity-derived constant luminance system
    AVCOL_SPC_ICTCP       = 14, ///< ITU-R BT.2100-0, ICtCp
    AVCOL_SPC_IPT_C2      = 15, ///< SMPTE ST 2128, IPT-C2
    AVCOL_SPC_YCGCO_RE    = 16, ///< YCgCo-R, even addition of bits
    AVCOL_SPC_YCGCO_RO    = 17, ///< YCgCo-R, odd addition of bits
    AVCOL_SPC_NB                ///< Not part of ABI
};

/**
 * Visual content 值 range.
 *
 * These 值 are based 上 definitions that can be found 中 multiple
 * spec如果ications, such as ITU-T BT.709 (3.4 - Quantization 的 RGB, luminance
 * 和 colour-d如果ference signals), ITU-T BT.2020 (Table 5 - Digital
 * Representation) as well as ITU-T BT.2100 (Table 9 - Digital 10- 和 12-bit
 * integer representation). At the time 的 writing, the BT.2100 one is
 * recommended, as it also defines the full range representation.
 *
 * Common definitions:
 *   - For RGB 和 luma planes such as Y 中 YCbCr 和 I 中 ICtCp,
 *     'E' is the original 值 中 range 的 0.0 到 1.0.
 *   - For chroma planes such as Cb,Cr 和 Ct,Cp, 'E' is the original
 *     值 中 range 的 -0.5 到 0.5.
 *   - 'n' is the 输出 bit depth.
 *   - For additional definitions such as rounding 和 clipping 到 valid n
 *     bit unsigned integer range, please refer 到 BT.2100 (Table 9).
 */
enum AVColorRange {
    AVCOL_RANGE_UNSPECIFIED = 0,

    /**
     * Narrow 或 limited range content.
     *
     * - For luma planes:
     *
     *       (219 * E + 16) * 2^(n-8)
     *
     *   F.ex. the range 的 16-235 用于 8 bits
     *
     * - For chroma planes:
     *
     *       (224 * E + 128) * 2^(n-8)
     *
     *   F.ex. the range 的 16-240 用于 8 bits
     */
    AVCOL_RANGE_MPEG        = 1,

    /**
     * Full range content.
     *
     * - For RGB 和 luma planes:
     *
     *       (2^n - 1) * E
     *
     *   F.ex. the range 的 0-255 用于 8 bits
     *
     * - For chroma planes:
     *
     *       (2^n - 1) * E + 2^(n - 1)
     *
     *   F.ex. the range 的 1-255 用于 8 bits
     */
    AVCOL_RANGE_JPEG        = 2,
    AVCOL_RANGE_NB               ///< Not part of ABI
};

/**
 * Location 的 chroma 采样s.
 *
 * Illustration showing the location 的 the first (top left) chroma 采样 的 the
 * image, the left shows only luma, the right
 * shows the location 的 the chroma 采样, the 2 could be imagined 到 overlay
 * each other but are drawn separately due 到 limitations 的 ASCII
 *
 *                1st 2nd       1st 2nd horizontal luma 采样 positions
 *                 v   v         v   v
 *                 ______        ______
 *1st luma line > |X   X ...    |3 4 X ...     X are luma 采样s,
 *                |             |1 2           1-6 are possible chroma positions
 *2nd luma line > |X   X ...    |5 6 X ...     0 is undefined/unknown position
 */
enum AVChromaLocation {
    AVCHROMA_LOC_UNSPECIFIED = 0,
    AVCHROMA_LOC_LEFT        = 1, ///< MPEG-2/4 4:2:0, H.264 default for 4:2:0
    AVCHROMA_LOC_CENTER      = 2, ///< MPEG-1 4:2:0, JPEG 4:2:0, H.263 4:2:0
    AVCHROMA_LOC_TOPLEFT     = 3, ///< ITU-R 601, SMPTE 274M 296M S314M(DV 4:1:1), mpeg2 4:2:2
    AVCHROMA_LOC_TOP         = 4,
    AVCHROMA_LOC_BOTTOMLEFT  = 5,
    AVCHROMA_LOC_BOTTOM      = 6,
    AVCHROMA_LOC_NB               ///< Not part of ABI
};

/**
 * Correlation between the alpha 声道 和 color 值.
 */
enum AVAlphaMode {
    AVALPHA_MODE_UNSPECIFIED   = 0, ///< Unknown alpha handling, or no alpha channel
    AVALPHA_MODE_PREMULTIPLIED = 1, ///< Alpha channel is multiplied into color values
    AVALPHA_MODE_STRAIGHT      = 2, ///< Alpha channel is independent of color values
    AVALPHA_MODE_NB                 ///< Not part of ABI
};

#endif /* AVUTIL_PIXFMT_H */
