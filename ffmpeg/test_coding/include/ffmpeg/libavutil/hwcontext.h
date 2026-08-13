/*
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

#ifndef AVUTIL_HWCONTEXT_H
#define AVUTIL_HWCONTEXT_H

#include "buffer.h"
#include "frame.h"
#include "log.h"
#include "pixfmt.h"

enum AVHWDeviceType {
    AV_HWDEVICE_TYPE_NONE,
    AV_HWDEVICE_TYPE_VDPAU,
    AV_HWDEVICE_TYPE_CUDA,
    AV_HWDEVICE_TYPE_VAAPI,
    AV_HWDEVICE_TYPE_DXVA2,
    AV_HWDEVICE_TYPE_QSV,
    AV_HWDEVICE_TYPE_VIDEOTOOLBOX,
    AV_HWDEVICE_TYPE_D3D11VA,
    AV_HWDEVICE_TYPE_DRM,
    AV_HWDEVICE_TYPE_OPENCL,
    AV_HWDEVICE_TYPE_MEDIACODEC,
    AV_HWDEVICE_TYPE_VULKAN,
    AV_HWDEVICE_TYPE_D3D12VA,
    AV_HWDEVICE_TYPE_AMF,
    /* OpenHarmony Codec 设备 */
    AV_HWDEVICE_TYPE_OHCODEC,
};

/**
 * This struct aggregates all the (硬件/vendor-spec如果ic) "high-level" state,
 * i.e. state that is not tied 到 a concrete processing configuration.
 * E.g., 中 an API that supports 硬件-accelerated 编码 和 解码,
 * this struct will (如果 possible) wrap the state that is common 到 both 编码
 * 和 解码 and，来自 which spec如果ic instances 的 编码器s 或 解码器s can be
 * derived.
 *
 * This struct is 引用-counted，使用 the AV缓冲区 mechanism. The
 * av_hw设备_ctx_alloc() constructor yields a 引用, whose data field
 * points 到 the actual AVHW设备上下文. Further objects derived from
 * AVHW设备上下文 (such as AVHW帧s上下文, describing a 帧 pool with
 * spec如果ic properties) will hold an internal 引用 到 it. After all the
 * 引用s are released, the AVHW设备上下文 itself will be 释放d,
 * 可选ly invoking a user-spec如果ied callback 用于 uninitializing the 硬件
 * state.
 */
typedef struct AVHWDeviceContext {
    /**
     * class 用于 logging. 设置 by av_hw设备_ctx_alloc().
     */
    const AVClass *av_class;

    /**
     * This field ident如果ies the underlying API 用于 硬件 access.
     *
     * This field is 设置 当 this struct is 分配d 和 never changed
     * afterwards.
     */
    enum AVHWDeviceType type;

    /**
     * 格式-spec如果ic data, 分配d 和 释放d by libavutil along with
     * this 上下文.
     *
     * Should be cast by the user 到 the 格式-spec如果ic 上下文 defined 中 the
     * corresponding header (hw上下文_*.h) 和 filled as described 中 the
     * documentation before calling av_hw设备_ctx_init().
     *
     * After calling av_hw设备_ctx_init() this struct should not be mod如果ied
     * by the caller.
     */
    void *hwctx;

    /**
     * This field may be 设置 by the caller before calling av_hw设备_ctx_init().
     *
     * 如果 non-NULL, this callback will be called 当 the last 引用 to
     * this 上下文 is un引用d, immediately before it is 释放d.
     *
     * @note 当 other objects (e.g an AVHW帧s上下文) are derived，来自 this
     *       struct, this callback will be invoked after all such child objects
     *       are fully un初始化d 和 their respective destructors invoked.
     */
    void (*free)(struct AVHWDeviceContext *ctx);

    /**
     * Arbitrary user data, 到 be used e.g. by the 释放() callback.
     */
    void *user_opaque;
} AVHWDeviceContext;

/**
 * This struct describes a 设置 或 pool 的 "硬件" 帧s (i.e. those with
 * data not located 中 normal system 内存). All the 帧s 中 the pool are
 * assumed 到 be 分配d 中 the same way 和 interchangeable.
 *
 * This struct is 引用-counted，使用 the AV缓冲区 mechanism 和 tied 到 a
 * given AVHW设备上下文 instance. av_hw帧_ctx_alloc() constructor
 * yields a 引用, whose data field points 到 the actual AVHW帧s上下文
 * struct.
 */
