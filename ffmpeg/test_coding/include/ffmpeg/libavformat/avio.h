/*
 * copyright (c) 2001 Fabrice Bellard
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
#ifndef AVFORMAT_AVIO_H
#define AVFORMAT_AVIO_H

/**
 * @file
 * @ingroup lavf_io 缓冲 I/O 操作
 */

#include <stdint.h>
#include <stdio.h>

#include "libavutil/attributes.h"
#include "libavutil/dict.h"
#include "libavutil/log.h"

#include "libavformat/version_major.h"

/**
 * 查找的工作方式类似于本地文件。
 */
#define AVIO_SEEKABLE_NORMAL (1 << 0)

/**
 * 可以使用 avio_seek_time() 按时间戳进行查找。
 */
#define AVIO_SEEKABLE_TIME   (1 << 1)

/**
 * 用于检查是否中止阻塞函数的回调。在这种情况下，中断函数会返回 AVERROR_EXIT。在阻塞操作期间，以 opaque 作为参数调用回调。如果回调返回1，则阻塞操作将被中止。
 *
 * 如果在 AVFormatContext 或 AVIOContext 中的该结构之后添加了新元素，则在没有重大影响的情况下，无法将任何成员添加到该结构中。
 */
typedef struct AVIOInterruptCB {
    int (*callback)(void*);
    void *opaque;
} AVIOInterruptCB;

/**
 * 目录条目类型。
 */
enum AVIODirEntryType {
    AVIO_ENTRY_UNKNOWN,
    AVIO_ENTRY_BLOCK_DEVICE,
    AVIO_ENTRY_CHARACTER_DEVICE,
    AVIO_ENTRY_DIRECTORY,
    AVIO_ENTRY_NAMED_PIPE,
    AVIO_ENTRY_SYMBOLIC_LINK,
    AVIO_ENTRY_SOCKET,
    AVIO_ENTRY_FILE,
    AVIO_ENTRY_SERVER,
    AVIO_ENTRY_SHARE,
    AVIO_ENTRY_WORKGROUP,
};

/**
 * 描述目录的单个条目。
 *
 * 仅保证设置名称和类型字段。其余字段取决于协议或/和平台，并且可能未知。
 */
typedef struct AVIODirEntry {
    char *name;                           /**< 文件名 */
    int type;                             /**< 条目类型 */
    int utf8;                             /**
 * < 当名称使用 UTF-8 编码时设置为 1，否则设置为 0。即使设置为 0，名称也可以使用 UTF-8 进行编码。
 */
    int64_t size;                         /**< 文件大小（以字节为单位），如果未知则为 -1。 */
    int64_t modification_timestamp;       /**
 * < 自 unix 纪元以来最后一次修改的时间（以微秒为单位），如果未知则为 -1。
 */
    int64_t access_timestamp;             /**
 * < 自 unix 纪元以来上次访问的时间（以微秒为单位），如果未知则为 -1。
 */
    int64_t status_change_timestamp;      /**
 * < 自 unix 纪元以来最后一次状态更改的时间（以微秒为单位），如果未知则为 -1。
 */
    int64_t user_id;                      /**< 所有者的用户 ID，如果未知则为 -1。 */
    int64_t group_id;                     /**< 所有者的组 ID，如果未知则为 -1。 */
    int64_t filemode;                     /**< Unix 文件模式，如果未知则为 -1。 */
} AVIODirEntry;

typedef struct AVIODirContext AVIODirContext;

/**
 * 可以通过 AVIO write_data_type 回调返回的不同数据类型。
 */
enum AVIODataMarkerType {
    /**
 * 标头数据；需要存在此信息才能使流可解码。
 */
    AVIO_DATA_MARKER_HEADER,
    /**
 * 输出字节流中解码器可以开始解码的点（即关键帧）。给定用 AVIO_DATA_MARKER_HEADER 标记的数据（后跟任何 AVIO_DATA_MARKER_SYNC_POINT）的解复用器/解码器应该给出可解码的结果。
 */
    AVIO_DATA_MARKER_SYNC_POINT,
    /**
 * 输出字节流中解复用器可以开始解析的点（对于非自同步字节流格式）。即任何非关键帧数据包的起始点。
 */
    AVIO_DATA_MARKER_BOUNDARY_POINT,
    /**
 * 这是任何未标记的数据。它可以是根本不标记任何位置的复用器，也可以是复用器选择不标记的实际边界/同步点，也可以是由于 IO 缓冲区大小有限而被切割成多个写入回调的数据包/片段的后续部分。
 */
    AVIO_DATA_MARKER_UNKNOWN,
    /**
 * 预告片数据，不包含实际内容，仅用于最终确定输出文件。
 */
    AVIO_DATA_MARKER_TRAILER,
    /**
 * 输出字节流中的一个点，底层 AVIOContext 可能会根据延迟或缓冲要求刷新缓冲区。通常意味着数据包的结束。
 */
    AVIO_DATA_MARKER_FLUSH_POINT,
};

