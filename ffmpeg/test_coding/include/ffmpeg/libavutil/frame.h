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

/**
 * @file
 * @ingroup lavu_帧
 * 引用-counted 帧 API
 */

#ifndef AVUTIL_FRAME_H
#define AVUTIL_FRAME_H

#include <stddef.h>
#include <stdint.h>

#include "avutil.h"
#include "buffer.h"
#include "channel_layout.h"
#include "dict.h"
#include "rational.h"
#include "samplefmt.h"
#include "pixfmt.h"
#include "version.h"


/**
 * @defgroup lavu_帧 AV帧
 * @ingroup lavu_data
 *
 * @{
 * AV帧 is an abstraction 用于 引用-counted raw multimedia data.
 */

enum AVFrameSideDataType {
    /**
     * data is the AVPanScan struct defined 中 libavcodec.
     */
    AV_FRAME_DATA_PANSCAN,
    /**
     * ATSC A53 Part 4 Closed Captions.
     * A53 CC bitstream is stored as uint8_t 中 AV帧SideData.data.
     * 数量 的 bytes 的 CC data is AV帧SideData.大小.
     */
    AV_FRAME_DATA_A53_CC,
    /**
     * Stereoscopic 3d 元数据.
     * data is the AVStereo3D struct defined 中 libavutil/stereo3d.h.
     */
    AV_FRAME_DATA_STEREO3D,
    /**
     * data is the AVMatrix编码 enum defined 中 libavutil/声道_layout.h.
     */
    AV_FRAME_DATA_MATRIXENCODING,
    /**
     * 元数据 relevant 到 a downmix procedure.
     * data is the AVDownmixInfo struct defined 中 libavutil/downmix_info.h.
     */
    AV_FRAME_DATA_DOWNMIX_INFO,
    /**
     * ReplayGain in格式ion 中 the form 的 the AVReplayGain struct.
     */
    AV_FRAME_DATA_REPLAYGAIN,
    /**
     * This side data 包含 a 3x3 trans格式ion matrix describing an affine
     * trans格式ion that needs 到 be applied 到 the 帧 用于 correct
     * presentation.
     *
     * 参见 libavutil/display.h 用于 a detailed 描述 的 the data.
     */
    AV_FRAME_DATA_DISPLAYMATRIX,
    /**
     * Active 格式 描述 data consisting 的 a single byte as spec如果ied
     * 中 ETSI TS 101 154 using AVActive格式描述 enum.
     */
    AV_FRAME_DATA_AFD,
    /**
     * Motion vectors exported by some codecs (on demand through the export_mvs
     * 标志 设置 中 the libavcodec AVCodec上下文 标志2 选项).
     * data is the AVMotionVector struct defined in
     * libavutil/motion_vector.h.
     */
    AV_FRAME_DATA_MOTION_VECTORS,
    /**
     * Recommends skipping the spec如果ied 数量 的 采样s. This is exported
     * only 如果 the "skip_manual" AV选项 is 设置 中 libavcodec.
     * This has the same 格式 as AV_PKT_DATA_SKIP_采样S.
     * @code
     * u32le 数量 的 采样s 到 skip，来自 start 的 this packet
     * u32le 数量 的 采样s 到 skip，来自 end 的 this packet
     * u8    reason 用于 start skip
     * u8    reason 用于 end   skip (0=padding silence, 1=convergence)
     * @endcode
     */
    AV_FRAME_DATA_SKIP_SAMPLES,
    /**
     * This side data must be associated，使用 an 音频 帧 和 corresponds to
     * enum AV音频ServiceType defined 中 avcodec.h.
     */
    AV_FRAME_DATA_AUDIO_SERVICE_TYPE,
    /**
     * Mastering display 元数据 associated，使用 a 视频 帧. payload is
     * an AVMasteringDisplay元数据 type 和 包含 in格式ion about the
     * mastering display color volume.
     */
    AV_FRAME_DATA_MASTERING_DISPLAY_METADATA,
    /**
     * GOP timecode 中 25 bit timecode 格式. Data 格式 is 64-bit integer.
     * This is 设置 上 the first 帧 的 a GOP that has a temporal 引用 的 0.
     */
    AV_FRAME_DATA_GOP_TIMECODE,

    /**
     * data represents the AVSphericalMapping 结构体 defined in
     * libavutil/spherical.h.
     */
    AV_FRAME_DATA_SPHERICAL,

    /**
     * Content light level (based 上 CTA-861.3). This payload 包含 data in
     * the form 的 the AVContentLight元数据 struct.
     */
    AV_FRAME_DATA_CONTENT_LIGHT_LEVEL,

    /**
     * data 包含 an ICC profile as an opaque octet 缓冲区 following the
     * 格式 described by ISO 15076-1，使用 an 可选 name defined 中 the
     * 元数据 key entry "name".
     */
    AV_FRAME_DATA_ICC_PROFILE,

    /**
     * Timecode which conforms 到 SMPTE ST 12-1. data is an 数组 的 4 uint32_t
     * where the first uint32_t describes how many (1-3) 的 the other timecodes are used.
     * timecode 格式 is described 中 the documentation 的 av_timecode_get_smpte_from_帧num()
     * function 中 libavutil/timecode.h.
     */
    AV_FRAME_DATA_S12M_TIMECODE,

    /**
     * HDR dynamic 元数据 associated，使用 a 视频 帧. payload is
     * an AVDynamicHDRPlus type 和 包含 in格式ion 用于 color
     * volume transform - application 4 的 SMPTE 2094-40:2016 standard.
     */
    AV_FRAME_DATA_DYNAMIC_HDR_PLUS,

    /**
     * Regions Of Interest, the data is an 数组 的 AVRegionOfInterest type, the 数量 of
     * 数组 element is implied by AV帧SideData.大小 / AVRegionOfInterest.self_大小.
     */
    AV_FRAME_DATA_REGIONS_OF_INTEREST,

    /**
     * 编码 parameters 用于 a 视频 帧, as described by AV视频EncParams.
     */
    AV_FRAME_DATA_VIDEO_ENC_PARAMS,