typedef struct AVHWFramesContext {
    /**
     * class 用于 logging.
     */
    const AVClass *av_class;

    /**
     * 引用 到 the parent AVHW设备上下文. This 引用 is owned and
     * managed by the enclosing AVHW帧s上下文, but the caller may derive
     * additional 引用s，来自 it.
     */
    AVBufferRef *device_ref;

    /**
     * parent AVHW设备上下文. This is simply a 指针 to
     * 设备_ref->data provided 用于 convenience.
     *
     * 设置 by libavutil 中 av_hw帧_ctx_init().
     */
    AVHWDeviceContext *device_ctx;

    /**
     * 格式-spec如果ic data, 分配d 和 释放d automatically along with
     * this 上下文.
     *
     * user shall ignore this field 如果 the corresponding 格式-spec如果ic
     * header (hw上下文_*.h) does not define a 上下文 到 be used as
     * AVHW帧s上下文.hwctx.
     *
     * Otherwise, it should be cast by the user 到 said 上下文 和 filled
     * as described 中 the documentation before calling av_hw帧_ctx_init().
     *
     * After any 帧s using this 上下文 are 创建d, the contents 的 this
     * struct should not be mod如果ied by the caller.
     */
    void *hwctx;

    /**
     * This field may be 设置 by the caller before calling av_hw帧_ctx_init().
     *
     * 如果 non-NULL, this callback will be called 当 the last 引用 to
     * this 上下文 is un引用d, immediately before it is 释放d.
     */
    void (*free)(struct AVHWFramesContext *ctx);

    /**
     * Arbitrary user data, 到 be used e.g. by the 释放() callback.
     */
    void *user_opaque;

    /**
     * pool，来自 which the 帧s are 分配d by av_hw帧_get_缓冲区().
     * This field may be 设置 by the caller before calling av_hw帧_ctx_init().
     * 缓冲区 返回ed by calling av_缓冲区_pool_get() 上 this pool must
     * have the properties described 中 the documentation 中 the corresponding hw
     * type's header (hw上下文_*.h). pool will be 释放d strictly before
     * this struct's 释放() callback is invoked.
     *
     * This field may be NULL, then libavutil will attempt 到 分配 a pool
     * internally. Note that certain 设备 types enforce pools 分配d at
     * fixed 大小 (帧 count), which cannot be extended dynamically. In such a
     * case, initial_pool_大小 must be 设置 appropriately.
     */
    AVBufferPool *pool;

    /**
     * Initial 大小 的 the 帧 pool. 如果 a 设备 type does not support
     * dynamically resizing the pool, then this is also the maximum pool 大小.
     *
     * May be 设置 by the caller before calling av_hw帧_ctx_init(). Must be
     * 设置 如果 pool is NULL 和 the 设备 type does not support dynamic pools.
     */
    int initial_pool_size;

    /**
     * 像素格式 ident如果ying the underlying HW surface type.
     *
     * Must be a hwaccel 格式, i.e. the corresponding 描述符 must have the
     * AV_PIX_FMT_标志_HWACCEL 标志 设置.
     *
     * Must be 设置 by the user before calling av_hw帧_ctx_init().
     */
    enum AVPixelFormat format;

    /**
     * 像素格式 ident如果ying the actual data layout 的 the 硬件
     * 帧s.
     *
     * Must be 设置 by the caller before calling av_hw帧_ctx_init().
     *
     * @note 当 the underlying API does not provide the exact data layout, but
     * only the colorspace/bit depth, this field should be 设置 到 the fully
     * planar version 的 that 格式 (e.g. 用于 8-bit 420 YUV it should be
     * AV_PIX_FMT_YUV420P, not AV_PIX_FMT_NV12 或 anything else).
     */
    enum AVPixelFormat sw_format;

    /**
     * 分配d dimensions 的 the 帧s 中 this pool.
     *
     * Must be 设置 by the user before calling av_hw帧_ctx_init().
     */
    int width, height;
} AVHWFramesContext;

/**
 * Look up an AVHW设备Type by name.
 *
 * @param name String name 的 the 设备 type (case-insensitive).
 * @返回 type，来自 enum AVHW设备Type, 或 AV_HW设备_TYPE_NONE 如果
 *         not found.
 */
enum AVHWDeviceType av_hwdevice_find_type_by_name(const char *name);

