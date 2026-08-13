/*
 * 滤镜层
 * Copyright (c) 2007 Bobby Bingham
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

#ifndef AVFILTER_AVFILTER_H
#define AVFILTER_AVFILTER_H

/**
 * @file
 * @ingroup lavfi 主要 libavfilter 公共 API 标头
 */

/**
 * @defgroup lavfi libavfilter 基于图形的帧编辑库。
 *
 * @{
 */

#include <stddef.h>

#include "libavutil/avutil.h"
#include "libavutil/buffer.h"
#include "libavutil/dict.h"
#include "libavutil/frame.h"
#include "libavutil/log.h"
#include "libavutil/pixfmt.h"
#include "libavutil/rational.h"

#include "libavfilter/version_major.h"
#ifndef HAVE_AV_CONFIG_H
/*
 * 当包含在 ffmpeg 构建中时，仅包含主要版本以避免不必要的重建。当外部包含时，请保留完整的版本信息。
 */
#include "libavfilter/version.h"
#endif

/**
 * 返回 LIBAVFILTER_VERSION_INT 常量。
 */
unsigned avfilter_version(void);

/**
 * 返回 libavfilter 的构建时配置。
 */
const char *avfilter_configuration(void);

/**
 * 返回 libavfilter 的许可证信息。
 */
const char *avfilter_license(void);

typedef struct AVFilterLink    AVFilterLink;
typedef struct AVFilterPad     AVFilterPad;
typedef struct AVFilterFormats AVFilterFormats;
typedef struct AVFilterChannelLayouts AVFilterChannelLayouts;

/**
 * 获取 AVFilterPad 的名称。
 *
 * @param pads AVFilterPad 数组
 * @param pad_idx 数组中 pad 的索引；调用者有责任确保索引有效
 *
 * @return pads 中 pad_idx'th pad 的名称
 */
const char *avfilter_pad_get_name(const AVFilterPad *pads, int pad_idx);

/**
 * 获取 AVFilterPad 的类型。
 *
 * @param pads AVFilterPad 数组
 * @param pad_idx 数组中 pad 的索引；调用者有责任确保索引有效
 *
 * @return pads 中 pad_idx'th pad 的类型
 */
enum AVMediaType avfilter_pad_get_type(const AVFilterPad *pads, int pad_idx);

/**
 * 获取过滤器链接的硬件框架上下文。
 *
 * @param link AVFilterLink
 *
 * @return 如果存在与链接关联的硬件帧上下文，则为链接的 hw_frames_ctx 字段的引用计数副本，否则为 NULL。返回的 AVBufferRef 不再使用时需要用 av_buffer_unref() 释放。
 */
AVBufferRef* avfilter_link_get_hw_frames_ctx(AVFilterLink *link);

/**
 * 链接末尾支持的格式/等列表。
 *
 * 该结构直接是 AVFilterLink 的一部分，有两份：一份用于源过滤器，一份用于目标过滤器。
 *
 * 这些列表用于协商实际使用的格式，在选择时将加载到 AVFilterLink 的 format 和 channel_layout 成员中。
 */
typedef struct AVFilterFormatsConfig {

    /**
 * 支持的格式列表（像素或样本）。
 */
    AVFilterFormats *formats;

    /**
 * 支持的采样率列表，仅适用于音频。
 */
    AVFilterFormats  *samplerates;

    /**
 * 支持的通道布局列表，仅适用于音频。
 */
    AVFilterChannelLayouts  *channel_layouts;

    /**
 * 支持的 YUV 颜色元数据列表，仅适用于 YUV 视频。
 */
    AVFilterFormats *color_spaces;  ///< AVColorSpace
    AVFilterFormats *color_ranges;  ///< AVColorRange

    /**
 * 支持的 Alpha 模式列表，仅适用于具有 Alpha 通道的视频。
 */
    AVFilterFormats *alpha_modes;  ///< AVAlphaMode

} AVFilterFormatsConfig;

/**
 * 滤波器输入的数量不仅仅由 AVFilter.inputs 决定。过滤器可能会在初始化期间添加额外的输入，具体取决于提供给它的选项。
 */
#define AVFILTER_FLAG_DYNAMIC_INPUTS        (1 << 0)
/**
 * 滤波器输出的数量不仅仅由 AVFilter.outputs 决定。过滤器可能会在初始化期间添加额外的输出，具体取决于提供给它的选项。
 */
#define AVFILTER_FLAG_DYNAMIC_OUTPUTS       (1 << 1)
/**
 * 该过滤器通过将帧拆分为多个部分并同时处理它们来支持多线程。
 */
#define AVFILTER_FLAG_SLICE_THREADS         (1 << 2)
/**
 * 该过滤器是一个“元数据”过滤器 - 它不会以任何方式修改帧数据。它可能只影响元数据（即由 av_frame_copy_props() 复制的那些字段）。
 *
 * 更准确地说，这意味着： - 视频：过滤器输出的任何帧的数据必须完全等于其输入之一接收到的某个帧。此外，给定输出上生成的所有帧必须对应于同一输入上接收到的帧，并且它们的顺序必须保持不变。请注意，过滤器仍可能会丢弃或重复帧。 - 音频：滤波器在其任何输出上产生的数据（例如，视为交错样本数组）必须完全等于滤波器在其输入之一上接收到的数据。
 */
#define AVFILTER_FLAG_METADATA_ONLY         (1 << 3)

