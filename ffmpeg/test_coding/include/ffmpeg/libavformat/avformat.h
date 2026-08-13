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

#ifndef AVFORMAT_AVFORMAT_H
#define AVFORMAT_AVFORMAT_H

/**
 * @file
 * @ingroup libavf 主要 libavformat 公共 API 标头
 */

/**
 * @defgroup libavf libavformat I/O 和复用/解复用库
 *
 * Libavformat (lavf) 是一个用于处理各种媒体容器格式的库。它的主要两个目的是解复用——即将媒体文件分割成组件流，以及复用的逆过程——以指定的容器格式写入提供的数据。它还具有一个 @ref lavf_io“I/O 模块”，支持多种访问数据的协议（例如文件、tcp、http 等）。除非您绝对确定不会使用 libavformat 的网络功能，否则您还应该调用 avformat_network_init()。
 *
 * 支持的输入格式由 AVInputFormat 结构描述，相反，输出格式由 AVOutputFormat 描述。您可以使用 av_demuxer_iterate / av_muxer_iterate() 函数迭代所有输入/输出格式。协议层不是公共 API 的一部分，因此您只能使用 avio_enum_protocols() 函数获取支持的协议的名称。
 *
 * 用于复用和解复用的主要 lavf 结构是 AVFormatContext，它导出有关正在读取或写入的文件的所有信息。与大多数 Libavformat 结构一样，它的大小不是公共 ABI 的一部分，因此不能在堆栈上分配或直接使用 av_malloc() 分配。要创建 AVFormatContext，请使用 avformat_alloc_context() （某些函数，如 avformat_open_input() 可能会为您执行此操作）。
 *
 * 最重要的是 AVFormatContext 包含：
 * @li @ref AVFormatContext.iformat“输入”或@ref AVFormatContext.oformat“输出”格式。它可以自动检测或由用户设置输入；始终由用户设置输出。
 * @li AVStreams 的 @ref AVFormatContext.streams“数组”，它描述了存储在文件中的所有基本流。通常使用该数组中的索引来引用 AVStream。
 * @li 一个 @ref AVFormatContext.pb“I/O 上下文”。它要么由 lavf 打开，要么由用户设置用于输入，始终由用户设置用于输出（除非您正在处理 AVFMT_NOFILE 格式）。
 *
 * @section lavf_options 将选项传递给（解）复用器 可以使用 @ref avoptions 机制配置 lavf 复用器和解复用器。通用（与格式无关）libavformat 选项由 AVFormatContext 提供，可以通过在分配的 AVFormatContext（或其来自 avformat_get_class() 的 AVClass）上调用 av_opt_next() / av_opt_find() 从用户程序中检查它们。当且仅当相应格式结构的 AVInputFormat.priv_class / AVOutputFormat.priv_class 为非 NULL 时，AVFormatContext.priv_data 才提供私有（特定于格式）选项。如果 AVClass 为非 NULL，则 @ref AVFormatContext.pb“I/O 上下文”和协议层可以提供更多选项。请参阅 @ref avoptions 文档中有关嵌套的讨论，了解如何访问它们。
 *
 * @section urls libavformat 中的 URL 字符串由方案/协议、“:”和方案特定字符串组成。支持不带方案的 URL 以及用于本地文件的“:”，但已弃用。 “file:”应用于本地文件。
 *
 * 重要的是，未经检查不得从不受信任的来源获取方案字符串。
 *
 * 请注意，某些方案/协议非常强大，允许访问本地和远程文件、文件的一部分、文件的串联、本地音频和视频设备等。
 *
 * @{
 *
 * @defgroup lavf_decoding 解复用器
 * @{
 * 解复用器读取媒体文件并将其拆分为数据块（@em 数据包）。 @ref AVPacket“数据包”包含属于单个基本流的一个或多个编码帧。在 lavf API 中，此过程由用于打开文件的 avformat_open_input() 函数、用于读取单个数据包的 av_read_frame() 以及最后进行清理的 avformat_close_input() 函数来表示。
 *
 * @section lavf_decoding_open 打开媒体文件 打开文件所需的最少信息是其 URL，该 URL 被传递给 avformat_open_input()，如以下代码所示：
 * @code
 * const char    *url = "file:in.mp3";
 * AVFormatContext *s = NULL;
 * int ret = avformat_open_input(&s, url, NULL, NULL);
 * if (ret < 0)
 *     abort();
 * @endcode
 * 上面的代码尝试分配一个 AVFormatContext，打开指定的文件（自动检测格式）并读取标头，将其中存储的信息导出到 s 中。某些格式没有标头或没有存储足够的信息，因此建议您调用 avformat_find_stream_info() 函数，该函数尝试读取和解码一些帧以查找丢失的信息。
 *
 * 在某些情况下，您可能希望自己使用 avformat_alloc_context() 预先分配 AVFormatContext，并在将其传递给 avformat_open_input() 之前对其进行一些调整。其中一种情况是当您想要使用自定义函数而不是 lavf 内部 I/O 层来读取输入数据时。为此，请使用 avio_alloc_context() 创建您自己的 AVIOContext，并将您的读取回调传递给它。然后将 AVFormatContext 的 @em pb 字段设置为新创建的 AVIOContext。
 *
 * 由于打开文件的格式通常在 avformat_open_input() 返回后才知道，因此不可能在预分配的上下文上设置分路器私有选项。相反，这些选项应该传递给包含在 AVDictionary 中的 avformat_open_input()：
 * @code
 * AVDictionary *options = NULL;
 * av_dict_set(&options, "video_size", "640x480", 0);
 * av_dict_set(&options, "pixel_format", "rgb24", 0);
 *
 * if (avformat_open_input(&s, url, NULL, &options) < 0)
 *     abort();
 * av_dict_free(&options);
 * @endcode
 * 此代码将私有选项“video_size”和“pixel_format”传递给解复用器。例如，它们是必要的。原始视频解复用器，因为它不知道如何解释原始视频数据。如果格式与原始视频不同，则解复用器将无法识别这些选项，因此不会应用。然后，此类无法识别的选项将返回到选项字典中（已识别的选项将被消耗）。调用程序可以根据需要处理此类无法识别的选项，例如
 * @code
 * const AVDictionaryEntry *e;
 * if ((e = av_dict_iterate(options, NULL))) {
 *     fprintf(stderr, "Option %s not recognized by the demuxer.\n", e->key);
 *     abort();
 * }
 * @endcode
 *
 * 读取完文件后，必须使用 avformat_close_input() 关闭它。它将释放与该文件关联的所有内容。
 *
 * @section lavf_decoding_read 从打开的文件中读取 从打开的 AVFormatContext 中读取数据是通过重复调用 av_read_frame() 来完成的。每次调用如果成功，将返回一个 AVPacket，其中包含一个 AVStream 的编码数据，由 AVPacket.stream_index 标识。如果调用者希望解码数据，则可以将该数据包直接传递到 libavcodec 解码函数 avcodec_send_packet() 或 avcodec_decode_subtitle2() 中。
 *
 * AVPacket.pts、AVPacket.dts 和 AVPacket.duration 计时信息（如果已知）将被设置。如果流不提供它们，它们也可能被取消设置（即 pts/dts 为 AV_NOPTS_VALUE，持续时间为 0）。计时信息将以 AVStream.time_base 为单位，即必须乘以时基才能将其转换为秒。
 *
 * av_read_frame() 返回的数据包始终是引用计数的，即 AVPacket.buf 已设置并且用户可以无限期地保留它。当不再需要数据包时，必须使用 av_packet_unref() 释放该数据包。
 *
 * @section lavf_decoding_seek 寻求
 * @}
 *
 * @defgroup lavf_encoding 复用
 * @{
 * 复用器以@ref AVPacket“AVPackets”的形式获取编码数据，并将其写入指定容器格式的文件或其他输出字节流中。
 *
 * 用于复用的主要 API 函数是用于写入文件头的 avformat_write_header()、用于写入数据包的 av_write_frame() / av_interleaved_write_frame() 以及用于最终确定文件的 av_write_trailer()。
 *
 * 在多路复用过程开始时，调用者必须首先调用 avformat_alloc_context() 创建多路复用上下文。然后，调用者通过填写此上下文中的各个字段来设置复用器：
 *
 * - 必须设置@ref AVFormatContext.oformat“oformat”字段以选择将使用的复用器。 - 除非格式是 AVFMT_NOFILE 类型，否则@ref AVFormatContext.pb“pb”字段必须设置为打开的 IO 上下文，可以从 avio_open2() 返回，也可以是自定义的。 - 除非格式是 AVFMT_NOSTREAMS 类型，否则必须使用 avformat_new_stream() 函数创建至少一个流。调用者应填写@ref AVStream.codecpar“流编解码器参数”信息，例如编解码器@ref AVCodecParameters.codec_type“类型”、@ref AVCodecParameters.codec_id“id”以及已知的其他参数（例如宽度/高度、像素或样本格式等）。 @ref AVStream.time_base“流时基”应设置为调用者希望用于该流的时基（请注意，复用器实际使用的时基可能不同，稍后将进行描述）。 - 建议仅手动初始化 AVCodecParameters 中的相关字段，而不是在重新混合期间使用 @ref avcodec_parameters_copy()：不能保证编解码器上下文值对于输入和输出格式上下文都保持有效。 - 调用者可以填写附加信息，例如@ref AVFormatContext.metadata“全局”或@ref AVStream.metadata“每个流”元数据、@ref AVFormatContext.chapters“章节”、@ref AVFormatContext.programs“程序”等，如 AVFormatContext 文档中所述。这些信息是否实际存储在输出中取决于容器格式和复用器支持的内容。
 *
 * 当复用上下文完全设置后，调用者必须调用 avformat_write_header() 来初始化复用器内部并写入文件头。在此步骤中是否实际将任何内容写入 IO 上下文取决于复用器，但必须始终调用此函数。任何复用器私有选项都必须通过 options 参数传递给此函数。
 *
 * 然后通过重复调用 av_write_frame() 或 av_interleaved_write_frame() 将数据发送到复用器（请参阅这些函数的文档以讨论它们之间的差异；只有其中一个可以与单个复用上下文一起使用，它们不应混合）。请注意，发送到复用器的数据包的计时信息必须位于相应 AVStream 的时基中。该时基由复用器设置（在 avformat_write_header() 步骤中），并且可能与调用者请求的时基不同。
 *
 * 一旦所有数据都被写入，调用者必须调用 av_write_trailer() 来刷新所有缓冲的数据包并最终确定输出文件，然后关闭 IO 上下文（如果有），最后使用 avformat_free_context() 释放多路复用上下文。
 * @}
 *
 * @defgroup lavf_io I/O 读/写
 * @{
 * @section lavf_io_dirlist 目录列表 目录列表 API 可以列出远程服务器上的文件。
 *
 * 一些可能的用例： - 用于从远程位置选择文件的“打开文件”对话框， - 递归媒体查找器，为播放器提供播放给定目录中所有文件的能力。
 *
 * @subsection lavf_io_dirlist_open 打开目录 首先，需要通过调用 avio_open_dir() 打开目录，并提供 URL 和可选的包含协议特定参数的 ::AVDictionary。该函数返回零或正整数并在成功时分配 AVIODirContext。
 *
 * @code
 * AVIODirContext *ctx = NULL;
 * if (avio_open_dir(&ctx, "smb://example.com/some_dir", NULL) < 0) {
 *     fprintf(stderr, "Cannot open directory.\n");
 *     abort();
 * }
 * @endcode
 *
 * 此代码尝试使用 smb 协议打开示例目录，无需任何其他参数。
 *
 * @subsection lavf_io_dirlist_read 读取条目 每个目录的条目（即文件、另一个目录、::AVIODirEntryType 中的任何其他内容）都由 AVIODirEntry 表示。从打开的 AVIODirContext 中读取连续条目是通过重复调用 avio_read_dir() 来完成的。如果成功，每次调用都会返回零或正整数。读取 NULL 条目后可以立即停止读取——这意味着没有剩余条目可供读取。以下代码从与 ctx 关联的目录中读取所有条目并将其名称打印到标准输出。
 * @code
 * AVIODirEntry *entry = NULL;
 * for (;;) {
 *     if (avio_read_dir(ctx, &entry) < 0) {
 *         fprintf(stderr, "Cannot list directory.\n");
 *         abort();
 *     }
 *     if (!entry)
 *         break;
 *     printf("%s\n", entry->name);
 *     avio_free_directory_entry(&entry);
 * }
 * @endcode
 * @}
 *
 * @defgroup lavf_codec 解复用器
 * @{
 * @defgroup lavf_codec_native 本机解复用器
 * @{
 * @}
 * @defgroup lavf_codec_wrappers 外部库包装器
 * @{
 * @}
 * @}
 * @defgroup lavf_protos I/O 协议
 * @{
 * @}
 * @defgroup lavf_internal 内部
 * @{
 * @}
 * @}
 */

#include <stdio.h>  /* 文件 */

#include "libavcodec/codec_par.h"
#include "libavcodec/defs.h"
#include "libavcodec/packet.h"

#include "libavutil/dict.h"
#include "libavutil/log.h"

#include "avio.h"
#include "libavformat/version_major.h"
#ifndef HAVE_AV_CONFIG_H
/*
 * 当包含在 ffmpeg 构建中时，仅包含主要版本以避免不必要的重建。当外部包含时，请保留完整的版本信息。
 */
#include "libavformat/version.h"

#include "libavutil/frame.h"
#include "libavcodec/codec.h"
#endif

struct AVFormatContext;
struct AVFrame;

