/*
 * Bitstream filters public API
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

#ifndef AVCODEC_BSF_H
#define AVCODEC_BSF_H

#include "libavutil/dict.h"
#include "libavutil/log.h"
#include "libavutil/rational.h"

#include "codec_id.h"
#include "codec_par.h"
#include "packet.h"

/**
 * @defgroup lavc_bsf 比特流过滤器
 * @ingroup libavc
 *
 * 比特流过滤器无需解码即可转换编码媒体数据，例如修改各种头字段值。
 * 比特流过滤器对 @ref AVPacket "AVPacket" 进行操作。
 *
 * 比特流过滤 API 以 AVBitStreamFilter 和 AVBSFContext 两个结构体为核心。
 * 前者抽象表示比特流过滤器，后者表示一次具体过滤过程。使用
 * av_bsf_get_by_name() 或 av_bsf_iterate() 获取 AVBitStreamFilter，
 * 再传给 av_bsf_alloc() 创建 AVBSFContext。按文档填充用户可设置字段后，
 * 调用 av_bsf_init() 准备过滤器上下文。
 *
 * 使用 av_bsf_send_packet() 提交待过滤数据包，通过 av_bsf_receive_packet()
 * 获取过滤结果。不再发送输入包时，提交 NULL AVPacket 通知过滤器流已结束。
 * 此后 av_bsf_receive_packet() 会返回过滤器产生的尾随数据包（如果有）。
 *
 * 最后使用 av_bsf_free() 释放过滤器上下文。
 * @{
 */

/**
 * 比特流过滤器状态。
 *
 * 必须使用 av_bsf_alloc() 分配此结构体，并使用 av_bsf_free() 释放。
 *
 * 结构体字段只能由调用方或过滤器按照各字段文档进行更改，其他情况下应视为不可变。
 */
typedef struct AVBSFContext {
    /**
     * 用于日志记录和 AVOptions 的类
     */
    const AVClass *av_class;

    /**
     * 此上下文所实例化的比特流过滤器。
     */
    const struct AVBitStreamFilter *filter;

    /**
     * 过滤器专用的不透明私有数据。如果 filter->priv_class 非 NULL，
     * 则这是启用了 AVOptions 的结构体。
     */
    void *priv_data;

    /**
     * 输入流参数。此字段由 av_bsf_alloc() 分配，调用方需要在
     * av_bsf_init() 之前填充。
     */
    AVCodecParameters *par_in;

    /**
     * 输出流参数。此字段由 av_bsf_alloc() 分配，并由过滤器在
     * av_bsf_init() 中设置。
     */
    AVCodecParameters *par_out;

    /**
     * 输入包时间戳使用的时间基。由调用方在 av_bsf_init() 前设置。
     */
    AVRational time_base_in;

    /**
     * 输出包时间戳使用的时间基。由过滤器在 av_bsf_init() 中设置。
     */
    AVRational time_base_out;
} AVBSFContext;

typedef struct AVBitStreamFilter {
    const char *name;

    /**
     * 过滤器支持的编解码器 ID 列表，以 AV_CODEC_ID_NONE 结尾。
     * 可以为 NULL，此时比特流过滤器适用于任意编解码器 ID。
     */
    const enum AVCodecID *codec_ids;

    /**
     * 私有数据的类，用于声明比特流过滤器的私有 AVOptions。
     * 不声明任何选项的比特流过滤器中，此字段为 NULL。
     *
     * 如果此字段非 NULL，过滤器私有数据的第一个成员必须是 AVClass 指针，
     * libavcodec 通用代码会将该指针设为此类。
     */
    const AVClass *priv_class;
} AVBitStreamFilter;

/**
 * @return 指定名称的比特流过滤器；不存在时返回 NULL。
 */
const AVBitStreamFilter *av_bsf_get_by_name(const char *name);

/**
 * 遍历所有已注册的比特流过滤器。
 *
 * @param opaque libavcodec 用于存储迭代状态的指针。开始迭代时必须指向 NULL。
 *
 * @return 下一个已注册的比特流过滤器；迭代结束时返回 NULL
 */
const AVBitStreamFilter *av_bsf_iterate(void **opaque);

