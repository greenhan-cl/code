/*
 * Copyright (C) 2024 Niklas Haas
 * Copyright (C) 2001-2011 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef SWSCALE_SWSCALE_H
#define SWSCALE_SWSCALE_H

/**
 * @file
 * @ingroup libsws
 * 外部 API 头文件
 */

#include <stdint.h>

#include "libavutil/avutil.h"
#include "libavutil/frame.h"
#include "libavutil/log.h"
#include "libavutil/pixfmt.h"
#include "version_major.h"
#ifndef HAVE_AV_CONFIG_H
/* 作为 ffmpeg 构建的一部分被包含时，仅包含主版本号，
 * 以避免不必要的重新构建。从外部包含时，仍包含完整的版本信息。 */
#include "version.h"
#endif

/**
 * @defgroup libsws libswscale
 * 颜色转换和缩放库。
 *
 * @{
 *
 * 返回 LIBSWSCALE_VERSION_INT 常量。
 */
unsigned swscale_version(void);

/**
 * 返回 libswscale 的构建时配置。
 */
const char *swscale_configuration(void);

/**
 * 返回 libswscale 的许可证文本。
 */
const char *swscale_license(void);

/**
 * 获取 SwsContext 的 AVClass。它可以与 AV_OPT_SEARCH_FAKE_OBJ 结合使用，
 * 以检查选项。
 *
 * @see av_opt_find().
 */
const AVClass *sws_get_class(void);

/******************************
 * 标志和质量设置           *
 ******************************/

typedef enum SwsDither {
    SWS_DITHER_NONE = 0, /* 禁用抖动 */
    SWS_DITHER_AUTO,     /* 从预设中自动选择 */
    SWS_DITHER_BAYER,    /* 有序抖动矩阵 */
    SWS_DITHER_ED,       /* 误差扩散 */
    SWS_DITHER_A_DITHER, /* 算术加法 */
    SWS_DITHER_X_DITHER, /* 算术异或 */
    SWS_DITHER_NB,       /* 不属于 ABI */
    SWS_DITHER_MAX_ENUM = 0x7FFFFFFF, /* 强制大小为 32 位，不是有效的抖动类型 */
} SwsDither;

typedef enum SwsAlphaBlend {
    SWS_ALPHA_BLEND_NONE = 0,
    SWS_ALPHA_BLEND_UNIFORM,
    SWS_ALPHA_BLEND_CHECKERBOARD,
    SWS_ALPHA_BLEND_NB,  /* 不属于 ABI */
    SWS_ALPHA_BLEND_MAX_ENUM = 0x7FFFFFFF, /* 强制大小为 32 位，不是有效的混合模式 */
} SwsAlphaBlend;

typedef enum SwsScaler {
    SWS_SCALE_AUTO = 0,
    SWS_SCALE_BILINEAR, ///< 双线性滤波
    SWS_SCALE_BICUBIC,  ///< 2 抽头三次 BC 样条
    SWS_SCALE_POINT,    ///< 最近邻（点采样）
    SWS_SCALE_AREA,     ///< 区域平均
    SWS_SCALE_GAUSSIAN, ///< 2 抽头高斯近似
    SWS_SCALE_SINC,     ///< 无窗 sinc
    SWS_SCALE_LANCZOS,  ///< 3 抽头 sinc/sinc
    SWS_SCALE_SPLINE,   ///< 无窗自然三次样条
    SWS_SCALE_NB,       ///< 不属于 ABI
    SWS_SCALE_MAX_ENUM = 0x7FFFFFFF, ///< 强制大小为 32 位，不是有效的滤波器类型
} SwsScaler;

