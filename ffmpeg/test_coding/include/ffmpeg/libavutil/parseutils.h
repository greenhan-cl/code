/*
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

#ifndef AVUTIL_PARSEUTILS_H
#define AVUTIL_PARSEUTILS_H

#include <time.h>

#include "rational.h"

/**
 * @file
 * misc parsing utilities
 */

/**
 * 解析 str 和 store the 解析d ratio 中 q.
 *
 * Note that a ratio，使用 infinite (1/0) 或 negative 值 is
 * considered valid, so you should 检查 上 the 返回ed 值 如果 you
 * want 到 exclude those 值.
 *
 * undefined 值 can be expressed using the "0:0" string.
 *
 * @param[in,out] q 指针 到 the AVRational which will contain the ratio
 * @param[in] str the string 到 解析: it has 到 be a string 中 the 格式
 * num:den, a float 数量 或 an expression
 * @param[in] max the maximum allowed numerator 和 denominator
 * @param[in] log_off设置 log level off设置 which is applied 到 the log
 * level 的 log_ctx
 * @param[in] log_ctx parent logging 上下文
 * @返回 >= 0 上 success, a negative error code otherwise
 */
int av_parse_ratio(AVRational *q, const char *str, int max,
                   int log_offset, void *log_ctx);

#define av_parse_ratio_quiet(rate, str, max) \
    av_parse_ratio(rate, str, max, AV_LOG_MAX_OFFSET, NULL)

/**
 * 解析 str 和 put 中 宽度_ptr 和 高度_ptr the detected 值.
 *
 * @param[in,out] 宽度_ptr 指针 到 the variable which will contain the detected
 * 宽度 值
 * @param[in,out] 高度_ptr 指针 到 the variable which will contain the detected
 * 高度 值
 * @param[in] str the string 到 解析: it has 到 be a string 中 the 格式
 * 宽度 x 高度 或 a valid 视频 大小 abbreviation.
 * @返回 >= 0 上 success, a negative error code otherwise
 */
int av_parse_video_size(int *width_ptr, int *height_ptr, const char *str);

/**
 * 解析 str 和 store the detected 值 中 *rate.
 *
 * @param[in,out] rate 指针 到 the AVRational which will contain the detected
 * 帧率
 * @param[in] str the string 到 解析: it has 到 be a string 中 the 格式
 * rate_num / rate_den, a float 数量 或 a valid 视频 rate abbreviation
 * @返回 >= 0 上 success, a negative error code otherwise
 */
int av_parse_video_rate(AVRational *rate, const char *str);

/**
 * Put the RGB值 that correspond 到 color_string 中 rgba_color.
 *
 * @param rgba_color 4-elements 数组 的 uint8_t 值, where the respective
 * red, green, blue 和 alpha component 值 are written.
 * @param color_string a string spec如果ying a color. It can be the name of
 * a color (case insensitive match) 或 a [0x|#]RRGGBB[AA] sequence,
 * possibly followed by "@" 和 a string representing the alpha
 * component.
 * alpha component may be a string composed by "0x" followed by an
 * hexadecimal 数量 或 a decimal 数量 between 0.0 和 1.0, which
 * represents the opacity 值 (0x00/0.0 means completely transparent,
 * 0xff/1.0 completely opaque).
 * 如果 the alpha component is not spec如果ied then 0xff is assumed.
 * string "random" will result 中 a random color.
 * @param slen length 的 the initial part 的 color_string containing the
 * color. It can be 设置 到 -1 如果 color_string is a null terminated string
 * containing nothing else than the color.
 * @param log_ctx a 指针 到 an arbitrary struct 的 which the first field
 * is a 指针 到 an AVClass struct (用于 av_log()). Can be NULL.
 * @返回 >= 0 中 case 的 success, a negative 值 中 case of
 * failure (for example 如果 color_string cannot be 解析d).
 */
int av_parse_color(uint8_t *rgba_color, const char *color_string, int slen,
                   void *log_ctx);

