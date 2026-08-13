/*
 * EXIF 元数据解析器
 * Copyright (c) 2013 Thilo Borgmann <thilo.borgmann _at_ mail.de>
 * Copyright (c) 2024-2025 Leo Izen <leo.izen@gmail.com>
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
 * EXIF metadata parser
 * @author Thilo Borgmann <thilo.borgmann _at_ mail.de>
 * @author Leo Izen <leo.izen@gmail.com>
 */

#ifndef AVCODEC_EXIF_H
#define AVCODEC_EXIF_H

#include <stddef.h>
#include <stdint.h>

#include "libavutil/buffer.h"
#include "libavutil/dict.h"
#include "libavutil/rational.h"

/** TIFF 标签的数据类型标识符 */
enum AVTiffDataType {
    AV_TIFF_BYTE = 1,
    AV_TIFF_STRING,
    AV_TIFF_SHORT,
    AV_TIFF_LONG,
    AV_TIFF_RATIONAL,
    AV_TIFF_SBYTE,
    AV_TIFF_UNDEFINED,
    AV_TIFF_SSHORT,
    AV_TIFF_SLONG,
    AV_TIFF_SRATIONAL,
    AV_TIFF_FLOAT,
    AV_TIFF_DOUBLE,
    AV_TIFF_IFD,
};

enum AVExifHeaderMode {
    /**
     * TIFF 头以 0x49492a00 或 0x4d4d002a 开始。
     * FFmpeg 内部使用此模式。
     */
    AV_EXIF_TIFF_HEADER,
    /** 跳过 TIFF 头，假定为小端序 */
    AV_EXIF_ASSUME_LE,
    /** 跳过 TIFF 头，假定为大端序 */
    AV_EXIF_ASSUME_BE,
    /** 前四个字节指向实际起点，随后按 AV_EXIF_TIFF_HEADER 处理 */
    AV_EXIF_T_OFF,
    /** 前六个字节包含 "Exif\0\0"，随后按 AV_EXIF_TIFF_HEADER 处理 */
    AV_EXIF_EXIF00,
};

typedef struct AVExifEntry AVExifEntry;

typedef struct AVExifMetadata {
    /* EXIF 元数据条目数组 */
    AVExifEntry *entries;
    /* 此数组中的条目数 */
    unsigned int count;
    /* 缓冲区大小，供 av_fast_realloc 使用 */
    unsigned int size;
} AVExifMetadata;

struct AVExifEntry {
    uint16_t id;
    enum AVTiffDataType type;
    uint32_t count;

    /*
     * 这些字段用于 IFD 风格的 MakerNote 条目。此类条目位于固定偏移之后，
     * 而不是位于条目开头。ifd_lead 字段包含通常用于标识 MakerNote 类型的前导字节。
     */
    uint32_t ifd_offset;
    uint8_t *ifd_lead;

    /*
     * 包含 count 个条目的数组。
     * 如果是 IFD，则它不是数组，并且 count = 1。
     */
    union {
        void *ptr;
        int64_t *sint;
        uint64_t *uint;
        double *dbl;
        char *str;
        uint8_t *ubytes;
        int8_t *sbytes;
        AVRational *rat;
        AVExifMetadata ifd;
    } value;
};

/**
 * 获取与给定标签 ID 关联的标签名称。
 * 标签 ID 未知时返回 NULL。
 *
 * 例如，av_exif_get_tag_name(0x112) 返回 "Orientation"。
 */
const char *av_exif_get_tag_name(uint16_t id);

/**
 * 获取与给定标签字符串名称关联的标签 ID。
 * 标签名称未知时返回负数，否则返回值始终可由 uint16_t 表示。
 *
 * 例如，av_exif_get_tag_id("Orientation") 返回 274（0x0112）。
 */
int32_t av_exif_get_tag_id(const char *name);

/**
  * 向给定 EXIF 元数据结构体添加条目。如果已存在具有给定 ID 的条目，
  * 则使用提供的其他信息更新现有条目；否则分配新条目。
  *
  * 此函数使用 av_fast_realloc 重新分配 ifd->entries，使用 av_malloc 为条目的
  * value 成员分配新空间，然后将 value 的内容复制到该缓冲区。
 */