/** 获取 the string name 的 an AVHW设备Type.
 *
 * @param type Type，来自 enum AVHW设备Type.
 * @返回 指针 到 a static string containing the name, 或 NULL 如果 the type
 *         is not valid.
 */
const char *av_hwdevice_get_type_name(enum AVHWDeviceType type);

/**
 * Iterate over supported 设备 types.
 *
 * @param prev AV_HW设备_TYPE_NONE initially, then the previous type
 *             返回ed by this function 中 subsequent iterations.
 * @返回 next usable 设备 type，来自 enum AVHW设备Type, or
 *         AV_HW设备_TYPE_NONE 如果 there are no more.
 */
enum AVHWDeviceType av_hwdevice_iterate_types(enum AVHWDeviceType prev);

/**
 * 分配 an AVHW设备上下文 用于 a given 硬件 type.
 *
 * @param type the type 的 the 硬件 设备 到 分配.
 * @返回 a 引用 到 the newly 创建d AVHW设备上下文 上 success 或 NULL
 *         上 failure.
 */
AVBufferRef *av_hwdevice_ctx_alloc(enum AVHWDeviceType type);

/**
 * Finalize the 设备 上下文 before use. 此函数 must be called after
 * the 上下文 is filled，使用 all the required in格式ion 和 before it is
 * used 中 any way.
 *
 * @param ref a 引用 到 the AVHW设备上下文
 * @返回 0 上 success, a negative AVERROR code 上 failure
 */
int av_hwdevice_ctx_init(AVBufferRef *ref);

/**
 * Open a 设备 的 the spec如果ied type 和 创建 an AVHW设备上下文 用于 it.
 *
 * This is a convenience function intended 到 cover the simple cases. Callers
 * who need 到 fine-tune 设备 creation/management should open the 设备
 * manually 和 then wrap it 中 an AVHW设备上下文 using
 * av_hw设备_ctx_alloc()/av_hw设备_ctx_init().
 *
 * 返回ed 上下文 is already 初始化d 和 ready 用于 use, the caller
 * should not call av_hw设备_ctx_init() 上 it. user_opaque/释放 fields of
 * the 创建d AVHW设备上下文 are 设置 by this function 和 should not be
 * touched by the caller.
 *
 * @param 设备_ctx On success, a 引用 到 the newly-创建d 设备 上下文
 *                   will be written here. 引用 is owned by the caller
 *                   和 must be released，使用 av_缓冲区_unref() 当 no longer
 *                   needed. On failure, NULL will be written 到 this 指针.
 * @param type type 的 the 设备 到 创建.
 * @param 设备 type-spec如果ic string ident如果ying the 设备 到 open.
 * @param opts dictionary 的 additional (type-spec如果ic) 选项 到 use in
 *             opening the 设备. dictionary remains owned by the caller.
 * @param 标志 currently unused
 *
 * @返回 0 上 success, a negative AVERROR code 上 failure.
 */
int av_hwdevice_ctx_create(AVBufferRef **device_ctx, enum AVHWDeviceType type,
                           const char *device, AVDictionary *opts, int flags);

/**
 * 创建 a new 设备 的 the spec如果ied type，来自 an existing 设备.
 *
 * 如果 the source 设备 is a 设备 的 the target type 或 was originally
 * derived，来自 such a 设备 (possibly through one 或 more intermediate
 * 设备s 的 other types), then this will 返回 a 引用 到 the
 * existing 设备 的 the same type as is requested.
 *
 * Otherwise, it will attempt 到 derive a new 设备，来自 the given source
 * 设备.  如果 direct derivation 到 the new type is not implemented, it will
 * attempt the same derivation，来自 each ancestor 的 the source 设备 in
 * turn looking 用于 an implemented derivation method.
 *
 * @param dst_ctx On success, a 引用 到 the newly-创建d
 *                AVHW设备上下文.
 * @param type    type 的 the new 设备 到 创建.
 * @param src_ctx 引用 到 an existing AVHW设备上下文 which will be
 *                用于 创建 the new 设备.
 * @param 标志   Currently unused; should be 设置 到 zero.
 * @返回        Zero 上 success, a negative AVERROR code 上 failure.
 */
int av_hwdevice_ctx_create_derived(AVBufferRef **dst_ctx,
                                   enum AVHWDeviceType type,
                                   AVBufferRef *src_ctx, int flags);