/**
 * 字节流 IO 上下文。可以通过较小的版本更新添加新的公共字段。对现有公共字段的删除、重新排序和更改需要主要版本更新。 sizeof(AVIOContext) 不得在 libav* 之外使用。
 *
 * @note AVIOContext 中的任何函数指针都不应直接调用，它们只能由客户端应用程序在实现自定义 I/O 时设置。通常这些被设置为 avio_alloc_context() 中指定的函数指针
 */
typedef struct AVIOContext {
    /**
 * 私人选项的类。
 *
 * 如果此 AVIOContext 是由 avio_open2() 创建的，则设置 av_class 并将选项传递给协议。
 *
 * 如果这个 AVIOContext 是手动分配的，那么 av_class 可以由调用者设置。
 *
 * 警告 - 该字段可以为 NULL，在这种情况下请确保不要将此 AVIOContext 传递给任何 av_opt_* 函数。
 */
    const AVClass *av_class;

    /*
 * 下面显示了读取和写入时 buffer、buf_ptr、buf_ptr_max、buf_end、buf_size 和 pos 之间的关系（因为两者都使用 AVIOContext）：
 *
 * *********************************************************************************
 * 阅读
 * *********************************************************************************
 *
 * |              缓冲区大小|
 *                            |---------------------------------------|
 * |                                       |
 *
 * 缓冲区 buf_ptr buf_end
 *                            +---------------+-----------------------+
 * |/ / / / / / / /|/ / / / / / /|         |读缓冲区：|//消耗/|待读/|         | |/ / / / / / / /|/ / / / / / /|         |
 *                            +---------------+-----------------------+
 *
 * 位置
 *              +-------------------------------------------+-----------------+
 * 输入文件：|                                           |                 |
 *              +-------------------------------------------+-----------------+
 *
 * *********************************************************************************
 * 写作
 * *********************************************************************************
 *
 * |          缓冲区大小|
 *                             |--------------------------------------|
 * |                                      |
 *
 * buf_ptr_max 缓冲区 (buf_ptr) buf_end
 *                             +-----------------------+--------------+
 * |/ / / / / / / / / / / /|              |写缓冲区：| // 待刷新 // |              | |/ / / / / / / / / / / /|              |
 *                             +-----------------------+--------------+
 * buf_ptr 可能位于此位置，因为向后查找
 *
 * pos
 *               +-------------+----------------------------------------------+
 * 输出文件： |             |                                              |
 *               +-------------+----------------------------------------------+
 */
    unsigned char *buffer;  /**< 缓冲区的开始。 */
    int buffer_size;        /**< 最大缓冲区大小 */
    unsigned char *buf_ptr; /**< 缓冲区中的当前位置 */
    unsigned char *buf_end; /**
 * < 数据结束，如果读取函数返回的数据少于请求的数据，则可能小于 buffer+buffer_size，例如对于尚未收到更多数据的流。
 */
    void *opaque;           /**
 * < 私有指针，传递给读/写/查找/...函数。
 */
    int (*read_packet)(void *opaque, uint8_t *buf, int buf_size);
    int (*write_packet)(void *opaque, const uint8_t *buf, int buf_size);
    int64_t (*seek)(void *opaque, int64_t offset, int whence);
    int64_t pos;            /**< 当前缓冲区文件中的位置 */
    int eof_reached;        /**< 如果由于错误或 eof 而无法读取，则为 true */
    int error;              /**< 包含错误代码，如果没有发生错误则为 0 */
    int write_flag;         /**< 如果打开用于写入，则为 true */
    int max_packet_size;
    int min_packet_size;    /**
 * < 在刷新之前尝试缓冲至少这个数量的数据。
 */
    unsigned long checksum;
    unsigned char *checksum_ptr;
    unsigned long (*update_checksum)(unsigned long checksum, const uint8_t *buf, unsigned int size);
    /**
 * 暂停或恢复网络流协议的播放 - 例如彩信。
 */
    int (*read_pause)(void *opaque, int pause);
    /**
 * 使用指定的stream_index查找流中的给定时间戳。某些不支持查找字节位置的网络流协议需要。
 */
    int64_t (*read_seek)(void *opaque, int stream_index,
                         int64_t timestamp, int flags);
    /**
 * AVIO_SEEKABLE_ 标志的组合，或者当流不可查找时为 0。
 */
    int seekable;

    /**
 * avio_read 和 avio_write 如果可能的话应该直接满足而不是通过缓冲区，并且 avio_seek 将始终直接调用底层的查找函数。
 */
    int direct;

    /**
 * ',' 分隔的允许协议列表。
 */
    const char *protocol_whitelist;

    /**
 * ',' 分隔的不允许的协议列表。
 */
    const char *protocol_blacklist;

    /**
 * 用于代替 write_packet 的回调。
 */
    int (*write_data_type)(void *opaque, const uint8_t *buf, int buf_size,
                           enum AVIODataMarkerType type, int64_t time);
    /**
 * 如果设置，则不要为 AVIO_DATA_MARKER_BOUNDARY_POINT 单独调用 write_data_type，而是忽略它们并将它们视为 AVIO_DATA_MARKER_UNKNOWN （以避免从回调返回不必要的小数据块）。
 */
    int ignore_boundary_point;

    /**
 * 在写入缓冲区中向后查找之前达到的最大位置，用于跟踪已写入的数据以供以后刷新。
 */
    unsigned char *buf_ptr_max;

    /**
 * 为此 AVIOContext 读取的字节的只读统计信息。
 */
    int64_t bytes_read;

    /**
 * 为此 AVIOContext 写入的字节的只读统计信息。
 */
    int64_t bytes_written;
} AVIOContext;

