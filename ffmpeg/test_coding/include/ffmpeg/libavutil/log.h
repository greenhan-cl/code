/*
 * 复制right (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef AVUTIL_LOG_H
#define AVUTIL_LOG_H

#include <stdarg.h>
#include "attributes.h"
#include "version.h"

typedef enum {
    AV_CLASS_CATEGORY_NA = 0,
    AV_CLASS_CATEGORY_INPUT,
    AV_CLASS_CATEGORY_OUTPUT,
    AV_CLASS_CATEGORY_MUXER,
    AV_CLASS_CATEGORY_DEMUXER,
    AV_CLASS_CATEGORY_ENCODER,
    AV_CLASS_CATEGORY_DECODER,
    AV_CLASS_CATEGORY_FILTER,
    AV_CLASS_CATEGORY_BITSTREAM_FILTER,
    AV_CLASS_CATEGORY_SWSCALER,
    AV_CLASS_CATEGORY_SWRESAMPLER,
    AV_CLASS_CATEGORY_HWDEVICE,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT = 40,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_INPUT,
    AV_CLASS_CATEGORY_NB  ///< not part of ABI/API
}AVClassCategory;

enum AVClassStateFlags {
    /**
     * Object initialization has finished 和 it is now 中 the 'runtime' stage.
     * This affects e.g. what 选项 can be 设置 上 the object (only
     * AV_OPT_标志_RUNTIME_PARAM 选项 can be 设置 上 初始化d objects).
     */
    AV_CLASS_STATE_INITIALIZED         = (1 << 0),
};

#define AV_IS_INPUT_DEVICE(category) \
    (((category) == AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT) || \
     ((category) == AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT) || \
     ((category) == AV_CLASS_CATEGORY_DEVICE_INPUT))

#define AV_IS_OUTPUT_DEVICE(category) \
    (((category) == AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT) || \
     ((category) == AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT) || \
     ((category) == AV_CLASS_CATEGORY_DEVICE_OUTPUT))

struct AVOptionRanges;

/**
 * Describe the class 的 an AVClass 上下文 结构体. That is an
 * arbitrary struct 的 which the first field is a 指针 到 an
 * AVClass struct (e.g. AVCodec上下文, AV格式上下文 etc.).
 */
typedef struct AVClass {
    /**
     * name 的 the class; usually it is the same name as the
     * 上下文 结构体 type 到 which the AVClass is associated.
     */
    const char* class_name;

    /**
     * 指针 到 a function which 返回s the name 的 a 上下文
     * instance ctx associated，使用 the class.
     */
    const char* (*item_name)(void* ctx);

    /**
     * 数组 的 选项 用于 the 结构体 或 NULL.
     * 当 non-NULL, the 数组 must be terminated by an 选项，使用 a NULL
     * name.
     *
     * @参见 av_设置_默认_选项()
     */
    const struct AVOption *option;

    /**
     * LIBAVUTIL_VERSION，使用 which this 结构体 was 创建d.
     * This is 用于 allow fields 到 be added 到 AVClass without requiring
     * major version bumps everywhere.
     */

    int version;

    /**
     * Off设置 中 the 结构体 where the log level off设置 is stored. log
     * level off设置 is an int added 到 the log level 用于 logging，使用 this
     * object as the 上下文.
     *
     * 0 means there is no such variable.
     */
    int log_level_offset_offset;

    /**
     * Off设置 中 the 结构体 where a 指针 到 the parent 上下文 for
     * logging is stored. For example a 解码器 could pass its AVCodec上下文
     * 到 eval as such a parent 上下文, which an ::av_log() implementation
     * could then leverage 到 display the parent 上下文.
     *
     * 当 the 指针 is NULL, 或 this off设置 is zero, the object is assumed
     * 到 have no parent.
     */
    int parent_log_context_offset;

    /**
     * Category 用于 visualization (like color).
     *
     * Only used 当 ::get_category() is NULL. Use this field 当 all
     * instances 的 this class have the same category, use ::get_category()
     * otherwise.
     */
    AVClassCategory category;

    /**
     * Callback 到 返回 the instance category. Use this callback 当
     * d如果ferent instances 的 this class may have d如果ferent categories,
     * ::category otherwise.
     */
    AVClassCategory (*get_category)(void* ctx);

    /**
     * Callback 到 返回 the supported/allowed ranges.
     */
    int (*query_ranges)(struct AVOptionRanges **, void *obj, const char *key, int flags);

    /**
     * 返回 next AV选项-enabled child 或 NULL
     */
    void* (*child_next)(void *obj, void *prev);

    /**
     * Iterate over the AVClasses corresponding 到 potential AV选项-enabled
     * children.
     *
     * @param iter 指针 到 opaque iteration state. caller must 初始化
     *             *iter 到 NULL before the first call.
     * @返回 AVClass 用于 the next AV选项-enabled child 或 NULL 如果 there are
     *         no more such children.
     *
     * @note d如果ference between ::child_next() 和 ::child_class_iterate()
     *       is that ::child_next() iterates over _actual_ children 的 an
     *       _existing_ object instance, while ::child_class_iterate() iterates
     *       over the classes 的 all _potential_ children 的 any possible
     *       instance 的 this class.
     */
    const struct AVClass* (*child_class_iterate)(void **iter);

    /**
     * 当 non-zero, off设置 中 the object 到 an unsigned int holding object
     * state 标志, a combination 的 AVClassState标志 值. 标志 are
     * updated by the object 到 signal its state 到 the generic code.
     *
     * Added 中 version 59.41.100.
     */
    int state_flags_offset;
} AVClass;