/**
 * @defgroup metadata_api Public Metadata API
 * @{
 * @ingroup libavf
 * 元数据 API 允许 libavformat 在解复用时将元数据标签导出到客户端应用程序。
 * 反过来，客户端应用程序也可以在复用时设置元数据。
 *
 * Metadata is exported or set as pairs of key/value strings in the 'metadata'
 * fields of the AVFormatContext, AVStream, AVChapter and AVProgram structs
 * using the @ref lavu_dict "AVDictionary" API. Like all strings in FFmpeg,
 * metadata is assumed to be UTF-8 encoded Unicode. Note that metadata
 * 解复用器导出的元数据在大多数情况下不会检查其是否为有效的 UTF-8。
 *
 * Important concepts to keep in mind:
 * -  Keys are unique; there can never be 2 tags with the same key. This is
 *    also meant semantically, i.e., a demuxer should not knowingly produce
 *    不应存在多个字面不同但语义相同的键。
 *    E.g., key=Author5, key=Author6. In this example, all authors must be
 *    placed in the same tag.
 * -  Metadata is flat, not hierarchical; there are no subtags. If you
 *    例如，如果需要存储制片人 Alice 的孩子的电子邮件地址
 *    and actor Bob, that could have key=alice_and_bobs_childs_email_address.
 * -  Several modifiers can be applied to the tag name. This is done by
 *    则应按照下面列表中的顺序，在键后追加连字符（'-'）和修饰符名称，
 *    例如 foo-eng-sort，而不是 foo-sort-eng。
 *    -  descriptor -- some formats (e.g. ID3v2 COMM and USLT frames) attach
 *       a free-form descriptor to a tag to distinguish multiple instances.
 *       The full key format is "<tag>-<descriptor>-<lang>", but either
 *       component may be absent. When writing, the last dash-separated suffix
 *       is interpreted as a language code if it is a valid ISO 639-2/B code;
 *       otherwise the entire portion after the first dash is treated as a
 *       descriptor. Examples: "comment-eng" (lang only),
 *       "comment-MusicMatch_Bio-eng" (descriptor + lang),
 *       "comment-foobar" (descriptor only, foobar is not a valid lang code).
 *    -  language -- a tag whose value is localized for a particular language
 *       键后会追加 ISO 639-2/B 三字母语言代码。
 *       For example: Author-ger=Michael, Author-eng=Mike
 *       The original/default language is in the unqualified "Author" tag.
 *       A demuxer should set a default if it sets any translated tag.
 *       When a language is required by the format but not specified in the key
 *       (e.g. ID3v2 COMM and USLT frames), the default is left to the
 *       underlying implementation (ID3v2 defaults to "und").
 *    -  sorting  -- a modified version of a tag that should be used for
 *       sorting will have '-sort' appended. E.g. artist="The Beatles",
 *       artist-sort="Beatles, The".
 * - Some protocols and demuxers support metadata updates. After a successful
 *   call to av_read_frame(), AVFormatContext.event_flags or AVStream.event_flags
 *   will be updated to indicate if metadata changed. In order to detect metadata
 *   changes on a stream, you need to loop through all streams in the AVFormatContext
 *   并检查每一路流各自的 event_flags。
 *
 * -  Demuxers attempt to export metadata in a generic format, however tags
 *    with no generic equivalents are left as they are stored in the container.
 *    Follows a list of generic tag names:
 *
 @verbatim
 album        -- name of the set this work belongs to
 album_artist -- main creator of the set/album, if different from artist.
                 e.g. "Various Artists" for compilation albums.
 artist       -- main creator of the work
 comment      -- any additional description of the file.
                 ID3v2 COMM frames: bare "comment" has no lang or descriptor;
                 "comment-<lang>" for lang only; "comment-<descriptor>-<lang>"
                 for both (see descriptor modifier above).
 composer     -- who composed the work, if different from artist.
 copyright    -- name of copyright holder.
 creation_time-- date when the file was created, preferably in ISO 8601.
 date         -- date when the work was created, preferably in ISO 8601.
 disc         -- number of a subset, e.g. disc in a multi-disc collection.
 disc_subtitle-- title of a subset, e.g. disc subtitle in a multi-disc collection.
 encoder      -- name/settings of the software/hardware that produced the file.
 encoded_by   -- person/group who created the file.
 filename     -- original name of the file.
 genre        -- <self-evident>.
 language     -- main language in which the work is performed, preferably
                 in ISO 639-2 format. Multiple languages can be specified by
                 separating them with commas.
 lyrics       -- lyrics for the work.
                 ID3v2 USLT frames: bare "lyrics" has no lang or descriptor;
                 "lyrics-<lang>" for lang only; "lyrics-<descriptor>-<lang>"
                 for both (see descriptor modifier above).
 performer    -- artist who performed the work, if different from artist.
                 E.g for "Also sprach Zarathustra", artist would be "Richard
                 Strauss" and performer "London Philharmonic Orchestra".
 publisher    -- name of the label/publisher.
 service_name     -- name of the service in broadcasting (channel name).
 service_provider -- name of the service provider in broadcasting.
 title        -- name of the work.
 track        -- number of this work in the set, can be in form current/total.
 variant_bitrate -- the total bitrate of the bitrate variant that the current stream is part of
 @endverbatim
 *
 * Look in the examples section for an application example how to use the Metadata API.
 *
 * @}
 */

/* 数据包功能 */


/**
 * 分配和读取数据包的有效负载，并使用默认值初始化其字段。
 *
 * @param s 关联的 IO 上下文
 * @param pkt 数据包
 * @param size 所需的有效负载大小
 * @return >0（读取大小）如果 OK，则 AVERROR_xxx 否则
 */
int av_get_packet(AVIOContext *s, AVPacket *pkt, int size);


/**
 * 读取数据并将其附加到 AVPacket 的当前内容中。如果 pkt->size 为 0，这与 av_get_packet 相同。请注意，这使用了 av_grow_packet，因此涉及低效的重新分配。因此，仅当没有合理的方法来知道最终大小（的上限）时才应使用此函数。
 *
 * @param s 关联的 IO 上下文
 * @param pkt 数据包
 * @param size 要读取的数据量
 * @return >0 (读取大小) 如果 OK，否则 AVERROR_xxx，即使发生错误，以前的数据也不会丢失。
 */
int av_append_packet(AVIOContext *s, AVPacket *pkt, int size);

/*************************************************/
/* 输入/输出格式 */

struct AVCodecTag;

/**
 * 此结构包含格式必须探测文件的数据。
 */
typedef struct AVProbeData {
    const char *filename;
    unsigned char *buf; /**< 缓冲区必须具有 AVPROBE_PADDING_SIZE 的额外分配字节，并用零填充。 */
    int buf_size;       /**< buf 的大小（额外分配的字节除外） */
    const char *mime_type; /**< mime_type（如果已知）。 */
} AVProbeData;

#define AVPROBE_SCORE_RETRY (AVPROBE_SCORE_MAX/4)
#define AVPROBE_SCORE_STREAM_RETRY (AVPROBE_SCORE_MAX/4-1)

#define AVPROBE_SCORE_EXTENSION  50 ///< 文件扩展名
#define AVPROBE_SCORE_MIME_BONUS 30 ///< 为匹配 mime 类型添加的分数
#define AVPROBE_SCORE_MAX       100 ///< 最大分数

#define AVPROBE_PADDING_SIZE 32             ///< 探测缓冲区末尾额外分配的字节

/// Demuxer 将使用 avio_open，调用者不应提供打开的文件。
#define AVFMT_NOFILE        0x0001
#define AVFMT_NEEDNUMBER    0x0002 /**< 文件名中需要“%d”。 */
/**
 * 复用器/解复用器是实验性的，应谨慎使用。
 *
 * 不会自动选择，必须明确指定。
 */
#define AVFMT_EXPERIMENTAL  0x0004
#define AVFMT_SHOW_IDS      0x0008 /**< 显示格式流 ID 编号。 */
#define AVFMT_GLOBALHEADER  0x0040 /**< 格式需要全局标头。 */
#define AVFMT_NOTIMESTAMPS  0x0080 /**< 格式不需要/有任何时间戳。 */
#define AVFMT_GENERIC_INDEX 0x0100 /**< 使用通用索引构建代码。 */
#define AVFMT_TS_DISCONT    0x0200 /**< 格式允许时间戳不连续。请注意，复用器始终需要有效（单调）时间戳 */
#define AVFMT_VARIABLE_FPS  0x0400 /**< 格式允许可变 fps。 */
#define AVFMT_NODIMENSIONS  0x0800 /**< 格式不需要宽度/高度 */
#define AVFMT_NOSTREAMS     0x1000 /**< 格式不需要任何流 */
#define AVFMT_NOBINSEARCH   0x2000 /**< 格式不允许通过 read_timestamp 回退到二分搜索 */
#define AVFMT_NOGENSEARCH   0x4000 /**< 格式不允许回退到通用搜索 */
#define AVFMT_NO_BYTE_SEEK  0x8000 /**< 格式不允许按字节查找 */
#define AVFMT_TS_NONSTRICT 0x20000 /**
 * < 格式不需要严格增加时间戳，但它们仍然必须是单调的
 */
#define AVFMT_TS_NEGATIVE  0x40000 /**
 * < 格式允许混合负时间戳。如果未设置，时间戳将在 av_write_frame 和 av_interleaved_write_frame 中移动，因此它们从 0 开始。用户或复用器可以通过 AVFormatContext.avoid_negative_ts
 */
#define AVFMT_FIXED_FRAMESIZE 0x80000 /**覆盖此设置<格式需要@ref AVCodecParameters.frame_size“固定大小的音频帧”。 */

#define AVFMT_SEEK_TO_PTS   0x4000000 /**< 搜索基于 PTS */

/**
 * @addtogroup lavf_encoding
 * @{
 */
typedef struct AVOutputFormat {
    const char *name;
    /**
 * 格式的描述性名称，意味着比名称更易于理解。您应该使用 NULL_IF_CONFIG_SMALL() 宏来定义它。
 */
    const char *long_name;
    const char *mime_type;
    const char *extensions; /**< 逗号分隔的文件扩展名 */
    /* 输出支持 */
    enum AVCodecID audio_codec;    /**< 默认音频编解码器 */
    enum AVCodecID video_codec;    /**< 默认视频编解码器 */
    enum AVCodecID subtitle_codec; /**< 默认字幕编解码器 */
    /**
 * 可以使用标志：AVFMT_NOFILE、AVFMT_NEEDNUMBER、AVFMT_EXPERIMENTAL、AVFMT_GLOBALHEADER、AVFMT_NOTIMESTAMPS、AVFMT_VARIABLE_FPS、AVFMT_NODIMENSIONS、AVFMT_NOSTREAMS、AVFMT_TS_NONSTRICT、AVFMT_TS_NEGATIVE、 AVFMT_FIXED_FRAMESIZE
 */
    int flags;

    /**
 * 支持的 codec_id-codec_tag 对列表，按“更好的选择优先”排序。数组均以 AV_CODEC_ID_NONE 终止。
 */
    const struct AVCodecTag * const *codec_tag;


    const AVClass *priv_class; ///< 私有上下文的 AVClass
} AVOutputFormat;
/**
 * @}
 */

/**
 * @addtogroup lavf_decoding
 * @{
 */
typedef struct AVInputFormat {
    /**
 * 格式的短名称的逗号分隔列表。新名称可能会附加一个小凸起。
 */
    const char *name;

    /**
 * 格式的描述性名称，比名称更易于理解。您应该使用 NULL_IF_CONFIG_SMALL() 宏来定义它。
 */
    const char *long_name;

    /**
 * 可以使用标志：AVFMT_NOFILE、AVFMT_NEEDNUMBER、AVFMT_EXPERIMENTAL、AVFMT_SHOW_IDS、AVFMT_NOTIMESTAMPS、AVFMT_GENERIC_INDEX、AVFMT_TS_DISCONT、AVFMT_NOBINSEARCH、 AVFMT_NOGENSEARCH、AVFMT_NO_BYTE_SEEK、AVFMT_SEEK_TO_PTS。
 */
    int flags;

    /**
 * 如果定义了扩展，则不会进行任何探测。您通常不应该使用扩展格式猜测，因为它不够可靠
 */
    const char *extensions;

    const struct AVCodecTag * const *codec_tag;

    const AVClass *priv_class; ///< AVClass 对于私有上下文

    /**
 * 以逗号分隔的 MIME 类型列表。它用于在探测时检查匹配的 mime 类型。
 * @see av_probe_input_format2
 */
    const char *mime_type;
} AVInputFormat;
/**
 * @}
 */

enum AVStreamParseType {
    AVSTREAM_PARSE_NONE,
    AVSTREAM_PARSE_FULL,       /**< 完全解析和重新打包 */
    AVSTREAM_PARSE_HEADERS,    /**< 仅解析标头，不重新打包。 */
    AVSTREAM_PARSE_TIMESTAMPS, /**< 对不在数据包边界上开始的帧进行时间戳的完全解析和插值 */
    AVSTREAM_PARSE_FULL_ONCE,  /**< 仅对第一帧进行完全解析和重新打包，当前仅针对 H.264 实现 */
    AVSTREAM_PARSE_FULL_RAW,   /**
 * < 完全解析和重新打包，并由解析器生成原始的时间戳和位置假设文件中的每个数据包不包含解复用器级别标头，而仅包含编解码器级别数据，否则位置生成将失败
 */
};

typedef struct AVIndexEntry {
    int64_t pos;
    int64_t timestamp;        /**
 * <
 * AVStream.time_base 单位中的时间戳，最好是在查找此条目时正确解码的帧可用的时间。这意味着基于关键帧的格式更适合 PTS。但解复用器可以选择存储不同的时间戳，如果这样更方便实现或者没有更好的已知的话
 */
#define AVINDEX_KEYFRAME 0x0001
#define AVINDEX_DISCARD_FRAME  0x0002    /**
 * 标志用于指示解码后应丢弃哪个帧。
 */
    int flags:2;
    int size:30; //是的，尝试保持较小的大小以减少内存需求（由于可能的 8 字节对齐，它是 24 与 32 字节）。
    int min_distance;         /**< 此关键帧与上一个关键帧之间的最小距离，用于避免不必要的搜索。 */
} AVIndexEntry;

/**
 * 除非用户明确指定，否则应在同一类型的其他流中默认选择该流。
 */
#define AV_DISPOSITION_DEFAULT              (1 << 0)
/**
 * 该流不是原始语言。
 *
 * @note AV_DISPOSITION_ORIGINAL 是此配置的逆过程。最多应将其中之一设置在正确标记的流中。
 * @note 此配置可以适用于任何流类型，而不仅仅是音频。
 */
#define AV_DISPOSITION_DUB                  (1 << 1)
/**
 * 该直播为原始语言。
 *
 * @see AV_DISPOSITION_DUB 的注释
 */
#define AV_DISPOSITION_ORIGINAL             (1 << 2)
/**
 * 该流是评论曲目。
 */
#define AV_DISPOSITION_COMMENT              (1 << 3)
/**
 * 该流包含歌词。
 */
#define AV_DISPOSITION_LYRICS               (1 << 4)
/**
 * 该流包含卡拉 OK 音频。默认情况下，播放期间应使用
 */
#define AV_DISPOSITION_KARAOKE              (1 << 5)

/**
 * 轨道。对于即使用户没有明确要求字幕也应该显示的字幕轨道很有用。
 */
#define AV_DISPOSITION_FORCED               (1 << 6)
/**
 * 该直播面向听力受损的观众。
 */
#define AV_DISPOSITION_HEARING_IMPAIRED     (1 << 7)
/**
 * 该直播面向视障观众。
 */