/**
 * 创建 a new 设备 的 the spec如果ied type，来自 an existing 设备.
 *
 * 此函数 performs the same action as av_hw设备_ctx_创建_derived,
 * however, it is able 到 设置 选项 用于 the new 设备 到 be derived.
 *
 * @param dst_ctx On success, a 引用 到 the newly-创建d
 *                AVHW设备上下文.
 * @param type    type 的 the new 设备 到 创建.
 * @param src_ctx 引用 到 an existing AVHW设备上下文 which will be
 *                用于 创建 the new 设备.
 * @param 选项 选项 用于 the new 设备 到 创建, same 格式 as in
 *                av_hw设备_ctx_创建.
 * @param 标志   Currently unused; should be 设置 到 zero.
 * @返回        Zero 上 success, a negative AVERROR code 上 failure.
 */
int av_hwdevice_ctx_create_derived_opts(AVBufferRef **dst_ctx,
                                        enum AVHWDeviceType type,
                                        AVBufferRef *src_ctx,
                                        AVDictionary *options, int flags);

/**
 * 分配 an AVHW帧s上下文 tied 到 a given 设备 上下文.
 *
 * @param 设备_ctx a 引用 到 a AVHW设备上下文. 此函数 will make
 *                   a new 引用 用于 internal use, the one passed 到 the
 *                   function remains owned by the caller.
 * @返回 a 引用 到 the newly 创建d AVHW帧s上下文 上 success 或 NULL
 *         上 failure.
 */
AVBufferRef *av_hwframe_ctx_alloc(AVBufferRef *device_ctx);

/**
 * Finalize the 上下文 before use. 此函数 must be called after the
 * 上下文 is filled，使用 all the required in格式ion 和 before it is attached
 * 到 any 帧s.
 *
 * @param ref a 引用 到 the AVHW帧s上下文
 * @返回 0 上 success, a negative AVERROR code 上 failure
 */
int av_hwframe_ctx_init(AVBufferRef *ref);

/**
 * 分配 a new 帧 attached 到 the given AVHW帧s上下文.
 *
 * @param hw帧_ctx a 引用 到 an AVHW帧s上下文
 * @param 帧 an empty (freshly 分配d 或 unreffed) 帧 到 be filled with
 *              newly 分配d 缓冲区.
 * @param 标志 currently unused, should be 设置 到 zero
 * @返回 0 上 success, a negative AVERROR code 上 failure
 */
int av_hwframe_get_buffer(AVBufferRef *hwframe_ctx, AVFrame *frame, int flags);

/**
 * 复制 data 到 or，来自 a hw surface. At least one 的 dst/src must have an
 * AVHW帧s上下文 attached.
 *
 * 如果 src has an AVHW帧s上下文 attached, then the 格式 的 dst (如果 设置)
 * must use one 的 the 格式s 返回ed by av_hw帧_transfer_get_格式s(src,
 * AV_HW帧_TRANSFER_DIRECTION_FROM).
 * 如果 dst has an AVHW帧s上下文 attached, then the 格式 的 src must use one
 * 的 the 格式s 返回ed by av_hw帧_transfer_get_格式s(dst,
 * AV_HW帧_TRANSFER_DIRECTION_TO)
 *
 * dst may be "clean" (i.e.，使用 data/buf 指针s un设置), 中 which case the
 * data 缓冲区 will be 分配d by this function using av_帧_get_缓冲区().
 * 如果 dst->格式 is 设置, then this 格式 will be used, otherwise (当
 * dst->格式 is AV_PIX_FMT_NONE) the first acceptable 格式 will be chosen.
 *
 * two 帧s must have matching 分配d dimensions (i.e. equal to
 * AVHW帧s上下文.宽度/高度), since not all 设备 types support
 * transferring a sub-rectangle 的 the whole surface. display dimensions
 * (i.e. AV帧.宽度/高度) may be smaller than the 分配d dimensions, but
 * also have 到 be equal 用于 both 帧s. 当 the display dimensions are
 * smaller than the 分配d dimensions, the content 的 the padding 中 the
 * destination 帧 is unspec如果ied.
 *
 * @param dst the destination 帧. dst is not touched 上 failure.
 * @param src the source 帧.
 * @param 标志 currently unused, should be 设置 到 zero
 * @返回 0 上 success, a negative AVERROR error code 上 failure.
 */
int av_hwframe_transfer_data(AVFrame *dst, const AVFrame *src, int flags);

