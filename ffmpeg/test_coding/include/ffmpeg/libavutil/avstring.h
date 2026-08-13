/*
 * Copyright (c) 2007 Mans Rullgard
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

#ifndef AVUTIL_AVSTRING_H
#define AVUTIL_AVSTRING_H

#include <stddef.h>
#include <stdint.h>
#include "attributes.h"

/**
 * @addtogroup lavu_string
 * @{
 */

/**
 * 如果 pfx 是 str 的前缀则返回非零，并将 *ptr 设为 str 中前缀之后首字符的地址。
 *
 * @param str 输入字符串
 * @param pfx 要测试的前缀
 * @param ptr 在 str 中匹配前缀时更新
 * @return 前缀匹配时返回非零，否则返回零
 */
int av_strstart(const char *str, const char *pfx, const char **ptr);

/**
 * 如果 pfx 是 str 的前缀（忽略大小写）则返回非零，并将 *ptr 设为 str 中前缀
 * 之后首字符的地址。
 *
 * @param str 输入字符串
 * @param pfx 要测试的前缀
 * @param ptr 在 str 中匹配前缀时更新
 * @return 前缀匹配时返回非零，否则返回零
 */
int av_stristart(const char *str, const char *pfx, const char **ptr);

/**
 * 在字符串 haystack 中定位字符串 needle 第一次出现的位置（忽略大小写）。
 * 零长度 needle 被视为在 haystack 开头匹配。
 *
 * 此函数是标准 strstr() 的不区分大小写版本。
 *
 * @param haystack 要在其中搜索的字符串
 * @param needle   要搜索的字符串
 * @return 指向 haystack 中匹配位置的指针；没有匹配时返回空指针
 */
char *av_stristr(const char *haystack, const char *needle);

/**
 * 在字符串 haystack 的最多 hay_length 个字符内定位字符串 needle 第一次出现的
 * 位置。零长度 needle 被视为在 haystack 开头匹配。
 *
 * 此函数是标准 strstr() 的长度受限版本。
 *
 * @param haystack   要在其中搜索的字符串
 * @param needle     要搜索的字符串
 * @param hay_length 要搜索的字符串长度
 * @return 指向 haystack 中匹配位置的指针；没有匹配时返回空指针
 */
char *av_strnstr(const char *haystack, const char *needle, size_t hay_length);

/**
 * 将字符串 src 复制到 dst，最多复制 size - 1 字节，并以 null 终止 dst。
 *
 * 此函数与 BSD strlcpy() 相同。
 *
 * @param dst 目标缓冲区
 * @param src 源字符串
 * @param size 目标缓冲区大小
 * @return src 的长度
 *
 * @warning 由于返回值是 src 的长度，src _必须_ 是正确以 0 结尾的字符串，
 * 否则会越过缓冲区末尾读取并可能崩溃。
 */
size_t av_strlcpy(char *dst, const char *src, size_t size);

/**
 * 将字符串 src 追加到 dst，总长度不超过 size - 1 字节，并以 null 终止 dst。
 *
 * 此函数类似 BSD strlcat()，但 size <= strlen(dst) 时行为不同。
 *
 * @param dst 目标缓冲区
 * @param src 源字符串
 * @param size 目标缓冲区大小
 * @return src 和 dst 的总长度
 *
 * @warning 由于返回值使用 src 和 dst 的长度，二者 _必须_ 是正确以 0 结尾的
 * 字符串，否则会越过缓冲区末尾读取并可能崩溃。
 */
size_t av_strlcat(char *dst, const char *src, size_t size);

/**
 * 按格式将输出追加到字符串。绝不会写出目标缓冲区，且始终在缓冲区内放置终止 0。
 * @param dst 目标缓冲区（输出追加到的字符串）
 * @param size 目标缓冲区总大小
 * @param fmt 与 printf 兼容的格式字符串，指定后续参数的使用方式
 * @return 如果空间足够，本应生成的字符串长度
 */
size_t av_strlcatf(char *dst, size_t size, const char *fmt, ...) av_printf_format(3, 4);

/**
 * 获取从开头起连续非零字符的数量。
 *
 * @param s   要计算长度的字符串
 * @param len 字符串中要检查的最大字符数，也是函数可能返回的最大值
 */
static inline size_t av_strnlen(const char *s, size_t len)
{
    size_t i;
    for (i = 0; i < len && s[i]; i++)
        ;
    return i;
}

/**
 * 按指定格式将参数打印到自动分配的足够大缓冲区，类似 GNU asprintf()。
 * @param fmt 与 printf 兼容的格式字符串，指定后续参数的使用方式。
 * @return 已分配的字符串
 * @note 必须自行使用 av_free() 释放字符串。
 */
char *av_asprintf(const char *fmt, ...) av_printf_format(1, 2);

