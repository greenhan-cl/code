/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef AVUTIL_AVUTIL_H
#define AVUTIL_AVUTIL_H

/**
 * @file
 * @ingroup lavu
 * 包含 @ref lavu "libavutil" 核心组件的便捷头文件。
 */

/**
 * @mainpage
 *
 * @section ffmpeg_intro 简介
 *
 * 本文档描述 FFmpeg 提供的不同库的用法。
 *
 * @li @ref libavc "libavcodec" 编码/解码库
 * @li @ref lavfi "libavfilter" 基于图的帧编辑库
 * @li @ref libavf "libavformat" I/O 和复用/解复用库
 * @li @ref lavd "libavdevice" 特殊设备复用/解复用库
 * @li @ref lavu "libavutil" 通用工具库
 * @li @ref lswr "libswresample" 音频重采样、格式转换和混音库
 * @li @ref libsws "libswscale" 颜色转换和缩放库
 *
 * @section ffmpeg_versioning 版本管理与兼容性
 *
 * 每个 FFmpeg 库都包含 version.h，它通过
 * <em>LIBRARYNAME_VERSION_{MAJOR,MINOR,MICRO}</em> 宏定义主、次和微版本号。
 * 主版本号在发生不向后兼容的变化时递增，例如删除部分公共 API、重新排列公共
 * 结构成员等。次版本号在发生向后兼容的 API 变化或增加主要新功能时递增，例如
 * 添加公共函数或解码器。微版本号在调用程序可能仍需检查的较小变化时递增，
 * 例如改变以前未规定场景中的行为。
 *
 * 只要库的主版本号不变，FFmpeg 就保证其 API 和 ABI 向后兼容。这意味着不会
 * 删除或重命名公共符号。公共结构成员的类型和名称、公共宏和枚举的值会保持
 * 不变（明确声明不属于公共 API 的除外），已记录的行为也不会改变。
 *
 * 换言之，任何能在给定 FFmpeg 快照上正确工作的程序，都应无需修改即可在主版本
 * 相同的后续快照上正常工作。这既适用于针对新版 FFmpeg 重新构建程序，也适用于
 * 替换程序所链接的 FFmpeg 动态库。
 *
 * 不过，可以添加新的公共符号，也可向大小不属于公共 ABI 的公共结构（FFmpeg
 * 中的大多数公共结构）末尾添加新成员，还可添加新宏和枚举值。未记录场景中的
 * 行为可能略有变化（随后会记录）。所有这些变化都会在 doc/APIchanges 中添加
 * 条目，并递增次版本号或微版本号。
 */

/**
 * @defgroup lavu libavutil
 * 所有 FFmpeg 库共享的通用代码。
 *
 * @note
 * libavutil 采用模块化设计。多数情况下，要使用 libavutil 某组件提供的函数，
 * 必须显式包含提供该功能的特定头文件。若只使用媒体相关组件，可直接包含
 * libavutil/avutil.h，它会引入大多数“核心”组件。
 *
 * @{
 *
 * @defgroup lavu_crypto 加密与哈希
 *
 * @{
 * @}
 *
 * @defgroup lavu_math 数学
 * @{
 *
 * @}
 *
 * @defgroup lavu_string 字符串操作
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_mem 内存管理
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_data 数据结构
 * @{
 *
 * @}
 *
 * @defgroup lavu_video 视频相关
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_audio 音频相关
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_error 错误码
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_log 日志工具
 *
 * @{
 *
 * @}
 *
 * @defgroup lavu_misc 其他
 *
 * @{
 *
 * @defgroup preproc_misc 预处理器字符串宏
 *
 * @{
 *
 * @}
 *
 * @defgroup version_utils 库版本宏
 *
 * @{
 *
 * @}
 */


/**
 * @addtogroup lavu_ver
 * @{
 */

/**
 * 返回 LIBAVUTIL_VERSION_INT 常量。
 */
unsigned avutil_version(void);

/**
 * 返回包含信息的版本字符串，通常是实际发行版本号或 git 提交说明。该字符串
 * 没有固定格式，可能随时改变，代码绝不应解析它。
 */
const char *av_version_info(void);

/**
 * 返回 libavutil 的构建时配置。
 */
const char *avutil_configuration(void);

/**
 * 返回 libavutil 的许可证文本。
 */
const char *avutil_license(void);

/**
 * @}
 */

/**
 * @addtogroup lavu_media 媒体类型
 * @brief 媒体类型
 */

