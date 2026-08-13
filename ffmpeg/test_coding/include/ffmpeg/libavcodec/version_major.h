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

#ifndef AVCODEC_VERSION_MAJOR_H
#define AVCODEC_VERSION_MAJOR_H

/**
 * @file
 * @ingroup libavc
 * Libavcodec 版本宏。
 */

#define LIBAVCODEC_VERSION_MAJOR  63

/**
 * 可在下方放置 FF_API_* 定义，用于标识将在未来版本升级时移除的公共 API。
 * 这些定义本身不属于公共 API，可能随时改变、失效或消失。
 *
 * @note 升级主版本时，建议在单独的提交中逐个手动禁用 FF_API_*，
 * 而不是随版本升级一次性全部禁用。这样更便于使用 git bisect 定位变更。
 */

#define FF_API_INIT_PACKET         (LIBAVCODEC_VERSION_MAJOR < 64)

#define FF_API_INTRA_DC_PRECISION  (LIBAVCODEC_VERSION_MAJOR < 64)
#define FF_API_MJPEG_EXTERN_HUFF   (LIBAVCODEC_VERSION_MAJOR < 64)

#endif /* AVCODEC_VERSION_MAJOR_H */