typedef enum SwsBackend {
    /* 稳定后端 */
    SWS_BACKEND_LEGACY      = (1 << 0), ///< 旧版、针对特定格式定制的代码
    SWS_BACKEND_STABLE      = SWS_BACKEND_LEGACY,

    /* 不稳定后端（仅在启用 SWS_UNSTABLE 时自动选择） */
    SWS_BACKEND_C           = (1 << 1), ///< 基于模板的 C 参考实现
    SWS_BACKEND_MEMCPY      = (1 << 2), ///< 使用 libc memcpy() / memset() 的快速路径
    SWS_BACKEND_X86         = (1 << 3), ///< 串联的 x86 SIMD 内核
    SWS_BACKEND_AARCH64     = (1 << 4), ///< 串联的 AArch64 NEON 内核
    SWS_BACKEND_SPIRV       = (1 << 5), ///< Vulkan SPIR-V 后端
    SWS_BACKEND_UNSTABLE    = SWS_BACKEND_C |
                              SWS_BACKEND_MEMCPY |
                              SWS_BACKEND_X86 |
                              SWS_BACKEND_AARCH64 |
                              SWS_BACKEND_SPIRV,

    SWS_BACKEND_ALL = SWS_BACKEND_STABLE | SWS_BACKEND_UNSTABLE,
    SWS_BACKEND_MAX_ENUM = 0x7FFFFFFF, ///< 强制大小为 32 位，不是有效的后端
} SwsBackend;

typedef enum SwsFlags {
    /**
     * 对参数指定不完整的转换返回错误。不设置此标志时，未指定字段会使用合理的默认值。
     */
    SWS_STRICT        = 1 << 11,

    /**
     * 输出缩放参数的详细日志。
     */
    SWS_PRINT_INFO    = 1 << 12,

    /**
     * 放大为 RGB 时执行完整色度上采样。
     *
     * 例如，将 50x50 yuv420p 转换为 100x100 rgba 时，设置此标志会把色度平面
     * 从 25x25 缩放为 100x100（4:4:4），然后在最终输出步骤中将 100x100
     * yuv444p 图像转换为 rgba。
     *
     * 不设置此标志时，色度平面会改为缩放到 50x100（4:2:2），水平方向相邻的
     * 两个 RGBA 输出像素会复用同一个色度样本。
     */
    SWS_FULL_CHR_H_INT = 1 << 13,

    /**
     * 缩小 RGB 源时执行完整色度插值。
     *
     * 例如，将 100x100 rgba 源转换为 50x50 yuv444p 时，设置此标志会生成
     * 100x100（4:4:4）色度平面，然后将其缩小到所需的 50x50。
     *
     * 不设置此标志时，色度平面会以 50x100 生成（每隔一个像素丢弃一个），
     * 然后再缩小到所需的 50x50 分辨率。
     */
    SWS_FULL_CHR_H_INP = 1 << 14,

    /**
     * 强制位精确输出。这会禁止使用可能导致细微舍入差异的平台特定优化，
     * 以始终保持输出与参考 C 代码逐位完全兼容。
     *
     * 注意：建议同时设置这两个标志。
     */
    SWS_ACCURATE_RND   = 1 << 18,
    SWS_BITEXACT       = 1 << 19,

    /**
     * 允许/优先使用实验性的新代码路径。它可能更快、更慢，或者产生不同的输出，
     * 其语义也可能随时变化。仅用于测试和调试。
     */
    SWS_UNSTABLE = 1 << 20,

    /**
     * 已弃用的标志。
     */
    SWS_DIRECT_BGR      = 1 << 15, ///< 此标志不起作用
    SWS_ERROR_DIFFUSION = 1 << 23, ///< 请改为设置 `SwsContext.dither`

    /**
     * 缩放器选择选项。同一时间只能启用一个。
     * 已弃用，请改用 `SwsContext.scaler`。
     */
    SWS_FAST_BILINEAR = 1 <<  0, ///< 快速双线性滤波
    SWS_BILINEAR      = 1 <<  1, ///< 双线性滤波
    SWS_BICUBIC       = 1 <<  2, ///< 2 抽头三次 B 样条
    SWS_X             = 1 <<  3, ///< 实验性
    SWS_POINT         = 1 <<  4, ///< 最近邻
    SWS_AREA          = 1 <<  5, ///< 区域平均
    SWS_BICUBLIN      = 1 <<  6, ///< 亮度使用双三次，色度使用双线性
    SWS_GAUSS         = 1 <<  7, ///< 高斯近似
    SWS_SINC          = 1 <<  8, ///< 无窗 sinc
    SWS_LANCZOS       = 1 <<  9, ///< 3 抽头 sinc/sinc
    SWS_SPLINE        = 1 << 10, ///< 无窗自然三次样条
} SwsFlags;