enum AVHWFrameTransferDirection {
    /**
     * Transfer the data，来自 the queried hw 帧.
     */
    AV_HWFRAME_TRANSFER_DIRECTION_FROM,

    /**
     * Transfer the data 到 the queried hw 帧.
     */
    AV_HWFRAME_TRANSFER_DIRECTION_TO,
};

/**
 * 获取 a 列表 的 possible source 或 target 格式s usable in
 * av_hw帧_transfer_data().
 *
 * @param hw帧_ctx the 帧 上下文 到 obtain the in格式ion for
 * @param dir the direction 的 the transfer
 * @param 格式s the 指针 到 the 输出 格式 列表 will be written here.
 *                列表 is terminated，使用 AV_PIX_FMT_NONE 和 must be 释放d
 *                by the caller 当 no longer needed using av_释放().
 *                如果 this function 返回s successfully, the 格式 列表 will
 *                have at least one item (not counting the terminator).
 *                On failure, the contents 的 this 指针 are unspec如果ied.
 * @param 标志 currently unused, should be 设置 到 zero
 * @返回 0 上 success, a negative AVERROR code 上 failure.
 */
int av_hwframe_transfer_get_formats(AVBufferRef *hwframe_ctx,
                                    enum AVHWFrameTransferDirection dir,
                                    enum AVPixelFormat **formats, int flags);


/**
 * This struct describes the constraints 上 硬件 帧s attached to
 * a given 设备，使用 a 硬件-spec如果ic configuration.  This is 返回ed
 * by av_hw设备_get_hw帧_constraints() 和 must be 释放d by
 * av_hw帧_constraints_释放() after use.
 */
typedef struct AVHWFramesConstraints {
    /**
     * 列表 的 possible 值 用于 格式 中 the hw_帧s_ctx,
     * terminated by AV_PIX_FMT_NONE.  This member will always be filled.
     */
    enum AVPixelFormat *valid_hw_formats;

    /**
     * 列表 的 possible 值 用于 sw_格式 中 the hw_帧s_ctx,
     * terminated by AV_PIX_FMT_NONE.  Can be NULL 如果 this in格式ion is
     * not known.
     */
    enum AVPixelFormat *valid_sw_formats;

    /**
     * minimum 大小 的 帧s 中 this hw_帧s_ctx.
     * (Zero 如果 not known.)
     */
    int min_width;
    int min_height;

    /**
     * maximum 大小 的 帧s 中 this hw_帧s_ctx.
     * (INT_MAX 如果 not known / no limit.)
     */
    int max_width;
    int max_height;
} AVHWFramesConstraints;

/**
 * 分配 a HW-spec如果ic configuration 结构体 用于 a given HW 设备.
 * After use, the user must 释放 all members as required by the spec如果ic
 * 硬件 结构体 being used, then 释放 the 结构体 itself with
 * av_释放().
 *
 * @param 设备_ctx a 引用 到 the associated AVHW设备上下文.
 * @返回 newly 创建d HW-spec如果ic configuration 结构体 on
 *         success 或 NULL 上 failure.
 */
void *av_hwdevice_hwconfig_alloc(AVBufferRef *device_ctx);

/**
 * 获取 the constraints 上 HW 帧s given a 设备 和 the HW-spec如果ic
 * configuration 到 be used，使用 that 设备.  如果 no HW-spec如果ic
 * configuration is provided, 返回s the maximum possible capabilities
 * 的 the 设备.
 *
 * @param ref a 引用 到 the associated AVHW设备上下文.
 * @param hwconfig a filled HW-spec如果ic configuration 结构体, 或 NULL
 *        到 返回 the maximum possible capabilities 的 the 设备.
 * @返回 AVHW帧sConstraints 结构体 describing the constraints
 *         上 the 设备, 或 NULL 如果 not available.
 */
AVHWFramesConstraints *av_hwdevice_get_hwframe_constraints(AVBufferRef *ref,
                                                           const void *hwconfig);

/**
 * 释放 an AVHW帧Constraints 结构体.
 *
 * @param constraints (filled 或 unfilled) AVHW帧Constraints 结构体.
 */
void av_hwframe_constraints_free(AVHWFramesConstraints **constraints);


/**
 * 标志 到 apply 到 帧 mappings.
 */
