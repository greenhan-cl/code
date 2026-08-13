/*
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

#ifndef AVUTIL_REPLAYGAIN_H
#define AVUTIL_REPLAYGAIN_H

#include <stdint.h>

/**
 * ReplayGain 信息（参见
 * http://wiki.hydrogenaudio.org/index.php?title=ReplayGain_1.0_specification).
 * 此结构体的大小属于公共 ABI。
 */
typedef struct AVReplayGain {
    /**
     * 轨道回放增益，单位为微贝尔（除以 100000 得到 dB 值）。未知时应设为 INT32_MIN。
     */
    int32_t track_gain;
    /**
     * 轨道峰值幅度，100000 表示满量程（值可能溢出）。未知时为 0。
     */
    uint32_t track_peak;
    /**
     * 与 track_gain 相同，但适用于整张专辑。
     */
    int32_t album_gain;
    /**
     * 与 track_peak 相同，但适用于整张专辑。
     */
    uint32_t album_peak;
} AVReplayGain;

#endif /* AVUTIL_REPLAYGAIN_H */