#define AV_DISPOSITION_VISUAL_IMPAIRED      (1 << 8)
/**
 * 音频流包含音乐和音效，但不含语音。
 */
#define AV_DISPOSITION_CLEAN_EFFECTS        (1 << 9)
/**
 * 该流作为附加图片/“封面艺术”存储在文件中（例如 ID3v2 中的 APIC 帧）。除非发生查找，否则与其关联的第一个（通常是唯一）数据包将在从文件读取的前几个数据包中返回。也可以随时在AVStream.attached_pic中访问。
 */
#define AV_DISPOSITION_ATTACHED_PIC         (1 << 10)
/**
 * 该流是稀疏的，并且包含缩略图，通常对应于章节标记。仅与 AV_DISPOSITION_ATTACHED_PIC 一起使用。
 */
#define AV_DISPOSITION_TIMED_THUMBNAILS     (1 << 11)

/**
 * 该流旨在与空间音轨混合。例如，它可以用于旁白或立体声音乐，并且可以通过听众头部旋转而保持不变。
 */
#define AV_DISPOSITION_NON_DIEGETIC         (1 << 12)

/**
 * 字幕流包含字幕，提供转录并可能提供音频翻译。通常面向有听力障碍的观众。
 */
#define AV_DISPOSITION_CAPTIONS             (1 << 16)
/**
 * 字幕流包含视频内容的文本描述。通常适用于视障观众或无法观看视频的情况。
 */
#define AV_DISPOSITION_DESCRIPTIONS         (1 << 17)
/**
 * 字幕流包含不打算直接呈现给用户的时间对齐元数据。
 */
#define AV_DISPOSITION_METADATA             (1 << 18)
/**
 * 该流旨在在呈现之前与另一个流混合。例如，用于指示流包含 HEIF 网格的图像部分，或用于 mpegts 中的 mix_type=0。
 */
#define AV_DISPOSITION_DEPENDENT            (1 << 19)
/**
 * 视频流包含静止图像。
 */
#define AV_DISPOSITION_STILL_IMAGE          (1 << 20)
/**
 * 视频流包含多个层，例如立体视图（参见 H.264 附件 G/H 或 HEVC 附件 F）。
 */
#define AV_DISPOSITION_MULTILAYER           (1 << 21)

/**
 * @return AV_DISPOSITION_* 标志对应于 disp 或负错误代码（如果 disp 不对应于已知的流配置）。
 */
int av_disposition_from_string(const char *disp);

/**
 * @param disposition AV_DISPOSITION_* 值的组合
 * @return 对应于处置中最低设置位的字符串描述。当最低设置位不对应于已知配置或当配置为 0 时，为 NULL。
 */
const char *av_disposition_to_string(int disposition);

/**
 * 时间戳回绕检测的行为选项。
 */
#define AV_PTS_WRAP_IGNORE      0   ///< 忽略换行
#define AV_PTS_WRAP_ADD_OFFSET  1   ///< 添加换行检测时的​​格式特定偏移
#define AV_PTS_WRAP_SUB_OFFSET  -1  ///< 减去换行检测时的​​格式特定偏移

/**
 * 流结构。新字段可以添加到末尾并进行较小的版本更新。对现有字段的删除、重新排序和更改需要主要版本更新。 sizeof(AVStream) 不得在 libav* 之外使用。
 */
typedef struct AVStream {
    /**
 * @ref avoptions 的类。设置流创建。
 */
    const AVClass *av_class;

    int index;    /**< AVFormatContext 中的流索引 */
    /**
 * 格式特定的流 ID。解码：由 libavformat 设置 编码：由用户设置，如果未设置，则替换为 libavformat
 */
    int id;

    /**
 * 与此流关联的编解码器参数。由 libavformat 分别在 avformat_new_stream() 和 avformat_free_context() 中分配和释放。
 *
 * - 解复用：在流创建时或在 avformat_find_stream_info() 中由 libavformat 填充 - 复用：在 avformat_write_header() 之前由调用者填充
 */
    AVCodecParameters *codecpar;

    void *priv_data;

    /**
 * 这是表示帧时间戳的基本时间单位（以秒为单位）。
 *
 * 解码：由 libavformat 设置 编码：可以由调用者在 avformat_write_header() 之前设置，以向复用器提供有关所需时基的提示。在 avformat_write_header() 中，复用器将使用实际用于写入文件的时间戳的时基覆盖此字段（该时基可能与用户提供的时间戳相关，也可能无关，具体取决于格式）。
 */
    AVRational time_base;

    /**
 * 解码：流时基中按呈现顺序排列的流第一帧的点。仅当您绝对 100% sure 确定您设置的值确实是第一帧的 pts 时才设置此值。这可能是未定义的（AV_NOPTS_VALUE）。
 * @note ASF 标头不包含正确的 start_time，ASF 解复用器不得设置此值。
 */
    int64_t start_time;

    /**
 * 解码：流的持续时间，以流时基为单位。如果源文件未指定持续时间，但指定了比特率，则将根据比特率和文件大小估计该值。
 *
 * 编码：可以由调用者在 avformat_write_header() 之前设置，以向复用器提供有关估计持续时间的提示。
 */
    int64_t duration;

    int64_t nb_frames;                 ///< 此流中的帧数（如果已知）或 0

    /**
 * 流配置 - AV_DISPOSITION_* 标志的组合。 - 解复用：在创建流时或在 avformat_find_stream_info() 中由 libavformat 设置。 - 多路复用：可以由调用者在 avformat_write_header() 之前设置。
 */
    int disposition;

    enum AVDiscard discard; ///< 选择哪些数据包可以随意丢弃，不需要解复用。

    /**
 * 样本宽高比（如果未知则为 0） - 编码：由用户设置。 - 解码：由 libavformat 设置。
 */
    AVRational sample_aspect_ratio;

    AVDictionary *metadata;

    /**
 * 平均帧速率
 *
 * - 解复用：可以在创建流时或在 avformat_find_stream_info() 中由 libavformat 设置。 - 多路复用：可以由调用者在 avformat_write_header() 之前设置。
 */
    AVRational avg_frame_rate;

    /**
 * 对于具有 AV_DISPOSITION_ATTACHED_PIC 配置的流，此数据包将包含附加图片。
 *
 * 解码：由 libavformat 设置，调用者不得修改。编码：未使用
 */
    AVPacket attached_pic;

    /**
 * 指示流上发生的事件的标志，AVSTREAM_EVENT_FLAG_* 的组合。
 *
 * - 解复用：可以由解复用器在 avformat_open_input()、avformat_find_stream_info() 和 av_read_frame() 中设置。处理事件后，用户必须清除标志。 - 多路复用：可以由用户在 avformat_write_header() 之后设置。指示用户触发的事件。  复用器将清除它在 av_[interleaved]_write_frame() 中处理的事件的标志。
 */
    int event_flags;
/**
 * - 解复用：解复用器从文件中读取新的元数据并相应地更新 AVStream.metadata - 复用：用户更新了 AVStream.metadata 并希望复用器将其写入文件
 */
#define AVSTREAM_EVENT_FLAG_METADATA_UPDATED 0x0001
/**
 * - 解复用：从文件中读取该流的新数据包。此事件仅供参考，并不保证该流的新数据包一定会从 av_read_frame() 返回。
 */
#define AVSTREAM_EVENT_FLAG_NEW_PACKETS (1 << 1)

    /**
 * 流的实际基本帧速率。这是可以准确表示所有时间戳的最低帧速率（它是流中所有帧速率的最小公倍数）。请注意，该值只是一个猜测！例如，如果时基为 1/90000，并且所有帧都有大约 3600 或 1800 个计时器滴答，则 r_frame_rate 将为 50/1。
 */
    AVRational r_frame_rate;

    /**
 * 时间戳中的位数。用于包裹控制。
 *
 * - 解复用：由 libavformat 设置 - 复用：由 libavformat 设置
 */
    int pts_wrap_bits;
} AVStream;

/**
 * AVStreamGroupTileGrid 保存有关如何在单个画布上组合多个独立图像以进行演示的信息。
 *
 * 输出应该是@ref AVStreamGroupTileGrid.background“背景”彩色@ref AVStreamGroupTileGrid.coded_width“coded_width”x @ref AVStreamGroupTileGrid.coded_height“coded_height”画布，其中@ref AVStreamGroupTileGrid.nb_tiles“nb_tiles”数量的图块按照它们在@ref AVStreamGroupTileGrid.offsets中出现的顺序放置“offsets”数组，位于为它们描述的确切偏移量处。特别是，如果两个或多个图块重叠，则 @ref AVStreamGroupTileGrid.offsets“offsets”数组中索引较高的图像优先。请注意，单个图像可以多次使用，即@ref AVStreamGroupTileGrid.offsets“offsets”中的多个条目可能具有相同的 idx 值。
 *
 * 以下是一个 3 行 4 列的简单网格示例：
 *
 * +---+---+---+---+
 * | 0 | 1 | 2 | 3 |
 * +---+---+---+---+
 * | 4 | 5 | 6 | 7 |
 * +---+---+---+---+
 * | 8 | 9 |10 |11 |
 * +---+---+---+---+
 *
 * 假设所有图块的尺寸为 512x512，则组中第一个 @ref AVStreamGroup.streams“流”的左上角像素的 @ref AVStreamGroupTileGrid.offsets“偏移量”为“0,0”，组中第二个@ref AVStreamGroup.streams“流”的左上角像素的@ref AVStreamGroupTileGrid.offsets“偏移”是“512,0”，组中第五个@ref AVStreamGroup.streams“流”的@ref AVStreamGroupTileGrid.offsets“偏移”是“0,512”，@ref AVStreamGroupTileGrid.offsets 组中第六个@ref AVStreamGroup.streams“流”的左上角像素的“偏移量”为“512,512”等。
 *
 * 以下是具有重叠图块的画布的示例：
 *
 * +-----------+
 * |   %%%%%   | |***%%3%%@@@| |**0%%%%%2@@| |***##1@@@@@| |   #####   |
 * +-----------+
 *
 * 假设画布尺寸为 1024x1024，所有图块尺寸为 512x512，组中第一个 @ref AVStreamGroup.streams“流”的左上角像素的可能 @ref AVStreamGroupTileGrid.offsets“偏移”为 0x256，第二个 @ref 左上角像素的 @ref AVStreamGroupTileGrid.offsets“偏移”组中的 AVStreamGroup.streams“流”将为 256x512，组中第三个 @ref AVStreamGroup.streams“流”的左上角像素的 @ref AVStreamGroupTileGrid.offsets“偏移”将为 512x256，组中第四个 @ref AVStreamGroup.streams“流”的左上角像素的 @ref AVStreamGroupTileGrid.offsets“偏移”将是 256x0。
 *
 * sizeof(AVStreamGroupTileGrid) 不是 ABI 的一部分，只能由 avformat_stream_group_create() 分配。
 */
typedef struct AVStreamGroupTileGrid {
    const AVClass *av_class;

    /**
 * 网格中的图块数量。
 *
 * 必须 > 0。
 */
    unsigned int nb_tiles;

    /**
 * 画布的宽度。
 *
 * 必须 > 0。
 */
    int coded_width;
    /**
 * 画布的宽度。
 *
 * 必须 > 0。
 */
    int coded_height;

    /**
 * @ref nb_tiles 大小的数组，以像素为单位距画布左上角边缘的偏移量，指示每个流应放置的位置。它必须使用 av_malloc() 系列函数进行分配。
 *
 * - 解复用：由 libavformat 设置，调用者不得修改。 - 混合：由调用者在 avformat_write_header() 之前设置。
 *
 * 由 libavformat 在 avformat_free_context() 中释放。
 */
    struct {
        /**
 * 此图块引用的组中流的索引。
 *
 * 必须是 < @ref AVStreamGroup.nb_streams "nb_streams"。
 */
        unsigned int idx;
        /**
 * 距应放置图块的画布左边缘的偏移量（以像素为单位）。
 */
        int horizontal;
        /**
 * 距应放置图块的画布上边缘的偏移量（以像素为单位）。
 */
        int vertical;
    } *offsets;

    /**
 * 如果任何图块的像素均不位于特定像素位置，则使用 RGBA 格式的每个通道的像素值。
 *
 * @see av_image_fill_color()。
 * @see av_parse_color()。
 */
    uint8_t background[4];

    /**
 * 距画布左边缘（用于演示的实际图像开始处）的偏移量（以像素为单位）。
 *
 * 该字段必须 >= 0 且 < @ref coded_width。
 */
    int horizontal_offset;
    /**
 * 距画布上边缘（用于演示的实际图像开始处）的偏移量（以像素为单位）。
 *
 * 该字段必须 >= 0 且 < @ref coded_height。
 */
    int vertical_offset;

    /**
 * 用于演示的最终图像的宽度。
 *
 * 必须 > 0 且 <= (@ref coded_width - @ref Horizo​​ntal_offset)。当它不等于（@ref coded_width - @refhorizo​​ntal_offset）时，（@ref coded_width - width - @refhorizo​​ntal_offset）的结果是在呈现之前从最终图像的右边缘裁剪的像素量。
 */
    int width;
    /**
 * 用于演示的最终图像的高度。
 *
 * 必须 > 0 且 <= (@ref coded_height - @ref Vertical_offset)。当它不等于（@ref coded_height - @ref Vertical_offset）时，（@ref coded_height - height - @ref Vertical_offset）的结果是在呈现之前从最终图像的底部边缘裁剪的像素量。
 */
    int height;

    /**
 * 与网格关联的附加数据。
 *
 * 应使用 av_packet_side_data_new() 或 av_packet_side_data_add() 分配，并将由 avformat_free_context() 释放。
 */
    AVPacketSideData *coded_side_data;

    /**
 * @ref coded_side_data 中的条目数量。
 */
    int nb_coded_side_data;
} AVStreamGroupTileGrid;

/**
 * AVStreamGroupLayeredVideo 旨在定义共同形成单层视频呈现的基础层视频流和单独的增强层流之间的关系（例如，包含 LCEVC 增强层 NALU 或 Dolby Vision Profile 7 双层编码的视频流和数据流）。
 *
 * 增强层流由@ref el_index标识。
 */
typedef struct AVStreamGroupLayeredVideo {
    const AVClass *av_class;

    /**
 * AVStreamGroup 中增强层流的索引。
 */
#if FF_API_LCEVC_STRUCT
    union {
#endif
        unsigned int el_index;
#if FF_API_LCEVC_STRUCT
        /**
 * @ref el_index 的别名，保留是为了向后兼容。
 */
        attribute_deprecated
        unsigned int lcevc_index;
    };
#endif
    /**
 * 用于演示的最终流的宽度。
 */
    int width;
    /**
 * 用于演示的最终图像的高度。
 */
    int height;
} AVStreamGroupLayeredVideo;

