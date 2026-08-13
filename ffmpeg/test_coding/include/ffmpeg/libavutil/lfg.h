/*
 * Lagged Fibonacci PRNG
 * Copyright (c) 2008 Michael Niedermayer
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

#ifndef AVUTIL_LFG_H
#define AVUTIL_LFG_H

#include <stdint.h>

/**
 * 滞后斐波那契伪随机数生成器的上下文结构。
 * 此结构的确切布局、类型和内容可能变化，不应直接访问。只保证其 `sizeof()`
 * 保持不变，以便轻松实例化。
 */
typedef struct AVLFG {
    unsigned int state[64];
    int index;
} AVLFG;

void av_lfg_init(AVLFG *c, unsigned int seed);

/**
 * 使用二进制数据为 ALFG 状态设定种子。
 *
 * @return 成功时返回 0，失败时返回负值（AVERROR）。
 */
int av_lfg_init_from_data(AVLFG *c, const uint8_t *data, unsigned int length);

/**
 * 使用 ALFG 获取下一个随机的无符号 32 位数。
 *
 * 也请考虑 state= state*1664525+1013904223 这样的简单 LCG；对于特定使用场景，
 * 它可能已经足够好且速度更快。
 */
static inline unsigned int av_lfg_get(AVLFG *c){
    unsigned a = c->state[c->index & 63] = c->state[(c->index-24) & 63] + c->state[(c->index-55) & 63];
    c->index += 1U;
    return a;
}

/**
 * 使用 MLFG 获取下一个随机的无符号 32 位数。
 *
 * 也请考虑上面的 av_lfg_get()，它速度更快。
 */
static inline unsigned int av_mlfg_get(AVLFG *c){
    unsigned int a= c->state[(c->index-55) & 63];
    unsigned int b= c->state[(c->index-24) & 63];
    a = c->state[c->index & 63] = 2*a*b+a+b;
    c->index += 1U;
    return a;
}

/**
 * 使用 lfg 生成的随机数，取得 Box-Muller 高斯生成器生成的下两个数。
 *
 * @param lfg 指向上下文结构的指针
 * @param out 存放两个生成数的数组
 */
void av_bmg_get(AVLFG *lfg, double out[2]);

#endif /* AVUTIL_LFG_H */