/**
 * 过滤器可以使用 AVFilterContext.hw_device_ctx 创建硬件帧。
 */
#define AVFILTER_FLAG_HWDEVICE              (1 << 4)
/**
 * 某些过滤器支持通用“启用”表达式选项，可用于启用或禁用时间线中的过滤器。支持此选项的过滤器设置了此标志。当启用表达式为 false 时，将调用默认的无操作 filter_frame() 函数来代替每个输入板上定义的 filter_frame() 回调，从而将帧原封不动地传递到下一个过滤器。
 */
#define AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC  (1 << 16)
/**
 * 与 AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC 相同，不同之处在于，即使启用表达式为 false，过滤器也会照常调用其 filter_frame() 回调。过滤器将在 filter_frame() 回调本身内禁用过滤，例如根据 AVFilterContext->is_disabled 值执行代码。
 */
#define AVFILTER_FLAG_SUPPORT_TIMELINE_INTERNAL (1 << 17)
/**
 * 方便的掩码，用于测试过滤器是否支持时间线功能（内部或一般）。
 */
#define AVFILTER_FLAG_SUPPORT_TIMELINE (AVFILTER_FLAG_SUPPORT_TIMELINE_GENERIC | AVFILTER_FLAG_SUPPORT_TIMELINE_INTERNAL)

/**
 * 过滤器定义。这定义了过滤器包含的 pad，以及用于与过滤器交互的所有回调函数。
 */
typedef struct AVFilter {
    /**
 * 过滤器名称。必须为非 NULL 并且在过滤器中是唯一的。
 */
    const char *name;

    /**
 * 过滤器的描述。可能为 NULL。
 *
 * 您应该使用 NULL_IF_CONFIG_SMALL() 宏来定义它。
 */
    const char *description;

    /**
 * 静态输入列表。
 *
 * 如果没有（静态）输入，则为 NULL。设置了 AVFILTER_FLAG_DYNAMIC_INPUTS 的过滤器实例可能具有比此列表中显示的更多的输入。
 */
    const AVFilterPad *inputs;

    /**
 * 静态输出列表。
 *
 * 如果没有（静态）输出，则为 NULL。设置了 AVFILTER_FLAG_DYNAMIC_OUTPUTS 的过滤器实例可能具有比此列表中显示的更多的输出。
 */
    const AVFilterPad *outputs;

    /**
 * 私有数据类，用于声明过滤器私有AVOptions。对于未声明任何选项的过滤器，此字段为 NULL。
 *
 * 如果此字段为非 NULL，则过滤器私有数据的第一个成员必须是指向 AVClass 的指针，该指针将由 libavfilter 通用代码设置到此类。
 */
    const AVClass *priv_class;

    /**
 * AVFILTER_FLAG_* 的组合
 */
    int flags;
} AVFilter;

/**
 * 获取 AVFilter 的输入或输出数组中的元素数量。
 */
unsigned avfilter_filter_pad_count(const AVFilter *filter, int is_output);

/**
 * 同时处理框架的多个部分。
 */
#define AVFILTER_THREAD_SLICE (1 << 0)

/** 过滤器的实例 */
typedef struct AVFilterContext {
    const AVClass *av_class;        ///< av_log() 和过滤器公共选项所需

    const AVFilter *filter;         ///< 这是其实例的 AVFilter

    char *name;                     ///< 此过滤器实例的名称

    AVFilterPad   *input_pads;      ///< 输入焊盘数组
    AVFilterLink **inputs;          ///< 指向输入链接的指针数组
    unsigned    nb_inputs;          ///< 输入焊盘数量

    AVFilterPad   *output_pads;     ///< 输出焊盘数组
    AVFilterLink **outputs;         ///< 指向输出链接的指针数组
    unsigned    nb_outputs;         ///< 输出焊盘数量

    void *priv;                     ///< 供过滤器使用的私有数据

    struct AVFilterGraph *graph;    ///< filtergraph 该过滤器属于

    /**
 * 允许/使用的多线程类型。 AVFILTER_THREAD_* 标志的组合。
 *
 * 可以由调用者在初始化过滤器之前设置，以禁止此过滤器的某些或所有类型的多线程。默认设置是允许一切。
 *
 * 初始化过滤器时，使用位 AND 与 AVFilterGraph.thread_type 组合该字段，以获得用于确定允许的线程类型的最终掩码。 IE。需要在两者中设置线程类型才能允许。
 *
 * 过滤器初始化后，libavfilter 将此字段设置为实际使用的线程类型（0 表示无多线程）。
 */
    int thread_type;

    /**
 * 此过滤器实例中允许的最大线程数。如果 <= 0，则忽略其值。覆盖每个过滤器图表设置的全局线程数。
 */
    int nb_threads;

    char *enable_str;               ///< 启用表达式字符串
    /**
 * 不得从外部 avfilter 访问。
 *
 * 最后一次表达式求值的启用状态
 */
    int is_disabled;

    /**
 * 对于将创建硬件帧的过滤器，设置过滤器应在其中创建它们的设备。所有其他过滤器将忽略此字段：特别是，消耗或处理硬件帧的过滤器将使用 AVFilterLink 中的 hw_frames_ctx 字段来携带硬件上下文信息。
 *
 * 在使用 avfilter_init_str() 或 avfilter_init_dict() 初始化过滤器之前，可以由调用者在标记有 AVFILTER_FLAG_HWDEVICE 的过滤器上设置。
 */
    AVBufferRef *hw_device_ctx;

    /**
 * 设置过滤器将在其输出链路上分配的额外硬件帧的数量，以供后续过滤器或调用者使用。
 *
 * 某些硬件过滤器要求在过滤开始之前提前定义它们将用于输出的所有帧。  对于此类过滤器，用于输出的任何硬件帧池都必须具有固定大小。  此处设置的额外帧位于过滤器内部正常运行所需的任何数量之上。
 *
 * 在配置包含此过滤器的图形之前必须设置此字段。
 */
    int extra_hw_frames;
} AVFilterContext;

