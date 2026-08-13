/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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
 * @ingroup lavu_crc32
 * CRC 哈希函数实现的公共头文件。
 */

#ifndef AVUTIL_CRC_H
#define AVUTIL_CRC_H

#include <stdint.h>
#include <stddef.h>
#include "attributes.h"

/**
 * @defgroup lavu_crc32 CRC
 * @ingroup lavu_hash
 * CRC（循环冗余校验）哈希函数实现。
 *
 * 除最常用的 CRC-32-IEEE 外，此模块还支持多种 CRC 多项式。可用多项式列表
 * 参见 @ref AVCRCId。
 *
 * @{
 */

typedef uint32_t AVCRC;

typedef enum {
    AV_CRC_8_ATM,
    AV_CRC_16_ANSI,
    AV_CRC_16_CCITT,
    AV_CRC_32_IEEE,
    AV_CRC_32_IEEE_LE,  /*< AV_CRC_32_IEEE 的位序反转版本 */
    AV_CRC_16_ANSI_LE,  /*< AV_CRC_16_ANSI 的位序反转版本 */
    AV_CRC_24_IEEE,
    AV_CRC_8_EBU,
    AV_CRC_MAX,         /*< 不属于公共 API！不要在 libavutil 之外使用。 */
}AVCRCId;

/**
 * 初始化 CRC 表。
 * @param ctx 必须是大小为 sizeof(AVCRC)*257 或 sizeof(AVCRC)*1024 的数组
 * @param le 为 1 时，最低位表示对应多项式最高次幂的系数（对 poly 和实际 CRC
 *           都如此）。为 0 时，如果需要标准表示，则必须交换 CRC 参数和 av_crc
 *           的结果（大多数情况下可简化为例如 bswap16）：
 *           av_bswap32(crc << (32-bits))
 * @param bits CRC 的位数
 * @param poly 不包含 x**bits 系数的生成多项式，采用 le 指定的表示方式
 * @param ctx_size ctx 的字节大小
 * @return 失败时返回 <0
 */
int av_crc_init(AVCRC *ctx, int le, int bits, uint32_t poly, int ctx_size);

/**
 * 获取已初始化的标准 CRC 表。
 * @param crc_id 标准 CRC 的 ID
 * @return 指向 CRC 表的指针；失败时返回 NULL
 */
const AVCRC *av_crc_get_table(AVCRCId crc_id);

/**
 * 计算数据块的 CRC。
 * @param ctx 已初始化的 AVCRC 数组（参见 av_crc_init()）
 * @param crc 前面数据块的 CRC（如果有），或 CRC 初始值
 * @param buffer 要计算 CRC 的缓冲区
 * @param length 缓冲区长度
 * @return 使用给定数据块更新后的 CRC
 *
 * @see av_crc_init() 的 "le" 参数
 */
uint32_t av_crc(const AVCRC *ctx, uint32_t crc,
                const uint8_t *buffer, size_t length) av_pure;

/**
 * @}
 */

#endif /* AVUTIL_CRC_H */