typedef enum SwsIntent {
    SWS_INTENT_PERCEPTUAL = 0,            ///< 感知色调映射
    SWS_INTENT_RELATIVE_COLORIMETRIC = 1, ///< 相对比色裁剪
    SWS_INTENT_SATURATION = 2,            ///< 饱和度映射
    SWS_INTENT_ABSOLUTE_COLORIMETRIC = 3, ///< 绝对比色裁剪
    SWS_INTENT_NB, ///< 不属于 ABI
} SwsIntent;

/***********************************
 * 上下文创建和管理           *
 ***********************************/

/**
 * 主要的外部 API 结构。次版本升级时可以在末尾添加新字段。删除、重新排序或
 * 更改现有字段需要升级主版本。sizeof(SwsContext) 不属于 ABI。
 */
typedef struct SwsContext {
    const AVClass *av_class;

    /**
     * 用户的私有数据，可用于携带应用程序特定内容。
     */
    void *opaque;

    /**
     * SWS_* 的位掩码。详情参见 `SwsFlags`。
     */
    unsigned flags;

    /**
     * 用于微调某些缩放器的额外参数。
     */
#define SWS_NUM_SCALER_PARAMS 2
    double scaler_params[SWS_NUM_SCALER_PARAMS];

    /**
     * 用于处理的线程数；设为 0 表示自动选择。
     */
    int threads;

    /**
     * 抖动模式。
     */
    SwsDither dither;

    /**
     * Alpha 混合模式。详情参见 `SwsAlphaBlend`。
     */
    SwsAlphaBlend alpha_blend;

    /**
     * 使用伽马校正缩放。
     */
    int gamma_flag;

    /**
     * 已弃用的帧属性覆盖，仅用于旧版 API。
     *
     * 在动态模式下使用 sws_scale_frame() 时会忽略这些字段；此时所有属性都
     * 直接从帧中获取。
     */
    int src_w, src_h;  ///< 源帧的宽度和高度
    int dst_w, dst_h;  ///< 目标帧的宽度和高度
    int src_format;    ///< 源像素格式
    int dst_format;    ///< 目标像素格式
    int src_range;     ///< 源使用全范围
    int dst_range;     ///< 目标使用全范围
    int src_v_chr_pos; ///< 源色度在亮度网格中的垂直位置 / 256
    int src_h_chr_pos; ///< 源色度的水平位置
    int dst_v_chr_pos; ///< 目标色度的垂直位置
    int dst_h_chr_pos; ///< 目标色度的水平位置

    /**
     * 颜色空间转换所需的 ICC 渲染意图。
     */
    int intent;

    /**
     * 缩放滤波器。如果设置为 SWS_SCALE_AUTO 以外的值，它将覆盖
     * `SwsContext.flags` 隐含的滤波器。
     *
     * 注意：不影响旧版（有状态）API。
     */
    SwsScaler scaler;

    /**
     * 专门用于对经过子采样的（色度）平面进行上/下采样的缩放器。如果设置为
     * SWS_SCALE_AUTO 以外的值，它将覆盖 `SwsContext.scaler` 隐含的滤波器。
     * 否则，主缩放和色度子采样将使用同一个滤波器。
     */
    SwsScaler scaler_sub;

    /**
     * SWS_BACKEND_* 的位掩码。非零时，将可用后端限制为指定集合。保持为零时，
     * 会根据 SWS_UNSTABLE 自动选择默认后端集合。
     *
     * 注意：这只与新 API（sws_scale_frame()）有关。有状态的旧版 API 始终隐含
     * SWS_BACKEND_LEGACY。
     */
    SwsBackend backends;

    /* 记得将新字段添加到 graph.c:opts_equal() */
} SwsContext;

/**
 * 分配一个空的 SwsContext，并将其字段设为默认值。
 */
SwsContext *sws_alloc_context(void);

/**
 * 释放上下文及其关联的所有内容，并向提供的指针写入 NULL。
 */
void sws_free_context(SwsContext **ctx);

/***************************
 * 支持的帧格式           *
 ***************************/