/**
 * 为给定比特流过滤器分配上下文。调用方必须按文档填充上下文参数，
 * 并在向过滤器发送任何数据之前调用 av_bsf_init()。
 *
 * @param filter 要为其分配实例的过滤器
 * @param[out] ctx 用于写入新分配上下文指针的位置。过滤完成后必须使用
 *                 av_bsf_free() 释放。
 *
 * @return 成功返回 0，失败返回负的 AVERROR 错误码
 */
int av_bsf_alloc(const AVBitStreamFilter *filter, AVBSFContext **ctx);

/**
 * 设置全部参数和选项后，准备过滤器以供使用。
 *
 * @param ctx 之前使用 av_bsf_alloc() 分配的 AVBSFContext
 */
int av_bsf_init(AVBSFContext *ctx);

/**
 * 提交数据包进行过滤。
 *
 * 每次发送数据包后，必须重复调用 av_bsf_receive_packet()，直到返回
 * AVERROR(EAGAIN) 或 AVERROR_EOF，以完全取空过滤器输出。
 *
 * @param ctx 已初始化的 AVBSFContext
 * @param pkt 要过滤的数据包。比特流过滤器取得数据包所有权并重置 pkt 内容。
 *            发生错误时不修改 pkt。如果 pkt 为空（即为 NULL，或 pkt->data 为 NULL
 *            且 pkt->side_data_elems 为零），表示流结束，并使过滤器输出内部缓冲的包。
 *
 * @return
 *  - 成功时返回 0。
 *  - 如果消费新输入前需要用 av_bsf_receive_packet() 从过滤器取包，
 *    返回 AVERROR(EAGAIN)。
 *  - 发生错误时返回其他负的 AVERROR 值。
 */
int av_bsf_send_packet(AVBSFContext *ctx, AVPacket *pkt);

/**
 * 获取过滤后的数据包。
 *
 * @param ctx 已初始化的 AVBSFContext
 * @param[out] pkt 此结构体将填入过滤后数据包的内容。它归调用方所有，
 *                 不再需要时必须使用 av_packet_unref() 释放。
 *                 调用时该参数应是“干净”的（刚由 av_packet_alloc() 分配，
 *                 或已用 av_packet_unref() 解引用）。成功时返回数据会完全覆盖 pkt；
 *                 失败时不修改 pkt。
 *
 * @return
 *  - 成功时返回 0。
 *  - 如需用 av_bsf_send_packet() 向过滤器发送更多包才能获得更多输出，
 *    返回 AVERROR(EAGAIN)。
 *  - 过滤器不会再有输出时返回 AVERROR_EOF。
 *  - 发生错误时返回其他负的 AVERROR 值。
 *
 * @note 一个输入包可能产生多个输出包，因此使用 av_bsf_send_packet() 发送包后，
 * 需要重复调用此函数，直到不再返回 0。过滤器输出的包也可能少于输入包，
 * 因此成功调用 av_bsf_send_packet() 后，此函数可能立即返回 AVERROR(EAGAIN)。
 */
int av_bsf_receive_packet(AVBSFContext *ctx, AVPacket *pkt);

/**
 * 重置比特流过滤器内部状态。例如在定位时应调用。
 */
void av_bsf_flush(AVBSFContext *ctx);

/**
 * 释放比特流过滤器上下文及其关联的所有内容，并向提供的指针写入 NULL。
 */
void av_bsf_free(AVBSFContext **ctx);

/**
 * 获取 AVBSFContext 的 AVClass。可与 AV_OPT_SEARCH_FAKE_OBJ 结合使用以检查选项。
 *
 * @see av_opt_find().
 */
const AVClass *av_bsf_get_class(void);

/**
 * 比特流过滤器链/列表结构体。
 * 可通过 av_bsf_list_alloc() 分配空列表。
 */
typedef struct AVBSFList AVBSFList;

/**
 * 分配空的比特流过滤器列表。
 * 之后必须使用 av_bsf_list_free() 释放，或使用 av_bsf_list_finalize() 完成。
 *
 * @return 成功返回指向 @ref AVBSFList 的指针，失败返回 NULL
 */
AVBSFList *av_bsf_list_alloc(void);

