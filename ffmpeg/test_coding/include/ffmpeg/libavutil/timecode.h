/*
 * 复制right (c) 2006 Smartjog S.A.S, Baptiste Coudurier <baptiste.coudurier@gmail.com>
 * 复制right (c) 2011-2012 Smartjog S.A.S, Clément Bœsch <clement.boesch@smartjog.com>
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
 * Timecode helpers header
 */

#ifndef AVUTIL_TIMECODE_H
#define AVUTIL_TIMECODE_H

#include <stdint.h>
#include "rational.h"

#define AV_TIMECODE_STR_SIZE 23

enum AVTimecodeFlag {
    AV_TIMECODE_FLAG_DROPFRAME      = 1<<0, ///< timecode is drop frame
    AV_TIMECODE_FLAG_24HOURSMAX     = 1<<1, ///< timecode wraps after 24 hours
    AV_TIMECODE_FLAG_ALLOWNEGATIVE  = 1<<2, ///< negative time values are allowed
};

typedef struct {
    int start;          ///< timecode frame start (first base frame number)
    uint32_t flags;     ///< flags such as drop frame, +24 hours support, ...
    AVRational rate;    ///< frame rate in rational form
    unsigned fps;       ///< frame per second; must be consistent with the rate field
} AVTimecode;

/**
 * Adjust 帧 数量 用于 NTSC drop 帧 time code.
 *
 * @param 帧num 帧 数量 到 adjust
 * @param fps      帧 per second, multiples 的 30
 * @返回         adjusted 帧 数量
 * @warning        adjustment is only valid 用于 multiples 的 NTSC 29.97
 */
int av_timecode_adjust_ntsc_framenum2(int framenum, int fps);

/**
 * 转换 帧 数量 到 SMPTE 12M binary representation.
 *
 * @param tc       timecode data correctly 初始化d
 * @param 帧num 帧 数量
 * @返回         the SMPTE binary representation
 *
 * 参见 SMPTE ST 314M-2005 Sec 4.4.2.2.1 "Time code pack (TC)"
 * the 格式 描述 as follows:
 * bits 0-5:   hours, 中 BCD(6bits)
 * bits 6:     BGF1
 * bits 7:     BGF2 (NTSC) 或 FIELD (PAL)
 * bits 8-14:  minutes, 中 BCD(7bits)
 * bits 15:    BGF0 (NTSC) 或 BGF2 (PAL)
 * bits 16-22: seconds, 中 BCD(7bits)
 * bits 23:    FIELD (NTSC) 或 BGF0 (PAL)
 * bits 24-29: 帧s, 中 BCD(6bits)
 * bits 30:    drop  帧 标志 (0: non drop,    1: drop)
 * bits 31:    color 帧 标志 (0: unsync mode, 1: sync mode)
 * @note BCD 数量s (6 或 7 bits): 4 或 5 lower bits 用于 units, 2 higher bits 用于 tens.
 * @note 帧 数量 adjustment is automatically done 中 case 的 drop timecode,
 *       you do NOT have 到 call av_timecode_adjust_ntsc_帧num2().
 * @note 帧 数量 is relative 到 tc->start.
 * @note Color 帧 (CF) 和 binary group 标志 (BGF) bits are 设置 到 zero.
 */
uint32_t av_timecode_get_smpte_from_framenum(const AVTimecode *tc, int framenum);

/**
 * 转换 sei info 到 SMPTE 12M binary representation.
 *
 * @param rate     帧率 中 rational form
 * @param drop     drop 标志
 * @param hh       hour
 * @param mm       minute
 * @param ss       second
 * @param ff       帧 数量
 * @返回         the SMPTE binary representation
 */
uint32_t av_timecode_get_smpte(AVRational rate, int drop, int hh, int mm, int ss, int ff);

/**
 * Load timecode string 中 buf.
 *
 * @param tc       timecode data correctly 初始化d
 * @param buf      destination 缓冲区, must be at least AV_TIMECODE_STR_大小 long
 * @param 帧num 帧 数量
 * @返回         the buf parameter
 *
 * @note Timecode representation can be a negative timecode 和 have more than
 *       24 hours, but will only be honored 如果 the 标志 are correctly 设置.
 * @note 帧 数量 is relative 到 tc->start.
 */