/**
 * 测试是否有任何后端支持给定的（软件）像素格式，不包括不稳定后端。
 *
 * @param output  为 0 时，测试是否与源/输入帧兼容；否则测试是否与目标/输出帧兼容。
 * @param format  要检查的格式。
 *
 * @return 支持时返回正整数，否则返回 0。
 */
int sws_test_format(enum AVPixelFormat format, int output);

/**
 * 测试是否有任何后端支持给定的硬件像素格式，不包括不稳定后端。
 *
 * @param format  要检查的硬件格式，或 AV_PIX_FMT_NONE。
 *
 * @return 支持给定格式或 AV_PIX_FMT_NONE 时返回正整数，否则返回 0。
 */
int sws_test_hw_format(enum AVPixelFormat format);

/**
 * 测试是否支持给定的颜色空间。
 *
 * @param output  为 0 时，测试是否与源/输入帧兼容；否则测试是否与目标/输出帧兼容。
 * @param colorspace 要检查的颜色空间。
 *
 * @return 支持时返回正整数，否则返回 0。
 */
int sws_test_colorspace(enum AVColorSpace colorspace, int output);

/**
 * 测试是否支持给定的一组色度原色。
 *
 * @param output  为 0 时，测试是否与源/输入帧兼容；否则测试是否与目标/输出帧兼容。
 * @param primaries 要检查的色度原色。
 *
 * @return 支持时返回正整数，否则返回 0。
 */
int sws_test_primaries(enum AVColorPrimaries primaries, int output);

/**
 * 测试是否支持给定的颜色传递函数。
 *
 * @param output  为 0 时，测试是否与源/输入帧兼容；否则测试是否与目标/输出帧兼容。
 * @param trc     要检查的颜色传递函数。
 *
 * @return 支持时返回正整数，否则返回 0。
 */
int sws_test_transfer(enum AVColorTransferCharacteristic trc, int output);

/**
 * 对帧运行所有 sws_test_*，并检查基本帧属性是否合理的辅助函数。它会忽略无关
 * 属性——例如，不会检查 RGB 帧的 AVColorSpace。
 */
int sws_test_frame(const AVFrame *frame, int output);

/**
 * 类似 `sws_scale_frame`，但不实际执行缩放。它只初始化执行该操作时所需的内部
 * 状态，并针对不支持的帧组合返回正确的错误码。
 *
 * @param ctx   缩放上下文。
 * @param dst   要考察的目标帧。
 * @param src   要考察的源帧。
 * @return 成功时返回 0，失败时返回负的 AVERROR 错误码。
 */
int sws_frame_setup(SwsContext *ctx, const AVFrame *dst, const AVFrame *src);

/********************
 * 主要缩放 API     *
 ********************/

/**
 * 检查给定转换是否无需执行任何操作。如果不需要执行操作则返回正整数，
 * 否则返回 0。
 */
int sws_is_noop(const AVFrame *dst, const AVFrame *src);

/**
 * 缩放 `src` 中的源数据，并将输出写入 `dst`。
 *
 * 此函数可以直接用于已分配的上下文，无需设置任何帧属性或调用
 * `sws_init_context()`。这种用法完全是动态的，帧属性变化时无需重新分配。
 *
 * 也可以在显式初始化过的上下文上调用此函数，但这仅用于向后兼容。在这种使用
 * 模式下，必须在初始化时正确设置所有帧属性，且初始化后不能再改变。
 *
 * @param ctx   缩放上下文。
 * @param dst   目标帧。数据缓冲区可以由调用者预先分配，也可以保持为空，此时
 *              缩放器会负责分配。后者可能具有性能优势——例如在某些情况下，
 *              部分（或全部）输出平面可引用输入平面，而不是进行复制。
 * @param src   源帧。如果数据缓冲区设为 NULL，则此函数的行为与
 *              `sws_frame_setup` 完全相同。
 * @return 成功时 >= 0，失败时返回负的 AVERROR 错误码。
 */
int sws_scale_frame(SwsContext *c, AVFrame *dst, const AVFrame *src);

/**
 * 滤波器内核截止值。绝对值小于此值的部分会从主滤波器内核中截除。请注意，
 * 窗函数始终会适配新的滤波器半径，因此（对于适当加窗的滤波器）不会发生
 * 频谱泄漏。
 */