/**
 * @addtogroup lavu_log
 *
 * @{
 *
 * @defgroup lavu_log_constants Logging Constants
 *
 * @{
 */

/**
 * Print no 输出.
 */
#define AV_LOG_QUIET    -8

/**
 * Something went really wrong 和 we will crash now.
 */
#define AV_LOG_PANIC     0

/**
 * Something went wrong 和 recovery is not possible.
 * For example, no header was found 用于 a 格式 which depends
 * 上 headers 或 an illegal combination 的 parameters is used.
 */
#define AV_LOG_FATAL     8

/**
 * Something went wrong 和 cannot losslessly be recovered.
 * However, not all future data is affected.
 */
#define AV_LOG_ERROR    16

/**
 * Something somehow does not look correct. This may 或 may not
 * lead 到 problems. example would be the use 的 '-vstrict -2'.
 */
#define AV_LOG_WARNING  24

/**
 * Standard in格式ion.
 */
#define AV_LOG_INFO     32

/**
 * Detailed in格式ion.
 */
#define AV_LOG_VERBOSE  40

/**
 * Stuff which is only useful 用于 libav* developers.
 */
#define AV_LOG_DEBUG    48

/**
 * Extremely verbose debugging, useful 用于 libav* development.
 */
#define AV_LOG_TRACE    56

#define AV_LOG_MAX_OFFSET (AV_LOG_TRACE - AV_LOG_QUIET)

/**
 * @}
 */

/**
 * 设置s additional colors 用于 extended debugging sessions.
 * @code
   av_log(ctx, AV_LOG_DEBUG|AV_LOG_C(134), "Message in purple\n");
   @endcode
 * Requires 256color terminal support. Uses outside debugging is not
 * recommended.
 */
#define AV_LOG_C(x) ((x) << 8)

/**
 * Send the spec如果ied message 到 the log 如果 the level is less than 或 equal
 * 到 the current av_log_level. By 默认, all logging messages are sent to
 * stderr. This behavior can be altered by 设置ting a d如果ferent logging callback
 * function.
 * @参见 av_log_设置_callback
 *
 * @param avcl 指针 到 an arbitrary struct 的 which the first field is a
 *        指针 到 an AVClass struct 或 NULL 如果 general log.
 * @param level importance level 的 the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * @param fmt 格式 string (printf-compatible) that spec如果ies how
 *        subsequent arguments are 转换ed 到 输出.
 */
void av_log(void *avcl, int level, const char *fmt, ...) av_printf_format(3, 4);

/**
 * Send the spec如果ied message 到 the log once，使用 the initial_level 和 then with
 * the subsequent_level. By 默认, all logging messages are sent to
 * stderr. This behavior can be altered by 设置ting a d如果ferent logging callback
 * function.
 * @参见 av_log
 *
 * @param avcl 指针 到 an arbitrary struct 的 which the first field is a
 *        指针 到 an AVClass struct 或 NULL 如果 general log.
 * @param initial_level importance level 的 the message expressed using a @ref
 *        lavu_log_constants "Logging Constant" 用于 the first occurrence.
 * @param subsequent_level importance level 的 the message expressed using a @ref
 *        lavu_log_constants "Logging Constant" after the first occurrence.
 * @param fmt 格式 string (printf-compatible) that spec如果ies how
 *        subsequent arguments are 转换ed 到 输出.
 * @param state a variable 到 keep trak 的 如果 a message has already been printed
 *        this must be 初始化d 到 0 before the first use. same state
 *        must not be accessed by 2 Threads simultaneously.
 */
