/*
 * 复制right (C) 2013 Reimar Döffinger <Reimar.Doeffinger@gmx.de>
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
 * @ingroup lavu_hash_generic
 * Generic hashing API
 */

#ifndef AVUTIL_HASH_H
#define AVUTIL_HASH_H

#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup lavu_hash Hash Functions
 * @ingroup lavu_crypto
 * Hash functions useful 中 multimedia.
 *
 * Hash functions are widely used 中 multimedia,，来自 error 检查ing and
 * concealment 到 internal regression testing. libavutil has efficient
 * implementations 的 a variety 的 hash functions that may be useful for
 * FFmpeg 和 other multimedia applications.
 *
 * @{
 *
 * @defgroup lavu_hash_generic Generic Hashing API
 * abstraction layer 用于 all hash functions supported by libavutil.
 *
 * 如果 your application needs 到 support a wide range 的 d如果ferent hash
 * functions, then the Generic Hashing API is 用于 you. It provides a generic,
 * reusable API 用于 @ref lavu_hash "all hash functions" implemented 中 libavutil.
 * 如果 you just need 到 use one particular hash function, use the @ref lavu_hash
 * "individual hash" directly.
 *
 * @section 采样 Code
 *
 * basic template 用于 using the Generic Hashing API follows:
 *
 * @code
 * struct AVHash上下文 *ctx = NULL;
 * const char *hash_name = NULL;
 * uint8_t *输出_buf = NULL;
 *
 * // Select，来自 a string 返回ed by av_hash_names()
 * hash_name = ...;
 *
 * // 分配 a hash 上下文
 * ret = av_hash_alloc(&ctx, hash_name);
 * 如果 (ret < 0)
 *     返回 ret;
 *
 * // 初始化 the hash 上下文
 * av_hash_init(ctx);
 *
 * // Update the hash 上下文，使用 data
 * while (data_left) {
 *     av_hash_update(ctx, data, 大小);
 * }
 *
 * // Now we have no more data, so it is time 到 finalize the hash 和 get the
 * // 输出. But we need 到 first 分配 an 输出 缓冲区. Note that you can
 * // use any 内存 allocation function, including malloc(), not just
 * // av_malloc().
 * 输出_buf = av_malloc(av_hash_get_大小(ctx));
 * 如果 (!输出_buf)
 *     返回 AVERROR(ENOMEM);
 *
 * // Finalize the hash 上下文.
 * // You can use any 的 the av_hash_final*() functions provided, 用于 other
 * // 输出 格式s. 如果 you do so, be sure 到 adjust the 内存 allocation
 * // above. 参见 the function documentation below 用于 the exact amount 的 extra
 * // 内存 needed.
 * av_hash_final(ctx, 输出_缓冲区);
 *
 * // 释放 the 上下文
 * av_hash_释放p(&ctx);
 * @endcode
 *
 * @section Hash Function-Spec如果ic In格式ion
 * 如果 the CRC32 hash is selected, the #AV_CRC_32_IEEE polynomial will be
 * used.
 *
 * 如果 the Murmur3 hash is selected, the 默认 seed will be used. 参见 @ref
 * lavu_murmur3_seedinfo "Murmur3" 用于 more in格式ion.
 *
 * @{
 */

/**
 * @example ffhash.c
 * This example is a simple command line application that takes one 或 more
 * arguments. It demonstrates a typical use 的 the hashing API，使用 allocation,
 * initialization, updating, 和 finalizing.
 */

struct AVHashContext;

/**
 * 分配 a hash 上下文 用于 the algorithm spec如果ied by name.
 *
 * @返回  >= 0 用于 success, a negative error code 用于 failure
 *
 * @note 上下文 is not 初始化d after a call 到 this function; you must
 * call av_hash_init() 到 do so.
 */
int av_hash_alloc(struct AVHashContext **ctx, const char *name);

/**
 * 获取 the names 的 available hash algorithms.
 *
 * 此函数 可用于 到 enumerate the algorithms.
 *
 * @param[in] i  Index 的 the hash algorithm, starting，来自 0
 * @返回       指针 到 a static string 或 `NULL` 如果 `i` is out 的 range
 */
const char *av_hash_names(int i);

/**
 * 获取 the name 的 the algorithm corresponding 到 the given hash 上下文.
 */
const char *av_hash_get_name(const struct AVHashContext *ctx);

/**
 * Maximum 值 that av_hash_get_大小() will currently 返回.
 *
 * You can use this 如果 you absolutely want 或 need 到 use static allocation for
 * the 输出 缓冲区 和 are fine，使用 not supporting hashes newly added to
 * libavutil without recompilation.
 *
 * @warning
 * Adding new hashes，使用 larger 大小s, 和 increasing the macro while doing
 * so, will not be considered an ABI change. To prevent your code from
 * overflowing a 缓冲区, either dynamically 分配 the 输出 缓冲区 with
 * av_hash_get_大小(), 或 limit your use 的 the Hashing API 到 hashes that are
 * already 中 FFmpeg during the time 的 compilation.
 */
#define AV_HASH_MAX_SIZE 64

/**
 * 获取 the 大小 的 the resulting hash 值 中 bytes.
 *
 * maximum 值 this function will currently 返回 is available as macro
 * #AV_HASH_MAX_大小.
 *
 * @param[in]     ctx Hash 上下文
 * @返回            大小 的 the hash 值 中 bytes
 */
int av_hash_get_size(const struct AVHashContext *ctx);

/**
 * 初始化 或 re设置 a hash 上下文.
 *
 * @param[in,out] ctx Hash 上下文
 */
void av_hash_init(struct AVHashContext *ctx);

/**
 * Update a hash 上下文，使用 additional data.
 *
 * @param[in,out] ctx Hash 上下文
 * @param[in]     src Data 到 be added 到 the hash 上下文
 * @param[in]     len 大小 的 the additional data
 */
void av_hash_update(struct AVHashContext *ctx, const uint8_t *src, size_t len);

/**
 * Finalize a hash 上下文 和 compute the actual hash 值.
 *
 * minimum 大小 的 `dst` 缓冲区 is given by av_hash_get_大小() or
 * #AV_HASH_MAX_大小. use 的 the latter macro is discouraged.
 *
 * It is not safe 到 update 或 finalize a hash 上下文 again, 如果 it has already
 * been finalized.
 *
 * @param[in,out] ctx Hash 上下文
 * @param[out]    dst Where the final hash 值 will be stored
 *
 * @参见 av_hash_final_bin() provides an alternative API
 */
void av_hash_final(struct AVHashContext *ctx, uint8_t *dst);

/**
 * Finalize a hash 上下文 和 store the actual hash 值 中 a 缓冲区.
 *
 * It is not safe 到 update 或 finalize a hash 上下文 again, 如果 it has already
 * been finalized.
 *
 * 如果 `大小` is smaller than the hash 大小 (given by av_hash_get_大小()), the
 * hash is truncated; 如果 大小 is larger, the 缓冲区 is padded，使用 0.
 *
 * @param[in,out] ctx  Hash 上下文
 * @param[out]    dst  Where the final hash 值 will be stored
 * @param[in]     大小 数量 的 bytes 到 write 到 `dst`
 */
void av_hash_final_bin(struct AVHashContext *ctx, uint8_t *dst, int size);

/**
 * Finalize a hash 上下文 和 store the hexadecimal representation 的 the
 * actual hash 值 as a string.
 *
 * It is not safe 到 update 或 finalize a hash 上下文 again, 如果 it has already
 * been finalized.
 *
 * string is always 0-terminated.
 *
 * 如果 `大小` is smaller than `2 * hash_大小 + 1`, where `hash_大小` is the
 * 值 返回ed by av_hash_get_大小(), the string will be truncated.
 *
 * @param[in,out] ctx  Hash 上下文
 * @param[out]    dst  Where the string will be stored
 * @param[in]     大小 Maximum 数量 的 bytes 到 write 到 `dst`
 */
void av_hash_final_hex(struct AVHashContext *ctx, uint8_t *dst, int size);

/**
 * Finalize a hash 上下文 和 store the Base64 representation 的 the
 * actual hash 值 as a string.
 *
 * It is not safe 到 update 或 finalize a hash 上下文 again, 如果 it has already
 * been finalized.
 *
 * string is always 0-terminated.
 *
 * 如果 `大小` is smaller than AV_BASE64_大小(hash_大小), where `hash_大小` is
 * the 值 返回ed by av_hash_get_大小(), the string will be truncated.
 *
 * @param[in,out] ctx  Hash 上下文
 * @param[out]    dst  Where the final hash 值 will be stored
 * @param[in]     大小 Maximum 数量 的 bytes 到 write 到 `dst`
 */
void av_hash_final_b64(struct AVHashContext *ctx, uint8_t *dst, int size);

/**
 * 释放 hash 上下文 和 设置 hash 上下文 指针 到 `NULL`.
 *
 * @param[in,out] ctx  指针 到 hash 上下文
 */
void av_hash_freep(struct AVHashContext **ctx);

/**
 * @}
 * @}
 */

#endif /* AVUTIL_HASH_H */