#define SWS_MAX_REDUCE_CUTOFF 0.002

/*************************
 * 旧版（有状态）API    *
 *************************/

#define SWS_SRC_V_CHR_DROP_MASK     0x30000
#define SWS_SRC_V_CHR_DROP_SHIFT    16

#define SWS_PARAM_DEFAULT           123456

#define SWS_CS_ITU709         1
#define SWS_CS_FCC            4
#define SWS_CS_ITU601         5
#define SWS_CS_ITU624         5
#define SWS_CS_SMPTE170M      5
#define SWS_CS_SMPTE240M      7
#define SWS_CS_DEFAULT        5
#define SWS_CS_BT2020         9

/**
 * 返回适用于 sws_setColorspaceDetails() 的给定颜色空间 yuv<->rgb 系数指针。
 *
 * @param colorspace SWS_CS_* 宏之一。无效时使用 SWS_CS_DEFAULT。
 */
const int *sws_getCoefficients(int colorspace);

// 用作滤波器时，它们必须包含奇数个元素
// coeffs 不能在向量之间共享
typedef struct SwsVector {
    double *coeff;              ///< 指向系数列表的指针
    int length;                 ///< 向量中的系数数量
} SwsVector;

// 向量可以共享
typedef struct SwsFilter {
    SwsVector *lumH;
    SwsVector *lumV;
    SwsVector *chrH;
    SwsVector *chrV;
} SwsFilter;

/**
 * 如果 pix_fmt 是支持的输入格式，则返回正值，否则返回 0。
 */
int sws_isSupportedInput(enum AVPixelFormat pix_fmt);

/**
 * 如果 pix_fmt 是支持的输出格式，则返回正值，否则返回 0。
 */
int sws_isSupportedOutput(enum AVPixelFormat pix_fmt);

/**
 * @param[in]  pix_fmt 像素格式
 * @return 如果支持对 pix_fmt 进行字节序转换，则返回正值，否则返回 0。
 */
int sws_isSupportedEndiannessConversion(enum AVPixelFormat pix_fmt);

/**
 * 初始化 swscaler 上下文 sws_context。
 *
 * 此函数已弃用，仅为兼容 sws_scale() 和 sws_frame_start() 而保留。使用
 * libswscale 的首选方式是正确设置所有帧属性，然后直接调用 sws_scale_frame()，
 * 无需显式初始化上下文。
 *
 * @return 成功时返回零或正值，出错时返回负值
 */
av_warn_unused_result
int sws_init_context(SwsContext *sws_context, SwsFilter *srcFilter, SwsFilter *dstFilter);

/**
 * 释放 swscaler 上下文 swsContext。
 * 如果 swsContext 为 NULL，则不执行任何操作。
 */
void sws_freeContext(SwsContext *swsContext);

/**
 * 分配并返回 SwsContext。使用 sws_scale() 执行缩放/转换操作时需要它。
 *
 * @param srcW 源图像的宽度
 * @param srcH 源图像的高度
 * @param srcFormat 源图像格式
 * @param dstW 目标图像的宽度
 * @param dstH 目标图像的高度
 * @param dstFormat 目标图像格式
 * @param flags 指定重新缩放时使用的算法和选项
 * @param param 用于调整所用缩放器的额外参数
 *              对于 SWS_BICUBIC，param[0] 和 param[1] 调整基函数的形状，
 *              param[0] 调整 f(1)，param[1] 调整 f´(1)
 *              对于 SWS_GAUSS，param[0] 调整指数，从而调整截止频率
 *              对于 SWS_LANCZOS，param[0] 调整窗函数的宽度
 * @return 指向已分配上下文的指针；出错时返回 NULL
 * @note 编写出更合理的替代方案后，将移除此函数
 */
SwsContext *sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
                           int dstW, int dstH, enum AVPixelFormat dstFormat,
                           int flags, SwsFilter *srcFilter,
                           SwsFilter *dstFilter, const double *param);