char *av_timecode_make_string(const AVTimecode *tc, char *buf, int framenum);

/**
 * 获取 the timecode string，来自 the SMPTE timecode 格式.
 *
 * In contrast 到 av_timecode_make_smpte_tc_string this function supports 50/60
 * fps timecodes by using the field bit.
 *
 * @param buf        destination 缓冲区, must be at least AV_TIMECODE_STR_大小 long
 * @param rate       帧率 的 the timecode
 * @param tcsmpte    the 32-bit SMPTE timecode
 * @param prevent_df prevent the use 的 a drop 标志 当 it is known the DF bit
 *                   is arbitrary
 * @param skip_field prevent the use 的 a field 标志 当 it is known the field
 *                   bit is arbitrary (e.g. because it is used as PC 标志)
 * @返回           the buf parameter
 */
char *av_timecode_make_smpte_tc_string2(char *buf, AVRational rate, uint32_t tcsmpte, int prevent_df, int skip_field);

/**
 * 获取 the timecode string，来自 the SMPTE timecode 格式.
 *
 * @param buf        destination 缓冲区, must be at least AV_TIMECODE_STR_大小 long
 * @param tcsmpte    the 32-bit SMPTE timecode
 * @param prevent_df prevent the use 的 a drop 标志 当 it is known the DF bit
 *                   is arbitrary
 * @返回           the buf parameter
 */
char *av_timecode_make_smpte_tc_string(char *buf, uint32_t tcsmpte, int prevent_df);

/**
 * 获取 the timecode string，来自 the 25-bit timecode 格式 (MPEG GOP 格式).
 *
 * @param buf     destination 缓冲区, must be at least AV_TIMECODE_STR_大小 long
 * @param tc25bit the 25-bits timecode
 * @返回        the buf parameter
 */
char *av_timecode_make_mpeg_tc_string(char *buf, uint32_t tc25bit);

/**
 * Init a timecode struct，使用 the passed parameters.
 *
 * @param tc          指针 到 an 分配d AVTimecode
 * @param rate        帧率 中 rational form
 * @param 标志       miscellaneous 标志 such as drop 帧, +24 hours, ...
 *                    (参见 AVTimecode标志)
 * @param 帧_start the first 帧 数量
 * @param log_ctx     a 指针 到 an arbitrary struct 的 which the first field
 *                    is a 指针 到 an AVClass struct (用于 av_log)
 * @返回            0 上 success, AVERROR otherwise
 */
int av_timecode_init(AVTimecode *tc, AVRational rate, int flags, int frame_start, void *log_ctx);

/**
 * Init a timecode struct，来自 the passed timecode components.
 *
 * @param tc          指针 到 an 分配d AVTimecode
 * @param rate        帧率 中 rational form
 * @param 标志       miscellaneous 标志 such as drop 帧, +24 hours, ...
 *                    (参见 AVTimecode标志)
 * @param hh          hours
 * @param mm          minutes
 * @param ss          seconds
 * @param ff          帧s
 * @param log_ctx     a 指针 到 an arbitrary struct 的 which the first field
 *                    is a 指针 到 an AVClass struct (用于 av_log)
 * @返回            0 上 success, AVERROR otherwise
 */
int av_timecode_init_from_components(AVTimecode *tc, AVRational rate, int flags, int hh, int mm, int ss, int ff, void *log_ctx);

/**
 * 解析 timecode representation (hh:mm:ss[:;.]ff).
 *
 * @param tc      指针 到 an 分配d AVTimecode
 * @param rate    帧率 中 rational form
 * @param str     timecode string which will determine the 帧 start
 * @param log_ctx a 指针 到 an arbitrary struct 的 which the first field is a
 *                指针 到 an AVClass struct (用于 av_log).
 * @返回        0 上 success, AVERROR otherwise
 */
int av_timecode_init_from_string(AVTimecode *tc, AVRational rate, const char *str, void *log_ctx);

/**
 * 检查 如果 the timecode feature is available 用于 the given 帧率
 *
 * @返回 0 如果 supported, <0 otherwise
 */
int av_timecode_check_frame_rate(AVRational rate);

#endif /* AVUTIL_TIMECODE_H */
