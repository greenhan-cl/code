/*
 * Copyright (C) 2023 Nuo Mi
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

#ifndef AVUTIL_EXECUTOR_H
#define AVUTIL_EXECUTOR_H

typedef struct AVExecutor AVExecutor;
typedef struct AVTask AVTask;

struct AVTask {
    AVTask *next;
};

typedef struct AVTaskCallbacks {
    void *user_data;

    int local_context_size;

    // 如果 a 的优先级高于 b，则返回 1
    int (*priority_higher)(const AVTask *a, const AVTask *b);

    // 任务已准备好运行
    int (*ready)(const AVTask *t, void *user_data);

    // 运行任务
    int (*run)(AVTask *t, void *local_context, void *user_data);
} AVTaskCallbacks;

/**
 * 分配执行器。
 * @param callbacks 执行器的回调结构
 * @param thread_count 工作线程数量；0 表示直接在调用者线程上运行
 * @return 返回执行器
 */
AVExecutor* av_executor_alloc(const AVTaskCallbacks *callbacks, int thread_count);

/**
 * 释放执行器。
 * @param e 指向执行器的指针
 */
void av_executor_free(AVExecutor **e);

/**
 * 向执行器添加任务。
 * @param e 指向执行器的指针
 * @param t 指向任务的指针。为 NULL 时会唤醒一个工作线程
 */
void av_executor_execute(AVExecutor *e, AVTask *t);

#endif //AVUTIL_EXECUTOR_H