/**
 * 缩放 srcSlice 中的图像切片，并将缩放后的切片放入 dst 图像中。切片是图像中
 * 一系列连续的行。要求上下文事先已使用 sws_init_context() 初始化。
 *
 * 切片必须按顺序提供，可以从上到下，也可以从下到上。如果以非连续顺序提供
 * 切片，此函数的行为未定义。
 *
 * @param c         之前使用 sws_getContext() 创建的缩放上下文
 * @param srcSlice  包含源切片各平面指针的数组
 * @param srcStride 包含源图像各平面步长的数组
 * @param srcSliceY 要处理的切片在源图像中的位置，即切片第一行在图像中的编号
 *                  （从零开始计数）
 * @param srcSliceH 源切片的高度，即切片中的行数
 * @param dst       包含目标图像各平面指针的数组
 * @param dstStride 包含目标图像各平面步长的数组
 * @return          输出切片的高度
 */
int sws_scale(SwsContext *c, const uint8_t *const srcSlice[],
              const int srcStride[], int srcSliceY, int srcSliceH,
              uint8_t *const dst[], const int dstStride[]);

/**
 * 为给定的一对源帧/目标帧初始化缩放过程。必须在调用 sws_send_slice() 和
 * sws_receive_slice() 之前调用。要求上下文事先已使用 sws_init_context()
 * 初始化。
 *
 * 此函数会保留对 src 和 dst 的引用，因此二者都必须使用引用计数缓冲区
 * （对于 dst，是指由调用者分配时）。
 *
 * @param c   缩放上下文
 * @param dst 目标帧。
 *
 *            数据缓冲区可以由调用者预先分配，也可以保持为空，此时缩放器会负责
 *            分配。后者可能具有性能优势——例如在某些情况下，某些输出平面可以
 *            引用输入平面，而不是进行复制。
 *
 *            sws_receive_slice() 调用成功时，输出数据会写入此帧。
 * @param src 源帧。必须分配数据缓冲区，但此时帧数据不必已经准备好。
 *            随后由 sws_send_slice() 通知数据可用。
 * @return 成功时返回 0，失败时返回负的 AVERROR 错误码
 *
 * @see sws_frame_end()
 */
int sws_frame_start(SwsContext *c, AVFrame *dst, const AVFrame *src);

/**
 * 结束此前通过 sws_frame_start() 提交的一对源帧/目标帧的缩放过程。必须在所有
 * sws_send_slice() 和 sws_receive_slice() 调用完成后、再次调用
 * sws_frame_start() 之前调用。
 *
 * @param c   缩放上下文
 */
void sws_frame_end(SwsContext *c);

/**
 * 表明此前提供给 sws_frame_start() 的源帧中，某个输入数据水平切片已经可用。
 * 切片可以按任意顺序提供，但不能重叠。对于垂直子采样像素格式，切片必须按照
 * 子采样方式对齐。
 *
 * @param c   缩放上下文
 * @param slice_start 切片的第一行
 * @param slice_height 切片中的行数
 *
 * @return 成功时返回非负数，失败时返回负的 AVERROR 错误码。
 */
int sws_send_slice(SwsContext *c, unsigned int slice_start,
                   unsigned int slice_height);

/**
 * 请求将输出数据的水平切片写入此前提供给 sws_frame_start() 的帧中。
 *
 * @param c   缩放上下文
 * @param slice_start 切片的第一行；必须是 sws_receive_slice_alignment() 的倍数
 * @param slice_height 切片中的行数；除最后一个切片外，必须是
 *                     sws_receive_slice_alignment() 的倍数（最后一个切片是指
 *                     slice_start+slice_height 等于输出帧高度的情况）
 *
 * @return 数据成功写入输出时返回非负数
 *         如果生成输出之前还需要更多输入数据，则返回 AVERROR(EAGAIN)
 *         发生其他类型的缩放失败时返回其他负的 AVERROR 错误码
 */
int sws_receive_slice(SwsContext *c, unsigned int slice_start,
                      unsigned int slice_height);

/**
 * 获取切片所需的对齐值。要求上下文事先已使用 sws_init_context() 初始化。
 *
 * @param c   缩放上下文
 * @return 使用 sws_receive_slice() 请求输出切片时所需的对齐值。传给
 *         sws_receive_slice() 的切片偏移量和大小必须是此函数返回值的倍数。
 */
unsigned int sws_receive_slice_alignment(const SwsContext *c);