#if FF_API_LCEVC_STRUCT
/**
 * 保留别名以实现向后兼容性。
 *
 * AVStreamGroupLCEVC 已重命名为@ref AVStreamGroupLayeredVideo。
 */
#define AVStreamGroupLCEVC AVStreamGroupLayeredVideo
#endif

/**
 * AVStreamGroupTREF 旨在定义视频、音频或字幕流与包含元数据的数据流之间的关系。
 *
 * 不得存在超过一个 @ref AVCodecParameters.codec_type "codec_type" AVMEDIA_TYPE_DATA 的流。
 */
typedef struct AVStreamGroupTREF {
    const AVClass *av_class;

    /**
 * AVStreamGroup 中元数据流的索引。
 */
    unsigned int metadata_index;
} AVStreamGroupTREF;

enum AVStreamGroupParamsType {
    AV_STREAM_GROUP_PARAMS_NONE,
    AV_STREAM_GROUP_PARAMS_IAMF_AUDIO_ELEMENT,
    AV_STREAM_GROUP_PARAMS_IAMF_MIX_PRESENTATION,
    AV_STREAM_GROUP_PARAMS_TILE_GRID,
    AV_STREAM_GROUP_PARAMS_LCEVC,
    AV_STREAM_GROUP_PARAMS_TREF,
    AV_STREAM_GROUP_PARAMS_DOLBY_VISION,
};

struct AVIAMFAudioElement;
struct AVIAMFMixPresentation;

typedef struct AVStreamGroup {
    /**
 * @ref avoptions 的类。由avformat_stream_group_create()设置。
 */
    const AVClass *av_class;

    void *priv_data;

    /**
 * AVFormatContext 中的组索引。
 */
    unsigned int index;

    /**
 * 组类型特定的组 ID。
 *
 * 解码：由 libavformat 设置 编码：可由用户设置
 */
    int64_t id;

    /**
 * 组类型
 *
 * 解码：由 libavformat 在组创建时设置 编码：由 avformat_stream_group_create() 设置
 */
    enum AVStreamGroupParamsType type;

    /**
 * 特定于组类型参数
 */
    union {
        struct AVIAMFAudioElement *iamf_audio_element;
        struct AVIAMFMixPresentation *iamf_mix_presentation;
        struct AVStreamGroupTileGrid *tile_grid;
        struct AVStreamGroupLayeredVideo *layered_video;
#if FF_API_LCEVC_STRUCT
        /**
 * 已弃用，使用 Layered_video。
 */
        attribute_deprecated
        struct AVStreamGroupLCEVC *lcevc;
#endif
        struct AVStreamGroupTREF *tref;
    } params;

    /**
 * 适用于整个组的元数据。
 *
 * - 解复用：在创建组时由 libavformat 设置 - 复用：可以由调用者在 avformat_write_header() 之前设置
 *
 * 由 libavformat 在 avformat_free_context() 中释放。
 */
    AVDictionary *metadata;

    /**
 * AVStreamGroup.streams 中的元素数量。
 *
 * 由 avformat_stream_group_add_stream() 设置的值不得被任何其他代码修改。
 */
    unsigned int nb_streams;

    /**
 * 组中的流列表。使用 avformat_stream_group_add_stream() 创建新条目。
 *
 * - 解复用：条目由 libavformat 在创建组时创建。如果 ctx_flags 中设置了 AVFMTCTX_NOHEADER，则新条目也可能出现在 av_read_frame() 中。 - 混合：条目由用户在 avformat_write_header() 之前创建。
 *
 * 由 libavformat 在 avformat_free_context() 中释放。
 */
    AVStream **streams;

    /**
 * 流组处置 - AV_DISPOSITION_* 标志的组合。该字段当前适用于所有定义的 AVStreamGroupParamsType。
 *
 * - 解复用：在创建组时或在 avformat_find_stream_info() 中由 libavformat 设置。 - 多路复用：可以由调用者在 avformat_write_header() 之前设置。
 */
    int disposition;
} AVStreamGroup;

struct AVCodecParserContext *av_stream_get_parser(const AVStream *s);

#define AV_PROGRAM_RUNNING 1

/**
 * 可以将新字段添加到末尾，并进行较小的版本更新。对现有字段的删除、重新排序和更改需要主要版本更新。 sizeof(AVProgram) 不得在 libav* 之外使用。
 */
typedef struct AVProgram {
    int            id;
    int            flags;
    enum AVDiscard discard;        ///< 选择要丢弃哪个程序以及将哪个程序提供给调用者
    unsigned int   *stream_index;
    unsigned int   nb_stream_indexes;
    AVDictionary *metadata;

    int program_num;
    int pmt_pid;
    int pcr_pid;
    int pmt_version;

    /**
 * **************************************************************
 * 此行下面的所有字段都不是公共 API 的一部分。它们不能在 libavformat 之外使用，并且可以随意更改和删除。新的公共字段应添加到正上方。
 * ****************************************************************
 */
    int64_t start_time;
    int64_t end_time;

    int64_t pts_wrap_reference;    ///< 用于回绕检测的参考 dts
    int pts_wrap_behavior;         ///< 回绕检测的行为
} AVProgram;

#define AVFMTCTX_NOHEADER      0x0001 /**
 * < 表示不存在标头的信号（动态添加流）
 */
#define AVFMTCTX_UNSEEKABLE    0x0002 /**
 * < 表明该流绝对不可查找，并且尝试调用查找函数将失败。对于某些网络协议（例如 HLS），这可以在运行时动态更改。
 */

typedef struct AVChapter {
    int64_t id;             ///< 用于标识章节的唯一 ID
    AVRational time_base;   ///< 指定开始/结束时间戳的时基
    int64_t start, end;     ///< 以 time_base 为单位的章节开始/结束时间
    AVDictionary *metadata;
} AVChapter;


/**
 * 设备用于与应用程序通信的回调。
 */
typedef int (*av_format_control_message)(struct AVFormatContext *s, int type,
                                         void *data, size_t data_size);

typedef int (*AVOpenCallback)(struct AVFormatContext *s, AVIOContext **pb, const char *url, int flags,
                              const AVIOInterruptCB *int_cb, AVDictionary **options);

/**
 * 视频的持续时间可以通过多种方式估计，并且可以使用此枚举来了解如何估计持续时间。
 */
enum AVDurationEstimationMethod {
    AVFMT_DURATION_FROM_PTS,    ///< 根据 PTS 准确估计的持续时间
    AVFMT_DURATION_FROM_STREAM, ///< 根据已知持续时间的流估计的持续时间
    AVFMT_DURATION_FROM_BITRATE ///< 根据比特率估计的持续时间（不太准确）
};

/**
 * 格式 I/O 上下文。新字段可以添加到末尾并进行较小的版本更新。对现有字段的删除、重新排序和更改需要主要版本更新。 sizeof(AVFormatContext) 不得在 libav* 之外使用，请使用 avformat_alloc_context() 创建 AVFormatContext。
 *
 * 字段可以通过 AVOptions (av_opt*) 访问，使用的名称字符串与关联的命令行参数名称匹配，可以在 libavformat/options_table.h 中找到。由于历史原因或简洁性，AVOption/命令行参数名称在某些情况下与 C 结构字段名称不同。
 */
