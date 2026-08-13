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

#ifndef AVUTIL_CONTAINER_FIFO_H
#define AVUTIL_CONTAINER_FIFO_H

#include <stddef.h>

/**
 * AVContainerFifo 是用于“容器”的 FIFO——容器是动态分配、可复用的结构（例如
 * AVFrame 或 AVPacket）。AVContainerFifo 使用此类容器的内部池，以避免重复
 * 分配和释放。
 */
typedef struct AVContainerFifo AVContainerFifo;

enum AVContainerFifoFlags {
    /**
     * 通知 av_container_fifo_write() 应创建对 src 中数据的新引用，而不是消耗
     * 其内容。
     *
     * @note 必须在自己的 fifo_transfer() 回调中手动处理此标志
     */
    AV_CONTAINER_FIFO_FLAG_REF  = (1 << 0),

    /**
     * flags 中此位及所有更高位可由调用者设置为任意值，并保证原样传给
     * fifo_transfer() 回调，而不会由 AVContainerFifo 代码解释。
     */
    AV_CONTAINER_FIFO_FLAG_USER = (1 << 16),
};

/**
 * 为所提供回调定义的容器类型分配新的 AVContainerFifo。
 *
 * @param opaque 将传给此函数所提供回调的用户数据
 * @param container_alloc 分配新的容器实例并返回其指针；失败时返回 NULL
 * @param container_reset 将所提供的容器实例重置为干净状态
 * @param container_free 释放所提供的容器实例
 * @param fifo_transfer 将容器 src 的内容传输到 dst。
 * @param flags 当前未使用
 *
 * @return 新分配的 AVContainerFifo；失败时返回 NULL
 */
AVContainerFifo*
av_container_fifo_alloc(void *opaque,
                        void* (*container_alloc)(void *opaque),
                        void  (*container_reset)(void *opaque, void *obj),
                        void  (*container_free) (void *opaque, void *obj),
                        int   (*fifo_transfer)  (void *opaque, void *dst, void *src, unsigned flags),
                        unsigned flags);

/**
 * 为 AVFrame 分配 AVContainerFifo 实例。
 *
 * @param flags 当前未使用
 */
AVContainerFifo *av_container_fifo_alloc_avframe(unsigned flags);

/**
 * 释放 AVContainerFifo 及其中的所有内容。
 */
void av_container_fifo_free(AVContainerFifo **cf);

/**
 * 将 obj 的内容写入 FIFO。
 *
 * 会调用之前提供给 av_container_fifo_alloc() 的 fifo_transfer() 回调，并将 obj
 * 作为 src，以执行实际传输。
 */
int av_container_fifo_write(AVContainerFifo *cf, void *obj, unsigned flags);

/**
 * 从 FIFO 中读取下一个可用对象到 obj。
 *
 * 会调用之前提供给 av_container_fifo_alloc() 的 fifo_read() 回调，并将 obj
 * 作为 dst，以执行实际传输。
 */
int av_container_fifo_read(AVContainerFifo *cf, void *obj, unsigned flags);

/**
 * 访问 FIFO 中存储的对象但不取出。不会调用 fifo_transfer() 回调，也不会修改
 * FIFO 状态。
 *
 * @param pobj 成功时在此写入指向 FIFO 中所存对象的指针。对象仍由 FIFO 所有，
 *             仅在 FIFO 未被修改期间，调用者才可访问它。
 * @param offset 要获取对象的位置——0 表示下一个将读取的项，1 表示再下一个，
 *               依此类推。必须小于 av_container_fifo_can_read()。
 *
 * @retval 0 成功，已将指针写入 pobj
 * @retval AVERROR(EINVAL) offset 值无效
 */
int av_container_fifo_peek(AVContainerFifo *cf, void **pobj, size_t offset);

/**
 * 从 FIFO 丢弃指定数量的元素。
 *
 * @param nb_elems 要丢弃的元素数，绝不能大于 av_fifo_can_read(f)
 */
void av_container_fifo_drain(AVContainerFifo *cf, size_t nb_elems);

/**
 * @return 可供读取的对象数量
 */
size_t av_container_fifo_can_read(const AVContainerFifo *cf);

#endif // AVCODEC_CONTAINER_FIFO_H