    /**
     * User data unregistered 元数据 associated，使用 a 视频 帧.
     * This is the H.26[45] UDU SEI message, 和 shouldn't be 用于 any other purpose
     * data is stored as uint8_t 中 AV帧SideData.data which is 16 bytes of
     * uuid_iso_iec_11578 followed by AV帧SideData.大小 - 16 bytes 的 user_data_payload_byte.
     */
    AV_FRAME_DATA_SEI_UNREGISTERED,

    /**
     * Film grain parameters 用于 a 帧, described by AVFilmGrainParams.
     * Must be present 用于 every 帧 which should have film grain applied.
     *
     * May be present multiple times, 用于 example 当 there are multiple
     * alternative parameter 设置s 用于 d如果ferent 视频 signal characteristics.
     * user should select the most appropriate 设置 用于 the application.
     */
    AV_FRAME_DATA_FILM_GRAIN_PARAMS,

    /**
     * Bounding boxes 用于 object detection 和 class如果ication,
     * as described by AVDetectionBBoxHeader.
     */
    AV_FRAME_DATA_DETECTION_BBOXES,

    /**
     * Dolby Vision RPU raw data, suitable 用于 passing 到 x265
     * 或 other libraries. 数组 的 uint8_t,，使用 NAL emulation
     * bytes intact.
     */
    AV_FRAME_DATA_DOVI_RPU_BUFFER,

    /**
     * 解析d Dolby Vision 元数据, suitable 用于 passing 到 a software
     * implementation. payload is the AVDOVI元数据 struct defined in
     * libavutil/dovi_meta.h.
     */
    AV_FRAME_DATA_DOVI_METADATA,

    /**
     * HDR Vivid dynamic 元数据 associated，使用 a 视频 帧. payload is
     * an AVDynamicHDRVivid type 和 包含 in格式ion 用于 color
     * volume transform - CUV005.1-2021.
     */
    AV_FRAME_DATA_DYNAMIC_HDR_VIVID,

    /**
     * Ambient viewing environment 元数据, as defined by H.274.
     */
    AV_FRAME_DATA_AMBIENT_VIEWING_ENVIRONMENT,

    /**
     * Provide 编码器-spec如果ic hinting in格式ion about changed/unchanged
     * portions 的 a 帧.  It 可用于 到 pass in格式ion about which
     * macroblocks can be skipped because they didn't change，来自 the
     * corresponding ones 中 the previous 帧. This could be useful for
     * applications which know this in格式ion 中 advance 到 speed up
     * 编码.
     */
    AV_FRAME_DATA_VIDEO_HINT,

    /**
     * Raw LCEVC payload data, as a uint8_t 数组,，使用 NAL emulation
     * bytes intact.
     */
    AV_FRAME_DATA_LCEVC,

    /**
     * This side data must be associated，使用 a 视频 帧.
     * presence 的 this side data indicates that the 视频 stream is
     * composed 的 multiple views (e.g. stereoscopic 3D content,
     * cf. H.264 Annex H 或 H.265 Annex G).
     * data is an int storing the view ID.
     */
    AV_FRAME_DATA_VIEW_ID,

    /**
     * This side data 包含 in格式ion about the 引用 display 宽度(s)
     * 和 引用 viewing distance(s) as well as in格式ion about the
     * corresponding 引用 stereo pair(s), i.e., the pair(s) 的 views 到 be
     * displayed 用于 the viewer's left 和 right eyes 上 the 引用 display
     * at the 引用 viewing distance.
     * payload is the AV3D引用DisplaysInfo struct defined in
     * libavutil/tdrdi.h.
     */
    AV_FRAME_DATA_3D_REFERENCE_DISPLAYS,

    /**
     * Exchangeable image file 格式 元数据. payload is a 缓冲区 containing
     * EX如果 元数据, starting，使用 either 49 49 2a 00, 或 4d 4d 00 2a. These four
     * bytes sign如果y the endianness, 和 occur as the first part 的 the T如果F header.
     */
    AV_FRAME_DATA_EXIF,

    /**
     * HDR dynamic 元数据 associated，使用 a 视频 帧. payload is
     * an AVDynamicHDRSmpte2094App5 type 和 包含 in格式ion 用于 color
     * volume transform as spec如果ied 中 the SMPTE 2094-50 standard.
     */
    AV_FRAME_DATA_DYNAMIC_HDR_SMPTE_2094_APP5,

    /**
     * IAMF Mix Gain Parameter Data associated，使用 the 音频 帧. This 元数据
     * is 中 the form 的 the AVIAMFParamDefinition struct 和 包含 in格式ion
     * defined 中 sections 3.6.1 和 3.8.1 的 the Immersive 音频 Model and
     * 格式s standard.
     */
    AV_FRAME_DATA_IAMF_MIX_GAIN_PARAM,

    /**
    * IAMF Demixing Info Parameter Data associated，使用 the 音频 帧. This
    * 元数据 is 中 the form 的 the AVIAMFParamDefinition struct 和 包含
    * in格式ion defined 中 sections 3.6.1 和 3.8.2 的 the Immersive 音频 Model
    * 和 格式s standard.
    */
    AV_FRAME_DATA_IAMF_DEMIXING_INFO_PARAM,

    /**
    * IAMF Recon Gain Info Parameter Data associated，使用 the 音频 帧. This
    * 元数据 is 中 the form 的 the AVIAMFParamDefinition struct 和 包含
    * in格式ion defined 中 sections 3.6.1 和 3.8.3 的 the Immersive 音频 Model
    * 和 格式s standard.
    */
    AV_FRAME_DATA_IAMF_RECON_GAIN_INFO_PARAM,

    /**
     * Color in格式ion，来自 a RAW camera codecs, needed 到 correctly process
     * the 视频 data. payload is an AVRawColorParams struct defined in
     * libavutil/raw_color_params.h.
     */
    AV_FRAME_DATA_RAW_COLOR_PARAMS,

    /**
     * 元数据 relevant 到 a downmix procedure 中 the form 的 a remixig matrix.
     * data is the AVDownmixMatrix struct defined 中 libavutil/downmix_info.h.
     */
    AV_FRAME_DATA_DOWNMIX_MATRIX,
};

