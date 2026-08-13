/*
 * Copyright (c) 2000, 2001, 2002 Fabrice Bellard
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

#ifndef AVUTIL_CPU_H
#define AVUTIL_CPU_H

#include <stddef.h>
#include "version.h"

#if FF_API_CPU_FLAG_FORCE
#define AV_CPU_FLAG_FORCE    0x80000000 /* @deprecated，不应使用 */
#endif

    /* 低 16 位——CPU 功能 */
#define AV_CPU_FLAG_MMX          0x0001 ///< 标准 MMX
#define AV_CPU_FLAG_MMXEXT       0x0002 ///< SSE 整数功能或 AMD MMX 扩展
#define AV_CPU_FLAG_MMX2         0x0002 ///< SSE 整数功能或 AMD MMX 扩展
#define AV_CPU_FLAG_3DNOW        0x0004 ///< AMD 3DNOW
#define AV_CPU_FLAG_SSE          0x0008 ///< SSE 功能
#define AV_CPU_FLAG_SSE2         0x0010 ///< PIV SSE2 功能
#define AV_CPU_FLAG_SSE2SLOW 0x40000000 ///< 支持 SSE2，但通常不比普通 MMX/SSE 快
                                        ///< （例如 Core1）
#define AV_CPU_FLAG_3DNOWEXT     0x0020 ///< AMD 3DNowExt
#define AV_CPU_FLAG_SSE3         0x0040 ///< Prescott SSE3 功能
#define AV_CPU_FLAG_SSE3SLOW 0x20000000 ///< 支持 SSE3，但通常不比普通 MMX/SSE 快
                                        ///< （例如 Core1）
#define AV_CPU_FLAG_SSSE3        0x0080 ///< Conroe SSSE3 功能
#define AV_CPU_FLAG_SSSE3SLOW 0x4000000 ///< 支持 SSSE3，但通常不更快
#define AV_CPU_FLAG_ATOM     0x10000000 ///< Atom 处理器，某些 SSSE3 指令较慢
#define AV_CPU_FLAG_SSE4         0x0100 ///< Penryn SSE4.1 功能
#define AV_CPU_FLAG_SSE42        0x0200 ///< Nehalem SSE4.2 功能
#define AV_CPU_FLAG_AESNI       0x80000 ///< 高级加密标准功能
#define AV_CPU_FLAG_CLMUL      0x400000 ///< 无进位乘法指令
#define AV_CPU_FLAG_AVX          0x4000 ///< AVX 功能：即使不用 YMM 寄存器也要求操作系统支持
#define AV_CPU_FLAG_AVXSLOW   0x8000000 ///< 支持 AVX，但使用 YMM 寄存器时较慢（例如 Bulldozer）
#define AV_CPU_FLAG_XOP          0x0400 ///< Bulldozer XOP functions
#define AV_CPU_FLAG_FMA4         0x0800 ///< Bulldozer FMA4 functions
#define AV_CPU_FLAG_CMOV         0x1000 ///< 支持 cmov 指令
#define AV_CPU_FLAG_AVX2         0x8000 ///< AVX2 功能：即使不用 YMM 寄存器也要求操作系统支持
#define AV_CPU_FLAG_FMA3        0x10000 ///< Haswell FMA3 功能
#define AV_CPU_FLAG_BMI1        0x20000 ///< 位操作指令集 1
#define AV_CPU_FLAG_BMI2        0x40000 ///< 位操作指令集 2
#define AV_CPU_FLAG_AVX512     0x100000 ///< AVX-512 功能：即使不用 YMM/ZMM 寄存器也要求操作系统支持
#define AV_CPU_FLAG_AVX512ICL  0x200000 ///< F/CD/BW/DQ/VL/VNNI/IFMA/VBMI/VBMI2/VPOPCNTDQ/BITALG/GFNI/VAES/VPCLMULQDQ
#define AV_CPU_FLAG_SLOW_GATHER  0x2000000 ///< CPU 的 gather 操作较慢。

#define AV_CPU_FLAG_ALTIVEC      0x0001 ///< 标准
#define AV_CPU_FLAG_VSX          0x0002 ///< ISA 2.06
#define AV_CPU_FLAG_POWER8       0x0004 ///< ISA 2.07