typedef struct AVFormatContext {
    /**
 * 用于日志记录和@ref avoptions 的类。由avformat_alloc_context()设置。导出（解）复用器私有选项（如果存在）。
 */
    const AVClass *av_class;

    /**
 * 输入容器格式。
 *
 * 仅解复用，由 avformat_open_input() 设置。
 */
    const struct AVInputFormat *iformat;

    /**
 * 输出容器格式。
 *
 * 仅复用，必须由调用者在 avformat_write_header() 之前设置。
 */
    const struct AVOutputFormat *oformat;

    /**
 * 格式化私有数据。当且仅当 iformat/oformat.priv_class 不为 NULL 时，这是一个启用 AVOptions 的结构。
 *
 * - 多路复用：由 avformat_write_header() 设置 - 多路分解：由 avformat_open_input() 设置
 */
    void *priv_data;

    /**
 * I/O 上下文。
 *
 * - 解复用：由用户在 avformat_open_input() 之前设置（然后用户必须手动关闭它）或由 avformat_open_input() 设置。 - 混合：由用户在 avformat_write_header() 之前设置。调用者必须负责关闭/释放 IO 上下文。
 *
 * 如果在 iformat/oformat.flags 中设置了 AVFMT_NOFILE 标志，请勿设置此字段。在这种情况下，（解）复用器将以其他方式处理 I/O，并且该字段将为 NULL。
 */
    AVIOContext *pb;

    /* 流信息 */
    /**
 * 标志信令流属性。 AVFMTCTX_* 的组合。由 libavformat 设置。
 */
    int ctx_flags;

    /**
 * AVFormatContext.streams 中的元素数量。
 *
 * 由 avformat_new_stream() 设置，不得由任何其他代码修改。
 */
    unsigned int nb_streams;
    /**
 * 文件中所有流的列表。使用 avformat_new_stream() 创建新流。
 *
 * - 解复用：流由 libavformat 在 avformat_open_input() 中创建。如果 ctx_flags 中设置了 AVFMTCTX_NOHEADER，那么新的流也可能出现在 av_read_frame() 中。 - 混合：流由用户在 avformat_write_header() 之前创建。
 *
 * 由 libavformat 在 avformat_free_context() 中释放。
 */
    AVStream **streams;

    /**
 * AVFormatContext.stream_groups 中的元素数量。
 *
 * 由 avformat_stream_group_create() 设置，不得由任何其他代码修改。
 */
    unsigned int nb_stream_groups;
    /**
 * 文件中所有流组的列表。新组使用 avformat_stream_group_create() 创建，并使用 avformat_stream_group_add_stream() 填充。
 *
 * - 多路分解：组可以由 libavformat 在 avformat_open_input() 中创建。如果 ctx_flags 中设置了 AVFMTCTX_NOHEADER，那么新的组也可能出现在 av_read_frame() 中。 - 混合：用户可以在 avformat_write_header() 之前创建组。
 *
 * 由 libavformat 在 avformat_free_context() 中释放。
 */
    AVStreamGroup **stream_groups;

    /**
 * AVChapter 数组中的章节数。混合时，章节通常写入文件头中，因此 nb_chapters 通常应在调用 write_header 之前初始化。一些混合器（例如 mov 和 mkv）也可以在预告片中编写章节。  要在预告片中写入章节，调用 write_header 时 nb_chapters 必须为零，调用 write_trailer 时 nb_chapters 必须非零。 - 复用：由用户设置 - 解复用：由 libavformat
 */
    unsigned int nb_chapters;
    AVChapter **chapters;

    /**
 * 输入或输出 URL 设置。与旧的文件名字段不同，该字段没有长度限制。
 *
 * - 解复用：由 avformat_open_input() 设置，如果 avformat_open_input() 中 url 参数为 NULL，则初始化为空字符串。 - 混合：可以由调用者在调用 avformat_write_header()（或 avformat_init_output()，如果先调用的话）之前设置为可由 av_free() 释放的字符串。如果 avformat_init_output() 中为 NULL，则设置为空字符串。
 *
 * 由 libavformat 在 avformat_free_context() 中释放。
 */
    char *url;

    /**
 * 组件第一帧的位置，以 AV_TIME_BASE 小数秒为单位。切勿直接设置该值：它是从 AVStream 值推导出来的。
 *
 * 仅解复用，由 libavformat 设置。
 */
    int64_t start_time;

    /**
 * 流的持续时间，以 AV_TIME_BASE 小数秒为单位。仅当您不知道任何单个流持续时间并且也不设置任何一个时才设置此值。如果未设置，则从 AVStream 值推导出来。
 *
 * 仅解复用，由 libavformat 设置。
 */
    int64_t duration;

    /**
 * 总流比特率（以位/秒为单位），如果不可用则为 0。如果 file_size 和持续时间已知，FFmpeg 可以自动计算，切勿直接设置它。
 */
    int64_t bit_rate;

    unsigned int packet_size;
    int max_delay;

    /**
 * 修改（解）复用器行为的标志。 AVFMT_FLAG_* 的组合。由用户在 avformat_open_input() / avformat_write_header() 之前设置。
 */
    int flags;
#define AVFMT_FLAG_GENPTS       0x0001 ///< 生成缺失的点，即使它需要解析未来的帧。
#define AVFMT_FLAG_IGNIDX       0x0002 ///< 忽略索引。
#define AVFMT_FLAG_NONBLOCK     0x0004 ///< 从输入读取数据包时不阻塞。
#define AVFMT_FLAG_IGNDTS       0x0008 ///< 忽略包含 DTS 和 PTS 的帧上的 DTS
#define AVFMT_FLAG_NOFILLIN     0x0010 ///< 不要从其他值推断任何值，仅返回容器中存储的内容
#define AVFMT_FLAG_NOPARSE      0x0020 ///< 不要使用 AVParsers，您还必须设置AVFMT_FLAG_NOFILLIN 因为填充代码适用于帧并且没有解析 -> 无帧。如果解析查找帧边界已被禁用，则查找帧也无法工作
#define AVFMT_FLAG_NOBUFFER     0x0040 ///< 尽可能不要缓冲帧
#define AVFMT_FLAG_CUSTOM_IO    0x0080 ///< 调用者提供了自定义 AVIOContext，不要 avio_close() 它。
#define AVFMT_FLAG_DISCARD_CORRUPT  0x0100 ///< 丢弃标记为损坏的帧
#define AVFMT_FLAG_FLUSH_PACKETS    0x0200 ///< 每个数据包刷新 AVIOContext。
/**
 * 复用时，尽量避免将任何随机/易失性数据写入输出。这包括任何随机 ID、实时时间戳/日期、复用器版本等。
 *
 * 该标志主要用于测试。
 */
#define AVFMT_FLAG_BITEXACT         0x0400
#define AVFMT_FLAG_SORT_DTS    0x10000 ///< 尝试通过 dts 交错输出数据包（使用此标志会减慢解复用速度）
#define AVFMT_FLAG_FAST_SEEK   0x80000 ///< 对某些格式启用快速但不准确的搜索
#define AVFMT_FLAG_AUTO_BSF   0x200000 ///< 根据复用器的请求添加比特流过滤器

    /**
 * 为了确定流属性而从输入读取的最大字节数。在读取全局标头和 avformat_find_stream_info() 时使用。
 *
 * 仅解复用，由调用者在 avformat_open_input() 之前设置。
 *
 * @note 不用于确定 AVInputFormat“输入格式”
 * @see format_probesize
 */
    int64_t probesize;

    /**
 * 从 avformat_find_stream_info() 中的输入读取的数据的最大持续时间（以 AV_TIME_BASE 为单位）。仅解复用，由调用者在 avformat_find_stream_info() 之前设置。可以设置为 0 让 avformat 使用启发式进行选择。
 */
    int64_t max_analyze_duration;

    const uint8_t *key;
    int keylen;

    unsigned int nb_programs;
    AVProgram **programs;

    /**
 * 强制视频编解码器_id。解复用：由用户设置。
 */
    enum AVCodecID video_codec_id;

    /**
 * 强制音频编解码器_id。解复用：由用户设置。
 */
    enum AVCodecID audio_codec_id;

    /**
 * 强制字幕codec_id。解复用：由用户设置。
 */
    enum AVCodecID subtitle_codec_id;

    /**
 * 强制数据编解码器_id。解复用：由用户设置。
 */
    enum AVCodecID data_codec_id;

    /**
 * 适用于整个文件的元数据。
 *
 * - 解复用：由 libavformat 在 avformat_open_input() 中设置 - 复用：可以由调用者在 avformat_write_header() 之前设置
 *
 * 由 libavformat 在 avformat_free_context() 中释放。
 */
    AVDictionary *metadata;

    /**
 * 现实世界时间中的流开始时间，以 Unix 纪元（1970 年 1 月 1 日 00:00）以来的微秒为单位。也就是说，流中的 pts=0 是在这个真实世界时间捕获的。 - 混合：由调用者在 avformat_write_header() 之前设置。如果设置为 0 或 AV_NOPTS_VALUE，则将使用当前的挂墙时间。 - 解复用：由 libavformat 设置。 AV_NOPTS_VALUE（如果未知）。请注意，在接收到一定数量的帧后，该值可能会变为已知。
 */
    int64_t start_time_realtime;

    /**
 * 用于确定 avformat_find_stream_info() 中帧速率的帧数。仅解复用，由调用者在 avformat_find_stream_info() 之前设置。
 */
    int fps_probe_size;

    /**
 * 错误识别；较高的值将检测到更多错误，但可能会将一些或多或少有效的部分误检测为错误。仅解复用，由调用者在 avformat_open_input() 之前设置。
 */
    int error_recognition;

    /**
 * I/O 层的自定义中断回调。
 *
 * 解复用：由用户在 avformat_open_input() 之前设置。 muxing：由用户在 avformat_write_header() 之前设置（主要对 AVFMT_NOFILE 格式有用）。如果用于打开文件，回调也应该传递给 avio_open2()。
 */
    AVIOInterruptCB interrupt_callback;

    /**
 * 用于启用调试的标志。
 */
    int debug;
#define AV_FDEBUG_TS        0x0001
#define AV_FDEBUG_ID3V2     0x0002

#if FF_API_FDEBUG_TS
#define FF_FDEBUG_TS AV_FDEBUG_TS
#endif

    /**
 * 最大流数。 - 编码：未使用 - 解码：由用户设置
 */
    int max_streams;

    /**
 * 用于每个流索引的最大内存量（以字节为单位）。如果索引超过此大小，则将根据需要丢弃条目以保持较小的大小。这可能会导致查找速度变慢或不太准确（取决于解复用器）。强制使用完整内存索引的解复用器将忽略这一点。 - 复用：未使用 - 解复用：由用户设置
 */
    unsigned int max_index_size;

    /**
 * 用于缓冲从实时捕获设备获取的帧的最大内存量（以字节为单位）。
 */
    unsigned int max_picture_buffer;

    /**
 * 交错的最大缓冲持续时间。
 *
 * 为了确保所有流正确交错，av_interleaved_write_frame() 将等待，直到每个流至少有一个数据包，然后才将任何数据包实际写入输出文件。当某些流“稀疏”时（即连续数据包之间存在较大间隙），这可能会导致过度缓冲。
 *
 * 该字段指定多路复用队列中第一个数据包和最后一个数据包的时间戳之间的最大差异，高于此值的 libavformat 将输出一个数据包，无论它是否已将所有流的数据包排队。
 *
 * 仅复用，由调用者在 avformat_write_header() 之前设置。
 */
    int64_t max_interleave_delta;

    /**
 * 等待第一个时间戳时读取的最大数据包数。仅解码。
 */
    int max_ts_probe;

    /**
 * 最大块时间（以微秒为单位）。请注意，并非所有格式都支持此功能，如果在不支持的情况下使用它，可能会发生不可预测的事情。 - 编码：由用户设置 - 解码：未使用
 */
    int max_chunk_duration;

    /**
 * 最大块大小（以字节为单位） 注意，并非所有格式都支持此功能，如果在不支持的情况下使用它，可能会发生不可预测的事情。 - 编码：由用户设置 - 解码：未使用
 */
    int max_chunk_size;

    /**
 * 可探测的最大数据包数 - 编码：未使用 - 解码：由用户设置
 */
    int max_probe_packets;

    /**
 * 允许非标准和实验性扩展
 * @see AVCodecContext.strict_std_compliance
 */
    int strict_std_compliance;

    /**
 * 指示文件上发生的事件的标志，AVFMT_EVENT_FLAG_* 的组合。
 *
 * - 解复用：可以由解复用器在 avformat_open_input()、avformat_find_stream_info() 和 av_read_frame() 中设置。处理事件后，用户必须清除标志。 - 多路复用：可以由用户在 avformat_write_header() 之后设置，以指示用户触发的事件。  复用器将清除它在 av_[interleaved]_write_frame() 中处理的事件的标志。
 */
    int event_flags;
/**
 * - 解复用器：解复用器从文件中读取新元数据并相应地更新 AVFormatContext.metadata - 复用器：用户更新了 AVFormatContext.metadata 并希望复用器将其写入文件
 */
#define AVFMT_EVENT_FLAG_METADATA_UPDATED 0x0001


    /**
 * 在复用期间避免出现负时间戳。 AVFMT_AVOID_NEG_TS_* 常量的任何值。请注意，使用 av_interleaved_write_frame() 时效果更好。 - 复用：由用户设置 - 解复用：未使用
 */
    int avoid_negative_ts;
#define AVFMT_AVOID_NEG_TS_AUTO             -1 ///< 当目标格式需要时启用
#define AVFMT_AVOID_NEG_TS_DISABLED          0 ///< 即使时间戳为负数，也不移动时间戳。
#define AVFMT_AVOID_NEG_TS_MAKE_NON_NEGATIVE 1 ///< 移位时间戳，使其为非负
#define AVFMT_AVOID_NEG_TS_MAKE_ZERO         2 ///< 移位时间戳，使其从 0 开始

    /**
 * 音频预加载（以微秒为单位）。请注意，并非所有格式都支持此功能，如果在不支持的情况下使用它，可能会发生不可预测的事情。 - 编码：由用户设置 - 解码：未使用
 */
    int audio_preload;

    /**
 * 强制使用挂钟时间戳作为数据包的 pts/dts 这在存在 B 帧的情况下会产生未定义的结果。 - 编码：未使用 - 解码：由用户设置
 */
    int use_wallclock_as_timestamps;

    /**
 * 跳过estimate_timings_from_pts中的持续时间计算。 - 编码：未使用 - 解码：由用户设置
 *
 * @see uration_probesize
 */
    int skip_estimate_duration_from_pts;

    /**
 * avio 标志，用于强制 AVIO_FLAG_DIRECT。 - 编码：未使用 - 解码：由用户设置
 */
    int avio_flags;

    /**
 * 持续时间字段可以通过多种方式估计，并且可以使用该字段来了解持续时间是如何估计的。 - 编码：未使用 - 解码：由用户读取
 */
    enum AVDurationEstimationMethod duration_estimation_method;

    /**
 * 打开流时跳过初始字节 - 编码：未使用 - 解码：由用户设置
 */
    int64_t skip_initial_bytes;

    /**
 * 纠正单个时间戳溢出 - 编码：未使用 - 解码：由用户设置
 */
    unsigned int correct_ts_overflow;

    /**
 * 强制寻找任何（也非关键）帧。 - 编码：未使用 - 解码：由用户设置
 */
    int seek2any;

    /**
 * 在每个数据包后刷新 I/O 上下文。 - 编码：由用户设置 - 解码：未使用的
 */
    int flush_packets;

    /**
 * 格式探测分数。最大分数是 AVPROBE_SCORE_MAX，它是在解复用器探测格式时设置的。 - 编码：未使用 - 解码：由 avformat 设置，由用户读取
 */
    int probe_score;

    /**
 * 从输入读取的最大字节数，以识别\ref AVInputFormat“输入格式”。仅当调用者未明确设置格式时使用。
 *
 * 仅解复用，由调用者在 avformat_open_input() 之前设置。
 *
 * @see 探测
 */
    int format_probesize;

    /**
 * ',' 分隔的允许解码器列表。如果为 NULL，则允许全部 - 编码：未使用 - 解码：由用户设置
 */
    char *codec_whitelist;

    /**
 * ',' 分隔的允许分路器列表。如果为 NULL，则允许所有协议 - 编码：未使用 - 解码：由用户设置
 */
    char *format_whitelist;

    /**
 * ',' 分隔的允许协议列表。 - 编码：未使用 - 解码：由用户设置
 */
    char *protocol_whitelist;

    /**
 * ',' 分隔的不允许的协议列表。 - 编码：未使用 - 解码：由用户设置
 */
    char *protocol_blacklist;

    /**
 * IO 重新定位标志。当底层 IO 上下文读取指针重新定位时（例如，进行基于字节的查找时），该值由 avformat 设置。多路分配器可以使用该标志来检测此类更改。
 */
    int io_repositioned;

    /**
 * 强制视频编解码器。这允许强制使用特定的解码器，即使有多个具有相同的 codec_id 也是如此。解复用：由用户设置
 */
    const struct AVCodec *video_codec;

    /**
 * 强制音频编解码器。这允许强制使用特定的解码器，即使有多个具有相同的 codec_id 也是如此。解复用：由用户
 */
    const struct AVCodec *audio_codec;

    /**
 * 强制字幕编解码器设置。这允许强制使用特定的解码器，即使有多个具有相同的 codec_id 也是如此。解复用：由用户设置
 */
    const struct AVCodec *subtitle_codec;

    /**
 * 强制数据编解码器。这允许强制使用特定的解码器，即使有多个具有相同的 codec_id 也是如此。解复用：由用户设置
 */
    const struct AVCodec *data_codec;

    /**
 * 要写入元数据标头中作为填充的字节数。解复用：未使用。复用：由用户设置。
 */
    int metadata_header_padding;

    /**
 * 用户数据。这是用户的一些私人数据的地方。
 */
    void *opaque;

    /**
 * 设备用于与应用程序通信的回调。
 */
    av_format_control_message control_message_cb;

    /**
 * 输出时间戳偏移量，以微秒为单位。复用：由用户设置
 */
    int64_t output_ts_offset;

    /**
 * 转储格式分隔符。可以是“、”或“\n”或其他任何内容 - 混合：由用户设置。 - 解复用：由用户设置。
 */
    uint8_t *dump_separator;

    /**
 * 打开新 IO 流的回调。
 *
 * 每当复用器或解复用器需要打开 IO 流（通常来自解复用器的 avformat_open_input() ，但对于某些格式也可能在其他时间发生），它将调用此回调来获取 IO 上下文。
 *
 * @param s 格式上下文
 * @param pb 成功时，应在此处返回新打开的 IO 上下文
 * @param url 打开
 * @param flags AVIO_FLAG_* 组合的 url
 * @param options 附加选项的字典，与 avio_open2() 中的语义相同
 * @return 成功时为 0，失败时为负 AVERROR 代码
 *
 * @note 某些复用器和解复用器进行嵌套，即它们打开一个或多个附加内部格式上下文。因此，传递给此回调的 AVFormatContext 指针可能与面向调用者的指针不同。然而，它将具有相同的“不透明”字段。
 */
    int (*io_open)(struct AVFormatContext *s, AVIOContext **pb, const char *url,
                   int flags, AVDictionary **options);

    /**
 * 用于关闭使用 AVFormatContext.io_open() 打开的流的回调。
 *
 * @param s 格式上下文
 * @param pb 要关闭和释放的 IO 上下文
 * @return 成功时为 0，失败时为负 AVERROR 代码
 */
    int (*io_close2)(struct AVFormatContext *s, AVIOContext *pb);

    /**
 * 从输入读取的最大字节数，以便在使用时确定流持续时间avformat_find_stream_info() 中的estimate_timings_from_pts。仅解复用，由调用者在 avformat_find_stream_info() 之前设置。可以设置为 0 让 avformat 使用启发式进行选择。
 *
 * @see skip_estimate_duration_from_pts
 */
    int64_t duration_probesize;

    /**
 * 此格式上下文的名称，仅用于记录目的。
 */
    char *name;

    /**
 * 深度递归限制，
 *
 * 解复用器可以在其内部打开解复用器的最大递归深度。
 *
 * - 解复用：由用户设置
 */
    int recursion_limit;
} AVFormatContext;

/**
 * @defgroup lavf_core 核心函数
 * @ingroup libavf
 *
 * 用于查询 libavformat 功能、分配核心结构等的函数。
 * @{
 */

/**
 * 返回 LIBAVFORMAT_VERSION_INT 常量。
 */
unsigned avformat_version(void);

/**
 * 返回 libavformat 的构建时配置。
 */
const char *avformat_configuration(void);

/**
 * 返回 libavformat 的许可证信息。
 */
const char *avformat_license(void);

/**
 * 进行网络库的全局初始化。这是可选的，不再推荐。
 *
 * 此函数仅用于解决旧版 GnuTLS 或 OpenSSL 库的线程安全问题。如果 libavformat 链接到这些库的较新版本，或者如果您不使用它们，则无需调用此函数。否则，您需要在使用它们的任何其他线程启动之前调用此函数。
 *
 * 一旦删除对旧版 GnuTLS 和 OpenSSL 库的支持，此函数将被弃用，并且此函数不再有任何用途。
 */
int avformat_network_init(void);

/**
 * 撤消 avformat_network_init 完成的初始化。每次调用 avformat_network_init 时仅调用一次。
 */
int avformat_network_deinit(void);

/**
 * 迭代所有已注册的复用器。
 *
 * @param opaque 一个指针，libavformat 将在其中存储迭代状态。必须指向 NULL 才能开始迭代。
 *
 * @return 下一个注册的复用器或迭代完成时为 NULL
 */
