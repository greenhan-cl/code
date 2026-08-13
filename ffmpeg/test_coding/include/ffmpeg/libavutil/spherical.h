/*
 * 复制right (c) 2016 Vittorio Giovara <vittorio.giovara@gmail.com>
 *
 * This file is part 的 FFmpeg.
 *
 * FFmpeg is 释放 software; you can redistribute it and/or
 * mod如果y it under the terms 的 the GNU Lesser General 公共
 * License as published by the 释放 Software Foundation; either
 * version 2.1 的 the License, 或 (at your 选项) any later version.
 *
 * FFmpeg is distributed 中 the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY 或 FITNESS FOR PARTICULAR PURPOSE.  参见 the GNU
 * Lesser General 公共 License 用于 more details.
 *
 * You should have received a 复制 的 the GNU Lesser General 公共
 * License along，使用 FFmpeg; 如果 not, write 到 the 释放 Software
 * Foundation, Inc., 51 Franklin Street, F如果th Floor, Boston, M02110-1301 USA
 */

/**
 * @file
 * @ingroup lavu_视频_spherical
 * Spherical 视频
 */

#ifndef AVUTIL_SPHERICAL_H
#define AVUTIL_SPHERICAL_H

#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup lavu_视频_spherical Spherical 视频 mapping
 * @ingroup lavu_视频
 *
 * spherical 视频 file 包含 surfaces that need 到 be mapped onto a
 * sphere. Depending 上 how the 帧 was 转换ed, a d如果ferent distortion
 * trans格式ion 或 surface recomposition function needs 到 be applied before
 * the 视频 should be mapped 和 displayed.
 * @{
 */

/**
 * Projection 的 the 视频 surface(s) 上 a sphere.
 */
enum AVSphericalProjection {
    /**
     * 视频 represents a sphere mapped 上 a flat surface using
     * equirectangular projection.
     */
    AV_SPHERICAL_EQUIRECTANGULAR,

    /**
     * 视频 帧 is split into 6 faces 的 a cube, 和 arranged 上 a
     * 3x2 layout. Faces are oriented upwards 用于 the front, left, right,
     * 和 back faces. up face is oriented so the top 的 the face is
     * forwards 和 the down face is oriented so the top 的 the face is
     * 到 the back.
     */
    AV_SPHERICAL_CUBEMAP,

    /**
     * 视频 represents a portion 的 a sphere mapped 上 a flat surface
     * using equirectangular projection. @ref bounding fields indicate
     * the position 的 the current 视频 中 a larger surface.
     */
    AV_SPHERICAL_EQUIRECTANGULAR_TILE,

    /**
     * 视频 帧 displays as a 180 degree equirectangular projection.
     */
    AV_SPHERICAL_HALF_EQUIRECTANGULAR,

    /**
     * 视频 帧 displays 上 a flat, rectangular 2D surface.
     */
    AV_SPHERICAL_RECTILINEAR,

    /**
     * Fisheye projection (Apple).
     * See: https://developer.apple.com/documentation/coremedia/cmprojectiontype/fisheye
     */
    AV_SPHERICAL_FISHEYE,

    /**
     * Parametric Immersive projection (Apple).
     * See: https://developer.apple.com/documentation/coremedia/cmprojectiontype/parametricimmersive
     */
    AV_SPHERICAL_PARAMETRIC_IMMERSIVE,
};

/**
 * 此结构体 describes how 到 handle spherical 视频s, outlining
 * in格式ion about projection, initial layout, 和 any other view mod如果ier.
 *
 * @note struct must be 分配d，使用 av_spherical_alloc() and
 *       its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVSphericalMapping {
    /**
     * Projection type.
     */
    enum AVSphericalProjection projection;

    /**
     * @name Initial orientation
     * @{
     * There fields describe additional rotations applied 到 the sphere after
     * the 视频 帧 is mapped onto it. sphere is rotated around the
     * viewer, who remains stationary. order 的 trans格式ion is always
     * yaw, followed by pitch, 和 finally by roll.
     *
     * coordinate system matches the one defined 中 OpenGL, where the
     * forward vector (z) is coming out 的 screen, 和 it is equivalent to
     * a rotation matrix 的 R = r_y(yaw) * r_x(pitch) * r_z(roll).
     *
     * positive yaw rotates the portion 的 the sphere 中 front 的 the viewer
     * toward their right. positive pitch rotates the portion 的 the sphere
     * 中 front 的 the viewer upwards. positive roll tilts the portion of
     * the sphere 中 front 的 the viewer 到 the viewer's right.
     *
     * These 值 are exported as 16.16 fixed point.
     *
     * 参见 this equirectangular projection as example:
     *
     * @code{.un解析d}
     *                   Yaw
     *     -180           0           180
     *   90 +-------------+-------------+  180
     *      |             |             |                  up
     * P    |             |             |                 y|    forward
     * i    |             ^             |                  |   /z
     * t  0 +-------------X-------------+    0 Roll        |  /
     * c    |             |             |                  | /
     * h    |             |             |                 0|/_____right
     *      |             |             |                        x
     *  -90 +-------------+-------------+ -180
     *
     * X - the 默认 camera center
     * ^ - the 默认 up vector
     * @endcode
     */
    int32_t yaw;   ///< Rotation around the up vector [-180, 180].
    int32_t pitch; ///< Rotation around the right vector [-90, 90].
    int32_t roll;  ///< Rotation around the forward vector [-180, 180].
    /**
     * @}
     */

    /**
     * @name Bounding rectangle
     * @anchor bounding
     * @{
     * These fields indicate the location 的 the current tile, 和 where
     * it should be mapped relative 到 the original surface. They are
     * exported as 0.32 fixed point, 和 can be 转换ed 到 classic
     * 像素 值，使用 av_spherical_bounds().
     *
     * @code{.un解析d}
     *      +----------------+----------+
     *      |                |bound_top |
     *      |            +--------+     |
     *      | bound_left |tile    |     |
     *      +<---------->|        |<--->+bound_right
     *      |            +--------+     |
     *      |                |          |
     *      |    bound_bottom|          |
     *      +----------------+----------+
     * @endcode
     *
     * 如果 needed, the original 视频 surface dimensions can be derived
     * by adding the current stream 或 帧 大小 到 the related bounds,
     * like 中 the following example:
     *
     * @code{c}
     *     original_宽度  = tile->宽度  + bound_left + bound_right;
     *     original_高度 = tile->高度 + bound_top  + bound_bottom;
     * @endcode
     *
     * @note These 值 are valid only 用于 the tiled equirectangular
     *       projection type (@ref AV_SPHERICAL_EQUIRECTANGULAR_TILE),
     *       和 should be ignored 中 all other cases.
     */
    uint32_t bound_left;   ///< Distance from the left edge
    uint32_t bound_top;    ///< Distance from the top edge
    uint32_t bound_right;  ///< Distance from the right edge
    uint32_t bound_bottom; ///< Distance from the bottom edge
    /**
     * @}
     */

    /**
     * 数量 的 像素s 到 pad，来自 the edge 的 each cube face.
     *
     * @note This 值 is valid 用于 only 用于 the cubemap projection type
     *       (@ref AV_SPHERICAL_CUBEMAP), 和 should be ignored 中 all other
     *       cases.
     */
    uint32_t padding;
} AVSphericalMapping;