/**
 * 两个过滤器之间的链接。它包含指向源过滤器和目标过滤器（其间存在此链接）的指针，以及所涉及的 pad 的索引。此外，该链接还包含过滤器之间已协商同意的参数，例如图像尺寸、格式等。
 *
 * 应用程序通常不得直接访问该链接结构。请改用 buffersrc 和 buffersink API。将来，对标头的访问可能会保留用于过滤器实现。
 */
struct AVFilterLink {
    AVFilterContext *src;       ///< 源滤波器
    AVFilterPad *srcpad;        ///< 源滤波器上的输出焊盘

    AVFilterContext *dst;       ///< 目标滤波器
    AVFilterPad *dstpad;        ///< 目标滤波器上的输入焊盘

    enum AVMediaType type;      ///< 过滤介质类型

    int format;                 ///< 商定的介质格式

    /* 这些参数仅适用于视频 */
    int w;                      ///< 商定的图像宽度
    int h;                      ///< 商定的图像高度
    AVRational sample_aspect_ratio; ///< 商定的样本纵横比
    /**
 * 对于非 YUV 链接，这些分别设置为后备值（适合该色彩空间）。
 *
 * 注意：这包括灰度格式，因为这些格式目前始终被视为强制全范围。
 */
    enum AVColorSpace colorspace;   ///< 商定的 YUV 颜色空间
    enum AVColorRange color_range;  ///< 商定的 YUV 颜色范围

    /* 这些参数仅适用于音频 */
    int sample_rate;            ///< 每秒采样数
    AVChannelLayout ch_layout;  ///< 当前缓冲区的通道布局（请参阅 libavutil/channel_layout.h）

    /**
 * 定义将通过此链接的帧/样本的 PTS 使用的时基。在配置阶段，每个滤波器应该仅更改输出时基，而输入链路的时基被假定为不可更改的属性。
 */
    AVRational time_base;

    AVFrameSideData **side_data;
    int nb_side_data;

    enum AVAlphaMode alpha_mode; ///< Alpha 模式（适用于具有 Alpha 通道的视频）

    /**
 * **************************************************************
 * 此行下方的所有字段都不属于公共 API 的一部分。它们不能在 libavfilter 之外使用，并且可以随意更改和删除。新的公共字段应添加到正上方。
 * ****************************************************************
 */

    /**
 * 输入过滤器支持的支持格式/等列表。
 */
    AVFilterFormatsConfig incfg;

    /**
 * 输出过滤器支持的支持格式/等列表。
 */
    AVFilterFormatsConfig outcfg;
};

/**
 * 将两个过滤器连接在一起。
 *
 * @param src 源过滤器
 * @param srcpad 源过滤器上输出 pad 的索引
 * @param dst 目标过滤器
 * @param dstpad 目标过滤器上输入 pad 的索引
 * @return 成功时为零
 */
int avfilter_link(AVFilterContext *src, unsigned srcpad,
                  AVFilterContext *dst, unsigned dstpad);

#define AVFILTER_CMD_FLAG_ONE   1 ///< 一旦过滤器理解命令（例如 target=all）就停止，自动优先使用快速过滤器
#define AVFILTER_CMD_FLAG_FAST  2 ///< 仅在快速时执行命令（如支持硬件对比度调整的视频输出）

/**
 * 使过滤器实例处理命令。推荐使用avfilter_graph_send_command()。
 */
int avfilter_process_command(AVFilterContext *filter, const char *cmd, const char *arg, char *res, int res_len, int flags);

/**
 * 迭代所有已注册的过滤器。
 *
 * @param opaque 一个指针，libavfilter 将在其中存储迭代状态。必须指向 NULL 才能开始迭代。
 *
 * @return 下一个注册的过滤器或迭代完成时为 NULL
 */
const AVFilter *av_filter_iterate(void **opaque);

/**
 * 获取与给定名称匹配的过滤器定义。
 *
 * @param name 过滤器名称，用于查找
 * @return 过滤器定义（如果注册了任何匹配的过滤器定义）。如果没有找到则为 NULL。
 */
const AVFilter *avfilter_get_by_name(const char *name);


/**
 * 使用提供的参数初始化过滤器。
 *
 * @param ctx 用于初始化的未初始化过滤器上下文
 * @param args 用于初始化过滤器的选项。这必须是“key=value”形式的以“:”分隔的选项列表。如果直接使用 AVOptions API 设置选项或者没有需要设置的选项，则可能为 NULL。
 * @return 成功时为 0，失败时为负 AVERROR
 */
int avfilter_init_str(AVFilterContext *ctx, const char *args);

