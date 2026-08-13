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

#ifndef AVUTIL_FILE_H
#define AVUTIL_FILE_H

#include <stddef.h>
#include <stdint.h>

#include "attributes.h"

/**
 * @file
 * 杂项文件工具。
 */

/**
 * 读取名为 filename 的文件，并将内容放入新分配的缓冲区；可用时则使用 mmap()
 * 映射。成功时，将 *bufptr 设为读取或映射的缓冲区，将 *size 设为 *bufptr 中
 * 缓冲区的字节大小。与 mmap 不同，此函数对零大小文件也会成功；此时
 * *bufptr 设为 NULL，*size 设为 0。返回的缓冲区必须使用 av_file_unmap() 释放。
 *
 * @param filename 文件路径
 * @param[out] bufptr 将其指向的对象设为映射或分配的缓冲区
 * @param[out] size 将其指向的对象设为缓冲区的字节大小
 * @param log_offset 用于日志记录的日志级别偏移量
 * @param log_ctx 用于日志记录的上下文
 * @return 成功时返回非负数，失败时返回与 AVERROR 错误码对应的负值
 */
av_warn_unused_result
int av_file_map(const char *filename, uint8_t **bufptr, size_t *size,
                int log_offset, void *log_ctx);

/**
 * 取消映射或释放 av_file_map() 创建的缓冲区 bufptr。
 *
 * @param bufptr 之前使用 av_file_map() 创建的缓冲区
 * @param size bufptr 的字节大小，必须与 av_file_map() 返回的值相同
 */
void av_file_unmap(uint8_t *bufptr, size_t size);

#endif /* AVUTIL_FILE_H */