void av_log_once(void* avcl, int initial_level, int subsequent_level, int *state, const char *fmt, ...) av_printf_format(5, 6);


/**
 * Send the spec如果ied message 到 the log 如果 the level is less than 或 equal
 * 到 the current av_log_level. By 默认, all logging messages are sent to
 * stderr. This behavior can be altered by 设置ting a d如果ferent logging callback
 * function.
 * @参见 av_log_设置_callback
 *
 * @param avcl 指针 到 an arbitrary struct 的 which the first field is a
 *        指针 到 an AVClass struct.
 * @param level importance level 的 the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * @param fmt 格式 string (printf-compatible) that spec如果ies how
 *        subsequent arguments are 转换ed 到 输出.
 * @param vl arguments 引用d by the 格式 string.
 */
void av_vlog(void *avcl, int level, const char *fmt, va_list vl);

/**
 * 获取 the current log level
 *
 * @参见 lavu_log_constants
 *
 * @返回 Current log level
 */
int av_log_get_level(void);

/**
 * 设置 the log level
 *
 * @参见 lavu_log_constants
 *
 * @param level Logging level
 */
void av_log_set_level(int level);

/**
 * 设置 the logging callback
 *
 * @note callback must be thread safe, even 如果 the application does not use
 *       threads itself as some codecs are multithreaded.
 *
 * @参见 av_log_默认_callback
 *
 * @param callback logging function，使用 a compatible signature.
 */
void av_log_set_callback(void (*callback)(void*, int, const char*, va_list));

/**
 * 默认 logging callback
 *
 * It prints the message 到 stderr, 可选ly colorizing it.
 *
 * @param avcl 指针 到 an arbitrary struct 的 which the first field is a
 *        指针 到 an AVClass struct.
 * @param level importance level 的 the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * @param fmt 格式 string (printf-compatible) that spec如果ies how
 *        subsequent arguments are 转换ed 到 输出.
 * @param vl arguments 引用d by the 格式 string.
 */
void av_log_default_callback(void *avcl, int level, const char *fmt,
                             va_list vl);

/**
 * 返回 the 上下文 name
 *
 * @param  ctx AVClass 上下文
 *
 * @返回 AVClass class_name
 */
const char* av_default_item_name(void* ctx);
AVClassCategory av_default_get_category(void *ptr);

/**
 * 格式 a line 的 log the same way as the 默认 callback.
 * @param line          缓冲区 到 receive the 格式ted line
 * @param line_大小     大小 的 the 缓冲区
 * @param print_prefix  用于 store 是否 the prefix must be printed;
 *                      must point 到 a persistent integer initially 设置 到 1
 */
void av_log_format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix);

/**
 * 格式 a line 的 log the same way as the 默认 callback.
 * @param line          缓冲区 到 receive the 格式ted line;
 *                      may be NULL 如果 line_大小 is 0
 * @param line_大小     大小 的 the 缓冲区; at most line_大小-1 characters will
 *                      be written 到 the 缓冲区, plus one null terminator
 * @param print_prefix  用于 store 是否 the prefix must be printed;
 *                      must point 到 a persistent integer initially 设置 到 1
 * @返回 返回 a negative 值 如果 an error occurred, otherwise 返回s
 *         the 数量 的 characters that would have been written 用于 a
 *         sufficiently large 缓冲区, not including the terminating null
 *         character. 如果 the 返回 值 is not less than line_大小, it means
 *         that the log message was truncated 到 fit the 缓冲区.
 */
int av_log_format_line2(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix);

/**
 * Skip repeated messages, this requires the user app 到 use av_log() instead of
 * (f)printf as the 2 would otherwise interfere 和 lead to
 * "Last message repeated x times" messages below (f)printf messages，使用 some
 * bad luck.
 * Also 到 receive the last, "last repeated" line 如果 any, the user app must
 * call av_log(NULL, AV_LOG_QUIET, "%s", ""); at the end
 */
#define AV_LOG_SKIP_REPEATED 1

/**
 * Include the log severity 中 messages originating，来自 codecs.
 *
 * Results 中 messages such as:
 * [raw视频 @ 0xDEADBEEF] [error] encode did not produce valid pts
 */
#define AV_LOG_PRINT_LEVEL 2

/**
 * Include system time 中 log 输出.
 */
#define AV_LOG_PRINT_TIME 4

/**
 * Include system date 和 time 中 log 输出.
 */
#define AV_LOG_PRINT_DATETIME 8

void av_log_set_flags(int arg);
int av_log_get_flags(void);

/**
 * @}
 */

#endif /* AVUTIL_LOG_H */
