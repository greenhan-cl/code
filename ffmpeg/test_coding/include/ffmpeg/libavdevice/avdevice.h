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

#ifndef AVDEVICE_AVDEVICE_H
#define AVDEVICE_AVDEVICE_H

#include "version_major.h"
#ifndef HAVE_AV_CONFIG_H
/* 作为 ffmpeg 构建的一部分被包含时，仅包含主版本号，
 * 以避免不必要的重新构建。从外部包含时，仍包含
 * 完整的版本信息。 */
#include "version.h"
#endif

/**
 * @file
 * @ingroup lavd
 * libavdevice 的主要 API 头文件
 */

/**
 * @defgroup lavd libavdevice
 * 特殊设备复用/解复用库。
 *
 * Libavdevice 是 @ref libavf "libavformat" 的补充库。它提供各种平台特定的
 * “特殊”复用器和解复用器，例如采集设备、音频捕获与播放等。因此，
 * libavdevice 中的（解）复用器属于 AVFMT_NOFILE 类型（它们使用自己的
 * I/O 函数）。传给 avformat_open_input() 的文件名通常并不指向一个真实存在的
 * 文件，而是具有设备特定的含义——例如，对 xcbgrab 而言，它表示显示器名称。
 *
 * 要使用 libavdevice，只需调用 avdevice_register_all() 注册所有已编译的
 * 复用器和解复用器。它们都使用标准的 libavformat API。
 *
 * @{
 */

#include "libavutil/log.h"
#include "libavutil/opt.h"
#include "libavutil/dict.h"
#include "libavformat/avformat.h"

/**
 * 返回 LIBAVDEVICE_VERSION_INT 常量。
 */
unsigned avdevice_version(void);

/**
 * 返回 libavdevice 的构建时配置。
 */
const char *avdevice_configuration(void);

/**
 * 返回 libavdevice 的许可证文本。
 */
const char *avdevice_license(void);

/**
 * 初始化 libavdevice，并注册所有输入和输出设备。
 */
void avdevice_register_all(void);

/**
 * 音频输入设备迭代器。
 *
 * 如果 d 为 NULL，则返回第一个已注册的输入音频/视频设备；
 * 如果 d 非 NULL，则返回 d 之后的下一个已注册输入音频/视频设备；
 * 如果 d 已是最后一个，则返回 NULL。
 */
const AVInputFormat *av_input_audio_device_next(const AVInputFormat  *d);

/**
 * 视频输入设备迭代器。
 *
 * 如果 d 为 NULL，则返回第一个已注册的输入音频/视频设备；
 * 如果 d 非 NULL，则返回 d 之后的下一个已注册输入音频/视频设备；
 * 如果 d 已是最后一个，则返回 NULL。
 */
const AVInputFormat *av_input_video_device_next(const AVInputFormat  *d);

/**
 * 音频输出设备迭代器。
 *
 * 如果 d 为 NULL，则返回第一个已注册的输出音频/视频设备；
 * 如果 d 非 NULL，则返回 d 之后的下一个已注册输出音频/视频设备；
 * 如果 d 已是最后一个，则返回 NULL。
 */
const AVOutputFormat *av_output_audio_device_next(const AVOutputFormat *d);

/**
 * 视频输出设备迭代器。
 *
 * 如果 d 为 NULL，则返回第一个已注册的输出音频/视频设备；
 * 如果 d 非 NULL，则返回 d 之后的下一个已注册输出音频/视频设备；
 * 如果 d 已是最后一个，则返回 NULL。
 */
const AVOutputFormat *av_output_video_device_next(const AVOutputFormat *d);

typedef struct AVDeviceRect {
    int x;      /**< 左上角的 x 坐标 */
    int y;      /**< 左上角的 y 坐标 */
    int width;  /**< 宽度 */
    int height; /**< 高度 */
} AVDeviceRect;

/**
 * avdevice_app_to_dev_control_message() 使用的消息类型。
 */
enum AVAppToDevMessageType {
    /**
     * 占位消息。
     */
    AV_APP_TO_DEV_NONE = MKBETAG('N','O','N','E'),

    /**
     * 窗口大小变更消息。
     *
     * 每当应用程序改变设备渲染目标窗口的大小时，都会向设备发送此消息。
     * 窗口创建后也应立即发送此消息。
     *
     * data：AVDeviceRect：新的窗口大小。
     */
    AV_APP_TO_DEV_WINDOW_SIZE = MKBETAG('G','E','O','M'),

    /**
     * 重绘请求消息。
     *
     * 需要重绘窗口时，将此消息发送给设备。
     *
     * data：AVDeviceRect：需要重绘的区域。
     *       NULL：需要重绘整个区域。
     */
    AV_APP_TO_DEV_WINDOW_REPAINT = MKBETAG('R','E','P','A'),