enum {
    /**
     * mapping must be readable.
     */
    AV_HWFRAME_MAP_READ      = 1 << 0,
    /**
     * mapping must be writeable.
     */
    AV_HWFRAME_MAP_WRITE     = 1 << 1,
    /**
     * mapped 帧 will be overwritten completely 中 subsequent
     * operations, so the current 帧 data need not be loaded.  Any 值
     * which are not overwritten are unspec如果ied.
     */
    AV_HWFRAME_MAP_OVERWRITE = 1 << 2,
    /**
     * mapping must be direct.  That is, there must not be any 复制ing in
     * the map 或 unmap steps.  Note that performance 的 direct mappings may
     * be much lower than normal 内存.
     */
    AV_HWFRAME_MAP_DIRECT    = 1 << 3,
};

/**
 * Map a 硬件 帧.
 *
 * This has a 数量 的 d如果ferent possible effects, depending 上 the 格式
 * 和 origin 的 the src 和 dst 帧s.  On 输入, src should be a usable
 * 帧，使用 valid 缓冲区 和 dst should be blank (typically as just 创建d
 * by av_帧_alloc()).  src should have an associated hw帧 上下文, and
 * dst may 可选ly have a 格式 和 associated hw帧 上下文.
 *
 * 如果 src was 创建d by mapping a 帧，来自 the hw帧 上下文 的 dst,
 * then this function undoes the mapping - dst is replaced by a 引用 to
 * the 帧 that src was originally mapped from.
 *
 * 如果 both src 和 dst have an associated hw帧 上下文, then this function
 * attempts 到 map the src 帧，来自 its 硬件 上下文 到 that 的 dst and
 * then fill dst，使用 appropriate data 到 be usable there.  This will only be
 * possible 如果 the hw帧 上下文s 和 associated 设备s are compatible -
 * given compatible 设备s, av_hw帧_ctx_创建_derived() 可用于 to
 * 创建 a hw帧 上下文 用于 dst 中 which mapping should be possible.
 *
 * 如果 src has a hw帧 上下文 but dst does not, then the src 帧 is
 * mapped 到 normal 内存 和 should thereafter be usable as a normal 帧.
 * 如果 the 格式 is 设置 上 dst, then the mapping will attempt 到 创建 dst
 *，使用 that 格式 和 fail 如果 it is not possible.  如果 格式 is un设置 (is
 * AV_PIX_FMT_NONE) then dst will be mapped，使用 whatever the most appropriate
 * 格式 到 use is (probably the sw_格式 的 the src hw帧 上下文).
 *
 * 返回 值 的 AVERROR(ENOSYS) indicates that the mapping is not
 * possible，使用 the given arguments 和 hw帧 设置up, while other 返回
 * 值 indicate that it failed somehow.
 *
 * On failure, the destination 帧 will be left blank, except 用于 the
 * hw_帧s_ctx/格式 fields they may have been 设置 by the caller - those will
 * be preserved as they were.
 *
 * @param dst Destination 帧, 到 contain the mapping.
 * @param src Source 帧, 到 be mapped.
 * @param 标志 Some combination 的 AV_HW帧_MAP_* 标志.
 * @返回 Zero 上 success, negative AVERROR code 上 failure.
 */
int av_hwframe_map(AVFrame *dst, const AVFrame *src, int flags);


/**
 * 创建 和 initialise an AVHW帧s上下文 as a mapping 的 another existing
 * AVHW帧s上下文 上 a d如果ferent 设备.
 *
 * av_hw帧_ctx_init() should not be called after this.
 *
 * @param derived_帧_ctx  On success, a 引用 到 the newly 创建d
 *                           AVHW帧s上下文.
 * @param 格式             AV像素格式 用于 the derived 上下文.
 * @param derived_设备_ctx 引用 到 the 设备 到 创建 the new
 *                           AVHW帧s上下文 on.
 * @param source_帧_ctx   引用 到 an existing AVHW帧s上下文
 *                           which will be mapped 到 the derived 上下文.
 * @param 标志  Some combination 的 AV_HW帧_MAP_* 标志, defining the
 *               mapping parameters 到 apply 到 帧s which are 分配d
 *               中 the derived 设备.
 * @返回       Zero 上 success, negative AVERROR code 上 failure.
 */
int av_hwframe_ctx_create_derived(AVBufferRef **derived_frame_ctx,
                                  enum AVPixelFormat format,
                                  AVBufferRef *derived_device_ctx,
                                  AVBufferRef *source_frame_ctx,
                                  int flags);

#endif /* AVUTIL_HWCONTEXT_H */
