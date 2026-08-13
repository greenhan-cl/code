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

#ifndef AVCODEC_ADTS_PARSER_H
#define AVCODEC_ADTS_PARSER_H

#include <stddef.h>
#include <stdint.h>

#define AV_AAC_ADTS_HEADER_SIZE 7

/**
 * 从 AAC 数据中提取采样数和帧数。
 * @param[in]  buf     指向 AAC 数据缓冲区的指针
 * @param[out] samples 用于写入采样数的位置
 * @param[out] frames  用于写入帧数的位置
 * @return 成功返回 0，失败返回错误码。
 */
int av_adts_header_parse(const uint8_t *buf, uint32_t *samples,
                         uint8_t *frames);

#endif /* AVCODEC_ADTS_PARSER_H */