/**
 * 返回将处理给定 URL 的协议名称。
 *
 * 如果找不到给定 URL 的协议，则返回 NULL。
 *
 * @return 协议名称或 NULL。
 */
const char *avio_find_protocol_name(const char *url);

/**
 * 返回与 url 中资源访问权限对应的 AVIO_FLAG_* 访问标志；失败时返回对应 AVERROR 代码的负值。返回的访问标志会与 flags 的值进行按位与掩码处理。
 *
 * @note 该函数本质上是不安全的，因为所检查的资源可能会从一次调用到另一次调用而改变其存在或权限状态。因此，您不应信任返回的值，除非您确定没有其他进程正在访问所检查的资源。
 */
int avio_check(const char *url, int flags);

/**
 * 打开目录进行读取。
 *
 * @param s 目录读取上下文。必须传递指向 NULL 指针的指针。
 * @param url 要列出的目录。
 * @param options 充满协议私有选项的字典。返回时，此参数将被销毁并替换为包含未找​​到的选项的字典。可能为 NULL。
 * @return >=0 表示成功，负值表示错误。
 */
int avio_open_dir(AVIODirContext **s, const char *url, AVDictionary **options);

/**
 * 获取下一个目录条目。
 *
 * 返回的条目必须使用 avio_free_directory_entry() 释放。特别是它可能比 AVIODirContext 更长寿。
 *
 * @param s 目录读取上下文。
 * @param[out] next 下一个条目或当没有更多条目时为 NULL。
 * @return >=0 表示成功，负值表示错误。列表末尾不被视为错误。
 */
int avio_read_dir(AVIODirContext *s, AVIODirEntry **next);

/**
 * 关闭目录。
 *
 * @note 使用 avio_read_dir() 创建的条目不会被删除，必须使用 avio_free_directory_entry() 释放。
 *
 * @param s 目录读取上下文。
 * @return >=0 表示成功，负值表示错误。
 */
int avio_close_dir(AVIODirContext **s);

/**
 * 由 avio_read_dir() 分配的空闲条目。
 *
 * @param entry 条目被释放。
 */
void avio_free_directory_entry(AVIODirEntry **entry);