/**
 * 使用提供的选项字典初始化过滤器。
 *
 * @param ctx 未初始化的过滤器上下文，用于初始化
 * @param options 一个 AVDictionary，其中填充了此过滤器的选项。返回时，此参数将被销毁并替换为包含未找​​到的选项的字典。该字典必须由调用者释放。可能为NULL，那么这个函数相当于第二个参数设置为NULL的avfilter_init_str()。
 * @return 成功时为 0，失败时为负 AVERROR
 *
 * @note 该函数和 avfilter_init_str() 本质上做同样的事情，区别在于传递选项的方式。由调用代码选择更优选的一个。当某些提供的选项未声明为过滤器支持时，这两个函数的行为也不同。在这种情况下， avfilter_init_str() 将失败，但该函数会将这些额外选项保留在选项 AVDictionary 中并照常继续。
 */
int avfilter_init_dict(AVFilterContext *ctx, AVDictionary **options);

/**
 * 释放过滤器上下文。这也将从其过滤器图表的过滤器列表中删除该过滤器。
 *
 * @param filter 用于释放的过滤器
 */
void avfilter_free(AVFilterContext *filter);

/**
 * 在现有链接的中间插入过滤器。
 *
 * @param link 应插入滤波器的链路
 * @param filt 要插入的滤波器
 * @param filt_srcpad_idx 要连接的滤波器上的输入焊盘
 * @param filt_dstpad_idx 要连接的滤波器上的输出焊盘
 * @return 成功时为零
 */
int avfilter_insert_filter(AVFilterLink *link, AVFilterContext *filt,
                           unsigned filt_srcpad_idx, unsigned filt_dstpad_idx);

/**
 * @return AVFilterContext 的 AVClass。
 *
 * @see av_opt_find()。
 */
const AVClass *avfilter_get_class(void);

/**
 * 传递给 @ref AVFilterGraph.execute 回调的函数指针，以便多次执行（可能是并行执行）。
 *
 * @param ctx 作业所属的过滤器上下文
 * @param arg 从 @ref AVFilterGraph.execute 传递的不透明参数
 * @param jobnr 正在执行的作业的索引
 * @param nb_jobs 作业总数
 *
 * @return 成功时为 0，错误时为负 AVERROR
 */
typedef int (avfilter_action_func)(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs);

/**
 * 执行多个作业（可能并行）的函数。
 *
 * @param ctx 作业所属的过滤器上下文
 * @param func 要多次调用的函数
 * @param arg 要传递给 func 的参数
 * @param ret 要填充每次调用 func 的返回值的 nb_jobs 大小的数组
 * @param nb_jobs 要执行的作业数
 *
 * @return 成功时为 0，错误时为负 AVERROR
 */
typedef int (avfilter_execute_func)(AVFilterContext *ctx, avfilter_action_func *func,
                                    void *arg, int *ret, int nb_jobs);

typedef struct AVFilterGraph {
    const AVClass *av_class;
    AVFilterContext **filters;
    unsigned nb_filters;

    char *scale_sws_opts; ///< 用于自动插入比例过滤器的 sws 选项

    /**
 * 该图中的过滤器允许的多线程类型。 AVFILTER_THREAD_* 标志的组合。
 *
 * 可由调用者随时设置，该设置将应用于此后初始化的所有过滤器。默认设置是允许一切。
 *
 * 初始化此图中的过滤器时，使用位 AND 与 AVFilterContext.thread_type 组合该字段，以获得用于确定允许的线程类型的最终掩码。 IE。需要在两者中设置线程类型才能允许。
 */
    int thread_type;

    /**
 * 该图中过滤器使用的最大线程数。可以由调用者在将任何过滤器添加到过滤器图表之前设置。零（默认值）意味着线程数是自动确定的。
 */
    int nb_threads;

    /**
 * 不透明的用户数据。可以由调用者设置为任意值，例如从 @ref AVFilterGraph.execute 等回调中使用。 Libavfilter 不会以任何方式触及这个领域。
 */
    void *opaque;

    /**
 * 调用者可以在分配图形之后和向其中添加任何过滤器之前立即设置此回调，以提供自定义多线程实现。
 *
 * 如果设置，具有切片线程功能的过滤器将调用此回调来并行执行多个作业。
 *
 * 如果此字段未设置，libavfilter 将使用其内部实现，该实现可能是也可能不是多线程，具体取决于平台和构建选项。
 */
    avfilter_execute_func *execute;

    char *aresample_swr_opts; ///< swr 选项用于自动插入的 aresample 过滤器，仅通过 AVOptions 访问

    /**
 * 设置组合过滤器图中缓冲帧的最大数量。
 *
 * 零表示没有限制。该字段必须在调用 avfilter_graph_config() 之前设置。
 */
    unsigned max_buffered_frames;
} AVFilterGraph;

/**
 * 分配过滤器图表。
 *
 * @return 成功或 NULL 时分配的过滤器图。
 */
AVFilterGraph *avfilter_graph_alloc(void);

/**
 * 在过滤器图中创建一个新的过滤器实例。
 *
 * @param graph 图，其中新过滤器将使用
 * @param filter 过滤器创建
 * @param name 实例 赋予新实例的名称（将被复制到 AVFilterContext.name）。调用者可以使用它来识别不同的过滤器，libavfilter 本身没有为此参数分配任何语义。可能为 NULL。
 *
 * @return 新创建的过滤器实例的上下文（请注意，它也可以直接通过 AVFilterGraph.filters 或使用 avfilter_graph_get_filter() 检索）成功时或失败时为 NULL。
 */