enum AVActiveFormatDescription {
    AV_AFD_SAME         = 8,
    AV_AFD_4_3          = 9,
    AV_AFD_16_9         = 10,
    AV_AFD_14_9         = 11,
    AV_AFD_4_3_SP_14_9  = 13,
    AV_AFD_16_9_SP_14_9 = 14,
    AV_AFD_SP_4_3       = 15,
};


/**
 * 结构体 到 hold side data 用于 an AV帧.
 *
 * 大小of(AV帧SideData) is not a part 的 the 公共 ABI, so new fields may be added
 * 到 the end，使用 a minor bump.
 */
typedef struct AVFrameSideData {
    enum AVFrameSideDataType type;
    uint8_t *data;
    size_t   size;
    AVDictionary *metadata;
    AVBufferRef *buf;
} AVFrameSideData;

enum AVSideDataProps {
    /**
     * side data type 可用于 中 stream-global 结构体s.
     * Side data types without this property are only meaningful 上 per-帧
     * basis.
     */
    AV_SIDE_DATA_PROP_GLOBAL = (1 << 0),

    /**
     * Multiple instances 的 this side data type can be meaningfully present in
     * a single side data 数组.
     */
    AV_SIDE_DATA_PROP_MULTI  = (1 << 1),

    /**
     * Side data depends 上 the 视频 dimensions. Side data，使用 this property
     * loses its meaning 当 rescaling 或 cropping the image, unless
     * either recomputed 或 adjusted 到 the new resolution.
     */
    AV_SIDE_DATA_PROP_SIZE_DEPENDENT = (1 << 2),

    /**
     * Side data depends 上 the 视频 color space. Side data，使用 this property
     * loses its meaning 当 changing the 视频 color 编码, e.g. by
     * adapting 到 a d如果ferent 设置 的 primaries 或 transfer characteristics.
     */
    AV_SIDE_DATA_PROP_COLOR_DEPENDENT = (1 << 3),

    /**
     * Side data depends 上 the 声道布局. Side data，使用 this property
     * loses its meaning 当 downmixing 或 upmixing, unless either recomputed
     * 或 adjusted 到 the new layout.
     */
    AV_SIDE_DATA_PROP_CHANNEL_DEPENDENT = (1 << 4),
};

/**
 * This struct describes the properties 的 a side data type. Its instance
 * corresponding 到 a given type can be obtained，来自 av_帧_side_data_desc().
 */
typedef struct AVSideDataDescriptor {
    /**
     * Human-readable side data 描述.
     */
    const char      *name;

    /**
     * Side data property 标志, a combination 的 AVSideDataProps 值.
     */
    unsigned         props;
} AVSideDataDescriptor;

/**
 * 结构体 describing a single Region Of Interest.
 *
 * 当 multiple regions are defined 中 a single side-data block, they
 * should be ordered，来自 most 到 least important - some 编码器s are only
 * capable 的 supporting a limited 数量 的 distinct regions, so will have
 * 到 truncate the 列表.
 *
 * 当 overlapping regions are defined, the first region containing a given
 * area 的 the 帧 applies.
 */
typedef struct AVRegionOfInterest {
    /**
     * Must be 设置 到 the 大小 的 this data 结构体 (that is,
     * 大小of(AVRegionOfInterest)).
     */
    uint32_t self_size;
    /**
     * Distance 中 像素s，来自 the top edge 的 the 帧 到 the top and
     * bottom edges and，来自 the left edge 的 the 帧 到 the left and
     * right edges 的 the rectangle defining this region 的 interest.
     *
     * constraints 上 a region are 编码器 dependent, so the region
     * actually affected may be slightly larger 用于 alignment 或 other
     * reasons.
     */
    int top;
    int bottom;
    int left;
    int right;
    /**
     * Quantisation off设置.
     *
     * Must be 中 the range -1 到 +1.  值 的 zero indicates no quality
     * change.  negative 值 asks 用于 better quality (less quantisation),
     * while a positive 值 asks 用于 worse quality (greater quantisation).
     *
     * range is calibrated so that the extreme 值 indicate the
     * largest possible off设置 - 如果 the rest 的 the 帧 is encoded，使用 the
     * worst possible quality, an off设置 的 -1 indicates that this region
     * should be encoded，使用 the best possible quality anyway.  Intermediate
     * 值 are then interpolated 中 some codec-dependent way.
     *
     * For example, 中 10-bit H.264 the quantisation parameter varies between
     * -12 和 51.  typical qoff设置 值 的 -1/10 therefore indicates that
     * this region should be encoded，使用 a QP around one-tenth 的 the full
     * range better than the rest 的 the 帧.  So, 如果 most 的 the 帧
     * were 到 be encoded，使用 a QP 的 around 30, this region would get a QP
     * 的 around 24 (an off设置 的 approximately -1/10 * (51 - -12) = -6.3).
     * extreme 值 的 -1 would indicate that this region should be
     * encoded，使用 the best possible quality regardless 的 the treatment of
     * the rest 的 the 帧 - that is, should be encoded at a QP 的 -12.
     */
    AVRational qoffset;
} AVRegionOfInterest;

/**
 * 此结构体 describes decoded (raw) 音频 或 视频 data.
 *
 * AV帧 must be 分配d using av_帧_alloc(). Note that this only
 * 分配s the AV帧 itself, the 缓冲区 用于 the data must be managed
 * through other means (参见 below).
 * AV帧 must be 释放d，使用 av_帧_释放().
 *
 * AV帧 is typically 分配d once 和 then reused multiple times 到 hold
 * d如果ferent data (e.g. a single AV帧 到 hold 帧s received，来自 a
 * 解码器). In such a case, av_帧_unref() will 释放 any 引用s held by
 * the 帧 和 re设置 it 到 its original clean state before it
 * is reused again.
 *
 * data described by an AV帧 is usually 引用 counted through the
 * AV缓冲区 API. underlying 缓冲区 引用s are stored 中 AV帧.buf /
 * AV帧.extended_buf. AV帧 is considered 到 be 引用 counted 如果 at
 * least one 引用 is 设置, i.e. 如果 AV帧.buf[0] != NULL. In such a case,
 * every single data plane must be contained 中 one 的 the 缓冲区 in
 * AV帧.buf 或 AV帧.extended_buf.
 * There may be a single 缓冲区 用于 all the data, 或 one separate 缓冲区 for
 * each plane, 或 anything 中 between.
 *
 * 大小of(AV帧) is not a part 的 the 公共 ABI, so new fields may be added
 * 到 the end，使用 a minor bump.
 *
 * Fields can be accessed through AV选项, the name string used, matches the
 * C 结构体 field name 用于 fields accessible through AV选项.
 */