#define AV_CPU_FLAG_ARMV5TE      (1 << 0)
#define AV_CPU_FLAG_ARMV6        (1 << 1)
#define AV_CPU_FLAG_ARMV6T2      (1 << 2)
#define AV_CPU_FLAG_VFP          (1 << 3)
#define AV_CPU_FLAG_VFPV3        (1 << 4)
#define AV_CPU_FLAG_NEON         (1 << 5)
#define AV_CPU_FLAG_ARMV8        (1 << 6)
#define AV_CPU_FLAG_VFP_VM       (1 << 7) ///< VFPv2 向量模式，在 ARMv7-A 中已弃用，且多种 CPU 实现不可用
#define AV_CPU_FLAG_DOTPROD      (1 << 8)
#define AV_CPU_FLAG_I8MM         (1 << 9)
#define AV_CPU_FLAG_SVE          (1 <<10)
#define AV_CPU_FLAG_SVE2         (1 <<11)
#define AV_CPU_FLAG_SME          (1 <<12)
#define AV_CPU_FLAG_ARM_CRC      (1 <<13)
#define AV_CPU_FLAG_SME2         (1 <<14)
#define AV_CPU_FLAG_SME_I16I64   (1 <<15)
#define AV_CPU_FLAG_SETEND       (1 <<16)
#define AV_CPU_FLAG_PMULL        (1 <<17)
#define AV_CPU_FLAG_EOR3         (1 <<18)

#define AV_CPU_FLAG_MMI          (1 << 0)
#define AV_CPU_FLAG_MSA          (1 << 1)

// LoongArch SIMD 扩展。
#define AV_CPU_FLAG_LSX          (1 << 0)
#define AV_CPU_FLAG_LASX         (1 << 1)

// RISC-V 扩展
#define AV_CPU_FLAG_RVI          (1 << 0) ///< I（完整 GPR 组）
#define AV_CPU_FLAG_RVV_I32      (1 << 3) ///< 8/16/32 位整数向量 */
#define AV_CPU_FLAG_RVV_F32      (1 << 4) ///< float 向量 */
#define AV_CPU_FLAG_RVV_I64      (1 << 5) ///< 64 位整数向量 */
#define AV_CPU_FLAG_RVV_F64      (1 << 6) ///< double 向量
#define AV_CPU_FLAG_RVB_BASIC    (1 << 7) ///< 基本位操作
#define AV_CPU_FLAG_RV_ZVBB      (1 << 9) ///< 向量基本位操作
#define AV_CPU_FLAG_RV_MISALIGNED (1 <<10) ///< 快速非对齐访问
#define AV_CPU_FLAG_RVB          (1 <<11) ///< B（位操作）

// WASM 扩展
#define AV_CPU_FLAG_SIMD128      (1 << 0)

/**
 * 返回指定 CPU 所支持扩展的标志。如果之前使用过 av_force_cpu_flags()，返回值
 * 会受到其影响。因此应用程序可方便地使用 av_get_cpu_flags() 检测已启用的
 * CPU 标志。
 */
int av_get_cpu_flags(void);

/**
 * 禁用 CPU 检测并强制使用指定标志。-1 是特殊值，表示不强制指定标志。
 */
void av_force_cpu_flags(int flags);

/**
 * 从字符串解析 CPU 能力，并据此更新给定的 AV_CPU_* 标志。
 *
 * @return 出错时返回负值。
 */
int av_parse_cpu_caps(unsigned *flags, const char *s);

/**
 * @return 当前逻辑 CPU 核心数量。
 */
int av_cpu_count(void);

/**
 * 覆盖 CPU 数量检测并强制使用指定数量。count < 1 表示不强制指定数量。
 */
void av_cpu_force_count(int count);

/**
 * 获取 FFmpeg 可能要求的最大数据对齐值。
 *
 * 请注意，此值受构建配置和 CPU 标志掩码影响。例如，即使 CPU 支持 AVX，
 * 但 libavutil 使用 --disable-avx 构建，或通过 av_set_cpu_flags_mask() 禁用了
 * AV_CPU_FLAG_AVX 标志，此函数也会表现得像不存在 AVX 一样。
 */
size_t av_cpu_max_align(void);

#endif /* AVUTIL_CPU_H */