AVFilterContext *avfilter_graph_alloc_filter(AVFilterGraph *graph,
                                             const AVFilter *filter,
                                             const char *name);

/**
 * 从图中获取由实例名称标识的过滤器实例。
 *
 * @param graph 要搜索的过滤图。
 * @param name 过滤器实例名称（在图中应该是唯一的）。
 * @return 指向找到的过滤器实例的指针，如果找不到，则为 NULL。
 */
AVFilterContext *avfilter_graph_get_filter(AVFilterGraph *graph, const char *name);

/**
 * 一个方便的包装器，只需一步即可分配和初始化过滤器。过滤器实例是从过滤器 filt 创建的，并使用参数 args 初始化。目前忽略不透明。
 *
 * 如果成功，请将 *filt_ctx 放入指向创建的过滤器实例的指针，否则将 *filt_ctx 设置为 NULL。
 *
 * @param name 赋予创建的过滤器实例的实例名称
 * @param graph_ctx 过滤器图
 * @return 如果失败则为负 AVERROR 错误代码，否则为非负值
 *
 * @warning 由于在此函数成功返回后过滤器被初始化，因此您可以不得在其上设置任何其他选项。如果您需要这样做，请调用 ::avfilter_graph_alloc_filter()，然后设置选项，然后调用 ::avfilter_init_dict() 而不是此函数。
 */
int avfilter_graph_create_filter(AVFilterContext **filt_ctx, const AVFilter *filt,
                                 const char *name, const char *args, void *opaque,
                                 AVFilterGraph *graph_ctx);

/**
 * 启用或禁用图形内的自动格式转换。
 *
 * 请注意，格式转换仍然可以在显式插入的比例和样本过滤器内发生。
 *
 * @param flags 任何 AVFILTER_AUTO_CONVERT_* 常量
 */
void avfilter_graph_set_auto_convert(AVFilterGraph *graph, unsigned flags);

enum {
    AVFILTER_AUTO_CONVERT_ALL  =  0, /**< 启用所有自动转换 */
    AVFILTER_AUTO_CONVERT_NONE = -1, /**< 禁用所有自动转换 */
};

/**
 * 检查有效性并配置图中的所有链接和格式。
 *
 * @param graphctx 用于记录的过滤器图
 * @param log_ctx 上下文
 * @return >= 0 如果成功，则为负 AVERROR 代码，否则
 */
int avfilter_graph_config(AVFilterGraph *graphctx, void *log_ctx);

/**
 * 释放图，销毁其链接，并将 *graph 设置为 NULL。如果 *graph 为 NULL，则不执行任何操作。
 */
void avfilter_graph_free(AVFilterGraph **graph);

/**
 * 过滤器链的输入/输出的链表。
 *
 * 这主要对 avfilter_graph_parse() / avfilter_graph_parse2() 有用，它用于与调用者之间传递开放（未链接）的输入和输出。该结构指定图中包含的每个未连接的焊盘、过滤器上下文和建立链接所需的焊盘索引。
 */
typedef struct AVFilterInOut {
    /** 列表中此输入/输出的唯一名称 */
    char *name;

    /** 与此输入/输出关联的过滤器上下文 */
    AVFilterContext *filter_ctx;

    /** 用于链接 */
    int pad_idx;

    /** 列表中下一个输入/输入的 filt_ctx 垫的索引，如果为 NULL，则为 NULL最后一个 */
    struct AVFilterInOut *next;
} AVFilterInOut;

/**
 * 分配单个 AVFilterInOut 条目。必须用 avfilter_inout_free() 释放。
 * @return 成功时分配 AVFilterInOut，失败时分配 NULL。
 */
AVFilterInOut *avfilter_inout_alloc(void);

/**
 * 释放提供的 AVFilterInOut 列表并将 *inout 设置为 NULL。如果 *inout 为 NULL，则不执行任何操作。
 */
void avfilter_inout_free(AVFilterInOut **inout);

/**
 * 将字符串描述的图形添加到图形中。
 *
 * @note 调用者必须提供输入和输出列表，因此在调用函数之前必须知道这些列表。
 *
 * @note 输入参数描述了图形中已存在部分的输入；即从新创建的部分的角度来看，它们是输出。类似地，outputs 参数描述了现有过滤器的输出，这些输出作为已解析过滤器的输入提供。
 *
 * @param graph 过滤器图，将解析的图上下文链接到其中
 * @param filters 要解析的字符串
 * @param inputs 链表到图的输入
 * @param outputs 链表到图的输出
 * @return 成功时为零，错误时为负 AVERROR 代码
 */
int avfilter_graph_parse(AVFilterGraph *graph, const char *filters,
                         AVFilterInOut *inputs, AVFilterInOut *outputs,
                         void *log_ctx);

/**
 * 将由字符串描述的图形添加到图形中。
 *
 * 在图形过滤器描述中，如果未指定第一个过滤器的输入标签，则假定为“in”；如果未指定最后一个过滤器的输出标签，则假定为“out”。
 *
 * @param graph 将解析的图形上下文链接到的过滤器图形
 * @param filters 要解析的字符串
 * @param inputs 指向图形输入的链接列表的指针，可以为 NULL。如果非 NULL，则 *inputs 会更新为包含解析后打开的输入列表，应使用 avfilter_inout_free() 释放。
 * @param outputs 指向图输出的链表的指针可以为 NULL。如果非 NULL，*outputs 会更新为包含解析后打开的输出列表，应使用 avfilter_inout_free() 释放。
 * @return 成功时为非负，错误时为负 AVERROR 代码
 */