typedef struct AVFrame {
#define AV_NUM_DATA_POINTERS 8
    /**
     * 指针 到 the picture/声道 planes.
     * This might be d如果ferent，来自 the first 分配d byte. For 视频,
     * it could even point 到 the end 的 the image data.
     *
     * All 指针s 中 data 和 extended_data must point into one 的 the
     * AV缓冲区Ref 中 buf 或 extended_buf.
     *
     * Some 解码器s access areas outside 0,0 - 宽度,高度, please
     * 参见 avcodec_align_dimensions2(). Some filters 和 swscale can read
     * up 到 16 bytes beyond the planes, 如果 these filters are 到 be used,
     * then 16 extra bytes must be 分配d.
     *
     * NOTE: 指针s not needed by the 格式 MUST be 设置 到 NULL.
     *
     * @attention In case 的 视频, the data[] 指针s can point 到 the
     * end 的 image data 中 order 到 reverse line order, 当 used in
     * combination，使用 negative 值 中 the line大小[] 数组.
     */
    uint8_t *data[AV_NUM_DATA_POINTERS];

    /**
     * For 视频, a positive 或 negative 值, which is typically indicating
     * the 大小 中 bytes 的 each picture line, but it can also be:
     * - the negative byte 大小 的 lines 用于 vertical flipping
     *   (with data[n] pointing 到 the end 的 the data
     * - a positive 或 negative multiple 的 the byte 大小 as 用于 accessing
     *   even 和 odd fields 的 a 帧 (possibly flipped)
     *
     * For 音频, only line大小[0] may be 设置. For planar 音频, each 声道
     * plane must be the same 大小.
     *
     * For 视频 the line大小s should be multiples 的 the CPUs alignment
     * p引用, this is 16 或 32 用于 modern desktop CPUs.
     * Some code requires such alignment other code can be slower without
     * correct alignment, 用于 yet other it makes no d如果ference.
     *
     * @note line大小 may be larger than the 大小 的 usable data -- there
     * may be extra padding present 用于 performance reasons.
     *
     * @attention In case 的 视频, line 大小 值 can be negative 到 achieve
     * a vertically inverted iteration over image lines.
     */
    int linesize[AV_NUM_DATA_POINTERS];

    /**
     * 指针s 到 the data planes/声道数.
     *
     * For 视频, this should simply point 到 data[].
     *
     * For planar 音频, each 声道 has a separate data 指针, and
     * line大小[0] 包含 大小 的 each 声道 缓冲区.
     * For packed 音频, there is just one data 指针, 和 line大小[0]
     * 包含 total 大小 的 the 缓冲区 用于 all 声道数.
     *
     * 注意： Both data 和 extended_data should always be 设置 中 a valid 帧,
     * but 用于 planar 音频，使用 more 声道数 that can fit 中 data,
     * extended_data must be used 中 order 到 access all 声道数.
     */
    uint8_t **extended_data;

    /**
     * @name 视频 dimensions
     * 视频 帧s only. coded dimensions (in 像素s) 的 the 视频 帧,
     * i.e. the 大小 的 the rectangle that 包含 some well-defined 值.
     *
     * @note part 的 the 帧 intended 用于 display/presentation is further
     * restricted by the @ref cropping "Cropping rectangle".
     * @{
     */
    int width, height;
    /**
     * @}
     */

    /**
     * 数量 的 音频 采样s (per 声道) described by this 帧
     */
    int nb_samples;

    /**
     * 格式 的 the 帧, -1 如果 unknown 或 un设置
     * 值 correspond 到 enum AV像素格式 用于 视频 帧s,
     * enum AV采样格式 用于 音频)
     */
    int format;

    /**
     * Picture type 的 the 帧.
     */
    enum AVPictureType pict_type;

    /**
     * 采样 aspect ratio 用于 the 视频 帧, 0/1 如果 unknown/unspec如果ied.
     */
    AVRational sample_aspect_ratio;

    /**
     * Presentation 时间戳 中 time_base units (time 当 帧 should be shown 到 user).
     */
    int64_t pts;

    /**
     * DTS copied，来自 the AVPacket that triggered 返回ing this 帧. (如果 帧 threading isn't used)
     * This is also the Presentation time 的 this AV帧 计算d from
     * only AVPacket.dts 值 without pts 值.
     */
    int64_t pkt_dts;

    /**
     * 时间基 用于 the 时间戳s 中 this 帧.
     * In the future, this field may be 设置 上 帧s 输出 by 解码器s or
     * filters, but its 值 will be by 默认 ignored 上 输入 到 编码器s
     * 或 filters.
     */
    AVRational time_base;

    /**
     * quality (between 1 (good) 和 FF_LAMBDA_MAX (bad))
     */
    int quality;

    /**
     * 帧 owner's 私有 data.
     *
     * This field may be 设置 by the code that 分配s/owns the 帧 data.
     * It is then not touched by any library functions, except:
     * - it is copied 到 other 引用s by av_帧_复制_props() (and hence by
     *   av_帧_ref());
     * - it is 设置 到 NULL 当 the 帧 is cleared by av_帧_unref()
     * - 上 the caller's explicit request. E.g. libavcodec 编码器s/解码器s
     *   will 复制 this field to/from @ref AVPacket "AVPackets" 如果 the caller 设置s
     *   @ref AV_CODEC_标志_复制_OPAQUE.
     *
     * @参见 opaque_ref the 引用-counted analogue
     */
    void *opaque;

    /**
     * 数量 的 fields 中 this 帧 which should be repeated, i.e. the total
     * 持续时间 的 this 帧 should be repeat_pict + 2 normal field 持续时间s.
     *
     * For interlaced 帧s this field may be 设置 到 1, which signals that this
     * 帧 should be presented as 3 fields: beginning，使用 the first field (as
     * determined by AV_帧_标志_TOP_FIELD_FIRST being 设置 或 not), followed
     * by the second field, 和 then the first field again.
     *
     * For progressive 帧s this field may be 设置 到 a multiple 的 2, which
     * signals that this 帧's 持续时间 should be (repeat_pict + 2) / 2
     * normal 帧 持续时间s.
     *
     * @note This field is computed，来自 MPEG2 repeat_first_field 标志 和 its
     * associated 标志, H.264 pic_struct，来自 picture timing SEI, and
     * their analogues 中 other codecs. Typically it should only be used 当
     * higher-layer timing in格式ion is not available.
     */
    int repeat_pict;

    /**
     * 采样率 的 the 音频 data.
     */
    int sample_rate;

    /**
     * AV缓冲区 引用s backing the data 用于 this 帧. All the 指针s in
     * data 和 extended_data must point inside one 的 the 缓冲区 中 buf or
     * extended_buf. This 数组 must be filled contiguously -- 如果 buf[i] is
     * non-NULL then buf[j] must also be non-NULL 用于 all j < i.
     *
     * There may be at most one AV缓冲区 per data plane, so 用于 视频 this 数组
     * always 包含 all the 引用s. For planar 音频，使用 more than
     * AV_NUM_DATA_指针S 声道数, there may be more 缓冲区 than can fit in
     * this 数组. Then the extra AV缓冲区Ref 指针s are stored 中 the
     * extended_buf 数组.
     */
    AVBufferRef *buf[AV_NUM_DATA_POINTERS];

    /**
     * For planar 音频 which requires more than AV_NUM_DATA_指针S
     * AV缓冲区Ref 指针s, this 数组 will hold all the 引用s which
     * cannot fit into AV帧.buf.
     *
     * Note that this is d如果ferent，来自 AV帧.extended_data, which always
     * 包含 all the 指针s. This 数组 only 包含 extra 指针s,
     * which cannot fit into AV帧.buf.
     *
     * This 数组 is always 分配d using av_malloc() by whoever constructs
     * the 帧. It is 释放d 中 av_帧_unref().
     */
    AVBufferRef **extended_buf;
    /**
     * 数量 的 elements 中 extended_buf.
     */
    int        nb_extended_buf;

    AVFrameSideData **side_data;
    int            nb_side_data;

/**
 * @defgroup lavu_帧_标志 AV_帧_标志
 * @ingroup lavu_帧
 * 标志 describing additional 帧 properties.
 *
 * @{
 */

/**
 * 帧 data may be corrupted, e.g. due 到 解码 errors.
 */
#define AV_FRAME_FLAG_CORRUPT       (1 << 0)
/**
 * 标志 到 mark 帧s that are key帧s.
 */
#define AV_FRAME_FLAG_KEY (1 << 1)
/**
 * 标志 到 mark the 帧s which need 到 be decoded, but shouldn't be 输出.
 */
#define AV_FRAME_FLAG_DISCARD   (1 << 2)
/**
 * 标志 到 mark 帧s whose content is interlaced.
 */
#define AV_FRAME_FLAG_INTERLACED (1 << 3)
/**
 * 标志 到 mark 帧s where the top field is displayed first 如果 the content
 * is interlaced.
 */
#define AV_FRAME_FLAG_TOP_FIELD_FIRST (1 << 4)
/**
 * 解码器 can use this 标志 到 mark 帧s which were originally encoded losslessly.
 *
 * For coding bitstream 格式s which support both lossless 和 lossy
 * 编码, it is sometimes possible 用于 a 解码器 到 determine which method
 * was used 当 the bitstream was encoded.
 */
#define AV_FRAME_FLAG_LOSSLESS        (1 << 5)
/**
 * @}
 */

    /**
     * 帧 标志, a combination 的 @ref lavu_帧_标志
     */
    int flags;

    /**
     * MPEG vs JPEG YUV range.
     * - 编码: 设置 by user
     * - 解码: 设置 by libavcodec
     */
    enum AVColorRange color_range;

    enum AVColorPrimaries color_primaries;

    enum AVColorTransferCharacteristic color_trc;

    /**
     * YUV colorspace type.
     * - 编码: 设置 by user
     * - 解码: 设置 by libavcodec
     */
    enum AVColorSpace colorspace;

    enum AVChromaLocation chroma_location;

    /**
     * 帧 时间戳 estimated using various heuristics, 中 stream 时间基
     * - 编码: unused
     * - 解码: 设置 by libavcodec, read by user.
     */
    int64_t best_effort_timestamp;

    /**
     * 元数据.
     * - 编码: 设置 by user.
     * - 解码: 设置 by libavcodec.
     */
    AVDictionary *metadata;

    /**
     * decode error 标志 的 the 帧, 设置 到 a combination of
     * FF_DECODE_ERROR_xxx 标志 如果 the 解码器 produced a 帧, but there
     * were errors during the 解码.
     * - 编码: unused
     * - 解码: 设置 by libavcodec, read by user.
     */
    int decode_error_flags;
#define FF_DECODE_ERROR_INVALID_BITSTREAM   1
#define FF_DECODE_ERROR_MISSING_REFERENCE   2
#define FF_DECODE_ERROR_CONCEALMENT_ACTIVE  4
#define FF_DECODE_ERROR_DECODE_SLICES       8

    /**
     * For hwaccel-格式 帧s, this should be a 引用 到 the
     * AVHW帧s上下文 describing the 帧.
     */
    AVBufferRef *hw_frames_ctx;

    /**
     * 帧 owner's 私有 data.
     *
     * This field may be 设置 by the code that 分配s/owns the 帧 data.
     * It is then not touched by any library functions, except:
     * - a new 引用 到 the underlying 缓冲区 is propagated by
     *   av_帧_复制_props() (and hence by av_帧_ref());
     * - it is un引用d 中 av_帧_unref();
     * - 上 the caller's explicit request. E.g. libavcodec 编码器s/解码器s
     *   will propagate a new 引用 to/from @ref AVPacket "AVPackets" 如果 the
     *   caller 设置s @ref AV_CODEC_标志_复制_OPAQUE.
     *
     * @参见 opaque the plain 指针 analogue
     */
    AVBufferRef *opaque_ref;

    /**
     * @anchor cropping
     * @name Cropping
     * 视频 帧s only. 数量 的 像素s 到 discard，来自 the
     * top/bottom/left/right border 的 the 帧 到 obtain the sub-rectangle of
     * the 帧 intended 用于 presentation.
     * @{
     */
    size_t crop_top;
    size_t crop_bottom;
    size_t crop_left;
    size_t crop_right;
    /**
     * @}
     */

    /**
     * RefStruct 引用 用于 internal use by a single libav* library.
     * Must not be 用于 transfer data between libraries.
     * Has 到 be NULL 当 ownership 的 the 帧 leaves the respective library.
     *
     * Code outside the FFmpeg libs must never 检查 或 change 私有_ref.
     */
    void *private_ref;

    /**
     * 声道布局 的 the 音频 data.
     */
    AVChannelLayout ch_layout;

    /**
     * 持续时间 的 the 帧, 中 the same units as pts. 0 如果 unknown.
     */
    int64_t duration;

    /**
     * Indicates how the alpha 声道 的 the 视频 is 到 be handled.
     * - 编码: 设置 by user
     * - 解码: 设置 by libavcodec
     */
    enum AVAlphaMode alpha_mode;
} AVFrame;