const AVOutputFormat *av_muxer_iterate(void **opaque);

/**
 * 迭代所有注册的复用器。
 *
 * @param opaque 一个指针，libavformat 将在其中存储迭代状态。必须指向 NULL 才能开始迭代。
 *
 * @return 迭代完成时下一个注册的分路器或 NULL
 */
const AVInputFormat *av_demuxer_iterate(void **opaque);

/**
 * 分配 AVFormatContext。 avformat_free_context() 可用于释放上下文以及框架在其中分配的所有内容。
 */
AVFormatContext *avformat_alloc_context(void);

/**
 * 释放 AVFormatContext 及其所有流。
 * @param s 上下文到免费
 */
void avformat_free_context(AVFormatContext *s);

/**
 * 获取 AVFormatContext 的 AVClass。它可以与 AV_OPT_SEARCH_FAKE_OBJ 结合使用来检查选项。
 *
 * @see av_opt_find()。
 */
const AVClass *avformat_get_class(void);

/**
 * 获取 AVStream 的 AVClass。它可以与 AV_OPT_SEARCH_FAKE_OBJ 结合使用来检查选项。
 *
 * @see av_opt_find()。
 */
const AVClass *av_stream_get_class(void);

/**
 * 获取 AVStreamGroup 的 AVClass。它可以与 AV_OPT_SEARCH_FAKE_OBJ 结合使用来检查选项。
 *
 * @see av_opt_find()。
 */
const AVClass *av_stream_group_get_class(void);

/**
 * @return 标识流组类型的字符串，如果未知则为 NULL
 */
const char *avformat_stream_group_name(enum AVStreamGroupParamsType type);

/**
 * 将新的空流组添加到媒体文件。
 *
 * 解复用时，可能会被解复用器在 read_header() 中调用。如果在 s.ctx_flags 中设置了标志 AVFMTCTX_NOHEADER，那么也可以在 read_packet() 中调用它。
 *
 * 当复用时，用户可以在 avformat_write_header() 之前调用。
 *
 * 用户需要调用 avformat_free_context() 来清理 avformat_stream_group_create() 的分配。
 *
 * 可以使用 avformat_stream_group_add_stream() 将新流添加到组中。
 *
 * @param s 媒体文件句柄
 *
 * @return 新创建的组或错误时为 NULL。
 * @see avformat_new_stream、avformat_stream_group_add_stream。
 */
AVStreamGroup *avformat_stream_group_create(AVFormatContext *s,
                                            enum AVStreamGroupParamsType type,
                                            AVDictionary **options);

/**
 * 将新流添加到媒体文件。
 *
 * 解复用时，由解复用器在 read_header() 中调用。如果在 s.ctx_flags 中设置了标志 AVFMTCTX_NOHEADER，那么也可以在 read_packet() 中调用它。
 *
 * 复用时，用户应在 avformat_write_header() 之前调用。
 *
 * 用户需要调用avformat_free_context()来清理avformat_new_stream()分配的空间。
 *
 * @param s 媒体文件句柄
 * @param c 未使用，不执行任何操作
 *
 * @return 新创建的流或错误时为 NULL。
 */
AVStream *avformat_new_stream(AVFormatContext *s, const struct AVCodec *c);

/**
 * 将已分配的流添加到流组。
 *
 * 解复用时，可能会被解复用器在 read_header() 中调用。如果在 s.ctx_flags 中设置了标志 AVFMTCTX_NOHEADER，那么也可以在 read_packet() 中调用它。
 *
 * 复用时，用户可以在使用 avformat_stream_group_create() 分配新组并使用 avformat_new_stream() 分配流之后，在 avformat_write_header() 之前调用。
 *
 * 用户需要调用avformat_free_context()来清理avformat_stream_group_add_stream()的分配。
 *
 * @param stg 属于媒体文件的流组。
 * @param st 要添加到组中的媒体文件中的流。
 *
 * @retval 0 成功
 * @retval AVERROR(EEXIST) 流已在组中
 * @retval "another 负错误代码“合法错误
 *
 * @see avformat_new_stream、avformat_stream_group_create。
 */
int avformat_stream_group_add_stream(AVStreamGroup *stg, AVStream *st);

AVProgram *av_new_program(AVFormatContext *s, int id);


#define AVFMT_PROGCOPY_MATCH_BY_ID          (1 << 0) ///< 使用流 ID 匹配流
#define AVFMT_PROGCOPY_MATCH_BY_INDEX       (1 << 1) ///< 使用流索引匹配流
#define AVFMT_PROGCOPY_OVERWRITE            (1 << 8) ///< 覆盖具有相同 ID 的预先存在的节目

/**
 * 将 AVProgram 从一个 AVFormatContext 复制到另一个 AVFormatContext。
 *
 * 目标上下文中指定属性与源 AVProgram 索引中流的属性匹配的流将添加到复制的 AVProgram 的流索引中。该属性使用 AVFMT_PROGCOPY_MATCH_ 标志指定。
 *
 * 如果必须添加新程序，则该函数期望并要求任何保存目标上下文中指向 AVPrograms 的指针数组的现有缓冲区的大小为 2 的幂值。如果所有早期程序都是使用 av_new_program 或此函数创建的，则应该是这种情况。
 *
 * @param dst 指向目标复用器上下文的指针
 * @param src 指向源复用器上下文的指针
 * @param progid 要复制的节目的 ID
 * @param flags 确定流如何匹配以及目标中是否预先存在 AVProgram 的标志组合被覆盖。如果未设置匹配条件，如果所有源流 id 均非零且唯一，则流将按 id 匹配，否则按索引匹配。
 *
 * @return >= 0 如果成功，则错误 EEXIST 如果目标已有具有相同 ID 的程序并且未设置覆盖标志，否则如果出现其他失败，则为负 AVERROR 代码。
 */
int av_program_copy(AVFormatContext *dst, const AVFormatContext *src, int progid, int flags);

/**
 * @}
 */


/**
 * 为输出格式分配 AVFormatContext。 avformat_free_context() 可用于释放上下文以及框架在其中分配的所有内容。
 *
 * @param ctx 指针对象设置为创建的格式上下文，或者在失败的情况下设置为 NULL
 * @param oformat 用于分配上下文的格式（如果使用 NULL format_name 和文件名）
 * @param format_name 用于分配上下文的输出格式的名称（如果使用 NULL 文件名）
 * @param filename 用于分配上下文的文件名的名称，如果成功，可能为 NULL
 *
 * @return >= 0；如果失败，则为负 AVERROR 代码
 */
int avformat_alloc_output_context2(AVFormatContext **ctx, const AVOutputFormat *oformat,
                                   const char *format_name, const char *filename);

/**
 * @addtogroup lavf_decoding
 * @{
 */

/**
 * 根据输入格式的短名称查找 AVInputFormat。
 */
const AVInputFormat *av_find_input_format(const char *short_name);

/**
 * 猜测文件格式。
 *
 * @param pd 待探测数据
 * @param is_opened 文件是否已打开；确定是否探测带有或不带有 AVFMT_NOFILE 的解复用器。
 */
const AVInputFormat *av_probe_input_format(const AVProbeData *pd, int is_opened);

/**
 * 猜测文件格式。
 *
 * @param pd 待探测数据
 * @param is_opened 文件是否已经打开；确定是否探测带有或不带有 AVFMT_NOFILE 的解复用器。
 * @param score_max 大于接受检测所需的探测分数，之后将该变量设置为实际检测分数。如果分数 <= AVPROBE_SCORE_MAX / 4，建议使用更大的探测缓冲区重试。
 */
const AVInputFormat *av_probe_input_format2(const AVProbeData *pd,
                                            int is_opened, int *score_max);

/**
 * 猜测文件格式。
 *
 * @param is_opened 文件是否已经打开；确定是否探测带有或不带有 AVFMT_NOFILE 的解复用器。
 * @param score_ret 最佳检测的分数。
 */
const AVInputFormat *av_probe_input_format3(const AVProbeData *pd,
                                            int is_opened, int *score_ret);

/**
 * 探测字节流以确定输入格式。每次探测返回的分数太低时，都会增加探测缓冲区大小并进行另一次尝试。当达到最大探针大小时，返回得分最高的输入格式。
 *
 * @param pb 要探测的字节流
 * @param fmt 输入格式放在这里
 * @param url 流的 url
 * @param logctx 日志上下文
 * @param offset 字节流内的偏移量来自
 * @param max_probe_size 的探测 最大探测缓冲区大小（默认为零）
 *
 * @return 成功情况下的分数，对应于最大分数的负值是 AVPROBE_SCORE_MAX AVERROR 代码，否则
 */
int av_probe_input_buffer2(AVIOContext *pb, const AVInputFormat **fmt,
                           const char *url, void *logctx,
                           unsigned int offset, unsigned int max_probe_size);

/**
 * 与 av_probe_input_buffer2() 类似，但返回 0成功
 */
int av_probe_input_buffer(AVIOContext *pb, const AVInputFormat **fmt,
                          const char *url, void *logctx,
                          unsigned int offset, unsigned int max_probe_size);

/**
 * 打开输入流并读取标头。编解码器未打开。必须使用 avformat_close_input() 关闭流。
 *
 * @param ps 指向用户提供的 AVFormatContext 的指针（由 avformat_alloc_context 分配）。可能是指向 NULL 的指针，在这种情况下，该函数会分配 AVFormatContext 并将其写入 ps 中。请注意，用户提供的 AVFormatContext 将在失败时释放，并将其指针设置为 NULL。
 * @param url 要打开的流的 URL。
 * @param fmt 如果非 NULL，则此参数强制使用特定的输入格式。否则会自动检测格式。
 * @param options 一个充满 AVFormatContext 和 demuxer-private 选项的字典。返回时，此参数将被销毁并替换为包含未找​​到的选项的字典。可能为 NULL。
 *
 * @return 成功时为 0；失败时：释放 ps，将其指针设置为 NULL，并返回负值 AVERROR。
 *
 * @note 如果要使用自定义 IO，请预先分配格式上下文并设置其 pb 字段。
 */
int avformat_open_input(AVFormatContext **ps, const char *url,
                        const AVInputFormat *fmt, AVDictionary **options);

/**
 * 读取媒体文件的数据包以获取流信息。这对于没有标头的文件格式（例如 MPEG）非常有用。该函数还计算 MPEG-2 重复帧模式下的实际帧速率。该函数不会改变逻辑文件位置；检查过的数据包可能会被缓冲以供以后处理。
 *
 * @param ic 媒体文件句柄
 * @param options 如果非 NULL，则为指向字典的 ic.nb_streams 长指针数组，其中第 i 个成员包含对应于第 i 个流的编解码器的选项。返回时，每个字典将填充未找到的选项。
 * @return >=0 如果 OK，AVERROR_xxx 错误
 *
 * @note 此函数不能保证打开所有编解码器，因此返回时选项非空是完全正常的行为。
 *
 * @todo 让用户以某种方式决定需要什么信息，这样我们就不会浪费时间获取用户不需要的信息。
 */
int avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options);

/**
 * 查找属于给定流的节目。
 *
 * @param ic 媒体文件句柄
 * @param last 最后找到的节目，搜索将从该节目之后开始，如果为 NULL，则从头开始
 * @param s 流索引
 *
 * @return 属于 s 的下一个节目，如果未找到节目则为 NULL 或最后一个程序不在ic的程序中。
 */
AVProgram *av_find_program_from_stream(AVFormatContext *ic, AVProgram *last, int s);

void av_program_add_stream_index(AVFormatContext *ac, int progid, unsigned int idx);

/**
 * 将提供的流索引添加到具有匹配 id 的 AVProgram 中。
 *
 * @param ac 包含目标 AVProgram 的格式上下文
 * @param progid 其流索引要更新的 AVProgram 的 ID
 * @param idx 要添加的流的索引
 *
 * @return 在成功添加或索引已经存在时 >=0，如果未找到匹配的程序或流索引无效或流索引数组重新分配失败，则 AVERROR。
 */
int av_program_add_stream_index2(AVFormatContext *ac, int progid, unsigned int idx);

/**
 * 在文件中查找“最佳”流。最佳流是​​根据各种启发法确定为最有可能是用户期望的流。如果解码器参数非NULL，av_find_best_stream将为流的编解码器查找默认解码器；无法找到解码器的流将被忽略。
 *
 * @param ic 媒体文件句柄
 * @param type 流类型：视频、音频、字幕等
 * @param wanted_stream_nb 用户请求的流编号，或 -1 表示自动选择
 * @param related_stream 尝试查找与此相关的流（例如在同一节目中），或 -1（如果没有）
 * @param decoder_ret 如果非 NULL，则返回所选流的解码器
 * @param flags 标志；当前未定义
 *
 * @return 如果成功，则为非负流编号；如果找不到具有请求类型的流，则为 AVERROR_STREAM_NOT_FOUND；如果找到流，但没有解码器，则为 AVERROR_DECODER_NOT_FOUND
 *
 * @note 如果 av_find_best_stream 成功返回，但解码器_ret 未返回NULL，则 *decoder_ret 保证设置为有效的 AVCodec。
 */
int av_find_best_stream(AVFormatContext *ic,
                        enum AVMediaType type,
                        int wanted_stream_nb,
                        int related_stream,
                        const struct AVCodec **decoder_ret,
                        int flags);

/**
 * 返回流中的下一帧。此函数返回文件中存储的内容，不会验证其中的内容是否是解码器可用的有效帧。
 * 它会将文件中的内容拆分为帧，每次调用返回一帧。为了向解码器提供尽可能多的解码信息，
 * 它不会省略有效帧之间的无效数据。
 *
 * 成功时，返回的数据包将进行引用计数（设置了 pkt->buf）并且无限期有效。当不再需要数据包时，必须使用 av_packet_unref() 释放该数据包。对于视频，数据包仅包含一帧。对于音频，如果每个帧具有已知的固定大小（例如 PCM 或 ADPCM 数据），则它包含整数个帧。如果音频帧的大小可变（例如 MPEG 音频），则它包含一帧。
 *
 * pkt->pts、pkt->dts 和 pkt->duration 始终设置为 AVStream.time_base 单位中的正确值（并猜测格式是否无法提供它们）。如果视频格式有 B 帧，pkt->pts 可以是 AV_NOPTS_VALUE，因此如果不解压缩有效负载，最好依赖 pkt->dts。
 *
 * @return 如果正常则为 0，如果发生错误或文件结尾则为 < 0。出错时，pkt 将为空（就好像它来自 av_packet_alloc()）。
 *
 * @note pkt将被初始化，因此它可能未初始化，但它不能包含需要释放的数据。
 */