int avfilter_graph_parse_ptr(AVFilterGraph *graph, const char *filters,
                             AVFilterInOut **inputs, AVFilterInOut **outputs,
                             void *log_ctx);

/**
 * 将由字符串描述的图形添加到图形中。
 *
 * @param[in]  graph 链接解析图上下文的过滤器图
 * @param[in]  filters 要解析的字符串
 * @param[out] inputs 解析图的所有空闲（未链接）输入的链接列表将在此处返回。它由调用者使用 avfilter_inout_free() 释放。
 * @param[out] outputs 解析图的所有空闲（未链接）输出的链接列表将在此处返回。它由调用者使用 avfilter_inout_free() 释放。
 * @return 成功时为零，错误时为负 AVERROR 代码
 *
 * @note 该函数返回解析图形后未链接的输入和输出，然后调用者处理它们。
 * @note 该函数不会引用图形的现有部分，并且输入参数将在返回时包含图形的新解析部分的输入。  类似地，输出参数将包含新创建的过滤器的输出。
 */
int avfilter_graph_parse2(AVFilterGraph *graph, const char *filters,
                          AVFilterInOut **inputs,
                          AVFilterInOut **outputs);

/**
 * 滤波器输入或输出垫的参数。
 *
 * 由 avfilter_graph_segment_parse() 创建为 AVFilterParams 的子级。在 avfilter_graph_segment_free() 中释放。
 */
typedef struct AVFilterPadParams {
    /**
 * 包含 pad 标签的 av_malloc() 字符串。
 *
 * 可能被调用者 av_free() 并设置为 NULL，在这种情况下，该 pad 将被视为未标记链接。也可以被另一个 av_malloc()'ed 字符串替换。
 */
    char *label;
} AVFilterPadParams;

/**
 * 描述要在过滤器图中创建的过滤器的参数。
 *
 * 由 avfilter_graph_segment_parse() 创建为 AVFilterGraphSegment 的子级。在 avfilter_graph_segment_free() 中释放。
 */
typedef struct AVFilterParams {
    /**
 * 过滤器上下文。
 *
 * 由avfilter_graph_segment_create_filters()根据AVFilterParams.filter_name和instance_name创建。
 *
 * 调用者也可以手动创建过滤器上下文，然后他们应该 av_free() filter_name 并将其设置为 NULL。然后，avfilter_graph_segment_create_filters() 会跳过此类 AVFilterParams 实例。
 */
    AVFilterContext     *filter;

    /**
 * 要使用的 AVFilter 的名称。
 *
 * av_malloc() 的字符串，由 avfilter_graph_segment_parse() 设置。将由 avfilter_graph_segment_create_filters() 传递给 avfilter_get_by_name()。
 *
 * 调用者可以 av_free() 该字符串并将其替换为另一个字符串或 NULL。如果调用者手动创建过滤器实例，则该字符串必须设置为 NULL。
 *
 * 当 AVFilterParams.filter 和 AVFilterParams.filter_name 均为 NULL 时，avfilter_graph_segment_*() 函数会跳过此 AVFilterParams 实例。
 */
    char                *filter_name;
    /**
 * 用于此过滤器实例的名称。
 *
 * av_malloc() 的字符串，可以通过 avfilter_graph_segment_parse() 设置或保留 NULL。调用者可以 av_free() 该字符串并替换为另一个字符串或 NULL。
 *
 * 将由 avfilter_graph_segment_create_filters() 使用 - 作为第三个参数传递给 avfilter_graph_alloc_filter()，然后释放并设置为 NULL。
 */
    char                *instance_name;

    /**
 * 应用于过滤器的选项。
 *
 * 由 avfilter_graph_segment_parse() 填充。之后可以由调用者自由修改。
 *
 * 将通过 avfilter_graph_segment_apply_opts() 应用于过滤器，相当于 av_opt_set_dict2(filter, &opts, AV_OPT_SEARCH_CHILDREN)，即任何未应用的选项将保留在此字典中。
 */
    AVDictionary        *opts;

    AVFilterPadParams  **inputs;
    unsigned          nb_inputs;

    AVFilterPadParams  **outputs;
    unsigned          nb_outputs;
} AVFilterParams;

/**
 * 过滤器链是过滤器规范的列表。
 *
 * 由 avfilter_graph_segment_parse() 创建为 AVFilterGraphSegment 的子级。在 avfilter_graph_segment_free() 中释放。
 */
typedef struct AVFilterChain {
    AVFilterParams  **filters;
    size_t         nb_filters;
} AVFilterChain;

/**
 * 过滤器图段的解析表示。
 *
 * 从概念上讲，filtergraph 段是一个过滤器链列表，带有一些补充信息（例如格式转换标志）。
 *
 * 由 avfilter_graph_segment_parse() 创建。必须用 avfilter_graph_segment_free() 释放。
 */
typedef struct AVFilterGraphSegment {
    /**
 * 与此段关联的过滤器图表。由avfilter_graph_segment_parse()设置。
 */
    AVFilterGraph *graph;

    /**
 * 该段中包含的过滤器链的列表。在avfilter_graph_segment_parse()中设置。
 */
    AVFilterChain **chains;
    size_t       nb_chains;

    /**
 * 一个字符串，其中包含应用于此段中所有比例过滤器的以冒号分隔的 key=value 选项列表。
 *
 * 可以通过 avfilter_graph_segment_parse() 设置。调用者可以使用 av_free() 释放该字符串，并将其替换为不同的 av_malloc() 字符串。
 */
    char *scale_sws_opts;
} AVFilterGraphSegment;