/**
 * 释放比特流过滤器列表。
 *
 * @param lst 指向 av_bsf_list_alloc() 返回指针的指针
 */
void av_bsf_list_free(AVBSFList **lst);

/**
 * 将比特流过滤器追加到列表。
 *
 * @param lst 要追加到的列表
 * @param bsf 要追加的过滤器上下文
 *
 * @return 成功返回 >=0，失败返回负的 AVERROR
 */
int av_bsf_list_append(AVBSFList *lst, AVBSFContext *bsf);

/**
 * 根据名称和选项构造新的比特流过滤器上下文，并将其追加到过滤器列表。
 *
 * @param lst      要追加到的列表
 * @param bsf_name 比特流过滤器名称
 * @param options  比特流过滤器选项，可以设为 NULL
 *
 * @return 成功返回 >=0，失败返回负的 AVERROR
 */
int av_bsf_list_append2(AVBSFList *lst, const char * bsf_name, AVDictionary **options);
/**
 * 完成比特流过滤器列表。
 *
 * 此函数将 @ref AVBSFList 转换为单个 @ref AVBSFContext ，使整个比特流过滤器链
 * 可像 av_bsf_alloc() 新分配的单个过滤器一样处理。调用成功时释放
 * @ref AVBSFList 并将 lst 设为 NULL；失败时调用方负责使用
 * av_bsf_list_free() 释放结构体。
 *
 * @param      lst 要转换的过滤器列表结构体
 * @param[out] bsf 用于写入新创建 @ref AVBSFContext 的指针，该上下文表示过滤器链
 *
 * @return 成功返回 >=0，失败返回负的 AVERROR
 */
int av_bsf_list_finalize(AVBSFList **lst, AVBSFContext **bsf);

/**
 * 解析描述比特流过滤器列表的字符串，并创建表示整个过滤器链的单个
 * @ref AVBSFContext 。得到的 @ref AVBSFContext 可像其他由 av_bsf_alloc() 新分配的
 * @ref AVBSFContext 一样处理。
 *
 * @param      str 描述比特流过滤器链的字符串，格式为
 *                 `bsf1[=opt1=val1:opt2=val2][,bsf2]`
 * @param[out] bsf 用于写入新创建 @ref AVBSFContext 的指针，该上下文表示过滤器链
 *
 * @return 成功返回 >=0，失败返回负的 AVERROR
 */
int av_bsf_list_parse_str(const char *str, AVBSFContext **bsf);

/**
 * 获取空/直通比特流过滤器。
 *
 * @param[out] bsf 用于写入新直通过滤器实例的指针
 *
 * @return
 */
int av_bsf_get_null_filter(AVBSFContext **bsf);

/**
 * @defgroup lavc_bsfgraph 比特流过滤器图
 * 实验性的图式比特流过滤器 API。
 * @{
 */

/**
 * 两个过滤器之间的链接。包含源和目标过滤器指针，以及相关 pad 的索引。
 */
typedef struct AVBitStreamFilterLink AVBitStreamFilterLink;

/**
 * 用于输入或输出的过滤器 pad。
 */
typedef struct AVBitStreamFilterPad AVBitStreamFilterPad;

/** 过滤器实例 */
typedef struct AVBitStreamFilterContext {
    /**
     * 用于日志记录和 AVOptions 的类
     */
    const AVClass *av_class;

    /**
     * 此上下文所实例化的比特流过滤器。
     */
    const struct AVBitStreamFilter *filter;

    /**
     * 此过滤器实例的名称
     */
    char *name;

    AVBitStreamFilterPad  *input_pads; ///< 输入 pad 数组
    AVBitStreamFilterLink    **inputs; ///< 输入链接指针数组
    unsigned                nb_inputs; ///< 输入 pad 数量

    AVBitStreamFilterPad *output_pads; ///< 输出 pad 数组
    AVBitStreamFilterLink   **outputs; ///< 输出链接指针数组
    unsigned               nb_outputs; ///< 输出 pad 数量

    /**
     * 过滤器专用的不透明私有数据。如果 filter->priv_class 非 NULL，
     * 则这是启用了 AVOptions 的结构体。
     */
    void *priv_data;

    /**
     * 此过滤器所属的过滤器图
     */
    struct AVBitStreamFilterGraph *graph;
} AVBitStreamFilterContext;