    /**
     * 请求暂停/播放。
     *
     * 应用程序请求暂停或恢复播放。
     * 主要适用于具有内部缓冲区的设备。
     * 默认情况下设备不处于暂停状态。
     *
     * data：NULL
     */
    AV_APP_TO_DEV_PAUSE        = MKBETAG('P', 'A', 'U', ' '),
    AV_APP_TO_DEV_PLAY         = MKBETAG('P', 'L', 'A', 'Y'),
    AV_APP_TO_DEV_TOGGLE_PAUSE = MKBETAG('P', 'A', 'U', 'T'),

    /**
     * 音量控制消息。
     *
     * 设置音量级别。音量是按流修改还是在系统范围内修改，可能取决于设备。
     * 在可能的情况下，期望按流修改音量。
     *
     * data：double：新的音量，范围为 0.0 - 1.0。
     */
    AV_APP_TO_DEV_SET_VOLUME = MKBETAG('S', 'V', 'O', 'L'),

    /**
     * 静音控制消息。
     *
     * 改变静音状态。静音状态是按流修改还是在系统范围内修改，可能取决于设备。
     * 在可能的情况下，期望按流修改静音状态。
     *
     * data：NULL。
     */
    AV_APP_TO_DEV_MUTE        = MKBETAG(' ', 'M', 'U', 'T'),
    AV_APP_TO_DEV_UNMUTE      = MKBETAG('U', 'M', 'U', 'T'),
    AV_APP_TO_DEV_TOGGLE_MUTE = MKBETAG('T', 'M', 'U', 'T'),

    /**
     * 获取音量/静音状态的消息。
     *
     * 强制设备分别发送 AV_DEV_TO_APP_VOLUME_LEVEL_CHANGED 或
     * AV_DEV_TO_APP_MUTE_STATE_CHANGED 命令。
     *
     * data：NULL。
     */
    AV_APP_TO_DEV_GET_VOLUME = MKBETAG('G', 'V', 'O', 'L'),
    AV_APP_TO_DEV_GET_MUTE   = MKBETAG('G', 'M', 'U', 'T'),
};

/**
 * avdevice_dev_to_app_control_message() 使用的消息类型。
 */
enum AVDevToAppMessageType {
    /**
     * 占位消息。
     */
    AV_DEV_TO_APP_NONE = MKBETAG('N','O','N','E'),

    /**
     * 创建窗口缓冲区消息。
     *
     * 设备请求创建窗口缓冲区。确切含义取决于设备和应用程序。
     * 此消息在渲染第一帧之前发送，所有一次性初始化都应在这里完成。
     * 应用程序可以忽略首选的窗口缓冲区大小。
     *
     * @note 应用程序必须通过 AV_APP_TO_DEV_WINDOW_SIZE 消息告知窗口缓冲区大小。
     *
     * data：AVDeviceRect：窗口缓冲区的首选大小。
     *       NULL：没有首选的窗口缓冲区大小。
     */
    AV_DEV_TO_APP_CREATE_WINDOW_BUFFER = MKBETAG('B','C','R','E'),

    /**
     * 准备窗口缓冲区消息。
     *
     * 设备请求为渲染准备窗口缓冲区。
     * 确切含义取决于设备和应用程序。
     * 每一帧渲染之前都会发送此消息。
     *
     * data：NULL。
     */
    AV_DEV_TO_APP_PREPARE_WINDOW_BUFFER = MKBETAG('B','P','R','E'),

    /**
     * 显示窗口缓冲区消息。
     *
     * 设备请求显示窗口缓冲区。
     * 新帧准备好显示时会发送此消息。
     * 通常需要在此消息的处理函数中交换缓冲区。
     *
     * data：NULL。
     */
    AV_DEV_TO_APP_DISPLAY_WINDOW_BUFFER = MKBETAG('B','D','I','S'),

    /**
     * 销毁窗口缓冲区消息。
     *
     * 设备请求销毁窗口缓冲区。
     * 当设备即将被销毁且不再需要窗口缓冲区时，会发送此消息。
     *
     * data：NULL。
     */
    AV_DEV_TO_APP_DESTROY_WINDOW_BUFFER = MKBETAG('B','D','E','S'),

    /**
     * 缓冲区充满状态消息。
     *
     * 设备报告缓冲区上溢/下溢。
     *
     * data：NULL。
     */
    AV_DEV_TO_APP_BUFFER_OVERFLOW = MKBETAG('B','O','F','L'),
    AV_DEV_TO_APP_BUFFER_UNDERFLOW = MKBETAG('B','U','F','L'),