/**
 * 对给定字符串解除转义，直到遇到未转义的终止字符，并返回对应标记。
 *
 * 支持普通的 \ 和 ' 转义。除非使用 '\' 转义或包含在 '' 中，否则会移除
 * 首尾空白。
 *
 * @param buf 要解析的缓冲区；buf 会更新为指向终止字符
 * @param term 以 0 结尾的终止字符列表
 * @return malloc 分配的解除转义字符串，用户必须使用 av_free 释放；分配失败时
 * 返回 NULL
 */
char *av_get_token(const char **buf, const char *term);

/**
 * 将字符串拆分为多个标记，可通过连续调用 av_strtok() 访问。
 *
 * 标记定义为不属于 delim 指定集合的字符序列。
 *
 * 首次调用 av_strtok() 时，s 应指向待解析字符串，saveptr 的值被忽略。后续调用
 * 中 s 应为 NULL，saveptr 应保持为上次调用后的值。
 *
 * 此函数类似 POSIX.1 定义的 strtok_r()。
 *
 * @param s 要解析的字符串，可以为 NULL
 * @param delim 以 0 结尾的标记分隔符列表，必须非 NULL
 * @param saveptr 用户提供的指针，指向 av_strtok() 继续扫描同一字符串所需的信息。
 *                它会更新为指向首个已找到分隔符后的下一个字符；字符串结束时
 *                更新为 NULL
 * @return 找到的标记；没有标记时返回 NULL
 */
char *av_strtok(char *s, const char *delim, char **saveptr);

/**
 * 与区域设置无关的 ASCII isdigit 转换。
 */
static inline av_const int av_isdigit(int c)
{
    return c >= '0' && c <= '9';
}

/**
 * 与区域设置无关的 ASCII isgraph 转换。
 */
static inline av_const int av_isgraph(int c)
{
    return c > 32 && c < 127;
}

/**
 * 与区域设置无关的 ASCII isspace 转换。
 */
static inline av_const int av_isspace(int c)
{
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
           c == '\v';
}

/**
 * 与区域设置无关地将 ASCII 字符转换为大写。
 */
static inline av_const int av_toupper(int c)
{
    if (c >= 'a' && c <= 'z')
        c ^= 0x20;
    return c;
}

/**
 * 与区域设置无关地将 ASCII 字符转换为小写。
 */
static inline av_const int av_tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
        c ^= 0x20;
    return c;
}

/**
 * 与区域设置无关的 ASCII isxdigit 转换。
 */
static inline av_const int av_isxdigit(int c)
{
    c = av_tolower(c);
    return av_isdigit(c) || (c >= 'a' && c <= 'f');
}

/**
 * 与区域设置无关、不区分大小写的比较。
 * @note 这意味着仅 ASCII 范围字符不区分大小写
 */
int av_strcasecmp(const char *a, const char *b);

/**
 * 与区域设置无关、不区分大小写的比较。
 * @note 这意味着仅 ASCII 范围字符不区分大小写
 */
int av_strncasecmp(const char *a, const char *b, size_t n);

/**
 * 与区域设置无关的字符串替换。
 * @note 这意味着仅替换 ASCII 范围字符。
 */
char *av_strireplace(const char *str, const char *from, const char *to);

/**
 * 线程安全的 basename。
 * @param path 要解析的字符串；DOS 上 \ 和 / 都视为分隔符。
 * @return 指向基本名称子串的指针。path 不含斜杠时返回 path 的副本；path 为
 * NULL 或指向空字符串时返回指向字符串 "." 的指针。
 */
const char *av_basename(const char *path);

/**
 * 线程安全的 dirname。
 * @param path 要解析的字符串；DOS 上 \ 和 / 都视为分隔符。
 * @return 指向 path 父目录字符串的指针。path 为 NULL 或指向空字符串时返回
 * 指向字符串 "." 的指针。
 * @note 此函数可能修改 path 内容，因此应传入副本。
 */
const char *av_dirname(char *path);

/**
 * 在逗号分隔的名称列表中匹配名称实例。从 names 列表开头到末尾检查条目，
 * 第一个匹配会终止后续处理。如果匹配前缀为 '-' 的条目，则返回 0。列表条目
 * "ALL" 被视为匹配所有名称。
 *
 * @param name  要查找的名称。
 * @param names 名称列表。
 * @return 匹配时返回 1，否则返回 0。
 */
int av_match_name(const char *name, const char *names);

/**
 * 将路径组件追加到现有路径。需要时会在中间放置路径分隔符 '/'。
 * 结果字符串必须使用 av_free() 释放。
 * @param path      基础路径
 * @param component 要追加的组件
 * @return 新路径；出错时返回 NULL。
 */
char *av_append_path_component(const char *path, const char *component);

