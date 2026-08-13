/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FFmpeg; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVUTIL_THREADMESSAGE_H
#define AVUTIL_THREADMESSAGE_H

typedef struct AVThreadMessageQueue AVThreadMessageQueue;

typedef enum AVThreadMessageFlags {

    /**
     * 执行非阻塞操作。设置后，send 和 recv 为非阻塞操作；无法继续时立即返回 AVERROR(EAGAIN)。
     */
    AV_THREAD_MESSAGE_NONBLOCK = 1,

} AVThreadMessageFlags;

/**
 * 分配新消息队列。
 *
 * @param mq      指向消息队列的指针
 * @param nelem   队列中的最大元素数
 * @param elsize  每个队列元素的大小
 * @return 成功返回 >=0；出错返回 <0；lavu 构建时未启用线程支持时返回 AVERROR(ENOSYS)
 */
int av_thread_message_queue_alloc(AVThreadMessageQueue **mq,
                                  unsigned nelem,
                                  unsigned elsize);

/**
 * 释放消息队列。
 *
 * 消息队列必须不再被其他线程使用。
 */
void av_thread_message_queue_free(AVThreadMessageQueue **mq);

/**
 * 向队列发送消息。
 */
int av_thread_message_queue_send(AVThreadMessageQueue *mq,
                                 void *msg,
                                 unsigned flags);

/**
 * 从队列接收消息。
 */
int av_thread_message_queue_recv(AVThreadMessageQueue *mq,
                                 void *msg,
                                 unsigned flags);

/**
 * 设置发送错误码。
 *
 * 错误码非零时，av_thread_message_queue_send() 会立即返回该值。
 * AVERROR_EOF、AVERROR(EAGAIN) 等常规值可使发送线程停止或暂停操作。
 */
void av_thread_message_queue_set_err_send(AVThreadMessageQueue *mq,
                                          int err);

/**
 * 设置接收错误码。
 *
 * 错误码非零且不再有可用消息时，av_thread_message_queue_recv() 会立即返回该值。
 * AVERROR_EOF、AVERROR(EAGAIN) 等常规值可使接收线程停止或暂停操作。
 */
void av_thread_message_queue_set_err_recv(AVThreadMessageQueue *mq,
                                          int err);

/**
 * 设置可选的消息释放回调；操作从队列移除消息时调用。
 */
void av_thread_message_queue_set_free_func(AVThreadMessageQueue *mq,
                                           void (*free_func)(void *msg));

/**
 * 返回队列中的当前消息数。
 *
 * @return 当前消息数；lavu 构建时未启用线程支持则返回 AVERROR(ENOSYS)
 */
int av_thread_message_queue_nb_elems(AVThreadMessageQueue *mq);

/**
 * 冲刷消息队列。
 *
 * 此函数基本等同于读取并释放每条消息，但会在单个操作中完成，读取之间不加锁/解锁。
 */
void av_thread_message_flush(AVThreadMessageQueue *mq);

#endif /* AVUTIL_THREADMESSAGE_H */
