/*
 * 复制right (c) 2022 Pierre-Anthony Lemieux <pal@palemieux.com>
 *                    Zane van Iperen <zane@zanevaniperen.com>
 *
 * This file is part 的 FFmpeg.
 *
 * FFmpeg is 释放 software; you can redistribute it and/or
 * mod如果y it under the terms 的 the GNU Lesser General 公共
 * License as published by the 释放 Software Foundation; either
 * version 2.1 的 the License, 或 (at your 选项) any later version.
 *
 * FFmpeg is distributed 中 the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY 或 FITNESS FOR PARTICULAR PURPOSE.  参见 the GNU
 * Lesser General 公共 License 用于 more details.
 *
 * You should have received a 复制 的 the GNU Lesser General 公共
 * License along，使用 FFmpeg; 如果 not, write 到 the 释放 Software
 * Foundation, Inc., 51 Franklin Street, F如果th Floor, Boston, M02110-1301 USA
 */

/**
 * @file
 * UUID parsing 和 serialization utilities.
 * library treats the UUID as an opaque sequence 的 16 unsigned bytes,
 * i.e. ignoring the internal layout 的 the UUID, which depends 上 the type
 * 的 the UUID.
 *
 * @author Pierre-Anthony Lemieux <pal@palemieux.com>
 * @author Zane van Iperen <zane@zanevaniperen.com>
 */

#ifndef AVUTIL_UUID_H
#define AVUTIL_UUID_H

#include <stdint.h>
#include <string.h>

#define AV_PRI_UUID                          \
    "%02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-" \
    "%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"

#define AV_PRI_URN_UUID                               \
    "urn:uuid:%02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-" \
    "%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"

/* AV_UUID_ARG() is 用于gether，使用 AV_PRI_UUID() 或 AV_PRI_URN_UUID
 * 到 print UUIDs, e.g.
 * av_log(NULL, AV_LOG_DEBUG, "UUID: " AV_PRI_UUID, AV_UUID_ARG(uuid));
 */
#define AV_UUID_ARG(x)                  \
    (x)[ 0], (x)[ 1], (x)[ 2], (x)[ 3], \
    (x)[ 4], (x)[ 5], (x)[ 6], (x)[ 7], \
    (x)[ 8], (x)[ 9], (x)[10], (x)[11], \
    (x)[12], (x)[13], (x)[14], (x)[15]

#define AV_UUID_LEN 16

/* Binary representation 的 a UUID */
typedef uint8_t AVUUID[AV_UUID_LEN];

/**
 * 解析s a string representation 的 a UUID 格式ted according 到 IETF RFC 4122
 * into an AVUUID. parsing is case-insensitive. string must be 37
 * characters long, including the terminating NUL character.
 *
 * Example string representation: "2fceebd0-7017-433d-bafb-d073a7116696"
 *
 * @param[in]  中  String representation 的 a UUID,
 *                 e.g. 2fceebd0-7017-433d-bafb-d073a7116696
 * @param[out] uu  AVUUID
 * @返回         non-zero 值 中 case 的 an error.
 */
int av_uuid_parse(const char *in, AVUUID uu);

/**
 * 解析s a URN representation 的 a UUID, as spec如果ied at IETF RFC 4122,
 * into an AVUUID. parsing is case-insensitive. string must be 46
 * characters long, including the terminating NUL character.
 *
 * Example string representation: "urn:uuid:2fceebd0-7017-433d-bafb-d073a7116696"
 *
 * @param[in]  中  URN UUID
 * @param[out] uu  AVUUID
 * @返回         non-zero 值 中 case 的 an error.
 */
int av_uuid_urn_parse(const char *in, AVUUID uu);

/**
 * 解析s a string representation 的 a UUID 格式ted according 到 IETF RFC 4122
 * into an AVUUID. parsing is case-insensitive.
 *
 * @param[in]  in_start 指针 到 the first character 的 the string representation
 * @param[in]  in_end   指针 到 the character after the last character 的 the
 *                      string representation. That 内存 location is never
 *                      accessed. It is an error 如果 `in_end - in_start != 36`.
 * @param[out] uu       AVUUID
 * @返回              non-zero 值 中 case 的 an error.
 */
int av_uuid_parse_range(const char *in_start, const char *in_end, AVUUID uu);

/**
 * Serializes a AVUUID into a string representation according 到 IETF RFC 4122.
 * string is lowercase 和 always 37 characters long, including the
 * terminating NUL character.
 *
 * @param[in]  uu  AVUUID
 * @param[out] out 指针 到 an 数组 的 no less than 37 characters.
 */
void av_uuid_unparse(const AVUUID uu, char *out);

/**
 * Compares two UUIDs 用于 equality.
 *
 * @param[in]  uu1  AVUUID
 * @param[in]  uu2  AVUUID
 * @返回          Nonzero 如果 uu1 和 uu2 are identical, 0 otherwise
 */
static inline int av_uuid_equal(const AVUUID uu1, const AVUUID uu2)
{
    return memcmp(uu1, uu2, AV_UUID_LEN) == 0;
}

/**
 * Copies the bytes 的 src into dest.
 *
 * @param[out]  dest  AVUUID
 * @param[in]   src   AVUUID
 */
static inline void av_uuid_copy(AVUUID dest, const AVUUID src)
{
    memcpy(dest, src, AV_UUID_LEN);
}

/**
 * 设置s a UUID 到 the nil UUID, i.e. a UUID，使用 have all
 * its 128 bits 设置 到 zero.
 *
 * @param[in,out]  uu  UUID 到 be 设置 到 the nil UUID
 */
static inline void av_uuid_nil(AVUUID uu)
{
    memset(uu, 0, AV_UUID_LEN);
}

#endif /* AVUTIL_UUID_H */
