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
 * @ingroup lavu_buffer
 * 引用计数数据缓冲区 API
 */

#ifndef AVUTIL_BUFFER_H
#define AVUTIL_BUFFER_H

#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup lavu_buffer AVBuffer
 * @ingroup lavu_data
 *
 * @{
 * AVBuffer 是引用计数数据缓冲区 API。
 *
 * 此 API 有两个核心对象：AVBuffer 和 AVBufferRef。AVBuffer 表示数据缓冲区本身，
 * 它是不透明的，调用者不应直接访问，只能通过 AVBufferRef 访问。不过调用者可
 * 比较两个 AVBuffer 指针，以检查两个不同引用是否描述同一数据缓冲区。
 * AVBufferRef 表示对 AVBuffer 的单个引用，是调用者可直接操作的对象。
 *
 * 有两个函数可创建带单个引用的新 AVBuffer：av_buffer_alloc() 直接分配新缓冲区，
 * av_buffer_create() 将现有数组封装为 AVBuffer。可使用 av_buffer_ref() 从现有
 * 引用创建更多引用。使用 av_buffer_unref() 释放引用（所有引用均释放后会自动
 * 释放数据）。
 *
 * 此 API 和 FFmpeg 其余部分约定：只有一个引用且未标为只读时，缓冲区才被视为
 * 可写。av_buffer_is_writable() 用于检查，av_buffer_make_writable() 会在需要时
 * 自动创建新的可写缓冲区。调用代码当然可以违反此约定，但只有在它控制所有
 * 现有引用时才安全。
 *
 * @note 引用和取消引用缓冲区是线程安全的，可从多个线程同时执行，无需额外加锁。
 *
 * @note 对同一缓冲区的两个不同引用可以指向缓冲区的不同部分（即它们的
 * AVBufferRef.data 可以不相等）。
 */

/**
 * 引用计数缓冲区类型。它是不透明的，应通过引用（AVBufferRef）使用。
 */
typedef struct AVBuffer AVBuffer;

/**
 * 对数据缓冲区的引用。
 *
 * 此结构的大小不属于公共 ABI，不应直接分配。
 */
typedef struct AVBufferRef {
    AVBuffer *buffer;

    /**
     * 数据缓冲区。当且仅当这是缓冲区的唯一引用时才视为可写，此时
     * av_buffer_is_writable() 返回 1。
     */
    uint8_t *data;
    /**
     * 数据的字节大小。
     */
    size_t   size;
} AVBufferRef;

/**
 * 使用 av_malloc() 分配给定大小的 AVBuffer。
 *
 * @return 给定大小的 AVBufferRef；内存不足时返回 NULL
 */
AVBufferRef *av_buffer_alloc(size_t size);

/**
 * 与 av_buffer_alloc() 相同，但返回的缓冲区会初始化为零。
 */
AVBufferRef *av_buffer_allocz(size_t size);

/**
 * 始终将缓冲区视为只读，即使它只有一个引用。
 */
#define AV_BUFFER_FLAG_READONLY (1 << 0)

/**
 * 从现有数组创建 AVBuffer。
 *
 * 成功时 data 归 AVBuffer 所有，调用者只能通过返回的 AVBufferRef 及其派生引用
 * 访问 data。失败时 data 保持不变。
 * @param data   数据数组
 * @param size   数据字节大小
 * @param free   释放此缓冲区数据的回调
 * @param opaque 供处理获取或传给 free 的参数
 * @param flags  AV_BUFFER_FLAG_* 的组合
 *
 * @return 成功时返回引用 data 的 AVBufferRef，失败时返回 NULL。
 */
AVBufferRef *av_buffer_create(uint8_t *data, size_t size,
                              void (*free)(void *opaque, uint8_t *data),
                              void *opaque, int flags);

/**
 * 默认释放回调，对缓冲区数据调用 av_free()。此函数用于传给
 * av_buffer_create()，不应直接调用。
 */
void av_buffer_default_free(void *opaque, uint8_t *data);

/**
 * 创建对 AVBuffer 的新引用。
 *
 * @return 引用与 buf 相同 AVBuffer 的新 AVBufferRef；失败时返回 NULL。
 */
AVBufferRef *av_buffer_ref(const AVBufferRef *buf);

/**
 * 释放给定引用；缓冲区没有其他引用时自动释放缓冲区。
 *
 * @param buf 要释放的引用。返回时将指针设为 NULL。
 */
void av_buffer_unref(AVBufferRef **buf);

/**
 * @return 调用者可写入 buf 引用的数据时返回 1（当且仅当 buf 是底层 AVBuffer
 * 的唯一引用时成立），否则返回 0。正值结果在对 buf 调用 av_buffer_ref() 前有效。
 */
int av_buffer_is_writable(const AVBufferRef *buf);

/**
 * @return av_buffer_create 设置的 opaque 参数。
 */
void *av_buffer_get_opaque(const AVBufferRef *buf);

int av_buffer_get_ref_count(const AVBufferRef *buf);

/**
 * 从给定缓冲区引用创建可写引用，并尽可能避免复制数据。
 *
 * @param buf 要变为可写的缓冲区引用。成功时 buf 保持不变，或先取消引用再写入
 *            新的可写 AVBufferRef；失败时保持不变。
 * @return 成功时返回 0，失败时返回负的 AVERROR。
 */
int av_buffer_make_writable(AVBufferRef **buf);

/**
 * 重新分配给定缓冲区。
 *
 * @param buf  要重新分配的缓冲区引用。成功时会取消 buf 的引用，并写入具有所需
 *             大小的新引用；失败时保持不变。*buf 可为 NULL，此时分配新缓冲区。
 * @param size 所需的新缓冲区大小。
 * @return 成功时返回 0，失败时返回负的 AVERROR。
 *
 * @note 仅当缓冲区最初通过 av_buffer_realloc(NULL) 分配且只有一个引用时，才会
 * 实际使用 av_realloc() 重新分配；其他情况都会分配新缓冲区并复制数据。
 */
int av_buffer_realloc(AVBufferRef **buf, size_t size);

/**
 * 确保 dst 引用与 src 相同的数据。
 *
 * *dst 已等同于 src 时不执行操作，否则取消 dst 的引用并替换为 src 的新引用。
 *
 * @param dst 指向有效缓冲区引用或 NULL。成功时指向等同于 src 的引用；失败时不变。
 * @param src 用于替换 dst 的缓冲区引用。可为 NULL，此时等同 av_buffer_unref(dst)。
 * @return 成功时返回 0；内存分配失败时返回 AVERROR(ENOMEM)。
 */
int av_buffer_replace(AVBufferRef **dst, const AVBufferRef *src);

/**
 * @}
 */

/**
 * @defgroup lavu_bufferpool AVBufferPool
 * @ingroup lavu_data
 *
 * @{
 * AVBufferPool 是无锁、线程安全的 AVBuffer 池 API。
 *
 * 频繁分配和释放大缓冲区可能很慢。AVBufferPool 用于调用者需要一组相同大小
 * 缓冲区的场景（最典型的是原始视频或音频帧缓冲区）。
 *
 * 首先调用 av_buffer_pool_init() 创建池；需要缓冲区时调用 av_buffer_pool_get()
 * 获取新引用。其行为与 av_buffer_alloc() 创建的引用相同，但最后一个引用取消时，
 * 缓冲区会返回池中而非释放，供后续调用复用。
 *
 * 调用者用完池且不再需要分配新缓冲区时，必须调用 av_buffer_pool_uninit() 将池
 * 标记为可释放。所有缓冲区都释放后，池会自动释放。
 *
 * 只要使用默认分配回调，或用户提供的回调本身线程安全，使用此 API 分配和释放
 * 缓冲区就是线程安全的。
 */

/**
 * 缓冲池。此结构是不透明的，不应直接访问。使用 av_buffer_pool_init() 分配，
 * 使用 av_buffer_pool_uninit() 释放。
 */
typedef struct AVBufferPool AVBufferPool;

/**
 * 分配并初始化缓冲池。
 *
 * @param size 池中每个缓冲区的大小
 * @param alloc 池为空时用于分配新缓冲区的函数。可以为 NULL，此时使用默认
 *              分配器（av_buffer_alloc()）。
 * @return 成功时返回新创建的缓冲池，出错时返回 NULL。
 */
AVBufferPool *av_buffer_pool_init(size_t size, AVBufferRef* (*alloc)(size_t size));

/**
 * 使用更复杂的分配器分配并初始化缓冲池。
 *
 * @param size 池中每个缓冲区的大小
 * @param opaque 分配器使用的任意用户数据
 * @param alloc 池为空时用于分配新缓冲区的函数。可以为 NULL，此时使用默认
 *              分配器（av_buffer_alloc()）。
 * @param pool_free 释放池之前立即调用的函数，即调用者调用
 *                  av_buffer_pool_uninit()，且所有帧都返回池并释放之后调用。
 *                  它用于反初始化用户 opaque 数据，可以为 NULL。
 * @return 成功时返回新创建的缓冲池，出错时返回 NULL。
 */
AVBufferPool *av_buffer_pool_init2(size_t size, void *opaque,
                                   AVBufferRef* (*alloc)(void *opaque, size_t size),
                                   void (*pool_free)(void *opaque));

/**
 * 将池标记为可释放。只有与池关联的所有已分配缓冲区都释放后，池才会真正释放。
 * 因此，即使某些已分配缓冲区仍在使用，也可以安全调用此函数。
 *
 * @param pool 指向要释放池的指针。它会被设为 NULL。
 */
void av_buffer_pool_uninit(AVBufferPool **pool);

/**
 * 分配新的 AVBuffer；池中有旧缓冲区可用时将其复用。可从多个线程同时调用。
 *
 * @return 成功时返回对新缓冲区的引用，出错时返回 NULL。
 */
AVBufferRef *av_buffer_pool_get(AVBufferPool *pool);

/**
 * 查询池中已分配缓冲区的原始 opaque 参数。
 *
 * @param ref 对 av_buffer_pool_get 返回缓冲区的缓冲区引用。
 * @return 缓冲池的缓冲区分配器函数所设置的 opaque 参数。
 *
 * @note ref 的 opaque 参数由缓冲池实现使用，因此必须通过此函数访问已分配
 * 缓冲区的原始 opaque 参数。
 */
void *av_buffer_pool_buffer_get_opaque(const AVBufferRef *ref);

/**
 * @}
 */

#endif /* AVUTIL_BUFFER_H */