/**
 * 获取 the name 的 a color，来自 the internal table 的 hard-coded named
 * colors.
 *
 * 此函数 is meant 到 enumerate the color names recognized by
 * av_解析_color().
 *
 * @param color_idx index 的 the requested color, starting，来自 0
 * @param rgb      如果 not NULL, will point 到 a 3-elements 数组，使用 the color 值 中 RGB
 * @返回 the color name string 或 NULL 如果 color_idx is not 中 the 数组
 */
const char *av_get_known_color_name(int color_idx, const uint8_t **rgb);

/**
 * 解析 timestr 和 返回 中 *time a corresponding 数量 of
 * microseconds.
 *
 * @param timeval puts here the 数量 的 microseconds corresponding
 * 到 the string 中 timestr. 如果 the string represents a 持续时间, it
 * is the 数量 的 microseconds contained 中 the time interval.  如果
 * the string is a date, is the 数量 的 microseconds since 1st of
 * January, 1970 up 到 the time 的 the 解析d date.  如果 timestr cannot
 * be successfully 解析d, 设置 *time 到 INT64_MIN.

 * @param timestr a string representing a date 或 a 持续时间.
 * - 如果 a date the syntax is:
 * @code
 * [{YYYY-MM-DD|YYYYMMDD}[T|t| ]]{{HH:MM:SS[.m...]]]}|{HHMMSS[.m...]]]}}[Z]
 * now
 * @endcode
 * 如果 the 值 is "now" it takes the current time.
 * Time is local time unless Z is appended, 中 which case it is
 * interpreted as UTC.
 * 如果 the year-month-day part is not spec如果ied it takes the current
 * year-month-day.
 * - 如果 a 持续时间 the syntax is:
 * @code
 * [-][HH:]MM:SS[.m...]
 * [-]S+[.m...]
 * @endcode
 * @param 持续时间 标志 which tells how 到 interpret timestr, 如果 not
 * zero timestr is interpreted as a 持续时间, otherwise as a date
 * @返回 >= 0 中 case 的 success, a negative 值 corresponding 到 an
 * AVERROR code otherwise
 */
int av_parse_time(int64_t *timeval, const char *timestr, int duration);

/**
 * Attempt 到 find a spec如果ic tag 中 a URL.
 *
 * syntax: '?tag1=val1&tag2=val2...'. Little URL 解码 is done.
 * 返回 1 如果 found.
 */
int av_find_info_tag(char *arg, int arg_size, const char *tag1, const char *info);

/**
 * Simpl如果ied version 的 strptime
 *
 * 解析 the 输入 string p according 到 the 格式 string fmt and
 * store its results 中 the 结构体 dt.
 * This implementation supports only a sub设置 的 the 格式s supported
 * by the standard strptime().
 *
 * supported 输入 field 描述符s are 列表ed below.
 * - `%%H`: the hour as a decimal 数量, using a 24-hour clock, 中 the
 *   range '00' through '23'
 * - `%%J`: hours as a decimal 数量, 中 the range '0' through INT_MAX
 * - `%%M`: the minute as a decimal 数量, using a 24-hour clock, 中 the
 *   range '00' through '59'
 * - `%%S`: the second as a decimal 数量, using a 24-hour clock, 中 the
 *   range '00' through '59'
 * - `%%Y`: the year as a decimal 数量, using the Gregorian calendar
 * - `%%m`: the month as a decimal 数量, 中 the range '1' through '12'
 * - `%%d`: the day 的 the month as a decimal 数量, 中 the range '1'
 *   through '31'
 * - `%%T`: alias 用于 `%%H:%%M:%%S`
 * - `%%`: a literal `%`
 *
 * @返回 a 指针 到 the first character not processed 中 this function
 *         call. In case the 输入 string 包含 more characters than
 *         required by the 格式 string the 返回 值 points right after
 *         the last consumed 输入 character. In case the whole 输入 string
 *         is consumed the 返回 值 points 到 the null byte at the end of
 *         the string. On failure NULL is 返回ed.
 */
char *av_small_strptime(const char *p, const char *fmt, struct tm *dt);

/**
 * 转换 the decomposed UTC time 中 tm 到 a time_t 值.
 */
time_t av_timegm(struct tm *tm);

#endif /* AVUTIL_PARSEUTILS_H */