/**
 * 为缓冲 I/O 分配并初始化 AVIOContext。稍后必须使用 avio_context_free() 释放它。
 *
 * @param buffer 通过 AVIOContext 进行输入/输出操作的内存块。缓冲区必须使用 av_malloc() 和朋友来分配。它可以被 libavformat 释放并替换为新的缓冲区。 AVIOContext.buffer 保存当前正在使用的缓冲区，稍后必须使用 av_free() 释放该缓冲区。
 * @param buffer_size 缓冲区大小对于性能非常重要。对于具有固定块大小的协议，应将其设置为此块大小。对于其他人来说，典型的大小是缓存页面，例如4kb。
 * @param write_flag 如果缓冲区应可写，则设置为 1，否则设置为 0。
 * @param opaque 指向用户特定数据的不透明指针。
 * @param read_packet 重新填充缓冲区的函数，可以为 NULL。对于流协议，绝不能返回 0，而是返回正确的 AVERROR 代码。
 * @param write_packet 用于写入缓冲区内容的函数，可以为 NULL。该函数可能不会更改输入缓冲区的内容。
 * @param seek 用于查找指定字节位置的函数，可以为 NULL。
 *
 * @return 已分配 AVIOContext 或失败时为 NULL。
 */
AVIOContext *avio_alloc_context(
                  unsigned char *buffer,
                  int buffer_size,
                  int write_flag,
                  void *opaque,
                  int (*read_packet)(void *opaque, uint8_t *buf, int buf_size),
                  int (*write_packet)(void *opaque, const uint8_t *buf, int buf_size),
                  int64_t (*seek)(void *opaque, int64_t offset, int whence));

/**
 * 释放提供的 IO 上下文以及与其关联的所有内容。
 *
 * @param s 指向 IO 上下文的双指针。该函数会将 NULL 写入 s。
 */
void avio_context_free(AVIOContext **s);

void avio_w8(AVIOContext *s, int b);
void avio_write(AVIOContext *s, const unsigned char *buf, int size);
void avio_wl64(AVIOContext *s, uint64_t val);
void avio_wb64(AVIOContext *s, uint64_t val);
void avio_wl32(AVIOContext *s, unsigned int val);
void avio_wb32(AVIOContext *s, unsigned int val);
void avio_wl24(AVIOContext *s, unsigned int val);
void avio_wb24(AVIOContext *s, unsigned int val);
void avio_wl16(AVIOContext *s, unsigned int val);
void avio_wb16(AVIOContext *s, unsigned int val);

/**
 * 写入 NULL 终止的字符串。
 * @return 写入的字节数。
 */
int avio_put_str(AVIOContext *s, const char *str);

/**
 * 将 UTF-8 字符串转换为 UTF-16LE 并写入。
 * @param s AVIOContext
 * @param str 以 NULL 结尾的 UTF-8 字符串
 *
 * @return 写入的字节数。
 */
int avio_put_str16le(AVIOContext *s, const char *str);

/**
 * 将 UTF-8 字符串转换为 UTF-16BE 并写入。
 * @param s AVIOContext
 * @param str 以 NULL 结尾的 UTF-8 字符串
 *
 * @return 写入的字节数。
 */
int avio_put_str16be(AVIOContext *s, const char *str);

/**
 * 将写入的字节流标记为特定类型。
 *
 * 输出中省略了零长度范围。
 *
 * @param s AVIOContext
 * @param time 当前字节流 pos 对应的流时间（以 AV_TIME_BASE 为单位），或者 AV_NOPTS_VALUE（如果未知或不适用）
 * @param type 从当前 pos 开始写入的数据类型
 */
void avio_write_marker(AVIOContext *s, int64_t time, enum AVIODataMarkerType type);

/**
 * 将其作为“whence”参数传递给查找函数会导致它返回文件大小而不在任何地方查找。支持此功能是可选的。如果不支持，则查找函数将返回 <0。
 */
#define AVSEEK_SIZE 0x10000

/**
 * 将此标志或“whence”参数插入到查找函数中会导致它通过任何方式（例如重新打开和线性读取）或其他通常不合理的方式进行查找，这可能会非常慢。这是默认值，因此自 2010 年以来被查找代码忽略。
 */
#define AVSEEK_FORCE 0x20000