/**
 * 分配 an AV帧 和 设置 its fields 到 默认 值.  resulting
 * struct must be 释放d using av_帧_释放().
 *
 * @返回 AV帧 filled，使用 默认 值 或 NULL 上 failure.
 *
 * @note this only 分配s the AV帧 itself, not the data 缓冲区. Those
 * must be 分配d through other means, e.g.，使用 av_帧_get_缓冲区() or
 * manually.
 */
AVFrame *av_frame_alloc(void);

/**
 * 释放 the 帧 和 any dynamically 分配d objects 中 it,
 * e.g. extended_data. 如果 the 帧 is 引用 counted, it will be
 * un引用d first.
 *
 * @param 帧 帧 到 be 释放d. 指针 will be 设置 到 NULL.
 */
void av_frame_free(AVFrame **frame);

/**
 * 设置 up a new 引用 到 the data described by the source 帧.
 *
 * 复制 帧 properties，来自 src 到 dst 和 创建 a new 引用 用于 each
 * AV缓冲区Ref，来自 src.
 *
 * 如果 src is not 引用 counted, new 缓冲区 are 分配d 和 the data is
 * copied.
 *
 * @warning: dst MUST have been either un引用d，使用 av_帧_unref(dst),
 *           或 newly 分配d，使用 av_帧_alloc() before calling this
 *           function, 或 undefined behavior will occur.
 *
 * @返回 0 上 success, a negative AVERROR 上 error
 */