enum AVEscapeMode {
    AV_ESCAPE_MODE_AUTO,      ///< 使用自动选择的转义模式。
    AV_ESCAPE_MODE_BACKSLASH, ///< 使用反斜杠转义。
    AV_ESCAPE_MODE_QUOTE,     ///< 使用单引号转义。
    AV_ESCAPE_MODE_XML,       ///< 使用 XML 非标记字符数据转义。
};

/**
 * 将空格视为特殊字符，即使位于字符串中间也对其转义。
 *
 * 这等同于把空白字符加入特殊字符列表，但保证使用与 libavutil 其余部分完全
 * 相同的空白字符列表。
 */
#define AV_ESCAPE_FLAG_WHITESPACE (1 << 0)

/**
 * 仅转义指定的特殊字符。不设置此标志时，还会转义 av_get_token() 可能视为
 * 特殊的任何字符，例如单引号。
 */
#define AV_ESCAPE_FLAG_STRICT (1 << 1)

/**
 * 在 AV_ESCAPE_MODE_XML 中，对单引号属性额外转义单引号。
 */
#define AV_ESCAPE_FLAG_XML_SINGLE_QUOTES (1 << 2)

/**
 * 在 AV_ESCAPE_MODE_XML 中，对双引号属性额外转义双引号。
 */
#define AV_ESCAPE_FLAG_XML_DOUBLE_QUOTES (1 << 3)


/**
 * 转义 src 中的字符串，并将结果放入 *dst 中已分配的字符串；必须使用 av_free()
 * 释放该字符串。
 *
 * @param dst           用于放置已分配字符串的指针
 * @param src           要转义的字符串，必须非 NULL
 * @param special_chars 包含要转义特殊字符的字符串，可以为 NULL
 * @param mode          使用的转义模式，参见 AV_ESCAPE_MODE_* 宏。任何未知值均
 *                      视同 AV_ESCAPE_MODE_BACKSLASH，但此行为可能不经通知而改变。
 * @param flags         控制转义方式的标志，参见 AV_ESCAPE_FLAG_ 宏
 * @return 已分配字符串的长度；出错时返回负错误码
 * @see av_bprint_escape()
 */
av_warn_unused_result
int av_escape(char **dst, const char *src, const char *special_chars,
              enum AVEscapeMode mode, int flags);

#define AV_UTF8_FLAG_ACCEPT_INVALID_BIG_CODES          1 ///< 接受大于 0x10FFFF 的码点
#define AV_UTF8_FLAG_ACCEPT_NON_CHARACTERS             2 ///< 接受非字符 0xFFFE 和 0xFFFF
#define AV_UTF8_FLAG_ACCEPT_SURROGATES                 4 ///< 接受 UTF-16 代理码
#define AV_UTF8_FLAG_EXCLUDE_XML_INVALID_CONTROL_CODES 8 ///< 排除 XML 不接受的控制码

#define AV_UTF8_FLAG_ACCEPT_ALL \
    AV_UTF8_FLAG_ACCEPT_INVALID_BIG_CODES|AV_UTF8_FLAG_ACCEPT_NON_CHARACTERS|AV_UTF8_FLAG_ACCEPT_SURROGATES

/**
 * 从 *buf 中的缓冲区读取并解码单个 UTF-8 码点（字符），并更新 *buf 使其指向
 * 下一个待解码字节。
 *
 * 遇到无效字节序列时，指针会更新到无效序列后的下一字节，函数返回错误码。
 *
 * 根据指定标志，如果解码码点不属于有效范围，函数也会失败。
 *
 * @note 对性能敏感的代码可能更适合谨慎实现并使用 GET_UTF8()。
 *
 * @param codep   成功时用于返回解析码的指针。即使范围检查失败也会设置 *codep。
 * @param bufp    指向待解码序列首字节地址的指针；函数会将其更新为指向已解码
 *                序列之后的下一字节
 * @param buf_end 指向缓冲区末尾的指针，即最后一字节之后的位置，用于避免缓冲区
 *                越界读取（缓冲区末尾存在未完成 UTF-8 序列时）
 * @param flags   AV_UTF8_FLAG_* 标志集合
 * @return 成功读取序列时 >= 0，序列无效时返回负值
 */
av_warn_unused_result
int av_utf8_decode(int32_t *codep, const uint8_t **bufp, const uint8_t *buf_end,
                   unsigned int flags);

/**
 * 检查名称是否在列表中。
 * @returns 未找到时返回 0，否则返回它在列表中的从 1 开始的索引。
 */
int av_match_list(const char *name, const char *list, char separator);

/**
 * 更多信息参见 libc sscanf 手册。
 * 与区域设置无关的 sscanf 实现。
 */
int av_sscanf(const char *string, const char *format, ...) av_scanf_format(2, 3);

/**
 * @}
 */

#endif /* AVUTIL_AVSTRING_H */