/**
 * fseek() 相当于 AVIOContext。
 * @return 新位置或 AVERROR。
 */
int64_t avio_seek(AVIOContext *s, int64_t offset, int whence);

/**
 * 向前跳过给定数量的字节
 * @return 新职位或 AVERROR。
 */
int64_t avio_skip(AVIOContext *s, int64_t offset);

/**
 * ftell() 相当于 AVIOContext。
 * @return 位置或 AVERROR。
 */
static av_always_inline int64_t avio_tell(AVIOContext *s)
{
    return avio_seek(s, 0, SEEK_CUR);
}

/**
 * 获取文件大小。
 * @return 文件大小或 AVERROR
 */
int64_t avio_size(AVIOContext *s);

/**
 * 与 feof() 类似，但在读取错误时也返回非零。
 * @return 当且仅当在文件末尾或读取时发生读取错误时非零。
 */
int avio_feof(AVIOContext *s);

/**
 * 将格式化字符串写入采用 va_list 的上下文。
 * @return 写入的字节数，出错时 < 0。
 */
int avio_vprintf(AVIOContext *s, const char *fmt, va_list ap);

/**
 * 将格式化字符串写入上下文。
 * @return 写入的字节数，出错时 < 0。
 */
int avio_printf(AVIOContext *s, const char *fmt, ...) av_printf_format(2, 3);

/**
 * 将 NULL 终止的字符串数组写入上下文。通常您不需要直接使用该函数，而是需要使用它的宏包装器 avio_print。
 */
void avio_print_string_array(AVIOContext *s, const char * const strings[]);

/**
 * 将字符串 (const char *) 写入上下文。这是一个围绕 avio_print_string_array 的便捷宏，它会自动从变量参数列表创建字符串数组。对于简单的字符串连接，此函数比使用 avio_printf 性能更高，因为它不需要临时缓冲区。
 */
#define avio_print(s, ...) \
    avio_print_string_array(s, (const char*[]){__VA_ARGS__, NULL})

/**
 * 强制刷新缓冲数据。
 *
 * 对于写入流，强制将缓冲数据立即写入输出，而无需等待填充内部缓冲区。
 *
 * 对于读取流，丢弃所有当前缓冲的数据，并将报告的文件位置前进到基础流的位置。这不会读取新数据，也不执行任何查找。
 */
void avio_flush(AVIOContext *s);

/**
 * 将 size 字节从 AVIOContext 读取到 buf 中。
 * @return 读取的字节数或 AVERROR
 */
int avio_read(AVIOContext *s, unsigned char *buf, int size);

/**
 * 将 size 字节从 AVIOContext 读取到 buf。与 avio_read() 不同，允许读取少于请求的字节数。可以在下一次调用中读取丢失的字节。它总是尝试读取至少 1 个字节。在某些情况下有助于减少延迟。
 * @return 读取的字节数或 AVERROR
 */
int avio_read_partial(AVIOContext *s, unsigned char *buf, int size);

/**
 * @name 从 AVIOContext 读取的函数
 * @{
 *
 * @note 如果 EOF 返回 0，因此如果需要 EOF 处理，则不能使用它
 */
int          avio_r8  (AVIOContext *s);
unsigned int avio_rl16(AVIOContext *s);
unsigned int avio_rl24(AVIOContext *s);
unsigned int avio_rl32(AVIOContext *s);
uint64_t     avio_rl64(AVIOContext *s);
unsigned int avio_rb16(AVIOContext *s);
unsigned int avio_rb24(AVIOContext *s);
unsigned int avio_rb32(AVIOContext *s);
uint64_t     avio_rb64(AVIOContext *s);
/**
 * @}
 */

/**
 * 将 pb 中的字符串读入 buf 中。当遇到 NULL 字符、已读取 maxlen 个字节或无法从 pb 读取更多内容时，读取将终止。结果保证以 NULL 终止，如果 buf 太小，结果将被截断。请注意，该字符串不会以任何方式解释或验证，它可能会在多字节编码序列的中间被截断。
 *
 * @return 读取的字节数（始终 <= maxlen）。如果读取在 EOF 或错误时结束，则返回值将比实际读取的字节数多 1。
 */
int avio_get_str(AVIOContext *pb, int maxlen, char *buf, int buflen);