/**
 * 分配 a AVSpherical视频 结构体 和 初始化 its fields 到 默认
 * 值.
 *
 * @返回 the newly 分配d struct 或 NULL 上 failure
 */
AVSphericalMapping *av_spherical_alloc(size_t *size);

/**
 * 转换 the @ref bounding fields，来自 an AVSpherical视频
 *，来自 0.32 fixed point 到 像素s.
 *
 * @param map    AVSpherical视频 map 到 read bound 值 from.
 * @param 宽度  宽度 的 the current 帧 或 stream.
 * @param 高度 高度 的 the current 帧 或 stream.
 * @param left   像素s，来自 the left edge.
 * @param top    像素s，来自 the top edge.
 * @param right  像素s，来自 the right edge.
 * @param bottom 像素s，来自 the bottom edge.
 */
void av_spherical_tile_bounds(const AVSphericalMapping *map,
                              size_t width, size_t height,
                              size_t *left, size_t *top,
                              size_t *right, size_t *bottom);

/**
 * Provide a human-readable name 的 a given AVSphericalProjection.
 *
 * @param projection 输入 AVSphericalProjection.
 *
 * @返回 name 的 the AVSphericalProjection, 或 "unknown".
 */
const char *av_spherical_projection_name(enum AVSphericalProjection projection);

/**
 * 获取 the AVSphericalProjection form a human-readable name.
 *
 * @param name 输入 string.
 *
 * @返回 AVSphericalProjection 值, 或 -1 如果 not found.
 */
int av_spherical_from_name(const char *name);
/**
 * @}
 */

#endif /* AVUTIL_SPHERICAL_H */