enum AVMediaType {
    AVMEDIA_TYPE_UNKNOWN = -1,  ///< 通常按 AVMEDIA_TYPE_DATA 处理
    AVMEDIA_TYPE_VIDEO,
    AVMEDIA_TYPE_AUDIO,
    AVMEDIA_TYPE_DATA,          ///< 通常连续的不透明数据信息
    AVMEDIA_TYPE_SUBTITLE,
    AVMEDIA_TYPE_ATTACHMENT,    ///< 通常稀疏的不透明数据信息
    AVMEDIA_TYPE_NB
};

/**
 * 返回描述 media_type 枚举的字符串；media_type 未知时返回 NULL。
 */
const char *av_get_media_type_string(enum AVMediaType media_type);

/**
 * @defgroup lavu_const 常量
 * @{
 *
 * @defgroup lavu_enc 编码相关
 *
 * @note 这些定义应移至 avcodec
 * @{
 */

#define FF_LAMBDA_SHIFT 7
#define FF_LAMBDA_SCALE (1<<FF_LAMBDA_SHIFT)
#define FF_QP2LAMBDA 118 ///< 从 H.263 QP 转换为 lambda 的因子
#define FF_LAMBDA_MAX (256*128-1)

#define FF_QUALITY_SCALE FF_LAMBDA_SCALE //FIXME 可能移除

/**
 * @}
 * @defgroup lavu_time 时间戳相关
 *
 * FFmpeg 内部时间基和时间戳定义
 *
 * @{
 */

/**
 * @brief 未定义的时间戳值
 *
 * 通常由处理既不提供 pts 也不提供 dts 的容器的解复用器报告。
 */

#define AV_NOPTS_VALUE          ((int64_t)UINT64_C(0x8000000000000000))

/**
 * 以整数表示的内部时间基
 */

#define AV_TIME_BASE            1000000

/**
 * 以分数表示的内部时间基
 */

#ifdef __cplusplus
/* ISO C++ 禁止复合字面量。 */
#define AV_TIME_BASE_Q          av_make_q(1, AV_TIME_BASE)
#else
#define AV_TIME_BASE_Q          (AVRational){1, AV_TIME_BASE}
#endif

/**
 * @}
 * @}
 * @defgroup lavu_picture 图像相关
 *
 * AVPicture 类型、像素格式和基本图像平面操作。
 *
 * @{
 */

enum AVPictureType {
    AV_PICTURE_TYPE_NONE = 0, ///< 未定义
    AV_PICTURE_TYPE_I,     ///< 帧内预测
    AV_PICTURE_TYPE_P,     ///< 前向预测
    AV_PICTURE_TYPE_B,     ///< 双向预测
    AV_PICTURE_TYPE_S,     ///< S(GMC)-VOP MPEG-4
    AV_PICTURE_TYPE_SI,    ///< 切换帧内帧
    AV_PICTURE_TYPE_SP,    ///< 切换预测帧
    AV_PICTURE_TYPE_BI,    ///< BI 类型
};

/**
 * 返回单个字母描述给定图像类型 pict_type。
 *
 * @param[in] pict_type 图像类型
 * @return 表示图像类型的单个字符；pict_type 未知时为 '?'
 */
char av_get_picture_type_char(enum AVPictureType pict_type);

/**
 * @}
 */

#include "common.h"
#include "rational.h"
#include "version.h"
#include "macros.h"
#include "mathematics.h"
#include "log.h"
#include "pixfmt.h"

/**
 * p 为 NULL 时返回默认指针 x。
 */
static inline void *av_x_if_null(const void *p, const void *x)
{
    return (void *)(intptr_t)(p ? p : x);
}

/**
 * 返回内部时间基的分数表示。
 */
AVRational av_get_time_base_q(void);

#define AV_FOURCC_MAX_STRING_SIZE 32

#define av_fourcc2str(fourcc) av_fourcc_make_string((char[AV_FOURCC_MAX_STRING_SIZE]){0}, fourcc)

/**
 * 使用包含 FourCC（四字符代码）表示的字符串填充所提供的缓冲区。
 *
 * @param buf    字节大小至少为 AV_FOURCC_MAX_STRING_SIZE 的缓冲区
 * @param fourcc 要表示的 fourcc
 * @return 输入缓冲区
 */
char *av_fourcc_make_string(char *buf, uint32_t fourcc);

/**
 * @}
 * @}
 */

#endif /* AVUTIL_AVUTIL_H */
