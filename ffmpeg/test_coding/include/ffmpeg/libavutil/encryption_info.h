/**
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

#ifndef AVUTIL_ENCRYPTION_INFO_H
#define AVUTIL_ENCRYPTION_INFO_H

#include <stddef.h>
#include <stdint.h>

typedef struct AVSubsampleEncryptionInfo {
    /** 明文字节数。 */
    unsigned int bytes_of_clear_data;

    /**
     * 受保护的字节数。使用模式加密时，该模式只应用于受保护字节；不使用模式
     * 加密时，所有这些字节都会被加密。
     */
    unsigned int bytes_of_protected_data;
} AVSubsampleEncryptionInfo;

/**
 * 描述数据包的加密信息，其中包含将数据包传给解码器前如何解密的帧特定信息。
 *
 * 此结构的大小不属于公共 ABI。
 */
typedef struct AVEncryptionInfo {
    /** fourcc 加密方案，采用大端字节序。 */
    uint32_t scheme;

    /**
     * 仅用于模式加密。这是被加密的 16 字节块数量。
     */
    uint32_t crypt_byte_block;

    /**
     * 仅用于模式加密。这是明文的 16 字节块数量。
     */
    uint32_t skip_byte_block;

    /**
     * 用于加密数据包的密钥 ID。它应始终为 16 字节长，但未来可能改变。
     */
    uint8_t *key_id;
    uint32_t key_id_size;

    /**
     * 初始化向量。它可能已经用零填充到正确的块大小。它应始终为 16 字节长，
     * 但未来可能改变。
     */
    uint8_t *iv;
    uint32_t iv_size;

    /**
     * 子样本加密信息数组，指定样本各部分如何加密。没有子样本时，整个样本都
     * 被加密。
     */
    AVSubsampleEncryptionInfo *subsamples;
    uint32_t subsample_count;
} AVEncryptionInfo;

/**
 * 描述用于初始化加密密钥系统的信息。
 *
 * 此结构的大小不属于公共 ABI。
 */
typedef struct AVEncryptionInitInfo {
    /**
     * 此信息所对应密钥系统的唯一标识符；未知时可以为 NULL。它应始终为 16
     * 字节，但未来可能改变。
     */
    uint8_t* system_id;
    uint32_t system_id_size;

    /**
     * 此初始化数据对应的密钥 ID 数组。所有 ID 长度相同。没有已知密钥 ID 时
     * 可以为 NULL。
     */
    uint8_t** key_ids;
    /** 密钥 ID 的数量。 */
    uint32_t num_key_ids;
    /**
     * 每个密钥 ID 的字节数。它应始终为 16，但未来可能改变。
     */
    uint32_t key_id_size;

    /**
     * 密钥系统特定的初始化数据。此数据直接从文件复制，格式取决于具体密钥系统。
     * 没有初始化数据时可以为 NULL；此时至少会有一个密钥 ID。
     */
    uint8_t* data;
    uint32_t data_size;

    /**
     * 指向列表中下一项初始化信息的可选指针。
     */
    struct AVEncryptionInitInfo *next;
} AVEncryptionInitInfo;

/**
 * 分配 AVEncryptionInfo 结构及其子指针，以容纳给定数量的子样本。这会为密钥
 * ID、IV 和子样本条目分配指针，设置大小成员，并将其余部分清零初始化。
 *
 * @param subsample_count 子样本数量。
 * @param key_id_size 密钥 ID 的字节数，应为 16。
 * @param iv_size IV 的字节数，应为 16。
 *
 * @return 新的 AVEncryptionInfo 结构；出错时返回 NULL。
 */
AVEncryptionInfo *av_encryption_info_alloc(uint32_t subsample_count, uint32_t key_id_size, uint32_t iv_size);

/**
 * 分配 AVEncryptionInfo 结构，其中包含给定数据的副本。
 * @return 新的 AVEncryptionInfo 结构；出错时返回 NULL。
 */
AVEncryptionInfo *av_encryption_info_clone(const AVEncryptionInfo *info);

/**
 * 释放给定的加密信息对象。绝不能用它释放侧数据的数据指针；后者应使用普通
 * 侧数据方法释放。
 */
void av_encryption_info_free(AVEncryptionInfo *info);

/**
 * 创建给定侧数据中所含 AVEncryptionInfo 的副本。使用完结果对象后，应将其
 * 传给 av_encryption_info_free()。
 *
 * @return 新的 AVEncryptionInfo 结构；出错时返回 NULL。
 */
AVEncryptionInfo *av_encryption_info_get_side_data(const uint8_t *side_data, size_t side_data_size);

/**
 * 分配并初始化保存给定加密信息副本的侧数据。结果指针应使用 av_free 释放，
 * 或传给 av_packet_add_side_data()。
 *
 * @return 新的侧数据指针，或 NULL。
 */
uint8_t *av_encryption_info_add_side_data(
      const AVEncryptionInfo *info, size_t *side_data_size);


/**
 * 分配 AVEncryptionInitInfo 结构及其子指针，以容纳给定大小。这会分配指针并
 * 设置所有字段。
 *
 * @return 新的 AVEncryptionInitInfo 结构；出错时返回 NULL。
 */
AVEncryptionInitInfo *av_encryption_init_info_alloc(
    uint32_t system_id_size, uint32_t num_key_ids, uint32_t key_id_size, uint32_t data_size);

/**
 * 释放给定的加密初始化信息对象。绝不能用它释放侧数据的数据指针；后者应使用
 * 普通侧数据方法释放。
 */
void av_encryption_init_info_free(AVEncryptionInitInfo* info);

/**
 * 创建给定侧数据中所含 AVEncryptionInitInfo 的副本。使用完结果对象后，应将其
 * 传给 av_encryption_init_info_free()。
 *
 * @return 新的 AVEncryptionInitInfo 结构；出错时返回 NULL。
 */
AVEncryptionInitInfo *av_encryption_init_info_get_side_data(
    const uint8_t* side_data, size_t side_data_size);

/**
 * 分配并初始化保存给定加密初始化信息副本的侧数据。结果指针应使用 av_free
 * 释放，或传给 av_packet_add_side_data()。
 *
 * @return 新的侧数据指针，或 NULL。
 */
uint8_t *av_encryption_init_info_add_side_data(
    const AVEncryptionInitInfo *info, size_t *side_data_size);

#endif /* AVUTIL_ENCRYPTION_INFO_H */