/**
 * 将文本过滤器图描述解析为中间形式。
 *
 * 此中间表示旨在由调用者按照 AVFilterGraphSegment 及其子项的文档中所述进行修改，然后手动或使用其他 avfilter_graph_segment_*() 函数应用于图形。有关应用 AVFilterGraphSegment 的规范方法，请参阅 avfilter_graph_segment_apply() 的文档。
 *
 * @param graph 与解析的段关联的过滤器图。仅用于日志记录和类似的辅助目的。该函数实际上不会修改图表 - 解析结果会存储在 seg 中以供进一步处理。
 * @param graph_str 描述filtergraph段的字符串
 * @param flags 保留供将来使用，调用者现在必须设置为0
 * @param seg 如果成功，则在此写入指向新创建的AVFilterGraphSegment的指针。图段由调用者拥有，并且必须在图本身被释放之前使用 avfilter_graph_segment_free() 释放。
 *
 * @retval "non-negative 编号“成功
 * @retval "negative 错误代码”失败
 */
int avfilter_graph_segment_parse(AVFilterGraph *graph, const char *graph_str,
                                 int flags, AVFilterGraphSegment **seg);

/**
 * 创建在图形段中指定的过滤器。
 *
 * 遍历段中待创建的 AVFilterParams 并为它们创建新的过滤器实例。创建待处理参数是 AVFilterParams.filter_name 为非 NULL 的参数（因此 AVFilterParams.filter 为 NULL）。所有其他 AVFilterParams 实例都将被忽略。
 *
 * 对于此函数创建的任何过滤器，相应的 AVFilterParams.filter 将设置为新创建的过滤器上下文，AVFilterParams.filter_name 和 AVFilterParams.instance_name 将被释放并设置为 NULL。
 *
 * @param seg 要处理的过滤器图段
 * @param flags 保留供将来使用，调用者现在必须设置为 0
 *
 * @retval "non-negative 数字” 成功，所有待创建的过滤器已成功创建
 * @retval AVERROR_FILTER_NOT_FOUND 某些过滤器的名称与已知过滤器不对应
 * @retval "another 负错误代码” 其他故障
 *
 * @note 多次调用此函数是安全的，因为它是幂等的。
 */
int avfilter_graph_segment_create_filters(AVFilterGraphSegment *seg, int flags);

/**
 * 应用解析的选项来过滤图形段中的实例。
 *
 * 遍历图形段中具有与其关联的选项字典的所有过滤器实例，并使用 av_opt_set_dict2(..., AV_OPT_SEARCH_CHILDREN) 应用这些选项。 AVFilterParams.opts 被 av_opt_set_dict2() 输出的字典替换，如果成功应用所有选项，该字典应该为空 (NULL)。
 *
 * 如果找不到任何选项，该函数将继续处理所有其他过滤器，并最终返回 AVERROR_OPTION_NOT_FOUND（除非发生另一个错误）。然后，调用程序可以根据需要处理未应用的选项。
 *
 * 段中存在的任何创建待处理过滤器（请参阅 avfilter_graph_segment_create_filters()）将导致此函数失败。没有关联过滤器上下文的 AVFilterParams 会被简单地跳过。
 *
 * @param seg 用于处理的过滤器图段
 * @param flags 保留供将来使用，调用者现在必须设置为 0
 *
 * @retval "non-negative 数字” 成功，所有选项均已成功应用。
 * @retval AVERROR_OPTION_NOT_FOUND 在过滤器中未找到某些选项
 * @retval "another 负错误代码” 其他故障
 *
 * @note 多次调用此函数是安全的，因为它是幂等的。
 */
int avfilter_graph_segment_apply_opts(AVFilterGraphSegment *seg, int flags);

/**
 * 初始化图段中的所有过滤器实例。
 *
 * 遍历图形段中的所有过滤器实例，并对尚未初始化的过滤器实例调用 avfilter_init_dict(..., NULL) 。
 *
 * 段中存在的任何创建待处理过滤器（请参阅 avfilter_graph_segment_create_filters()）将导致此函数失败。没有关联过滤器上下文或其过滤器上下文已初始化的 AVFilterParams 将被简单地跳过。
 *
 * @param seg 要处理的 FilterGraph 段
 * @param flags 保留供将来使用，调用者现在必须设置为 0
 *
 * @retval "non-negative 数字“成功，所有过滤器实例均已成功初始化
 * @retval "negative 错误代码”失败
 *
 * @note 多次调用此函数是安全的，因为它是幂等的。
 */
int avfilter_graph_segment_init(AVFilterGraphSegment *seg, int flags);