/**
 * @param c 缩放上下文
 * @param dstRange 表示输出黑白范围的标志（1=jpeg / 0=mpeg）
 * @param srcRange 表示输入黑白范围的标志（1=jpeg / 0=mpeg）
 * @param table 描述输出 yuv 空间的 yuv2rgb 系数，通常为 ff_yuv2rgb_coeffs[x]
 * @param inv_table 描述输入 yuv 空间的 yuv2rgb 系数，通常为 ff_yuv2rgb_coeffs[x]
 * @param brightness 16.16 定点亮度校正值
 * @param contrast 16.16 定点对比度校正值
 * @param saturation 16.16 定点饱和度校正值
 *
 * @return 出错时返回负错误码，否则返回非负值。
 *         如果 `LIBSWSCALE_VERSION_MAJOR < 7`，不支持时返回 -1。
 */
int sws_setColorspaceDetails(SwsContext *c, const int inv_table[4],
                             int srcRange, const int table[4], int dstRange,
                             int brightness, int contrast, int saturation);

/**
 * @return 出错时返回负错误码，否则返回非负值。
 *         如果 `LIBSWSCALE_VERSION_MAJOR < 7`，不支持时返回 -1。
 */
int sws_getColorspaceDetails(SwsContext *c, int **inv_table,
                             int *srcRange, int **table, int *dstRange,
                             int *brightness, int *contrast, int *saturation);

/**
 * 分配并返回一个包含 length 个系数、尚未初始化的向量。
 */
SwsVector *sws_allocVec(int length);

/**
 * 返回用于滤波的归一化高斯曲线。
 * quality = 3 表示高质量，值越低质量越低。
 */
SwsVector *sws_getGaussianVec(double variance, double quality);

/**
 * 将 a 的所有系数乘以标量值 scalar。
 */
void sws_scaleVec(SwsVector *a, double scalar);

/**
 * 缩放 a 的所有系数，使其总和等于 height。
 */
void sws_normalizeVec(SwsVector *a, double height);

void sws_freeVec(SwsVector *a);

SwsFilter *sws_getDefaultFilter(float lumaGBlur, float chromaGBlur,
                                float lumaSharpen, float chromaSharpen,
                                float chromaHShift, float chromaVShift,
                                int verbose);
void sws_freeFilter(SwsFilter *filter);

/**
 * 检查上下文能否复用；否则重新分配一个新上下文。
 *
 * 如果 context 为 NULL，则直接调用 sws_getContext() 获取新上下文。否则检查
 * 参数是否与 context 中已保存的参数相同。如果相同，则返回当前上下文；
 * 否则释放 context，并使用新参数获取新上下文。
 *
 * 请注意，不会检查 srcFilter 和 dstFilter，而是假定它们保持不变。
 */
SwsContext *sws_getCachedContext(SwsContext *context, int srcW, int srcH,
                                 enum AVPixelFormat srcFormat, int dstW, int dstH,
                                 enum AVPixelFormat dstFormat, int flags,
                                 SwsFilter *srcFilter, SwsFilter *dstFilter,
                                 const double *param);

/**
 * 将 8 位调色板帧转换为颜色深度为 32 位的帧。
 *
 * 输出帧将采用与调色板相同的打包格式。
 *
 * @param src        源帧缓冲区
 * @param dst        目标帧缓冲区
 * @param num_pixels 要转换的像素数
 * @param palette    包含 [256] 个条目的数组，必须与 src 的颜色排列（RGB 或 BGR）匹配
 */
void sws_convertPalette8ToPacked32(const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette);

/**
 * 将 8 位调色板帧转换为颜色深度为 24 位的帧。
 *
 * 当调色板格式为 "ABCD" 时，目标帧最终采用 "ABC" 格式。
 *
 * @param src        源帧缓冲区
 * @param dst        目标帧缓冲区
 * @param num_pixels 要转换的像素数
 * @param palette    包含 [256] 个条目的数组，必须与 src 的颜色排列（RGB 或 BGR）匹配
 */
void sws_convertPalette8ToPacked24(const uint8_t *src, uint8_t *dst, int num_pixels, const uint8_t *palette);

/**
 * @}
 */

#endif /* SWSCALE_SWSCALE_H */