/**
 * 从 pb 读取 UTF-16 字符串并将其转换为 UTF-8。当遇到空字符或无效字符或已读取 maxlen 字节时，读取将终止。
 * @return 读取的字节数（始终 <= maxlen）
 */
int avio_get_str16le(AVIOContext *pb, int maxlen, char *buf, int buflen);
int avio_get_str16be(AVIOContext *pb, int maxlen, char *buf, int buflen);


/**
 * @name URL 打开模式 avio_open 的 flags 参数必须是以下常量之一，可以选择与其他标志进行或运算。
 * @{
 */
#define AVIO_FLAG_READ  1                                      /**< 只读 */
#define AVIO_FLAG_WRITE 2                                      /**< 只写 */
#define AVIO_FLAG_READ_WRITE (AVIO_FLAG_READ|AVIO_FLAG_WRITE)  /**< 读写伪标志 */
/**
 * @}
 */

/**
 * 使用非阻塞模式。如果设置了此标志，则上下文上的操作如果无法立即执行，将返回 AVERROR(EAGAIN)。如果未设置此标志，则对上下文的操作将永远不会返回 AVERROR(EAGAIN)。请注意，该标志不会影响上下文的打开/连接。如有必要，连接协议将始终阻塞（例如在网络协议上），但永远不会挂起（例如在繁忙的设备上）。警告：非阻塞协议正在开发中；该标志可能会被默默地忽略。
 */
#define AVIO_FLAG_NONBLOCK 8

/**
 * 使用直接模式。如果可能的话，avio_read 和 avio_write 应该直接满足，而不是通过缓冲区，并且 avio_seek 将始终直接调用底层的查找函数。
 */
#define AVIO_FLAG_DIRECT 0x8000

/**
 * 创建并初始化一个 AVIOContext 用于访问 url 指示的资源。
 * @note 当 url 指示的资源以读+写模式打开时，AVIOContext 只能用于写入。
 *
 * @param s 用于返回指向创建的 AVIOContext 的指针。如果失败，指向的值将设置为 NULL。
 * @param url 要访问的资源
 * @param flags 控制如何打开 url 指示的资源的标志
 * @return >= 0（成功时），对应于 AVERROR 代码（失败时）的负值
 */
int avio_open(AVIOContext **s, const char *url, int flags);

/**
 * 创建并初始化一个 AVIOContext，用于访问 url 指示的资源。
 * @note 当 url 指示的资源以读+写模式打开时，AVIOContext 只能用于写入。
 *
 * @param s 用于返回指向创建的 AVIOContext 的指针。如果失败，指向的值将设置为 NULL。
 * @param url 用于访问的资源
 * @param flags 控制如何打开 url 指示的资源的标志
 * @param int_cb 在协议级别使用的中断回调
 * @param options 充满协议私有选项的字典。返回时，此参数将被销毁并替换为包含未找​​到的选项的字典。可能为 NULL。如果成功，则为
 * @return >= 0；如果失败，则为对应于 AVERROR 代码的负值
 */
int avio_open2(AVIOContext **s, const char *url, int flags,
               const AVIOInterruptCB *int_cb, AVDictionary **options);

/**
 * 关闭 AVIOContext 访问的资源并释放它。仅当 s 由 avio_open() 打开时才能使用此函数。
 *
 * 在关闭资源之前，内部缓冲区会自动刷新。
 *
 * @return 成功时为 0，错误时 AVERROR < 0。
 * @see avio_closep
 */
int avio_close(AVIOContext *s);

/**
 * 关闭 AVIOContext *s 访问的资源，释放它并将指向它的指针设置为 NULL。仅当 s 由 avio_open() 打开时才能使用此函数。
 *
 * 在关闭资源之前，内部缓冲区会自动刷新。
 *
 * @return 成功时为 0，错误时 AVERROR < 0。
 * @see avio_close
 */
int avio_closep(AVIOContext **s);


/**
 * 打开只写内存流。
 *
 * @param s 新 IO 上下文
 * @return 如果没有错误则为零。
 */
int avio_open_dyn_buf(AVIOContext **s);

/**
 * 返回已写入的大小和指向缓冲区的指针。AVIOContext 流保持不变。不得释放该缓冲区，也不会向缓冲区添加填充。
 *
 * @param s IO 上下文
 * @param pbuffer 指向字节缓冲区的指针
 * @return 字节缓冲区的长度
 */