int av_read_frame(AVFormatContext *s, AVPacket *pkt);

/**
 * 查找时间戳处的关键帧。 “stream_index”中的“时间戳”。
 *
 * @param s 媒体文件句柄
 * @param stream_index 如果stream_index 为(-1)，则选择默认流，并且时间戳会自动从AV_TIME_BASE 单位转换为流特定的time_base。
 * @param timestamp 以 AVStream.time_base 为单位的时间戳，或者如果未指定流，则以 AV_TIME_BASE 为单位。
 * @param flags 选择方向和搜索模式的标志
 *
 * @return >= 0 表示成功
 */
int av_seek_frame(AVFormatContext *s, int stream_index, int64_t timestamp,
                  int flags);

/**
 * 寻求时间戳 ts。将进行查找，以便可以成功呈现所有活动流的点将最接近 ts 并在 min/max_ts 之内。活动流是 AVStream.discard < AVDISCARD_ALL 的所有流。
 *
 * 如果标志包含 AVSEEK_FLAG_BYTE，则所有时间戳均以字节为单位，并且是文件位置（可能并非所有解复用器都支持）。如果标志包含 AVSEEK_FLAG_FRAME，则所有时间戳都位于具有stream_index 的流中的帧中（这可能不受所有解复用器支持）。否则，所有时间戳均以stream_index选择的流为单位，或者如果stream_index为-1，则以AV_TIME_BASE为单位。如果标志包含 AVSEEK_FLAG_ANY，则非关键帧将被视为关键帧（这可能不受所有解复用器支持）。如果标志包含 AVSEEK_FLAG_BACKWARD，则会被忽略。
 *
 * @param s 媒体文件句柄
 * @param stream_index 用作时基参考的流索引
 * @param min_ts 最小可接受时间戳
 * @param ts 目标时间戳
 * @param max_ts 最大可接受时间戳
 * @param flags 成功时标记
 * @return >=0，否则错误代码
 *
 * @note 这是仍在构建中的新搜索 API 的一部分。
 */
int avformat_seek_file(AVFormatContext *s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags);

/**
 * 丢弃所有内部缓冲的数据。这在处理字节流中的不连续性时非常有用。通常仅适用于可以重新同步的格式。这包括 MPEG-TS/TS 等无标头格式，但也应与 NUT、Ogg 以及有限的 AVI 等一起使用。
 *
 * 调用此函数时，流集、检测到的持续时间、流参数和编解码器不会更改。如果您想要完全重置，最好打开一个新的 AVFormatContext。
 *
 * 这不会刷新 AVIOContext (s->pb)。如有必要，请在调用此函数之前调用 avio_flush(s->pb)。
 *
 * @param s 媒体文件句柄
 * @return >=0 成功，否则错误代码
 */
int avformat_flush(AVFormatContext *s);

/**
 * 在当前位置开始播放基于网络的流（例如 RTSP 流）。
 */
int av_read_play(AVFormatContext *s);

/**
 * 暂停基于网络的流（例如 RTSP 流）。
 *
 * 使用 av_read_play() 恢复它。
 */
int av_read_pause(AVFormatContext *s);

/**
 * 可以发送到解复用器的命令 ID
 *
 * 可以使用 ::avformat_send_command 将以下命令发送到解复用器。
 */
enum AVFormatCommandID {
    /**
 * 向服务器发送 RTSP `SET_PARAMETER` 请求
 *
 * 将 SET_PARAMETER RTSP 命令发送到服务器，其数据负载类型为 ::AVRTSPCommandRequest，其所有权及其数据仍属于调用者。
 *
 * 检索到的回复的类型为::AVRTSPResponse，并且它及其内容必须由调用者释放。
 */
    AVFORMAT_COMMAND_RTSP_SET_PARAMETER,
};

typedef struct AVRTSPCommandRequest {
    /**
 * 在请求中发送到服务器的标头
 */
    AVDictionary *headers;

    /**
 * 主体有效负载大小
 */
    size_t body_len;

    /**
 * 主体有效负载
 */
    char *body;
} AVRTSPCommandRequest;

typedef struct AVRTSPResponse {
    /**
 * 来自服务器的响应状态代码
 */
    int status_code;

    /**
 * 来自服务器的原因短语，描述以人类可读的方式显示状态。
 */
    char *reason;

    /**
 * 主体有效负载大小
 */
    size_t body_len;

    /**
 * 主体有效负载
 */
    unsigned char *body;
} AVRTSPResponse;

/**
 * 向解复用器发送命令
 *
 * 发送指定的命令和（取决于命令）可选的命令特定有效负载到解复用器进行处理。
 *
 * @param s 格式上下文，必须使用 ::avformat_alloc_context 分配。
 * @param id ::AVFormatCommandID 类型的标识符，指示要发送的命令。
 * @param data 命令特定数据，由调用者分配，所有权仍属于调用者。有关此处期望的详细信息，请参阅相应 ::AVFormatCommandID 的文档。
 */
int avformat_send_command(AVFormatContext *s, enum AVFormatCommandID id, void *data);

/**
 * 从多路分配器接收命令回复
 *
 * 从多路复用器检索先前发送的命令的回复。
 *
 * @param s 格式上下文，必须使用 ::avformat_alloc_context 分配。
 * @param id ::AVFormatCommandID 类型的标识符，指示要检索其回复的命令。
 * @param data_out Pointee设置为命令回复，实际类型取决于命令。这是由复用器分配的，必须使用 ::av_free 释放。有关此处实际数据集的详细信息，请参阅相应 ::AVFormatCommandID 的文档。
 */
int avformat_receive_command_reply(AVFormatContext *s, enum AVFormatCommandID id, void **data_out);

/**
 * 关闭打开的输入 AVFormatContext。释放它及其所有内容并将 *s 设置为 NULL。
 */
void avformat_close_input(AVFormatContext **s);
/**
 * @}
 */

#define AVSEEK_FLAG_BACKWARD 1 ///< 向后查找
#define AVSEEK_FLAG_BYTE     2 ///< 基于字节位置查找
#define AVSEEK_FLAG_ANY      4 ///< 查找任何帧，甚至非关键帧
#define AVSEEK_FLAG_FRAME    8 ///< 基于帧编号查找

/**
 * @addtogroup lavf_encoding
 * @{
 */

#define AVSTREAM_INIT_IN_WRITE_HEADER 0 ///< avformat_write_header 中初始化的流参数
#define AVSTREAM_INIT_IN_INIT_OUTPUT  1 ///< avformat_init_output 中初始化的流参数

/**
 * 分配流私有数据并将流标头写入输出媒体文件。
 *
 * @param s 媒体文件句柄，必须使用 avformat_alloc_context() 分配。其\ref AVFormatContext.oformat“oformat”字段必须设置为所需的输出格式；其\ref AVFormatContext.pb“pb”字段必须设置为已打开的::AVIOContext。
 * @param options 一个 ::AVDictionary，其中包含 AVFormatContext 和 muxer-private 选项。返回时，此参数将被销毁并替换为包含未找​​到的选项的字典。可能为 NULL。
 *
 * @retval AVSTREAM_INIT_IN_WRITE_HEADER 如果编解码器尚未在 avformat_init_output() 中完全初始化，则成功。
 * @retval AVSTREAM_INIT_IN_INIT_OUTPUT 如果编解码器已在 avformat_init_output() 中完全初始化，则成功。
 * @retval AVERROR 失败时为负 AVERROR。
 *
 * @see av_opt_find、av_dict_set、avio_open、av_oformat_next、avformat_init_output。
 */
av_warn_unused_result
int avformat_write_header(AVFormatContext *s, AVDictionary **options);

/**
 * 分配流私有数据并初始化编解码器，但不写入标头。可以选择在 avformat_write_header() 之前使用，以在实际写入标头之前初始化流参数。如果使用此函数，请勿将相同的选项传递给 avformat_write_header()。
 *
 * @param s 媒体文件句柄，必须使用 avformat_alloc_context() 分配。其\ref AVFormatContext.oformat“oformat”字段必须设置为所需的输出格式；其\ref AVFormatContext.pb“pb”字段必须设置为已打开的::AVIOContext。
 * @param options 一个 ::AVDictionary，其中包含 AVFormatContext 和 muxer-private 选项。返回时，此参数将被销毁并替换为包含未找​​到的选项的字典。可能为 NULL。
 *
 * @retval AVSTREAM_INIT_IN_WRITE_HEADER 成功时，如果编解码器需要 avformat_write_header 来完全初始化。
 * @retval AVSTREAM_INIT_IN_INIT_OUTPUT 如果编解码器已完全初始化，则成功。
 * @retval AVERROR 失败时为负 AVERROR。
 *
 * @see av_opt_find、av_dict_set、avio_open、av_oformat_next、avformat_write_header。
 */
av_warn_unused_result
int avformat_init_output(AVFormatContext *s, AVDictionary **options);

/**
 * 将数据包写入输出媒体文件。
 *
 * 此函数将数据包直接传递到复用器，无需任何缓冲或重新排序。如果格式需要，调用者负责正确交错数据包。希望 libavformat 处理交错的调用者应该调用 av_interleaved_write_frame() 而不是此函数。
 *
 * @param s 媒体文件句柄
 * @param pkt 包含要写入的数据的数据包。请注意，与 av_interleaved_write_frame() 不同，此函数不获取传递给它的数据包的所有权（尽管某些复用器可能会对输入数据包进行内部引用）。 <br> 对于在将数据写入输出之前在内部缓冲数据的复用器，此参数可以为 NULL（任何时候，而不仅仅是在最后），以便立即刷新复用器内缓冲的数据。 <br> 数据包的@ref AVPacket.stream_index“stream_index”字段必须设置为@ref AVFormatContext.streams“s->streams”中相应流的索引。 <br> 时间戳（@ref AVPacket.pts“pts”、@ref AVPacket.dts“dts”）必须设置为流时基中的正确值（除非输出格式标记有 AVFMT_NOTIMESTAMPS 标志，则可以将它们设置为 AV_NOPTS_VALUE）。与各自的时基相比，传递到此函数的后续数据包的 dts 必须严格递增（除非输出格式标记有 AVFMT_TS_NONSTRICT，则它们只需非递减）。  如果已知的话，还应该设置@ref AVPacket.duration“duration”）。
 * @return < 0 错误，= 0 如果正常，1 如果刷新并且没有更多数据要刷新
 *
 * @see av_interleaved_write_frame()
 */
int av_write_frame(AVFormatContext *s, AVPacket *pkt);

/**
 * 将数据包写入输出媒体文件，确保正确交错。
 *
 * 该函数将根据需要在内部缓冲数据包，以确保输出文件中的数据包正确交错，通常通过增加 dts 进行排序。进行自己的交错的调用者应该调用 av_write_frame() 而不是此函数。
 *
 * 使用此函数代替 av_write_frame() 可以让复用器预先了解未来的数据包，从而改进例如分段模式下 VFR 内容的 mp4 复用器的行为。
 *
 * @param s 媒体文件句柄
 * @param pkt 包含要写入的数据的数据包。 <br> 如果数据包是引用计数的，则该函数将取得该引用的所有权，并在以后认为合适时取消引用它。如果数据包不是引用计数的，libavformat 将制作一个副本。即使出现错误，返回的数据包也将为空白（就像从 av_packet_alloc() 返回一样）。 <br> 该参数可以为 NULL（任何时候，而不仅仅是在最后），以刷新交错队列。 <br> 数据包的@ref AVPacket.stream_index“stream_index”字段必须设置为@ref AVFormatContext.streams“s->streams”中相应流的索引。 <br> 时间戳（@ref AVPacket.pts“pts”、@ref AVPacket.dts“dts”）必须设置为流时基中的正确值（除非输出格式标记有 AVFMT_NOTIMESTAMPS 标志，则可以将它们设置为 AV_NOPTS_VALUE）。一个流中后续数据包的 dts 必须严格递增（除非输出格式标记有 AVFMT_TS_NONSTRICT，则它们只需非递减）。 @ref AVPacket.duration 如果已知，还应设置“持续时间”。
 *
 * @return 成功时为 0，错误时为负 AVERROR。
 *
 * @see av_write_frame(), AVFormatContext.max_interleave_delta
 */
int av_interleaved_write_frame(AVFormatContext *s, AVPacket *pkt);

/**
 * 将未编码的帧写入输出媒体文件。
 *
 * 框架必须按照集装箱规格正确交错；如果不是，则必须使用 av_interleaved_write_uncoded_frame()。
 *
 * 有关详细信息，请参阅 av_interleaved_write_uncoded_frame()。
 */
int av_write_uncoded_frame(AVFormatContext *s, int stream_index,
                           struct AVFrame *frame);

/**
 * 将未编码的帧写入输出媒体文件。
 *
 * 如果复用器支持，此函数可以直接编写 AVFrame 结构，而无需将其编码到数据包中。它对于使用原始视频或 PCM 数据并且不会将其序列化为字节流的设备和类似的特殊复用器最有用。
 *
 * 要测试是否可以将其与给定的复用器和流一起使用，请使用 av_write_uncoded_frame_query()。
 *
 * 调用者放弃帧的所有权，并且之后不得访问它。
 *
 * @return >=0 表示成功，负代码表示错误
 */
int av_interleaved_write_uncoded_frame(AVFormatContext *s, int stream_index,
                                       struct AVFrame *frame);

/**
 * 测试复用器是否支持未编码帧。如果可以将未编码帧写入该复用器和流，则
 *
 * @return >=0；如果不能，则 <0
 */
int av_write_uncoded_frame_query(AVFormatContext *s, int stream_index);

/**
 * 将流预告片写入输出媒体文件并释放文件私有数据。
 *
 * 只能在成功调用 avformat_write_header 后调用。
 *
 * @param s 媒体文件句柄
 * @return 如果正常则为 0，如果出错则为 AVERROR_xxx
 */
int av_write_trailer(AVFormatContext *s);

/**
 * 返回已注册输出格式列表中与给定参数最匹配的输出格式；如果没有匹配项则返回 NULL。
 *
 * @param short_name 如果为非 NULL，则检查 Short_name 是否与注册格式的名称匹配
 * @param filename 如果为非 NULL，则检查文件名是否以注册格式的扩展名
 * @param mime_type 如果非 NULL 检查 mime_type 是否与注册格式的 MIME 类型匹配
 */
const AVOutputFormat *av_guess_format(const char *short_name,
                                      const char *filename,
                                      const char *mime_type);

/**
 * 根据复用器和文件名猜测编解码器 ID。
 */