int av_frame_ref(AVFrame *dst, const AVFrame *src);

/**
 * Ensure the destination 帧 refers 到 the same data described by the source
 * 帧, either by creating a new 引用 用于 each AV缓冲区Ref，来自 src 如果
 * they d如果fer，来自 those 中 dst, by allocating new 缓冲区 和 复制ing data 如果
 * src is not 引用 counted, 或 by unreferencing it 如果 src is empty.
 *
 * 帧 properties 上 dst will be replaced by those，来自 src.
 *
 * @返回 0 上 success, a negative AVERROR 上 error. On error, dst is
 *         un引用d.
 */
int av_frame_replace(AVFrame *dst, const AVFrame *src);

/**
 * 创建 a new 帧 that 引用s the same data as src.
 *
 * This is a shortcut 用于 av_帧_alloc()+av_帧_ref().
 *
 * @返回 newly 创建d AV帧 上 success, NULL 上 error.
 */
AVFrame *av_frame_clone(const AVFrame *src);

/**
 * Un引用 all the 缓冲区 引用d by 帧 和 re设置 the 帧 fields.
 */
void av_frame_unref(AVFrame *frame);

/**
 * Move everything contained 中 src 到 dst 和 re设置 src.
 *
 * @warning: dst is not un引用d, but directly overwritten without reading
 *           或 deallocating its contents. Call av_帧_unref(dst) manually
 *           before calling this function 到 ensure that no 内存 is leaked.
 */
void av_frame_move_ref(AVFrame *dst, AVFrame *src);

/**
 * 分配 new 缓冲区(s) 用于 音频 或 视频 data.
 *
 * following fields must be 设置 上 帧 before calling this function:
 * - 格式 (像素格式 用于 视频, 采样格式 用于 音频)
 * - 宽度 和 高度 用于 视频
 * - nb_采样s 和 ch_layout 用于 音频
 *
 * 此函数 will fill AV帧.data 和 AV帧.buf 数组s and, 如果
 * necessary, 分配 和 fill AV帧.extended_data 和 AV帧.extended_buf.
 * For planar 格式s, one 缓冲区 will be 分配d 用于 each plane.
 *
 * @warning: 如果 帧 already has been 分配d, calling this function will
 *           leak 内存. In addition, undefined behavior can occur 中 certain
 *           cases.
 *
 * @param 帧 帧 中 which 到 store the new 缓冲区.
 * @param align Required 缓冲区 大小 和 data 指针 alignment. 如果 equal 到 0,
 *              alignment will be chosen automatically 用于 the current CPU.
 *              It is highly recommended 到 pass 0 here unless you know what
 *              you are doing.
 *
 * @返回 0 上 success, a negative AVERROR 上 error.
 */
int av_frame_get_buffer(AVFrame *frame, int align);

/**
 * 检查 如果 the 帧 data is writable.
 *
 * @返回 positive 值 如果 the 帧 data is writable (which is true 如果 and
 * only 如果 each 的 the underlying 缓冲区 has only one 引用, namely the one
 * stored 中 this 帧). 返回 0 otherwise.
 *
 * 如果 1 is 返回ed the answer is valid until av_缓冲区_ref() is called 上 any
 * 的 the underlying AV缓冲区Refs (e.g. through av_帧_ref() 或 directly).
 *
 * @参见 av_帧_make_writable(), av_缓冲区_is_writable()
 */
