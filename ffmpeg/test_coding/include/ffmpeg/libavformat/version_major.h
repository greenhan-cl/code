/*
 * 版本宏。
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

#ifndef AVFORMAT_VERSION_MAJOR_H
#define AVFORMAT_VERSION_MAJOR_H

/**
 * @file
 * @ingroup libavf Libavformat 版本宏
 */

// 主要碰撞可能会影响 Ticket5467、5421、5451（与 Chromium 的兼容性）
// 另外，请在此处添加您认为可能受到影响的任何票号
#define LIBAVFORMAT_VERSION_MAJOR  63

/**
 * FF_API_* 定义可以放置在下面，以指示将在未来版本升级时删除的公共 API。定义本身不属于公共 API 的一部分，可能随时更改、中断或消失。
 *
 * @note，在升级主要版本时，建议在其自己的提交中手动禁用每个 FF_API_*，而不是通过升级一次禁用它们。这提高了变更的 git 二分能力。
 */
#define FF_API_COMPUTE_PKT_FIELDS2      (LIBAVFORMAT_VERSION_MAJOR < 64)

#define FF_API_FDEBUG_TS                (LIBAVFORMAT_VERSION_MAJOR < 64)

#define FF_API_LCEVC_STRUCT             (LIBAVFORMAT_VERSION_MAJOR < 64)

#define FF_API_OLD_ID3V2_COMMENT        (LIBAVFORMAT_VERSION_MAJOR < 64)

#define FF_API_R_FRAME_RATE            1

#endif /* AVFORMAT_VERSION_MAJOR_H */