int avio_get_dyn_buf(AVIOContext *s, uint8_t **pbuffer);

/**
 * 返回已写入的大小和指向缓冲区的指针。必须使用 av_free() 释放该缓冲区。缓冲区会添加 AV_INPUT_BUFFER_PADDING_SIZE 大小的填充。
 *
 * @param s IO 上下文
 * @param pbuffer 指向字节缓冲区的指针
 * @return 字节缓冲区的长度
 */
int avio_close_dyn_buf(AVIOContext *s, uint8_t **pbuffer);

/**
 * 迭代可用协议的名称。
 *
 * @param opaque 表示当前协议的私有指针。它在第一次迭代时必须是指向 NULL 的指针，并将通过连续调用 avio_enum_protocols 进行更新。
 * @param output 如果设置为 1，则迭代输出协议，否则迭代输入协议。
 *
 * @return 包含当前协议名称的静态字符串或 NULL
 */
const char *avio_enum_protocols(void **opaque, int output);

/**
 * 通过可用协议名称获取 AVClass。
 *
 * @return 输入协议名称的 AVClass 或 NULL
 */
const AVClass *avio_protocol_get_class(const char *name);

/**
 * 暂停和恢复播放 - 仅在使用网络流协议（例如 MMS）时才有意义。
 *
 * @param h 从中调用 read_pause 函数指针的 IO 上下文
 * @param pause 1 表示暂停，0 表示恢复
 */
int     avio_pause(AVIOContext *h, int pause);

/**
 * 查找相对于某个组件流的给定时间戳。仅在使用网络流协议（例如彩信）时才有意义。
 *
 * @param h 从中调用查找函数指针的 IO 上下文
 * @param stream_index 时间戳相关的流索引。如果stream_index为(-1)，则时间戳应以AV_TIME_BASE为单位，从演示开始算起。如果使用stream_index >= 0并且协议不支持基于组件流的查找，则调用将失败。
 * @param timestamp 时间戳，以 AVStream.time_base 为单位，或者如果没有指定流，则以 AV_TIME_BASE 为单位。
 * @param flags AVSEEK_FLAG_BACKWARD、AVSEEK_FLAG_BYTE 和 AVSEEK_FLAG_ANY 的可选组合。该协议可能会默默地忽略 AVSEEK_FLAG_BACKWARD 和 AVSEEK_FLAG_ANY，但如果使用但不支持 AVSEEK_FLAG_BYTE 将失败。
 * @return >= 0 成功
 * @see AVInputFormat::read_seek
 */
int64_t avio_seek_time(AVIOContext *h, int stream_index,
                       int64_t timestamp, int flags);

/* 避免警告。无法包含标头，因为它破坏了 C++。 */
struct AVBPrint;

/**
 * 将 h 的内容读入打印缓冲区，最多为 max_size 字节，或最多为 EOF。
 *
 * @return 0 表示成功（读取的 max_size 字节或达到 EOF），否则为负错误代码
 */
int avio_read_to_bprint(AVIOContext *h, struct AVBPrint *pb, size_t max_size);

/**
 * 接受并在服务器上下文上分配客户端上下文。
 * @param  s 服务器上下文
 * @param  c 客户端上下文，必须未分配
 * @return >= 0（成功时）或对应于 AVERROR（失败时）的负值
 */
int avio_accept(AVIOContext *s, AVIOContext **c);

/**
 * 执行协议握手的一个步骤以接受新客户端。在将其用作读/写上下文之前，必须在 avio_accept() 返回的客户端上调用此函数。它与 avio_accept() 是分开的，因为它可能会阻塞。握手的步骤由应用程序可能决定改变进程的位置来定义。例如，在具有请求标头和答复标头的协议上，每个标头都可以构成一个步骤，因为应用程序可以使用请求中的参数来更改答复中的参数；或者请求的每个单独块都可以构成一个步骤。如果握手已经完成，avio_handshake() 不执行任何操作并立即返回 0。
 *
 * @param  c 在
 * @return 上执行握手的客户端上下文 0 一次完整且成功的握手 > 0 如果握手进行，但未完成 < 0 对于 AVERROR 代码
 */
int avio_handshake(AVIOContext *c);
#endif /* AVFORMAT_AVIO_H */