int av_frame_is_writable(AVFrame *frame);

/**
 * Ensure that the 帧 data is writable, avoiding data 复制 如果 possible.
 *
 * Do nothing 如果 the 帧 is writable, 分配 new 缓冲区 和 复制 the data
 * 如果 it is not. Non-refcounted 帧s behave as non-writable, i.e. a 复制
 * is always made.
 *
 * @返回 0 上 success, a negative AVERROR 上 error.
 *
 * @参见 av_帧_is_writable(), av_缓冲区_is_writable(),
 * av_缓冲区_make_writable()
 */
int av_frame_make_writable(AVFrame *frame);

/**
 * 复制 the 帧 data，来自 src 到 dst.
 *
 * 此函数 does not 分配 anything, dst must be already 初始化d and
 * 分配d，使用 the same parameters as src.
 *
 * 此函数 only copies the 帧 data (i.e. the contents 的 the data /
 * extended data 数组s), not any other properties.
 *
 * @返回 >= 0 上 success, a negative AVERROR 上 error.
 */
int av_frame_copy(AVFrame *dst, const AVFrame *src);

/**
 * 复制 only "元数据" fields，来自 src 到 dst.
 *
 * 元数据 用于 the purpose 的 this function are those fields that do not affect
 * the data layout 中 the 缓冲区.  E.g. pts, 采样率 (for 音频) 或 采样
 * aspect ratio (for 视频), but not 宽度/高度 或 声道布局.
 * Side data is also copied.
 */
int av_frame_copy_props(AVFrame *dst, const AVFrame *src);

/**
 * 获取 the 缓冲区 引用 a given data plane is stored in.
 *
 * @param 帧 the 帧 到 get the plane's 缓冲区 from
 * @param plane index 的 the data plane 的 interest 中 帧->extended_data.
 *
 * @返回 the 缓冲区 引用 that 包含 plane 或 NULL 如果 the 输入
 * 帧 is not valid.
 */
AVBufferRef *av_frame_get_plane_buffer(const AVFrame *frame, int plane);

/**
 * Add a new side data 到 a 帧.
 *
 * @param 帧 a 帧 到 which the side data should be added
 * @param type type 的 the added side data
 * @param 大小 大小 的 the side data
 *
 * @返回 newly added side data 上 success, NULL 上 error
 */
AVFrameSideData *av_frame_new_side_data(AVFrame *frame,
                                        enum AVFrameSideDataType type,
                                        size_t size);

/**
 * Add a new side data 到 a 帧，来自 an existing AV缓冲区Ref
 *
 * @param 帧 a 帧 到 which the side data should be added
 * @param type  the type 的 the added side data
 * @param buf   an AV缓冲区Ref 到 add as side data. ownership of
 *              the 引用 is transferred 到 the 帧.
 *
 * @返回 newly added side data 上 success, NULL 上 error. On failure
 *         the 帧 is unchanged 和 the AV缓冲区Ref remains owned by
 *         the caller.
 */
AVFrameSideData *av_frame_new_side_data_from_buf(AVFrame *frame,
                                                 enum AVFrameSideDataType type,
                                                 AVBufferRef *buf);

/**
 * @返回 a 指针 到 the side data 的 a given type 上 success, NULL 如果 there
 * is no side data，使用 such type 中 this 帧.
 */
AVFrameSideData *av_frame_get_side_data(const AVFrame *frame,
                                        enum AVFrameSideDataType type);

/**
 * Remove 和 释放 all side data instances 的 the given type.
 */
void av_frame_remove_side_data(AVFrame *frame, enum AVFrameSideDataType type);


/**
 * 标志 用于 帧 cropping.
 */
enum {
    /**
     * Apply the maximum possible cropping, even 如果 it requires 设置ting the
     * AV帧.data[] entries 到 unaligned 指针s. Passing unaligned data
     * 到 FFmpeg API is generally not allowed, 和 causes undefined behavior
     * (such as crashes). You can pass unaligned data only 到 FFmpeg APIs that
     * are explicitly documented 到 accept it. Use this 标志 only 如果 you
     * absolutely know what you are doing.
     */
    AV_FRAME_CROP_UNALIGNED     = 1 << 0,
};

/**
 * Crop the given 视频 AV帧 according 到 its crop_left/crop_top/crop_right/
 * crop_bottom fields. 如果 cropping is successful, the function will adjust the
 * data 指针s 和 the 宽度/高度 fields, 和 设置 the crop fields 到 0.
 *
 * In all cases, the cropping boundaries will be rounded 到 the inherent
 * alignment 的 the 像素格式. In some cases, such as 用于 opaque hwaccel
 * 格式s, the left/top cropping is ignored. crop fields are 设置 到 0 even
 * 如果 the cropping was rounded 或 ignored.
 *
 * @param 帧 the 帧 which should be cropped
 * @param 标志 Some combination 的 AV_帧_CROP_* 标志, 或 0.
 *
 * @返回 >= 0 上 success, a negative AVERROR 上 error. 如果 the cropping fields
 * were invalid, AVERROR(ERANGE) is 返回ed, 和 nothing is changed.
 */
int av_frame_apply_cropping(AVFrame *frame, int flags);

/**
 * @返回 a string ident如果ying the side data type
 */
const char *av_frame_side_data_name(enum AVFrameSideDataType type);

/**
 * @返回 side data 描述符 corresponding 到 a given side data type, NULL
 *         当 not available.
 */
const AVSideDataDescriptor *av_frame_side_data_desc(enum AVFrameSideDataType type);

/**
 * 释放 all side data entries 和 their contents, then zeroes out the
 * 值 which the 指针s are pointing to.
 *
 * @param sd    指针 到 数组 的 side data 到 释放. Will be 设置 到 NULL
 *              upon 返回.
 * @param nb_sd 指针 到 an integer containing the 数量 的 entries in
 *              the 数组. Will be 设置 到 0 upon 返回.
 */
void av_frame_side_data_free(AVFrameSideData ***sd, int *nb_sd);