/**
 * 过滤器输入数量并非仅由其静态输入决定。过滤器可能根据提供的选项在初始化时添加输入。
 */
#define AV_BSF_FLAG_DYNAMIC_INPUTS        (1 << 0)
/**
 * 过滤器输出数量并非仅由其静态输出决定。过滤器可能根据提供的选项在初始化时添加输出。
 */
#define AV_BSF_FLAG_DYNAMIC_OUTPUTS       (1 << 1)
/**
 * 该过滤器是“元数据”过滤器，不以任何方式修改数据包数据。
 * 它只能影响元数据（即 av_packet_copy_props() 复制的字段）。
 *
 * 更准确地说，过滤器输出的任意数据包必须与某个输入接收的数据包完全相同。
 * 此外，给定输出产生的所有包必须对应同一输入接收的包，且顺序不得改变。
 * 注意，过滤器仍可能丢弃或复制帧。
 */
#define AV_BSF_FLAG_METADATA_ONLY         (1 << 2)

/**
 * 获取 AVBitStreamFilterPad 的名称。
 *
 * @param pads AVBitStreamFilterPad 数组
 * @param pad_idx pad 在数组中的索引；调用方负责确保索引有效
 *
 * @return pads 中第 pad_idx 个 pad 的名称
 */
const char *av_bsf_pad_get_name(const AVBitStreamFilterPad *pads, int pad_idx);

/**
 * 获取 AVBitStreamFilterPad 支持的编解码器 ID。
 *
 * @param pads AVBitStreamFilterPad 数组
 * @param pad_idx pad 在数组中的索引；调用方负责确保索引有效
 *
 * @return 以 AV_CODEC_ID_NONE 结尾的 AVCodecID 数组；pad 没有编解码器 ID
 *         约束时返回 NULL。
 */
const enum AVCodecID *av_bsf_pad_get_codec_ids(const AVBitStreamFilterPad *pads, int pad_idx);

/**
 * 将两个过滤器链接在一起。
 *
 * @param src    源过滤器
 * @param srcpad 源过滤器输出 pad 的索引
 * @param dst    目标过滤器
 * @param dstpad 目标过滤器输入 pad 的索引
 * @return       成功返回 0
 */
int av_bsf_link(AVBitStreamFilterContext *src, unsigned srcpad,
                AVBitStreamFilterContext *dst, unsigned dstpad);

/**
 * 使用提供的参数初始化过滤器。
 *
 * @param ctx  要初始化的未初始化过滤器上下文
 * @param args 初始化过滤器的选项。必须是以 ':' 分隔、形式为 'key=value' 的选项列表。
 *             如果选项已通过 AVOptions API 直接设置，或无需设置选项，则可为 NULL。
 * @return 成功返回 0，失败返回负的 AVERROR
 */
int av_bsf_init_str(AVBitStreamFilterContext *ctx, const char *args);

/**
 * 使用提供的选项字典初始化过滤器。
 *
 * @param ctx     要初始化的未初始化过滤器上下文
 * @param options 填有此过滤器选项的 AVDictionary。返回时此参数会被销毁，
 *                并替换为包含未找到选项的字典。该字典必须由调用方释放。
 *                可以为 NULL，此时等价于第二个参数为 NULL 的 av_bsf_init_str()。
 * @return 成功返回 0，失败返回负的 AVERROR
 *
 * @note 此函数与 av_bsf_init_str() 的作用基本相同，区别在于选项传递方式。
 * 调用代码可选择更合适的函数。当部分选项未声明为过滤器支持时，两者行为也不同：
 * av_bsf_init_str() 会失败，而此函数会将额外选项留在 options AVDictionary 中并继续。
 */
int av_bsf_init_dict(AVBitStreamFilterContext *ctx, AVDictionary **options);

typedef struct AVBitStreamFilterGraph {
    const AVClass *av_class;

    AVBitStreamFilterContext **filters;

    unsigned nb_filters;

    /**
     * 设置整个过滤器图合计可缓冲的最大数据包数。
     *
     * 0 表示无限制。必须在调用 av_bsf_graph_config() 前设置此字段。
     */
    unsigned max_buffered_packets;
} AVBitStreamFilterGraph;