/**
 * 图形段中的链接过滤器。
 *
 * 遍历图形段中的所有过滤器实例，并尝试链接所有未链接的输入和输出板。段中存在的任何待创建的过滤器（请参阅 avfilter_graph_segment_create_filters()）将导致此函数失败。禁用的过滤器和已链接的焊盘将被跳过。
 *
 * 每个具有相应 AVFilterPadParams（带有非 NULL 标签）的过滤器输出板 - 链接到具有匹配标签的输入（如果存在）； - 否则导出到输出链接列表中，并保留标签。未标记的输出 - 链接到链中下一个非禁用过滤器中的第一个未链接的未标记输入（如果存在） - 在输出链表中导出，否则使用 NULL 标签
 *
 * 类似地，未链接的输入板在输入链表中导出。
 *
 * @param seg 用于处理的过滤器图段
 * @param flags 保留供将来使用，调用者现在必须设置为 0
 * @param[out] inputs 该图段中过滤器的所有空闲（未链接）输入的链接列表将在此处返回。它由调用者使用 avfilter_inout_free() 释放。
 * @param[out] outputs 此处将返回此图段中过滤器的所有空闲（未链接）输出的链接列表。它由调用者使用 avfilter_inout_free() 释放。
 *
 * @retval "non-negative 编号“成功
 * @retval "negative 错误代码”失败
 *
 * @note 多次调用此函数是安全的，因为它是幂等的。
 */
int avfilter_graph_segment_link(AVFilterGraphSegment *seg, int flags,
                                AVFilterInOut **inputs,
                                AVFilterInOut **outputs);

/**
 * 将图形段中的所有过滤器/链接描述应用到关联的过滤器图形。
 *
 * 该函数目前相当于按顺序调用以下函数： - avfilter_graph_segment_create_filters(); - avfilter_graph_segment_apply_opts(); - avfilter_graph_segment_init(); - avfilter_graph_segment_link();如果其中任何一个失败，则失败。该列表将来可能会扩展。
 *
 * 由于上述函数是幂等的，调用者可能会手动调用其中一些函数，然后对filtergraph进行一些自定义处理，然后调用此函数来完成其余的操作。
 *
 * @param seg 要处理的 filtergraph 段
 * @param flags 保留供将来使用，调用者现在必须设置为 0
 * @param[out] inputs 传递给 avfilter_graph_segment_link()
 * @param[out] outputs 传递给 avfilter_graph_segment_link()
 *
 * @retval "non-negative 数字“成功
 * @retval "negative 错误代码”失败
 *
 * @note 多次调用此函数是安全的，因为它是幂等的。
 */
int avfilter_graph_segment_apply(AVFilterGraphSegment *seg, int flags,
                                 AVFilterInOut **inputs,
                                 AVFilterInOut **outputs);

/**
 * 释放提供的 AVFilterGraphSegment 以及与之相关的所有内容。
 *
 * @param seg 指向要释放的 AVFilterGraphSegment 的双指针。退出该函数时，NULL 将被写入该指针。
 *
 * @note 过滤器上下文 (AVFilterParams.filter) 由 AVFilterGraph 而不是 AVFilterGraphSegment 所有，因此它们不会被释放。
 */
void avfilter_graph_segment_free(AVFilterGraphSegment **seg);

/**
 * 向一个或多个过滤器实例发送命令。
 *
 * @param graph 过滤器图
 * @param target 命令应发送到的过滤器 “all”发送到所有过滤器，否则它可以是过滤器或过滤器实例名称，它将命令发送到所有匹配的过滤器。
 * @param cmd 要发送的命令，为了处理简单，所有命令都只能是字母数字
 * @param arg 命令的参数
 * @param res 大小为 res_size 的缓冲区，过滤器可以在其中返回响应。
 *
 * @returns >=0 成功，否则错误代码。对不支持的命令执行 AVERROR(ENOSYS)
 */
int avfilter_graph_send_command(AVFilterGraph *graph, const char *target, const char *cmd, const char *arg, char *res, int res_len, int flags);

/**
 * 将一个或多个过滤器实例的命令排队。
 *
 * @param graph 过滤器图
 * @param target 命令应发送到的过滤器 “all”发送到所有过滤器，否则它可以是过滤器或过滤器实例名称，它将命令发送到所有匹配的过滤器。
 * @param cmd 要发送的命令，为了处理简单，所有命令只能是字母数字
 * @param arg 命令的参数
 * @param ts 命令应发送到过滤器的时间
 *
 * @note 因为此函数返回后执行命令，所以不返回提供了来自过滤器的代码，也不支持 AVFILTER_CMD_FLAG_ONE。
 */
int avfilter_graph_queue_command(AVFilterGraph *graph, const char *target, const char *cmd, const char *arg, int flags, double ts);


/**
 * 将图形转储为人类可读的字符串表示形式。
 *
 * @param graph 用于转储
 * @param options 格式选项的图表；当前忽略
 * @return 字符串，或者在内存分配失败的情况下忽略 NULL；必须使用 av_free 释放字符串
 */
char *avfilter_graph_dump(AVFilterGraph *graph, const char *options);

/**
 * 在最旧的接收器链路上请求帧。
 *
 * 如果请求返回 AVERROR_EOF，请尝试下一步。
 *
 * 请注意，此函数并不是filtergraph的唯一调度机制，只是一个在正常情况下帮助以平衡方式耗尽filtergraph的便利函数。
 *
 * 另请注意，AVERROR_EOF 并不意味着在此过程中帧未到达某些接收器。当存在多个接收器链接时，如果请求的链接返回 EOF，则这可能会导致过滤器刷新发送到另一个接收器链接的待处理帧（尽管未经请求）。
 *
 * @return ff_request_frame() 的返回值，如果所有链接都返回 AVERROR_EOF，则为 AVERROR_EOF
 */
int avfilter_graph_request_oldest(AVFilterGraph *graph);

/**
 * @}
 */

#endif /* AVFILTER_AVFILTER_H */