/**
 * Remove existing entries before adding new ones.
 */
#define AV_FRAME_SIDE_DATA_FLAG_UNIQUE (1 << 0)
/**
 * Don't add a new entry 如果 another 的 the same type exists.
 * Applies only 用于 side data types without the AV_SIDE_DATA_PROP_MULTI prop.
 */
#define AV_FRAME_SIDE_DATA_FLAG_REPLACE (1 << 1)
/**
 * 创建 a new 引用 到 the passed 中 缓冲区 instead 的 taking ownership
 * 的 it.
 */
#define AV_FRAME_SIDE_DATA_FLAG_NEW_REF (1 << 2)

/**
 * Add new side data entry 到 an 数组.
 *
 * @param sd    指针 到 数组 的 side data 到 which 到 add another entry,
 *              或 到 NULL 中 order 到 start a new 数组.
 * @param nb_sd 指针 到 an integer containing the 数量 的 entries in
 *              the 数组.
 * @param type  type 的 the added side data
 * @param 大小  大小 的 the side data
 * @param 标志 Some combination 的 AV_帧_SIDE_DATA_标志_* 标志, 或 0.
 *
 * @返回 newly added side data 上 success, NULL 上 error.
 * @note In case 的 AV_帧_SIDE_DATA_标志_UNIQUE being 设置, entries of
 *       matching AV帧SideDataType will be removed before the addition
 *       is attempted.
 * @note In case 的 AV_帧_SIDE_DATA_标志_REPLACE being 设置, 如果 an
 *       entry 的 the same type already exists, it will be replaced instead.
 */
AVFrameSideData *av_frame_side_data_new(AVFrameSideData ***sd, int *nb_sd,
                                        enum AVFrameSideDataType type,
                                        size_t size, unsigned int flags);

/**
 * Add a new side data entry 到 an 数组，来自 an existing AV缓冲区Ref.
 *
 * @param sd    指针 到 数组 的 side data 到 which 到 add another entry,
 *              或 到 NULL 中 order 到 start a new 数组.
 * @param nb_sd 指针 到 an integer containing the 数量 的 entries in
 *              the 数组.
 * @param type  type 的 the added side data
 * @param buf   指针 到 AV缓冲区Ref 到 add 到 the 数组. On success,
 *              the function takes ownership 的 the AV缓冲区Ref 和 *buf is
 *              设置 到 NULL, unless AV_帧_SIDE_DATA_标志_NEW_REF is 设置
 *              中 which case the ownership will remain，使用 the caller.
 * @param 标志 Some combination 的 AV_帧_SIDE_DATA_标志_* 标志, 或 0.
 *
 * @返回 newly added side data 上 success, NULL 上 error.
 * @note In case 的 AV_帧_SIDE_DATA_标志_UNIQUE being 设置, entries of
 *       matching AV帧SideDataType will be removed before the addition
 *       is attempted.
 * @note In case 的 AV_帧_SIDE_DATA_标志_REPLACE being 设置, 如果 an
 *       entry 的 the same type already exists, it will be replaced instead.
 *
 */
AVFrameSideData *av_frame_side_data_add(AVFrameSideData ***sd, int *nb_sd,
                                        enum AVFrameSideDataType type,
                                        AVBufferRef **buf, unsigned int flags);

/**
 * Add a new side data entry 到 an 数组 based 上 existing side data, taking
 * a 引用 towards the contained AV缓冲区Ref.
 *
 * @param sd    指针 到 数组 的 side data 到 which 到 add another entry,
 *              或 到 NULL 中 order 到 start a new 数组.
 * @param nb_sd 指针 到 an integer containing the 数量 的 entries in
 *              the 数组.
 * @param src   side data 到 be cloned,，使用 a new 引用 utilized
 *              用于 the 缓冲区.
 * @param 标志 Some combination 的 AV_帧_SIDE_DATA_标志_* 标志, 或 0.
 *
 * @返回 negative error code 上 failure, >=0 上 success.
 * @note In case 的 AV_帧_SIDE_DATA_标志_UNIQUE being 设置, entries of
 *       matching AV帧SideDataType will be removed before the addition
 *       is attempted.
 * @note In case 的 AV_帧_SIDE_DATA_标志_REPLACE being 设置, 如果 an
 *       entry 的 the same type already exists, it will be replaced instead.
 */
int av_frame_side_data_clone(AVFrameSideData ***sd, int *nb_sd,
                             const AVFrameSideData *src, unsigned int flags);

/**
 * 获取 a side data entry 的 a spec如果ic type，来自 an 数组.
 *
 * @param sd    数组 的 side data.
 * @param nb_sd integer containing the 数量 的 entries 中 the 数组.
 * @param type  type 的 side data 到 be queried
 *
 * @返回 a 指针 到 the side data 的 a given type 上 success, NULL 如果 there
 *         is no side data，使用 such type 中 this 设置.
 */
const AVFrameSideData *av_frame_side_data_get_c(const AVFrameSideData * const *sd,
                                                const int nb_sd,
                                                enum AVFrameSideDataType type);

/**
 * Wrapper around av_帧_side_data_get_c() 到 workaround the limitation
 * that 用于 any type T the conversion，来自 T * const * 到 const T * const *
 * is not performed automatically 中 C.
 * @参见 av_帧_side_data_get_c()
 */
static inline
const AVFrameSideData *av_frame_side_data_get(AVFrameSideData * const *sd,
                                              const int nb_sd,
                                              enum AVFrameSideDataType type)
{
    return av_frame_side_data_get_c((const AVFrameSideData * const *)sd,
                                    nb_sd, type);
}

/**
 * Remove 和 释放 all side data instances 的 the given type，来自 an 数组.
 */
void av_frame_side_data_remove(AVFrameSideData ***sd, int *nb_sd,
                               enum AVFrameSideDataType type);

/**
 * Remove 和 释放 all side data instances that match any 的 the given
 * side data properties. (参见 enum AVSideDataProps)
 */
void av_frame_side_data_remove_by_props(AVFrameSideData ***sd, int *nb_sd,
                                        int props);

/**
 * @}
 */

#endif /* AVUTIL_FRAME_H */
