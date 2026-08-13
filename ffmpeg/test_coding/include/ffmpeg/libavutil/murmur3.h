/*
 * Copyright (C) 2013 Reimar Döffinger <Reimar.Doeffinger@gmx.de>
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
 * @ingroup lavu_murmur3
 * MurmurHash3 哈希函数实现的公共头文件。
 */

#ifndef AVUTIL_MURMUR3_H
#define AVUTIL_MURMUR3_H

#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup lavu_murmur3 Murmur3
 * @ingroup lavu_hash
 * MurmurHash3 哈希函数实现。
 *
 * MurmurHash3 是一种非加密哈希函数，其发明者 Austin Appleby 创建了三个
 * 互不兼容的版本：
 *
 * - 32 位输出
 * - 面向 32 位平台的 128 位输出
 * - 面向 64 位平台的 128 位输出
 *
 * FFmpeg 仅实现最后一种：为 64 位平台设计的 128 位输出。虽然该哈希函数是为
 * 64 位平台设计的，但实际上也能在 32 位系统上工作，只是性能会降低。
 *
 * @anchor lavu_murmur3_seedinfo
 * 按照设计，MurmurHash3 需要种子才能运行。因此 libavutil 提供两个哈希初始化
 * 函数：一个要求传入种子（av_murmur3_init_seeded()），另一个使用固定的任意
 * 整数作为种子，因此无需传入种子（av_murmur3_init()）。
 *
 * 为使哈希值可比较，如果由你提供种子，则调用此哈希函数时应始终使用相同种子。
 *
 * @{
 */

/**
 * 分配 AVMurMur3 哈希上下文。
 *
 * @return 未初始化的哈希上下文；出错时返回 `NULL`
 */
struct AVMurMur3 *av_murmur3_alloc(void);

/**
 * 使用种子初始化或重新初始化 AVMurMur3 哈希上下文。
 *
 * @param[out] c    哈希上下文
 * @param[in]  seed 随机种子
 *
 * @see av_murmur3_init()
 * @see 关于 MurmurHash3 种子的讨论，请参见 @ref lavu_murmur3_seedinfo "详细说明"。
 */
void av_murmur3_init_seeded(struct AVMurMur3 *c, uint64_t seed);

/**
 * 初始化或重新初始化 AVMurMur3 哈希上下文。
 *
 * 等价于使用内置种子调用 av_murmur3_init_seeded()。
 *
 * @param[out] c    哈希上下文
 *
 * @see av_murmur3_init_seeded()
 * @see 关于 MurmurHash3 种子的讨论，请参见 @ref lavu_murmur3_seedinfo "详细说明"。
 */
void av_murmur3_init(struct AVMurMur3 *c);

/**
 * 使用新数据更新哈希上下文。
 *
 * @param[out] c    哈希上下文
 * @param[in]  src  用于更新哈希的输入数据
 * @param[in]  len  从 `src` 读取的字节数
 */
void av_murmur3_update(struct AVMurMur3 *c, const uint8_t *src, size_t len);

/**
 * 完成哈希并输出摘要值。
 *
 * @param[in,out] c    哈希上下文
 * @param[out]    dst  存储输出摘要值的缓冲区
 */
void av_murmur3_final(struct AVMurMur3 *c, uint8_t dst[16]);

/**
 * @}
 */

#endif /* AVUTIL_MURMUR3_H */