enum AVCodecID av_guess_codec(const AVOutputFormat *fmt, const char *short_name,
                              const char *filename, const char *mime_type,
                              enum AVMediaType type);

/**
 * 获取当前输出数据的时序信息。 “当前输出”的确切含义取决于格式。它主要与具有内部缓冲区和/或实时工作的设备相关。
 * @param s 媒体文件句柄
 * @param stream 媒体文件中的流
 * @param[out] dts 流的最后一个数据包输出的 DTS（以流 time_base 为单位）
 * @param[out] wall 该数据包输出时的绝对时间（以微秒为单位）
 * @retval  0 成功
 * @retval  AVERROR(ENOSYS) 该格式不支持
 *
 * @note 某些格式或设备可能不允许自动测量 dts 和 wall。
 */
int av_get_output_timestamp(struct AVFormatContext *s, int stream,
                            int64_t *dts, int64_t *wall);


/**
 * @}
 */


/**
 * @defgroup lavf_misc 实用函数
 * @ingroup libavf
 * @{
 *
 * 与复用和解复用（或两者都不相关）相关的其他实用函数。
 */

/**
 * 将缓冲区的十六进制转储发送到指定的文件流。
 *
 * @param f 转储应发送到的文件流指针。
 * @param buf buffer
 * @param size 缓冲区大小
 *
 * @see av_hex_dump_log、av_pkt_dump2、av_pkt_dump_log2
 */
void av_hex_dump(FILE *f, const uint8_t *buf, int size);

/**
 * 将缓冲区的十六进制转储发送到日志。
 *
 * @param avcl 指向任意结构的指针，其中第一个字段是指向 AVClass 结构的指针。
 * @param level 消息的重要性级别，值越低表示重要性越高。
 * @param buf 缓冲区
 * @param size 缓冲区大小
 *
 * @see av_hex_dump、av_pkt_dump2、av_pkt_dump_log2
 */
void av_hex_dump_log(void *avcl, int level, const uint8_t *buf, int size);

/**
 * 将数据包的良好转储发送到指定的文件流。
 *
 * @param f 转储应发送到的文件流指针。
 * @param pkt 用于转储的数据包
 * @param dump_payload 如果还必须显示有效负载，则为 true。
 * @param st 数据包所属的 AVStream
 */
void av_pkt_dump2(FILE *f, const AVPacket *pkt, int dump_payload, const AVStream *st);


/**
 * 将数据包的良好转储发送到日志。
 *
 * @param avcl 指向任意结构的指针，其中第一个字段是指向 AVClass 结构的指针。
 * @param level 消息的重要性级别，值越低表示重要性越高。
 * @param pkt 用于转储的数据包
 * @param dump_payload 如果还必须显示有效负载，则为 true。
 * @param st 数据包所属的 AVStream
 */
void av_pkt_dump_log2(void *avcl, int level, const AVPacket *pkt, int dump_payload,
                      const AVStream *st);

/**
 * 获取给定编解码器标签的 AVCodecID。如果未找到编解码器 ID，则返回 AV_CODEC_ID_NONE。
 *
 * @param tags 支持的 codec_id-codec_tag 对列表，存储在 AVInputFormat.codec_tag 和 AVOutputFormat.codec_tag 中
 * @param tag 与编解码器 ID 匹配的编解码器标签
 */
enum AVCodecID av_codec_get_id(const struct AVCodecTag * const *tags, unsigned int tag);

/**
 * 获取给定编解码器 ID 的编解码器标签。如果未找到编解码器标签，则返回 0。
 *
 * @param tags 支持的 codec_id-codec_tag 对列表，存储在 AVInputFormat.codec_tag 和 AVOutputFormat.codec_tag 中
 * @param id 与编解码器标签匹配的编解码器 ID
 */
unsigned int av_codec_get_tag(const struct AVCodecTag * const *tags, enum AVCodecID id);

/**
 * 获取给定编解码器 ID 的编解码器标签。
 *
 * @param tags 支持的 codec_id - codec_tag 对的列表，存储在 AVInputFormat.codec_tag 和 AVOutputFormat.codec_tag 中
 * @param id 应在列表中搜索的编解码器 ID
 * @param tag 指向找到的标记的指针
 * @return 如果在标签中未找到 id，则为 0；如果找到，则 > 0
 */
int av_codec_get_tag2(const struct AVCodecTag * const *tags, enum AVCodecID id,
                      unsigned int *tag);

int av_find_default_stream_index(AVFormatContext *s);

/**
 * 获取特定时间戳的索引。
 *
 * @param st 时间戳所属的流
 * @param timestamp 时间戳检索
 * @param flags 的索引 如果 AVSEEK_FLAG_BACKWARD 那么返回的索引将对应于 <= 请求的时间戳，如果向后为 0，则它将 >= 如果AVSEEK_FLAG_ANY 查找任何帧，仅查找关键帧，否则
 * @return < 0（如果找不到此类时间戳）
 */
int av_index_search_timestamp(AVStream *st, int64_t timestamp, int flags);

/**
 * 获取给定 AVStream 的索引条目计数。
 *
 * @param st 流
 * @return 流中索引条目的数量
 */
int avformat_index_get_entries_count(const AVStream *st);

/**
 * 获取与给定索引对应的 AVIndexEntry。
 *
 * @param st 包含所请求的 AVIndexEntry 的流。
 * @param idx 所需的索引。
 * @return 指向所请求的 AVIndexEntry 的指针（如果存在），否则为 NULL。
 *
 * @note 仅保证此函数返回的指针有效，直到调用任何将流或父级 AVFormatContext 作为输入参数的函数为止。
 */
const AVIndexEntry *avformat_index_get_entry(AVStream *st, int idx);

/**
 * 获取给定时间戳对应的AVIndexEntry。
 *
 * @param st 包含所请求的 AVIndexEntry 的流。
 * @param wanted_timestamp 用于检索索引条目的时间戳。
 * @param flags 如果 AVSEEK_FLAG_BACKWARD 那么返回的条目将对应于 <= 请求的时间戳，如果向后为 0，那么如果 AVSEEK_FLAG_ANY 寻找任何帧，则它将>=，否则仅寻找关键帧。
 * @return 指向所请求的 AVIndexEntry 的指针（如果存在），否则为 NULL。
 *
 * @note 仅保证此函数返回的指针有效，直到调用任何将流或父级 AVFormatContext 作为输入参数的函数为止。
 */
const AVIndexEntry *avformat_index_get_entry_from_timestamp(AVStream *st,
                                                            int64_t wanted_timestamp,
                                                            int flags);
/**
 * 将索引条目添加到排序列表中。如果列表已包含该条目，则更新该条目。
 *
 * @param timestamp 给定流的时基中的时间戳
 */
int av_add_index_entry(AVStream *st, int64_t pos, int64_t timestamp,
                       int size, int distance, int flags);


/**
 * 将 URL 字符串拆分为多个组件。
 *
 * 指向用于存储各个组件的缓冲区的指针可以为空，以便忽略该组件。未找到的组件的缓冲区设置为空字符串。如果未找到该端口，则将其设置为负值。
 *
 * @param proto 协议缓冲区
 * @param proto_size proto 缓冲区的大小
 * @param authorization 授权缓冲区
 * @param authorization_size 授权缓冲区的大小
 * @param hostname 主机名
 * @param hostname_size 主机名缓冲区的大小
 * @param port_ptr 用于在
 * @param path 中存储端口号的指针 路径缓冲区
 * @param path_size 路径缓冲区的大小
 * @param url 要分割的 URL
 */
void av_url_split(char *proto,         int proto_size,
                  char *authorization, int authorization_size,
                  char *hostname,      int hostname_size,
                  int *port_ptr,
                  char *path,          int path_size,
                  const char *url);


/**
 * 打印有关输入或输出格式的详细信息，例如持续时间、比特率、流、容器、程序、元数据、辅助数据、编解码器和时基。
 *
 * @param ic 要分析的上下文
 * @param index 用于转储有关信息的流的索引
 * @param url 要打印的 URL，例如源文件或目标文件
 * @param is_output 选择指定的上下文是输入 (0) 还是输出 (1)
 */
void av_dump_format(AVFormatContext *ic,
                    int index,
                    const char *url,
                    int is_output);


#define AV_FRAME_FILENAME_FLAGS_MULTIPLE          1  ///< 允许多个 %d
#define AV_FRAME_FILENAME_FLAGS_IGNORE_TRUNCATION 2  ///< 忽略截断的输出而不是返回错误

/**
 * 将用数字替换 '%d' 后得到的路径写入 'buf'。
 *
 * 还处理“%0nd”格式，其中“n”是位数，“%%”。
 *
 * @param buf 目标缓冲区
 * @param buf_size 目标缓冲区大小
 * @param path 编号序列字符串
 * @param number 帧编号
 * @param flags AV_FRAME_FILENAME_FLAGS_*
 * @return 如果正常则为 0，如果格式错误则为 -1
 */
int av_get_frame_filename2(char *buf, int buf_size,
                          const char *path, int number, int flags);

int av_get_frame_filename(char *buf, int buf_size,
                          const char *path, int number);

/**
 * 检查 filename 是否实际上是编号序列生成器。
 *
 * @param filename 可能的编号序列字符串
 * @return 如果是有效的编号序列字符串则为 1，否则为 0
 */
int av_filename_number_test(const char *filename);

/**
 * 为 RTP 会话生成 SDP。
 *
 * 请注意，这会覆盖复用器上下文中 AVStreams 的 id 值，以获取唯一的动态负载类型。
 *
 * @param ac 描述 RTP 流的 AVFormatContext 数组。如果该数组仅由一个上下文组成，则该上下文可以包含多个 AVStream（每个 RTP 流一个 AVStream）。否则，数组中的所有上下文（每个 RTP 流一个 AVCodecContext）必须仅包含一个 AVStream。
 * @param n_files ac 中包含的 AVCodecContext 数量
 * @param buf 将存储 SDP 的缓冲区（必须由调用者分配）
 * @param size 缓冲区的大小
 * @return 如果正常则为 0，AVERROR_xxx出现错误
 */
int av_sdp_create(AVFormatContext *ac[], int n_files, char *buf, int size);

/**
 * 如果给定文件名具有给定扩展名之一，则返回正值，否则返回 0。
 *
 * @param filename 要根据给定扩展名进行检查的文件名
 * @param extensions 以逗号分隔的文件扩展名列表
 */
int av_match_ext(const char *filename, const char *extensions);

/**
 * 测试给定容器是否可以存储编解码器。
 *
 * @param ofmt 用于检查兼容性的容器
 * @param codec_id 编解码器可能存储在容器中
 * @param std_compliance 标准合规级别，FF_COMPLIANCE_* 之一
 *
 * @return 1（如果具有 ID codec_id 的编解码器可以存储在） ofmt，如果不能则为 0。如果此信息不可用，则为负数。
 */
int avformat_query_codec(const AVOutputFormat *ofmt, enum AVCodecID codec_id,
                         int std_compliance);

struct AVBPrint;
/**
 * 创建类似于 RFC 4281/6381 的字符串，描述 MIME 类型的编解码器。
 *
 * @param par 指向描述编解码器的 AVCodecParameters 结构的指针
 * @param frame_rate 帧速率的 AVRational，用于确定视频编解码器的正确配置文件。传递无效的 AVRational (1/0) 以指示它未知。
 * @param out AVBPrint 将输出写入
 * @return <0 错误
 */
int av_mime_codec_str(const AVCodecParameters *par,
                      AVRational frame_rate, struct AVBPrint *out);

/**
 * @defgroup riff_fourcc RIFF FourCCs
 * @{
 * 获取将 RIFF FourCCs 映射到 libavcodec AVCodecID 的表。这些表旨在传递到 av_codec_get_id()/av_codec_get_tag()，如以下代码所示：
 * @code
 * uint32_t tag = MKTAG('H', '2', '6', '4');
 * const struct AVCodecTag *table[] = { avformat_get_riff_video_tags(), 0 };
 * enum AVCodecID id = av_codec_get_id(table, tag);
 * @endcode
 */
/**
 * @return 将视频的 RIFF FourCC 映射到 libavcodec AVCodecID 的表。
 */
const struct AVCodecTag *avformat_get_riff_video_tags(void);
/**
 * @return 将音频的 RIFF FourCC 映射到 AVCodecID 的表。
 */
const struct AVCodecTag *avformat_get_riff_audio_tags(void);
/**
 * @return 将视频的 MOV FourCC 映射到 libavcodec AVCodecID 的表。
 */
const struct AVCodecTag *avformat_get_mov_video_tags(void);
/**
 * @return 将音频的 MOV FourCC 映射到 AVCodecID 的表。
 */
const struct AVCodecTag *avformat_get_mov_audio_tags(void);

/**
 * @}
 */

/**
 * 根据流和帧长宽比猜测帧的样本长宽比。
 *
 * 由于帧宽高比由编解码器设置，而流宽高比由解复用器设置，因此这两者可能不相等。如果您想显示框架，此函数会尝试返回您应该使用的值。
 *
 * 基本逻辑是使用流宽高比（如果设置为合理），否则使用帧宽高比。这样，通常很容易修改的容器设置可以覆盖帧中的编码值。
 *
 * @param format 流是其一部分的格式上下文
 * @param stream 帧是其一部分的流
 * @param frame 长宽比待确定的帧
 * @return 猜测的（有效）sample_aspect_ratio，0/1如果不知道
 */
AVRational av_guess_sample_aspect_ratio(AVFormatContext *format, AVStream *stream,
                                        struct AVFrame *frame);

/**
 * 根据容器和编解码器信息猜测帧速率。
 *
 * @param ctx 流是其一部分的格式上下文
 * @param stream 帧是其一部分的流
 * @param frame 应确定帧速率的帧，可能为 NULL
 * @return 猜测的（有效）帧速率，如果不知道则为 0/1
 */
AVRational av_guess_frame_rate(AVFormatContext *ctx, AVStream *stream,
                               struct AVFrame *frame);

/**
 * 检查 s 中包含的流 st 是否与流说明符规范匹配。
 *
 * 有关spec 语法的信息，请参阅文档中的“流说明符”一章。
 *
 * @return >0 如果 st 与规格匹配；如果 st 与 spec 不匹配，则为 0；如果规范无效，则为 AVERROR 代码
 *
 * @note 流说明符可以匹配格式中的多个流。
 */
int avformat_match_stream_specifier(AVFormatContext *s, AVStream *st,
                                    const char *spec);

int avformat_queue_attached_pictures(AVFormatContext *s);

/**
 * @}
 */

#endif /* AVFORMAT_AVFORMAT_H */