int av_exif_set_entry(void *logctx, AVExifMetadata *ifd, uint16_t id, enum AVTiffDataType type,
                      uint32_t count, const uint8_t *ifd_lead, uint32_t ifd_offset, const void *value);

/**
 * 同时检查子目录。
 */
#define AV_EXIF_FLAG_RECURSIVE (1 << 0)

/**
 * 从 EXIF 元数据结构体获取具有指定标签 ID 的条目。条目指针将写入 *value。
 *
 * 如果条目存在并成功返回，则返回正数。
 *
 * 该正数等于找到条目的偏移加 1。如果条目位于顶层，则返回值范围为 1 到 IFD
 * 条目数（含端点）。如果条目位于子 IFD，则返回值大于顶层 IFD 的条目数。
 *
 * 未找到条目时，*value 保持不变并返回 0。
 * 发生错误时返回负的 AVERROR。
 */
int av_exif_get_entry(void *logctx, AVExifMetadata *ifd, uint16_t id, int flags, AVExifEntry **value);

/**
 * 从给定 EXIF 元数据结构体中移除条目。
 *
 * 如果条目存在并成功移除，则返回正数。
 *
 * 该正数等于找到条目的偏移加 1。如果条目位于顶层，则返回值范围为 1 到 IFD
 * 条目数（含端点）。如果条目位于子 IFD，则返回值大于顶层 IFD 的条目数。
 * 此值对应移除条目前的 IFD 状态。例如移除最后一个条目时返回 1，
 * 但此后 IFD 已不再包含条目。
 *
 * 未找到条目时返回 0，且 IFD 保持不变。
 * 发生错误时返回负的 AVERROR。
 */
int av_exif_remove_entry(void *logctx, AVExifMetadata *ifd, uint16_t id, int flags);

/**
 * 解码缓冲区中提供的 EXIF 数据并写入结构体 *ifd。函数成功时，IFD 归调用方所有，
 * 使用后必须调用 av_exif_free() 清理；函数失败并返回负值时，
 * 会在返回前调用 av_exif_free(ifd)。
 */
int av_exif_parse_buffer(void *logctx, const uint8_t *data, size_t size,
                         AVExifMetadata *ifd, enum AVExifHeaderMode header_mode);

/**
 * 使用 av_malloc 分配适当大小的缓冲区，并将 ifd 表示的 EXIF 数据写入该缓冲区。
 *
 * 发生错误时，*buffer 将为 NULL。成功时缓冲区归调用方所有。
 * 调用前 *buffer 参数必须为 NULL。
 */
int av_exif_write(void *logctx, const AVExifMetadata *ifd, AVBufferRef **buffer, enum AVExifHeaderMode header_mode);

/**
 * 释放与给定 EXIF 元数据结构体关联的所有资源。
 * 为兼容结构体在栈上分配的情况，不释放传入指针本身。
 * 如果传入指针在堆上分配，则必须由调用方释放。允许传入 NULL。
 */
void av_exif_free(AVExifMetadata *ifd);

/**
 * 递归读取 IFD 中的所有标签，并将其存储到给定元数据字典中。
 */
int av_exif_ifd_to_dict(void *logctx, const AVExifMetadata *ifd, AVDictionary **metadata);

/**
 * 分配给定 EXIF 元数据结构体的副本。副本归调用方所有，必须使用
 * av_exif_free 释放。复制失败时返回 NULL。
 */
AVExifMetadata *av_exif_clone_ifd(const AVExifMetadata *ifd);

/**
 * 将 AV_FRAME_DATA_DISPLAYMATRIX 使用的显示矩阵转换为 EXIF 方向标签使用的方向常量。
 *
 * 根据旋转和翻转因子返回 1 到 8（含端点）之间的 EXIF 方向。
 * 如果矩阵是奇异矩阵，则返回 0。
 */
int av_exif_matrix_to_orientation(const int32_t *matrix);

/**
 * 将 EXIF 方向标签使用的方向常量转换为 AV_FRAME_DATA_DISPLAYMATRIX 使用的显示矩阵。
 *
 * 成功返回 0；方向无效（即不在 1 到 8 的闭区间内）时返回负数。
 */
int av_exif_orientation_to_matrix(int32_t *matrix, int orientation);

#endif /* AVCODEC_EXIF_H */
