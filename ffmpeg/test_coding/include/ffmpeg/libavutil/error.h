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
 * 错误码定义
 */

#ifndef AVUTIL_ERROR_H
#define AVUTIL_ERROR_H

#include <errno.h>
#include <stddef.h>

#include "macros.h"

/**
 * @addtogroup lavu_error
 *
 * @{
 */


/* 错误处理 */
#if EDOM > 0
#define AVERROR(e) (-(e))   ///< 将 POSIX 错误码转换为供库函数返回的负错误码。
#define AVUNERROR(e) (-(e)) ///< 将库函数返回的错误值转换为 POSIX 错误码。
#else
/* 某些平台的 E* 和 errno 已经是负值。 */
#define AVERROR(e) (e)
#define AVUNERROR(e) (e)
#endif

#define FFERRTAG(a, b, c, d) (-(int)MKTAG(a, b, c, d))

#define AVERROR_BSF_NOT_FOUND      FFERRTAG(0xF8,'B','S','F') ///< 未找到比特流过滤器
#define AVERROR_BUG                FFERRTAG( 'B','U','G','!') ///< 内部错误，另见 AVERROR_BUG2
#define AVERROR_BUFFER_TOO_SMALL   FFERRTAG( 'B','U','F','S') ///< 缓冲区太小
#define AVERROR_DECODER_NOT_FOUND  FFERRTAG(0xF8,'D','E','C') ///< 未找到解码器
#define AVERROR_DEMUXER_NOT_FOUND  FFERRTAG(0xF8,'D','E','M') ///< 未找到解复用器
#define AVERROR_ENCODER_NOT_FOUND  FFERRTAG(0xF8,'E','N','C') ///< 未找到编码器
#define AVERROR_EOF                FFERRTAG( 'E','O','F',' ') ///< 文件结束
#define AVERROR_EXIT               FFERRTAG( 'E','X','I','T') ///< 请求立即退出；不应重新启动被调用函数
#define AVERROR_EXTERNAL           FFERRTAG( 'E','X','T',' ') ///< 外部库中的通用错误
#define AVERROR_FILTER_NOT_FOUND   FFERRTAG(0xF8,'F','I','L') ///< 未找到过滤器
#define AVERROR_INVALIDDATA        FFERRTAG( 'I','N','D','A') ///< 处理输入时发现无效数据
#define AVERROR_MUXER_NOT_FOUND    FFERRTAG(0xF8,'M','U','X') ///< 未找到复用器
#define AVERROR_OPTION_NOT_FOUND   FFERRTAG(0xF8,'O','P','T') ///< 未找到选项
#define AVERROR_PATCHWELCOME       FFERRTAG( 'P','A','W','E') ///< FFmpeg 尚未实现，欢迎提交补丁
#define AVERROR_PROTOCOL_NOT_FOUND FFERRTAG(0xF8,'P','R','O') ///< 未找到协议

#define AVERROR_STREAM_NOT_FOUND   FFERRTAG(0xF8,'S','T','R') ///< 未找到流
/**
 * 其语义与 AVERROR_BUG 相同。它在我们的 AVERROR_BUG 之后引入 Libav，
 * 并使用了不同的值。
 */
#define AVERROR_BUG2               FFERRTAG( 'B','U','G',' ')
#define AVERROR_UNKNOWN            FFERRTAG( 'U','N','K','N') ///< 未知错误，通常来自外部库
#define AVERROR_EXPERIMENTAL       (-0x2bb2afa8) ///< 请求的功能被标记为实验性。确实需要使用时请设置 strict_std_compliance。
#define AVERROR_INPUT_CHANGED      (-0x636e6701) ///< 两次调用之间输入发生变化，需要重新配置。（可与 AVERROR_OUTPUT_CHANGED 按位或）
#define AVERROR_OUTPUT_CHANGED     (-0x636e6702) ///< 两次调用之间输出发生变化，需要重新配置。（可与 AVERROR_INPUT_CHANGED 按位或）
/* HTTP 和 RTSP 错误 */
#define AVERROR_HTTP_BAD_REQUEST   FFERRTAG(0xF8,'4','0','0')
#define AVERROR_HTTP_UNAUTHORIZED  FFERRTAG(0xF8,'4','0','1')
#define AVERROR_HTTP_FORBIDDEN     FFERRTAG(0xF8,'4','0','3')
#define AVERROR_HTTP_NOT_FOUND     FFERRTAG(0xF8,'4','0','4')
#define AVERROR_HTTP_TOO_MANY_REQUESTS FFERRTAG(0xF8,'4','2','9')
#define AVERROR_HTTP_OTHER_4XX     FFERRTAG(0xF8,'4','X','X')
#define AVERROR_HTTP_SERVER_ERROR  FFERRTAG(0xF8,'5','X','X')

#define AV_ERROR_MAX_STRING_SIZE 64

/**
 * 将 AVERROR 错误码 errnum 的描述放入 errbuf。失败时会设置全局变量 errno 以
 * 表明错误。即使失败，av_strerror() 也会向 errbuf 写入指出所给 errnum 的
 * 通用错误消息。
 *
 * @param errnum      要描述的错误码
 * @param errbuf      写入描述的缓冲区
 * @param errbuf_size errbuf 的字节大小
 * @return 成功时返回 0；找不到 errnum 的描述时返回负值
 */
int av_strerror(int errnum, char *errbuf, size_t errbuf_size);

/**
 * 使用与 AVERROR 错误码 errnum 对应的错误字符串填充所提供的缓冲区。
 *
 * @param errbuf         缓冲区
 * @param errbuf_size    errbuf 的字节大小
 * @param errnum         要描述的错误码
 * @return 输入的缓冲区，其中已填入错误描述
 * @see av_strerror()
 */
static inline char *av_make_error_string(char *errbuf, size_t errbuf_size, int errnum)
{
    av_strerror(errnum, errbuf, errbuf_size);
    return errbuf;
}

/**
 * 便捷宏，其返回值只能直接用于函数参数，绝不能单独使用。
 */
#define av_err2str(errnum) \
    av_make_error_string((char[AV_ERROR_MAX_STRING_SIZE]){0}, AV_ERROR_MAX_STRING_SIZE, errnum)

/**
 * @}
 */

#endif /* AVUTIL_ERROR_H */
