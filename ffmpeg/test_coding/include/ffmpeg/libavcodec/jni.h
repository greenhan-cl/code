/*
 * JNI public API functions
 *
 * Copyright (c) 2015-2016 Matthieu Bouron <matthieu.bouron stupeflix.com>
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

#ifndef AVCODEC_JNI_H
#define AVCODEC_JNI_H

/*
 * 手动设置用于获取 JNI 环境的 Java 虚拟机。Java VM 一旦设置便不能更改，
 * 因此可以使用同一个 Java VM 指针多次调用 av_jni_set_java_vm，
 * 但尝试设置不同的 Java VM 时会返回错误。
 *
 * @param vm Java 虚拟机
 * @param log_ctx 用于日志记录的上下文，可以为 NULL
 * @return 成功返回 0，否则返回 < 0
 */
int av_jni_set_java_vm(void *vm, void *log_ctx);

/*
 * 获取通过 av_jni_set_java_vm 设置的 Java 虚拟机。
 *
 * @param vm Java 虚拟机
 * @return 指向 Java 虚拟机的指针
 */
void *av_jni_get_java_vm(void *log_ctx);

/*
 * 设置 Android 应用上下文，该上下文用于获取处理内容 URI 的 Android 内容解析器。
 *
 * 此函数仅在 Android 上可用。
 *
 * @param app_ctx Android 应用上下文的全局 JNI 引用
 * @return 成功返回 0，否则返回 < 0
 */
int av_jni_set_android_app_ctx(void *app_ctx, void *log_ctx);

/*
 * 获取通过 av_jni_set_android_app_ctx 设置的 Android 应用上下文。
 *
 * 此函数仅在 Android 上可用。
 *
 * @return 指向 Android 应用上下文的指针
 */
void *av_jni_get_android_app_ctx(void);

#endif /* AVCODEC_JNI_H */