    /**
     * 缓冲区可读/可写。
     *
     * 设备通知缓冲区可读/可写。
     * 在可能的情况下，设备会告知可以读取/写入多少字节。
     *
     * @warning 当可读/可写字节数发生变化时，设备可能不会通知。
     *
     * data：int64_t：可供读取/写入的字节数。
     *       NULL：可供读取/写入的字节数未知。
     */
    AV_DEV_TO_APP_BUFFER_READABLE = MKBETAG('B','R','D',' '),
    AV_DEV_TO_APP_BUFFER_WRITABLE = MKBETAG('B','W','R',' '),

    /**
     * 静音状态变更消息。
     *
     * 设备通知静音状态已改变。
     *
     * data：int：0 表示未静音，非零表示已静音。
     */
    AV_DEV_TO_APP_MUTE_STATE_CHANGED = MKBETAG('C','M','U','T'),

    /**
     * 音量级别变更消息。
     *
     * 设备通知音量级别已改变。
     *
     * data：double：新的音量，范围为 0.0 - 1.0。
     */
    AV_DEV_TO_APP_VOLUME_LEVEL_CHANGED = MKBETAG('C','V','O','L'),
};

/**
 * 从应用程序向设备发送控制消息。
 *
 * @param s         设备上下文。
 * @param type      消息类型。
 * @param data      消息数据。确切类型取决于消息类型。
 * @param data_size 消息数据的大小。
 * @return 成功时 >= 0，出错时为负值。
 *         当设备未实现该消息的处理函数时返回 AVERROR(ENOSYS)。
 */
int avdevice_app_to_dev_control_message(struct AVFormatContext *s,
                                        enum AVAppToDevMessageType type,
                                        void *data, size_t data_size);

/**
 * 从设备向应用程序发送控制消息。
 *
 * @param s         设备上下文。
 * @param type      消息类型。
 * @param data      消息数据，可以为 NULL。
 * @param data_size 消息数据的大小。
 * @return 成功时 >= 0，出错时为负值。
 *         当应用程序未实现该消息的处理函数时返回 AVERROR(ENOSYS)。
 */
int avdevice_dev_to_app_control_message(struct AVFormatContext *s,
                                        enum AVDevToAppMessageType type,
                                        void *data, size_t data_size);

/**
 * 描述设备基本参数的结构体。
 */
typedef struct AVDeviceInfo {
    char *device_name;                   /**< 设备名称，其格式取决于设备 */
    char *device_description;            /**< 便于用户理解的名称 */
    enum AVMediaType *media_types;       /**< 表示设备能够提供哪些媒体类型（如果有）的数组。为 null 时表示不能提供任何媒体类型 */
    int nb_media_types;                  /**< media_types 数组的长度；设备不能提供任何媒体类型时为 0 */
} AVDeviceInfo;

/**
 * 设备列表。
 */
typedef struct AVDeviceInfoList {
    AVDeviceInfo **devices;              /**< 自动检测到的设备列表 */
    int nb_devices;                      /**< 自动检测到的设备数量 */
    int default_device;                  /**< 默认设备的索引；没有默认设备时为 -1 */
} AVDeviceInfoList;

/**
 * 列出设备。
 *
 * 返回可用设备的名称及其参数。
 *
 * @note 某些设备可以接受依赖于系统、且无法自动检测的设备名称。
 *       因此不能认为此函数返回的列表始终是完整的。
 *
 * @param s                设备上下文。
 * @param[out] device_list 自动检测到的设备列表。
 * @return 自动检测到的设备数量；出错时为负值。
 */
int avdevice_list_devices(struct AVFormatContext *s, AVDeviceInfoList **device_list);

/**
 * 用于释放 avdevice_list_devices() 结果的便捷函数。
 *
 * @param device_list 要释放的设备列表。
 */
void avdevice_free_list_devices(AVDeviceInfoList **device_list);

/**
 * 列出设备。
 *
 * 返回可用设备的名称及其参数。
 * 这些函数是 avdevice_list_devices() 的便捷封装。
 * 设备上下文会在内部进行分配和释放。
 *
 * @param device           设备格式。设置了设备名称时可以为 NULL。
 * @param device_name      设备名称。设置了设备格式时可以为 NULL。
 * @param device_options   填有设备私有选项的 AVDictionary，可以为 NULL。
 *                         之后，对于输出设备，必须将相同的选项传给 avformat_write_header()；
 *                         对于输入设备，必须传给 avformat_open_input()；在其他会影响设备私有
 *                         选项的位置也必须传入相同选项。
 * @param[out] device_list 自动检测到的设备列表
 * @return 自动检测到的设备数量；出错时为负值。
 * @note device 与 device_name 同时设置时，device 参数优先。
 */
int avdevice_list_input_sources(const AVInputFormat *device, const char *device_name,
                                AVDictionary *device_options, AVDeviceInfoList **device_list);
int avdevice_list_output_sinks(const AVOutputFormat *device, const char *device_name,
                               AVDictionary *device_options, AVDeviceInfoList **device_list);

/**
 * @}
 */

#endif /* AVDEVICE_AVDEVICE_H */