/**
 * 分配过滤器图。
 *
 * @return 成功返回已分配的过滤器图，否则返回 NULL。
 */
AVBitStreamFilterGraph *av_bsf_graph_alloc(void);

/**
 * 在过滤器图中创建新的过滤器实例。
 *
 * @param[out] filt_ctx 成功时用于写入新分配上下文指针的位置，可以为 NULL。
 *                      也可通过 AVBitStreamFilterGraph.filters 或
 *                      @ref av_bsf_graph_get_filter() 获取。
 * @param[in] filter 要实例化的过滤器
 * @param[in] name 新实例名称（会复制到 AVBitStreamFilterContext.name）。
 *                 调用方可用它区分过滤器；libavcodec 不赋予其语义。可以为 NULL。
 * @param[in] graph 使用新过滤器的图
 *
 * @note 失败且 filt_ctx 非 NULL 时，*filt_ctx 会被设为 NULL。
 * @return 失败返回负的 AVERROR 错误码，否则返回非负值
 */
int av_bsf_graph_alloc_filter(AVBitStreamFilterContext **filt_ctx,
                              const AVBitStreamFilter *filter,
                              const char *name,
                              AVBitStreamFilterGraph *graph);

/**
 * 在单一步骤中分配并初始化过滤器的便捷包装函数。
 * 过滤器实例由 filt 创建，并使用参数 args 初始化。
 *
 * @param[out] filt_ctx 成功时用于写入新分配上下文指针的位置，可以为 NULL。
 *                      也可通过 AVBitStreamFilterGraph.filters 或
 *                      @ref av_bsf_graph_get_filter() 获取。
 * @param[in] name 创建的过滤器实例名称
 * @param[in] graph_ctx 过滤器图
 * @return 失败返回负的 AVERROR 错误码，否则返回非负值
 *
 * @note 失败且 filt_ctx 非 NULL 时，*filt_ctx 会被设为 NULL。
 * @warning 此函数成功返回后过滤器已初始化，因此绝对不能再设置任何选项。
 *          如需继续设置，请改为依次调用 ::av_bsf_graph_alloc_filter()、
 *          设置选项、::av_bsf_init_dict()。
 */
int av_bsf_graph_create_filter(AVBitStreamFilterContext **filt_ctx,
                               const AVBitStreamFilter *filt,
                               const char *name, AVDictionary **options,
                               AVBitStreamFilterGraph *graph_ctx);

/**
 * 从图中获取由实例名称标识的过滤器实例。
 *
 * @param graph 要搜索的过滤器图
 * @param name 过滤器实例名称（在图中应唯一）
 * @return 找到的过滤器实例指针；找不到时返回 NULL
 */
AVBitStreamFilterContext *av_bsf_graph_get_filter(AVBitStreamFilterGraph *graph, const char *name);

/**
 * 检查有效性并配置图中的所有链接和格式。
 *
 * @param graphctx 过滤器图
 * @param log_ctx 用于日志记录的上下文
 * @return 成功返回 >= 0，否则返回负的 AVERROR 错误码
 */
int av_bsf_graph_config(AVBitStreamFilterGraph *graphctx, void *log_ctx);

/**
 * 获取过滤器图中报告更急需输入的源过滤器索引。
 *
 * @return 过滤器图中源过滤器的索引；没有源再接受数据包时返回 AVERROR(EOF)
 */
int av_bsf_graph_source_needs_input(const AVBitStreamFilterGraph *graph);

/**
 * 释放图、销毁链接并将 *graph 设为 NULL。如果 *graph 为 NULL，则不执行任何操作。
 */
void av_bsf_graph_free(AVBitStreamFilterGraph **graph);

/**
 * @defgroup lavc_bsfgraph_source 数据包源 API
 *
 * 源过滤器用于将过滤器图连接到应用程序。它只有一个连接到图的输出，没有输入。
 * 必须使用 av_bsf_source_add_packet() 向其输入数据包。
 * @{
 */

enum {
    /**
     * 立即将数据包推送到输出。
     */
    AV_BSF_SOURCE_FLAG_PUSH = 1 << 0,

