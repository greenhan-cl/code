/*
 * Copyright (c) 2015 Kevin Wheatley <kevin.j.wheatley@gmail.com>
 * Copyright (c) 2016 Ronald S. Bultje <rsbultje@gmail.com>
 * Copyright (c) 2023 Leo Izen <leo.izen@gmail.com>
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

#ifndef AVUTIL_CSP_H
#define AVUTIL_CSP_H

#include "pixfmt.h"
#include "rational.h"

/**
 * @file
 * libavutil 的颜色空间值工具函数。
 * @ingroup lavu_math_csp
 * @author Ronald S. Bultje <rsbultje@gmail.com>
 * @author Leo Izen <leo.izen@gmail.com>
 * @author Kevin Wheatley <kevin.j.wheatley@gmail.com>
 */

/**
 * @defgroup lavu_math_csp 颜色空间工具
 * @ingroup lavu_math
 * @{
 */

/**
 * 包含用于 RGB 到 YUV/YCoCg 或类似计算的亮度系数的结构。
 */
typedef struct AVLumaCoefficients {
    AVRational cr, cg, cb;
} AVLumaCoefficients;

/**
 * 包含标准 CIE 1931 色度定义的 x、y 色度值的结构。
 */
typedef struct AVCIExy {
    AVRational x, y;
} AVCIExy;

/**
 * 使用 CIE 1931 色度 x、y 定义红、绿、蓝原色位置的结构。
 */
typedef struct AVPrimaryCoefficients {
    AVCIExy r, g, b;
} AVPrimaryCoefficients;

/**
 * 使用 CIE 1931 色度 x、y 定义白点位置的结构。
 */
typedef AVCIExy AVWhitepointCoefficients;

/**
 * 同时包含白点位置和原色位置、用于完整描述色域的结构。
 */
typedef struct AVColorPrimariesDesc {
    AVWhitepointCoefficients wp;
    AVPrimaryCoefficients prim;
} AVColorPrimariesDesc;

/**
 * 表示 double -> double 传递函数的函数指针，该函数执行 OETF，或执行逆 EOTF
 * （尤其用于 SMPTE ST 2084 / PQ）。此函数输入线性光，输出伽马编码光。
 *
 * 更多信息参见 ITU-T H.273。
 */
typedef double (*av_csp_trc_function)(double);

/**
 * 根据描述颜色空间的枚举常量，获取构造转换矩阵所需的亮度系数。
 * @param csp 表示 YUV 或类似颜色空间的枚举常量。
 * @return 与该颜色空间关联的亮度系数；libavutil 不识别该常量时返回 NULL。
 */
const AVLumaCoefficients *av_csp_luma_coeffs_from_avcsp(enum AVColorSpace csp);

/**
 * 根据描述颜色原色的枚举常量获取完整色域描述。
 * @param prm 表示原色的枚举常量
 * @return 与该枚举常量关联的颜色空间色域描述；libavutil 不识别该常量时返回 NULL。
 */
const AVColorPrimariesDesc *av_csp_primaries_desc_from_id(enum AVColorPrimaries prm);

/**
 * 检测哪个 AVColorPrimaries 枚举常量对应给定的完整色域描述。
 * @see enum AVColorPrimaries
 * @param prm 颜色空间色域描述
 * @return 与此色域关联的枚举常量；无法识别明确匹配时返回 AVCOL_PRI_UNSPECIFIED。
 */
enum AVColorPrimaries av_csp_primaries_id_from_desc(const AVColorPrimariesDesc *prm);

/**
 * 确定与所提供 AVColorTransferCharacteristic 匹配的合适“gamma”值。
 *
 * 参见 Apple 技术说明 TN2257（https://developer.apple.com/library/mac/technotes/tn2257/_index.html）
 *
 * 此函数返回 OETF 的 gamma 指数。例如，sRGB 用 gamma 2.2 近似，而不是 0.45455。
 *
 * @return 返回与所提供传递特性匹配的简单 gamma 函数近似值；无法合理匹配时
 *         返回 0.0。
 */
double av_csp_approximate_trc_gamma(enum AVColorTransferCharacteristic trc);

/**
 * 确定与所提供 AVColorTransferCharacteristic 匹配的合适 EOTF“gamma”值。
 *
 * 此函数返回用于近似所提供 AVColorTransferCharacteristic 的简单纯幂函数 gamma
 * 值（指数）；不存在合理近似时返回 0。
 *
 * EOTF(v) = (L_w - L_b) * v^gamma + L_b
 *
 * @return 返回与所提供传递特性 EOTF 匹配的简单 gamma 函数近似值；无法合理
 *         匹配时返回 0.0。
 */
double av_csp_approximate_eotf_gamma(enum AVColorTransferCharacteristic trc);

/**
 * 确定将给定 AVColorTransferCharacteristic 应用于线性输入所需的函数。
 *
 * 返回函数的标称定义域和值域应为 [0.0-1.0]；此范围外的值是否有效取决于所选
 * 特性函数。
 *
 * @return 返回与所提供传递特性匹配的函数指针；未指定时返回 NULL：
 */
av_csp_trc_function av_csp_trc_func_from_id(enum AVColorTransferCharacteristic trc);

/**
 * 返回对应 TRC 函数的数学逆函数。
 */
av_csp_trc_function av_csp_trc_func_inv_from_id(enum AVColorTransferCharacteristic trc);

/**
 * 表示给定参考显示配置的 ITU EOTF 传递函数的函数指针。
 *
 * @param Lw 显示器白点亮度，单位为 nit（cd/m^2）。
 * @param Lb 显示器黑点亮度，单位为 nit（cd/m^2）。
 */
typedef void (*av_csp_eotf_function)(double Lw, double Lb, double c[3]);

/**
 * 返回与给定 TRC 对应的 ITU EOTF。它把信号电平 [0,1] 转换为以 nit（cd/m^2）
 * 表示的原始输出显示亮度。除假定输入输出均为 CIE XYZ 的 AVCOL_TRC_SMPTE428
 * 外，此操作在 RGB 空间中按声道进行。
 *
 * @return 指向实现给定 TRC 的函数的指针；未定义此类函数时返回 NULL。
 *
 * @note 通常，只要可能，结果函数也会为超出范围的值定义，即使这些值在给定
 *       显示器上没有物理意义。如果不需要此行为，用户应钳制输入（或输出）。
 *
 *       对 PQ 等在独立于目标显示能力的绝对信号范围上定义的函数也是如此。
 */
av_csp_eotf_function av_csp_itu_eotf(enum AVColorTransferCharacteristic trc);

/**
 * 返回对应 EOTF 的数学逆函数。
 */
av_csp_eotf_function av_csp_itu_eotf_inv(enum AVColorTransferCharacteristic trc);

/**
 * @}
 */

#endif /* AVUTIL_CSP_H */
