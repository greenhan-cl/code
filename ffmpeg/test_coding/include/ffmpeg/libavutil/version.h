/*
 * copyright (c) 2003 Fabrice Bellard
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
 * @ingroup lavu
 * Libavutil 版本宏
 */

#ifndef AVUTIL_VERSION_H
#define AVUTIL_VERSION_H

#include "macros.h"

/**
 * @addtogroup version_utils
 *
 * 用于检查和匹配库版本，以维持向后兼容性。
 *
 * FFmpeg 库遵循与语义化版本非常相似的版本方案。区别在于 PATCH 在 FFmpeg 中称为
 * MICRO，重置为 100 而不是 0；也不会为 git master 中每个修复或变更增加 MICRO。
 *
 * FFmpeg 3.2 之前，补丁版本不更改库版本号，以免不同 git master 检出发生混淆。
 * 从 FFmpeg 3.2 开始，发布库版本使用 master 开发分支不使用的独立 MAJOR.MINOR。
 * 例如从 master 55.10.123 创建发布分支时，发布版升为 55.11.100，master 随后继续为
 * 55.12.100。每个新补丁版本再提升 MICRO，从而增强库版本号的实用性。
 *
 * @{
 */

#define AV_VERSION_INT(a, b, c) ((a)<<16 | (b)<<8 | (c))
#define AV_VERSION_DOT(a, b, c) a ##.## b ##.## c
#define AV_VERSION(a, b, c) AV_VERSION_DOT(a, b, c)

/**
 * 从 ::avformat_version()、::avcodec_version() 等返回的完整 ::AV_VERSION_INT
 * 整数中提取版本组成部分。
 */
#define AV_VERSION_MAJOR(a) ((a) >> 16)
#define AV_VERSION_MINOR(a) (((a) & 0x00FF00) >> 8)
#define AV_VERSION_MICRO(a) ((a) & 0xFF)

/**
 * @}
 */

/**
 * @defgroup lavu_ver 版本和构建诊断
 *
 * 用于在编译时和运行时检查所用 libavutil 版本的宏和函数。
 *
 * @{
 */

#define LIBAVUTIL_VERSION_MAJOR  61
#define LIBAVUTIL_VERSION_MINOR   5
#define LIBAVUTIL_VERSION_MICRO 100

#define LIBAVUTIL_VERSION_INT   AV_VERSION_INT(LIBAVUTIL_VERSION_MAJOR, \
                                               LIBAVUTIL_VERSION_MINOR, \
                                               LIBAVUTIL_VERSION_MICRO)
#define LIBAVUTIL_VERSION       AV_VERSION(LIBAVUTIL_VERSION_MAJOR,     \
                                           LIBAVUTIL_VERSION_MINOR,     \
                                           LIBAVUTIL_VERSION_MICRO)
#define LIBAVUTIL_BUILD         LIBAVUTIL_VERSION_INT

#define LIBAVUTIL_IDENT         "Lavu" AV_STRINGIFY(LIBAVUTIL_VERSION)

/**
 * @defgroup lavu_depr_guards 弃用保护
 * 可在下方放置 FF_API_* 定义，标识未来版本升级时将移除的公共 API。
 * 这些定义本身不属于公共 API，可能随时改变、失效或消失。
 *
 * @note 升级主版本时，建议在独立提交中逐个手动禁用 FF_API_*，
 * 而不是一次性全部禁用，以便使用 git bisect 定位变更。
 *
 * @{
 */

#define FF_API_CPU_FLAG_FORCE           (LIBAVUTIL_VERSION_MAJOR < 62)
#define FF_API_DOVI_L11_INVALID_PROPS   (LIBAVUTIL_VERSION_MAJOR < 62)
#define FF_API_ASSERT_FPU               (LIBAVUTIL_VERSION_MAJOR < 62)
#define FF_API_VULKAN_SYNC_QUEUES       (LIBAVUTIL_VERSION_MAJOR < 62)

/**
 * @}
 * @}
 */

#endif /* AVUTIL_VERSION_H */
