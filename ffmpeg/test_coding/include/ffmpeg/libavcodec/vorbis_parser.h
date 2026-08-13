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

/**
 * @file
 * Vorbis 解析的公共 API
 *
 * 确定每个数据包的时长。
 */

#ifndef AVCODEC_VORBIS_PARSER_H
#define AVCODEC_VORBIS_PARSER_H

#include <stdint.h>

typedef struct AVVorbisParseContext AVVorbisParseContext;

/**
 * 使用 extradata 中的头信息分配并初始化 Vorbis 解析器。
 */
AVVorbisParseContext *av_vorbis_parse_init(const uint8_t *extradata,
                                           int extradata_size);

/**
 * 释放解析器及其关联的所有内容。
 */
void av_vorbis_parse_free(AVVorbisParseContext **s);

#define VORBIS_FLAG_HEADER  0x00000001
#define VORBIS_FLAG_COMMENT 0x00000002
#define VORBIS_FLAG_SETUP   0x00000004

/**
 * 获取 Vorbis 数据包的时长。
 *
 * 如果 @p flags 为 @c NULL，则特殊帧会被视为无效。
 *
 * @param s        Vorbis 解析器上下文
 * @param buf      包含 Vorbis 帧的缓冲区
 * @param buf_size 缓冲区大小
 * @param flags    特殊帧标志
 */
int av_vorbis_parse_frame_flags(AVVorbisParseContext *s, const uint8_t *buf,
                                int buf_size, int *flags);

/**
 * 获取 Vorbis 数据包的时长。
 *
 * @param s        Vorbis 解析器上下文
 * @param buf      包含 Vorbis 帧的缓冲区
 * @param buf_size 缓冲区大小
 */
int av_vorbis_parse_frame(AVVorbisParseContext *s, const uint8_t *buf,
                          int buf_size);

void av_vorbis_parse_reset(AVVorbisParseContext *s);

#endif /* AVCODEC_VORBIS_PARSER_H */