    /**
     * 保留数据包引用。
     */
    AV_BSF_SOURCE_FLAG_KEEP_REF = 1 << 1,
};

/**
 * 使用提供的参数初始化源过滤器。此函数可多次调用，后续调用覆盖之前调用。
 * 某些参数也可通过 AVOptions 设置，以最后使用的方法为准。
 *
 * @param ctx 源过滤器实例
 * @param param 流参数。之后传给此过滤器的数据包必须符合这些参数。
 *              param 中所有已分配字段仍归调用方所有，libavcodec 会按需创建内部副本或引用。
 * @return 成功返回 0，失败返回负的 AVERROR 错误码。
 */
int av_bsf_source_parameters_set(AVBitStreamFilterContext *ctx, const AVCodecParameters *par);

/**
 * 向缓冲区源添加数据包。
 *
 * 默认情况下，此函数取得引用的所有权并重置数据包。可使用 flags 控制此行为。
 *
 * 如果此函数返回错误，则不修改输入数据包。
 *
 * @param buffer_src 指向源过滤器上下文的指针
 * @param packet     数据包；传入 NULL 表示 EOF
 * @param flags      AV_BSF_FLAG_* 的组合
 * @return           成功返回 >= 0，失败返回负的 AVERROR 错误码
 */
av_warn_unused_result
int av_bsf_source_add_packet(AVBitStreamFilterContext *ctx, AVPacket *pkt, int flags);

/**
 * 返回 0 或负的 AVERROR 错误码。目前只会返回 AVERROR(EOF)，表示缓冲区源已关闭：
 * 可能是调用 av_bsf_source_close() 的结果，也可能是下游过滤器不再接受新数据。
 */
int av_bsf_source_get_status(AVBitStreamFilterContext *ctx);

/**
 * 在 EOF 后关闭源。
 *
 * 这与向 av_bsf_source_add_packet() 传入 NULL 类似，但它接收 EOF 的时间戳，
 * 即最后一个数据包结束位置的时间戳。
 */
int av_bsf_source_close(AVBitStreamFilterContext *ctx, int64_t pts, unsigned flags);

/**
 * @}
 */

/**
 * @defgroup lavc_bsfgraph_sink 数据包汇 API
 * @{
 *
 * 汇过滤器用于将过滤器图连接到应用程序。它只有一个连接到图的输入，没有输出。
 * 必须使用 av_bsf_sink_get_packet() 提取数据包。
 */

enum {
    /**
     * 通知 av_buffersink_get_buffer_ref() 读取视频/采样缓冲区引用，但不从缓冲区移除。
     * 仅需读取而不取出视频/采样缓冲区时很有用。
     */
    AV_BSF_SINK_FLAG_PEEK = 1 << 0,

    /**
     * 通知 av_bsf_sink_get_packet() 不要向输入请求数据包。已有缓冲包时读取并移除；
     * 没有数据包时返回 AVERROR(EAGAIN)。
     */
    AV_BSF_SINK_FLAG_NO_REQUEST = 1 << 1,
};

/**
 * 从汇获取包含过滤数据的数据包，并放入 packet。
 *
 * @param ctx    指向汇过滤器上下文的指针
 * @param packet 指向已分配数据包的指针，该包将被填入数据。
 *               必须使用 av_packet_unref() / av_packet_free() 释放数据
 * @param flags  AV_BSF_SINK_FLAG_* 标志的组合
 *
 * @retval AVERROR(EAGAIN) 无法产生输出。如果未设置 AV_BSF_SINK_FLAG_NO_REQUEST，
 *                         可调用 @ref av_bsf_graph_needs_input 了解哪个源更急需输入。
 * @retval >= 0            成功
 * @retval "another negative error code" 合法错误
 */
int av_bsf_sink_get_packet(AVBitStreamFilterContext *ctx, AVPacket *pkt, int flags);

AVRational av_bsf_sink_get_time_base(const AVBitStreamFilterContext *ctx);
const AVCodecParameters *av_bsf_sink_get_parameters(const AVBitStreamFilterContext *ctx);

/**
 * @}
 *
 * @}
 *
 * @}
 */

#endif // AVCODEC_BSF_H
