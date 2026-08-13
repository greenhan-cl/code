/*
 * Immersive 音频 Model 和 格式s helper functions 和 defines
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

#ifndef AVUTIL_IAMF_H
#define AVUTIL_IAMF_H

/**
 * @file
 * Immersive 音频 Model 和 格式s API header
 * @参见 <a href="https://aomediacodec.github.io/iamf/">Immersive 音频 Model 和 格式s</a>
 */

#include <stdint.h>
#include <stddef.h>

#include "attributes.h"
#include "avassert.h"
#include "channel_layout.h"
#include "dict.h"
#include "rational.h"

/**
 * @defgroup lavu_iamf Immersive 音频 Model 和 格式s
 * @ingroup lavu_音频
 *
 * Immersive 音频 Model 和 格式s related functions 和 defines
 *
 * @defgroup lavu_iamf_params Parameter Definition
 * @ingroup lavu_iamf
 * @{
 * Parameters as defined 中 section 3.6.1 和 3.8 的 IAMF.
 * @}
 *
 * @defgroup lavu_iamf_音频 音频 Element
 * @ingroup lavu_iamf
 * @{
 * 音频 Elements as defined 中 section 3.6 的 IAMF.
 * @}
 *
 * @defgroup lavu_iamf_mix Mix Presentation
 * @ingroup lavu_iamf
 * @{
 * Mix Presentations as defined 中 section 3.7 的 IAMF.
 * @}
 *
 * @addtogroup lavu_iamf_params
 * @{
 */
enum AVIAMFAnimationType {
    AV_IAMF_ANIMATION_TYPE_STEP,
    AV_IAMF_ANIMATION_TYPE_LINEAR,
    AV_IAMF_ANIMATION_TYPE_BEZIER,
};

/**
 * Mix Gain Parameter Data as defined 中 section 3.8.1 的 IAMF.
 *
 * @note This struct's 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVIAMFMixGain {
    const AVClass *av_class;

    /**
     * 持续时间 用于 the given subblock, 中 units of
     * 1 / @ref AVIAMFParamDefinition.parameter_rate "parameter_rate".
     * It must not be 0.
     */
    unsigned int subblock_duration;
    /**
     * type 的 animation applied 到 the parameter 值.
     */
    enum AVIAMFAnimationType animation_type;
    /**
     * Parameter 值 that is applied at the start 的 the subblock.
     * Applies 到 all defined Animation Types.
     *
     * Valid range 的 值 is -128.0 到 128.0
     */
    AVRational start_point_value;
    /**
     * Parameter 值 that is applied at the end 的 the subblock.
     * Applies only 到 AV_IAMF_ANIMATION_TYPE_LINEAR and
     * AV_IAMF_ANIMATION_TYPE_BEZIER Animation Types.
     *
     * Valid range 的 值 is -128.0 到 128.0
     */
    AVRational end_point_value;
    /**
     * Parameter 值 的 the middle control point 的 a quadratic Bezier
     * curve, i.e., its y-axis 值.
     * Applies only 到 AV_IAMF_ANIMATION_TYPE_BEZIER Animation Type.
     *
     * Valid range 的 值 is -128.0 到 128.0
     */
    AVRational control_point_value;
    /**
     * Parameter 值 的 the time 的 the middle control point 的 a
     * quadratic Bezier curve, i.e., its x-axis 值.
     * Applies only 到 AV_IAMF_ANIMATION_TYPE_BEZIER Animation Type.
     *
     * Valid range 的 值 is 0.0 到 1.0
     */
    AVRational control_point_relative_time;
} AVIAMFMixGain;

/**
 * Demixing Info Parameter Data as defined 中 section 3.8.2 的 IAMF.
 *
 * @note This struct's 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVIAMFDemixingInfo {
    const AVClass *av_class;

    /**
     * 持续时间 用于 the given subblock, 中 units of
     * 1 / @ref AVIAMFParamDefinition.parameter_rate "parameter_rate".
     * It must not be 0.
     */
    unsigned int subblock_duration;
    /**
     * Pre-defined combination 的 demixing parameters.
     */
    unsigned int dmixp_mode;
} AVIAMFDemixingInfo;

/**
 * Recon Gain Info Parameter Data as defined 中 section 3.8.3 的 IAMF.
 *
 * @note This struct's 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVIAMFReconGain {
    const AVClass *av_class;

    /**
     * 持续时间 用于 the given subblock, 中 units of
     * 1 / @ref AVIAMFParamDefinition.parameter_rate "parameter_rate".
     * It must not be 0.
     */
    unsigned int subblock_duration;

    /**
     * 数组 的 gain 值 到 be applied 到 each 声道 用于 each layer
     * defined 中 the 音频 Element referencing the parent Parameter Definition.
     * 值 用于 layers where the AV_IAMF_LAYER_标志_RECON_GAIN 标志 is not 设置
     * are undefined.
     *
     * 声道 order is: FL, C, FR, SL, SR, TFL, TFR, BL, BR, TBL, TBR, LFE
     */
    uint8_t recon_gain[6][12];
} AVIAMFReconGain;

enum AVIAMFParamDefinitionType {
   /**
    * Subblocks are 的 struct type AVIAMFMixGain
    */
    AV_IAMF_PARAMETER_DEFINITION_MIX_GAIN,
   /**
    * Subblocks are 的 struct type AVIAMFDemixingInfo
    */
    AV_IAMF_PARAMETER_DEFINITION_DEMIXING,
   /**
    * Subblocks are 的 struct type AVIAMFReconGain
    */
    AV_IAMF_PARAMETER_DEFINITION_RECON_GAIN,
};

/**
 * Parameters as defined 中 section 3.6.1 的 IAMF.
 *
 * struct is 分配d by av_iamf_param_definition_alloc() along，使用 an
 * 数组 的 subblocks, its type depending 上 the 值 的 type.
 * This 数组 is placed subblocks_off设置 bytes after the start 的 this struct.
 *
 * @note This struct's 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVIAMFParamDefinition {
    const AVClass *av_class;

    /**
     * Off设置 中 bytes，来自 the start 的 this struct, at which the subblocks
     * 数组 is located.
     */
    size_t subblocks_offset;
    /**
     * 大小 中 bytes 的 each element 中 the subblocks 数组.
     */
    size_t subblock_size;
    /**
     * 数量 的 subblocks 中 the 数组.
     */
    unsigned int nb_subblocks;

    /**
     * Parameters type. Determines the type 的 the subblock elements.
     */
    enum AVIAMFParamDefinitionType type;

    /**
     * Ident如果ier 用于 the parameter substream.
     */
    unsigned int parameter_id;
    /**
     * 采样率 用于 the parameter substream. It must not be 0.
     */
    unsigned int parameter_rate;

    /**
     * accumulated 持续时间 的 all blocks 中 this parameter definition,
     * 中 units 的 1 / @ref parameter_rate.
     *
     * May be 0, 中 which case all 持续时间 值 should be spec如果ied in
     * another parameter definition referencing the same parameter_id.
     */
    unsigned int duration;
    /**
     * 持续时间 的 every subblock 中 the case where all subblocks, with
     * the 可选 exception 的 the last subblock, have equal 持续时间s.
     *
     * Must be 0 如果 subblocks have d如果ferent 持续时间s.
     */
    unsigned int constant_subblock_duration;
} AVIAMFParamDefinition;

const AVClass *av_iamf_param_definition_get_class(void);

/**
 * 分配s 内存 用于 AVIAMFParamDefinition, plus an 数组 的 {@code nb_subblocks}
 * amount 的 subblocks 的 the given type 和 初始化s the variables. Can be
 * 释放d，使用 a normal av_释放() call.
 *
 * @param 大小 如果 non-NULL, the 大小 中 bytes 的 the resulting data 数组 is written here.
 */
AVIAMFParamDefinition *av_iamf_param_definition_alloc(enum AVIAMFParamDefinitionType type,
                                                      unsigned int nb_subblocks, size_t *size);

/**
 * 获取 the subblock at the spec如果ied {@code idx}. Must be between 0 和 nb_subblocks - 1.
 *
 * @ref AVIAMFParamDefinition.type "param definition type" defines
 * the struct type 的 the 返回ed 指针.
 */
static av_always_inline void*
av_iamf_param_definition_get_subblock(const AVIAMFParamDefinition *par, unsigned int idx)
{
    av_assert0(idx < par->nb_subblocks);
    return (void *)((uint8_t *)par + par->subblocks_offset + idx * par->subblock_size);
}

/**
 * @}
 * @addtogroup lavu_iamf_音频
 * @{
 */

enum AVIAMFAmbisonicsMode {
    AV_IAMF_AMBISONICS_MODE_MONO,
    AV_IAMF_AMBISONICS_MODE_PROJECTION,
};

/**
 * Recon gain in格式ion 用于 the layer is present 中 AVIAMFReconGain
 */
#define AV_IAMF_LAYER_FLAG_RECON_GAIN (1 << 0)

/**
 * layer defining a 声道布局 中 the 音频 Element.
 *
 * 当 @ref AVIAMF音频Element.音频_element_type "the parent's 音频 Element type"
 * is AV_IAMF_音频_ELEMENT_TYPE_声道, this corresponds 到 an Scalable 声道
 * Layout layer as defined 中 section 3.6.2 的 IAMF.
 * For AV_IAMF_音频_ELEMENT_TYPE_SCENE, it is an Ambisonics 声道
 * layout as defined 中 section 3.6.3 的 IAMF.
 *
 * @note struct should be 分配d，使用 av_iamf_音频_element_add_layer()
 *       和 its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVIAMFLayer {
    const AVClass *av_class;

    AVChannelLayout ch_layout;

    /**
     * bitmask which may contain a combination 的 AV_IAMF_LAYER_标志_* 标志.
     */
    unsigned int flags;
    /**
     * 输出 gain 声道 标志 as defined 中 section 3.6.2 的 IAMF.
     *
     * This field is defined only 如果 @ref AVIAMF音频Element.音频_element_type
     * "the parent's 音频 Element type" is AV_IAMF_音频_ELEMENT_TYPE_声道,
     * must be 0 otherwise.
     */
    unsigned int output_gain_flags;
    /**
     * 输出 gain as defined 中 section 3.6.2 的 IAMF.
     *
     * Must be 0 如果 @ref 输出_gain_标志 is 0.
     */
    AVRational output_gain;
    /**
     * Ambisonics mode as defined 中 section 3.6.3 的 IAMF.
     *
     * This field is defined only 如果 @ref AVIAMF音频Element.音频_element_type
     * "the parent's 音频 Element type" is AV_IAMF_音频_ELEMENT_TYPE_SCENE.
     *
     * 如果 AV_IAMF_AMBISONICS_MODE_MONO, 声道_mapping is defined implicitly
     * (Ambisonic Order) 或 explicitly (Custom Order，使用 ambi 声道数) in
     * @ref ch_layout.
     * 如果 AV_IAMF_AMBISONICS_MODE_PROJECTION, @ref demixing_matrix must be 设置.
     */
    enum AVIAMFAmbisonicsMode ambisonics_mode;

    /**
     * Demixing matrix as defined 中 section 3.6.3 的 IAMF.
     *
     * May be 设置 only 如果 @ref ambisonics_mode == AV_IAMF_AMBISONICS_MODE_PROJECTION,
     * must be NULL otherwise.
     */
    AVRational *demixing_matrix;

    /**
     * length 的 the Demixing matrix 数组. Must be ch_layout.nb_声道数 multiplied
     * by the sum 的 the amount 的 streams 中 the group plus the amount 的 streams in
     * the group that are stereo.
     */
    unsigned int nb_demixing_matrix;
} AVIAMFLayer;


enum AVIAMFAudioElementType {
    AV_IAMF_AUDIO_ELEMENT_TYPE_CHANNEL,
    AV_IAMF_AUDIO_ELEMENT_TYPE_SCENE,
};

/**
 * In格式ion 上 how 到 combine one 或 more 音频 streams, as defined in
 * section 3.6 的 IAMF.
 *
 * @note struct should be 分配d，使用 av_iamf_音频_element_alloc()
 *       和 its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVIAMFAudioElement {
    const AVClass *av_class;

    AVIAMFLayer **layers;
    /**
     * 数量 的 layers, 或 声道 groups, 中 the 音频 Element.
     * There may be 6 layers at most, 和 用于 @ref 音频_element_type
     * AV_IAMF_音频_ELEMENT_TYPE_SCENE, there may be exactly 1.
     *
     * 设置 by av_iamf_音频_element_add_layer(), must not be
     * mod如果ied by any other code.
     */
    unsigned int nb_layers;

    /**
     * Demixing in格式ion 用于 reconstruct a scalable 声道 音频
     * representation.
     * @ref AVIAMFParamDefinition.type "type" must be
     * AV_IAMF_PARAMETER_DEFINITION_DEMIXING.
     */
    AVIAMFParamDefinition *demixing_info;
    /**
     * Recon gain in格式ion 用于 reconstruct a scalable 声道 音频
     * representation.
     * @ref AVIAMFParamDefinition.type "type" must be
     * AV_IAMF_PARAMETER_DEFINITION_RECON_GAIN.
     */
    AVIAMFParamDefinition *recon_gain_info;

    /**
     * 音频 element type as defined 中 section 3.6 的 IAMF.
     */
    enum AVIAMFAudioElementType audio_element_type;

    /**
     * 默认 weight 值 as defined 中 section 3.6 的 IAMF.
     */
    unsigned int default_w;
} AVIAMFAudioElement;

const AVClass *av_iamf_audio_element_get_class(void);

/**
 * 分配s a AVIAMF音频Element, 和 初始化s its fields，使用 默认 值.
 * No layers are 分配d. Must be 释放d，使用 av_iamf_音频_element_释放().
 *
 * @参见 av_iamf_音频_element_add_layer()
 */
AVIAMFAudioElement *av_iamf_audio_element_alloc(void);

/**
 * 分配 a layer 和 add it 到 a given AVIAMF音频Element.
 * It is 释放d by av_iamf_音频_element_释放() alongside the rest 的 the parent
 * AVIAMF音频Element.
 *
 * @返回 a 指针 到 the 分配d layer.
 */
AVIAMFLayer *av_iamf_audio_element_add_layer(AVIAMFAudioElement *audio_element);

/**
 * 释放 an AVIAMF音频Element 和 all its contents.
 *
 * @param 音频_element 指针 到 指针 到 an 分配d AVIAMF音频Element.
 *                      upon 返回, *音频_element will be 设置 到 NULL.
 */
void av_iamf_audio_element_free(AVIAMFAudioElement **audio_element);

/**
 * @}
 * @addtogroup lavu_iamf_mix
 * @{
 */

enum AVIAMFHeadphonesMode {
    /**
     * 引用d 音频 Element shall be rendered 到 stereo loudspeakers.
     */
    AV_IAMF_HEADPHONES_MODE_STEREO,
    /**
     * 引用d 音频 Element shall be rendered，使用 a binaural renderer.
     */
    AV_IAMF_HEADPHONES_MODE_BINAURAL,
};

/**
 * Submix element as defined 中 section 3.7 的 IAMF.
 *
 * @note struct should be 分配d，使用 av_iamf_submix_add_element()
 *       和 its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVIAMFSubmixElement {
    const AVClass *av_class;

    /**
     * id 的 the 音频 Element this submix element 引用s.
     */
    unsigned int audio_element_id;

    /**
     * In格式ion required required 用于 applying any processing 到 the
     * 引用d 和 rendered 音频 Element before being summed，使用 other
     * processed 音频 Elements.
     * @ref AVIAMFParamDefinition.type "type" must be
     * AV_IAMF_PARAMETER_DEFINITION_MIX_GAIN.
     */
    AVIAMFParamDefinition *element_mix_config;

    /**
     * 默认 mix gain 值 到 apply 当 there are no AVIAMFParamDefinition
     *，使用 @ref element_mix_config "element_mix_config's"
     * @ref AVIAMFParamDefinition.parameter_id "parameter_id" available 用于 a
     * given 音频 帧.
     */
    AVRational default_mix_gain;

    /**
     * 值 that indicates 是否 the 引用d 声道-based 音频 Element
     * shall be rendered 到 stereo loudspeakers 或 spatialized，使用 a binaural
     * renderer 当 played back 上 headphones.
     * 如果 the 音频 Element is not 的 @ref AVIAMF音频Element.音频_element_type
     * "type" AV_IAMF_音频_ELEMENT_TYPE_声道, then this field is undefined.
     */
    enum AVIAMFHeadphonesMode headphones_rendering_mode;

    /**
     * dictionary 的 strings describing the submix 中 d如果ferent languages.
     * Must have the same amount 的 entries as
     * @ref AVIAMFMixPresentation.annotations "the mix's annotations", stored
     * 中 the same order, and，使用 the same key strings.
     *
     * @ref AVDictionaryEntry.key "key" is a string conforming 到 BCP-47 that
     * spec如果ies the language 用于 the string stored in
     * @ref AVDictionaryEntry.值 "值".
     */
    AVDictionary *annotations;
} AVIAMFSubmixElement;

enum AVIAMFSubmixLayoutType {
    /**
     * layout follows the loudspeaker sound system convention 的 ITU-2051-3.
     * @ref AVIAMFSubmixLayout.sound_system must be 设置.
     */
    AV_IAMF_SUBMIX_LAYOUT_TYPE_LOUDSPEAKERS = 2,
    /**
     * layout is binaural.
     *
     * @note @ref AVIAMFSubmixLayout.sound_system may be 设置 to
     * AV_声道_LAYOUT_BINAURAL 到 simpl如果y API usage, but it's not mandatory.
     */
    AV_IAMF_SUBMIX_LAYOUT_TYPE_BINAURAL = 3,
};

/**
 * Submix layout as defined 中 section 3.7.6 的 IAMF.
 *
 * @note struct should be 分配d，使用 av_iamf_submix_add_layout()
 *       和 its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVIAMFSubmixLayout {
    const AVClass *av_class;

    enum AVIAMFSubmixLayoutType layout_type;

    /**
     * 声道布局 matching one 的 Sound Systems 到 J 的 ITU-2051-3, plus
     * 7.1.2ch, 3.1.2ch, 和 binaural.
     * 如果 layout_type is not AV_IAMF_SUBMIX_LAYOUT_TYPE_LOUDSPEAKERS or
     * AV_IAMF_SUBMIX_LAYOUT_TYPE_BINAURAL, this field is undefined.
     */
    AVChannelLayout sound_system;
    /**
     * program integrated loudness in格式ion, as defined in
     * ITU-1770-4.
     */
    AVRational integrated_loudness;
    /**
     * digital (采样d) peak 值 的 the 音频 signal, as defined
     * 中 ITU-1770-4.
     */
    AVRational digital_peak;
    /**
     * true peak 的 the 音频 signal, as defined 中 ITU-1770-4.
     */
    AVRational true_peak;
    /**
     * Dialogue loudness in格式ion, as defined 中 ITU-1770-4.
     */
    AVRational dialogue_anchored_loudness;
    /**
     * Album loudness in格式ion, as defined 中 ITU-1770-4.
     */
    AVRational album_anchored_loudness;
} AVIAMFSubmixLayout;

/**
 * Submix layout as defined 中 section 3.7 的 IAMF.
 *
 * @note struct should be 分配d，使用 av_iamf_mix_presentation_add_submix()
 *       和 its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVIAMFSubmix {
    const AVClass *av_class;

    /**
     * 数组 的 submix elements.
     *
     * 设置 by av_iamf_submix_add_element(), must not be mod如果ied by any
     * other code.
     */
    AVIAMFSubmixElement **elements;
    /**
     * 数量 的 elements 中 the submix.
     *
     * 设置 by av_iamf_submix_add_element(), must not be mod如果ied by any
     * other code.
     */
    unsigned int nb_elements;

    /**
     * 数组 的 submix layouts.
     *
     * 设置 by av_iamf_submix_add_layout(), must not be mod如果ied by any
     * other code.
     */
    AVIAMFSubmixLayout **layouts;
    /**
     * 数量 的 layouts 中 the submix.
     *
     * 设置 by av_iamf_submix_add_layout(), must not be mod如果ied by any
     * other code.
     */
    unsigned int nb_layouts;

    /**
     * In格式ion required 用于 post-processing the mixed 音频 signal to
     * generate the 音频 signal 用于 playback.
     * @ref AVIAMFParamDefinition.type "type" must be
     * AV_IAMF_PARAMETER_DEFINITION_MIX_GAIN.
     */
    AVIAMFParamDefinition *output_mix_config;

    /**
     * 默认 mix gain 值 到 apply 当 there are no AVIAMFParamDefinition
     *，使用 @ref 输出_mix_config "输出_mix_config's"
     * @ref AVIAMFParamDefinition.parameter_id "parameter_id" available 用于 a
     * given 音频 帧.
     */
    AVRational default_mix_gain;
} AVIAMFSubmix;

/**
 * In格式ion 上 how 到 render 和 mix one 或 more AVIAMF音频Element 到 generate
 * the final 音频 输出, as defined 中 section 3.7 的 IAMF.
 *
 * @note struct should be 分配d，使用 av_iamf_mix_presentation_alloc()
 *       和 its 大小 is not a part 的 the 公共 ABI.
 */
typedef struct AVIAMFMixPresentation {
    const AVClass *av_class;

    /**
     * 数组 的 submixes.
     *
     * 设置 by av_iamf_mix_presentation_add_submix(), must not be mod如果ied
     * by any other code.
     */
    AVIAMFSubmix **submixes;
    /**
     * 数量 的 submixes 中 the presentation.
     *
     * 设置 by av_iamf_mix_presentation_add_submix(), must not be mod如果ied
     * by any other code.
     */
    unsigned int nb_submixes;

    /**
     * dictionary 的 strings describing the mix 中 d如果ferent languages.
     * Must have the same amount 的 entries as every
     * @ref AVIAMFSubmixElement.annotations "Submix element annotations",
     * stored 中 the same order, and，使用 the same key strings.
     *
     * @ref AVDictionaryEntry.key "key" is a string conforming 到 BCP-47
     * that spec如果ies the language 用于 the string stored in
     * @ref AVDictionaryEntry.值 "值".
     */
    AVDictionary *annotations;
} AVIAMFMixPresentation;

const AVClass *av_iamf_mix_presentation_get_class(void);

/**
 * 分配s a AVIAMFMixPresentation, 和 初始化s its fields，使用 默认
 * 值. No submixes are 分配d.
 * Must be 释放d，使用 av_iamf_mix_presentation_释放().
 *
 * @参见 av_iamf_mix_presentation_add_submix()
 */
AVIAMFMixPresentation *av_iamf_mix_presentation_alloc(void);

/**
 * 分配 a submix 和 add it 到 a given AVIAMFMixPresentation.
 * It is 释放d by av_iamf_mix_presentation_释放() alongside the rest 的 the
 * parent AVIAMFMixPresentation.
 *
 * @返回 a 指针 到 the 分配d submix.
 */
AVIAMFSubmix *av_iamf_mix_presentation_add_submix(AVIAMFMixPresentation *mix_presentation);

/**
 * 分配 a submix element 和 add it 到 a given AVIAMFSubmix.
 * It is 释放d by av_iamf_mix_presentation_释放() alongside the rest 的 the
 * parent AVIAMFSubmix.
 *
 * @返回 a 指针 到 the 分配d submix.
 */
AVIAMFSubmixElement *av_iamf_submix_add_element(AVIAMFSubmix *submix);

/**
 * 分配 a submix layout 和 add it 到 a given AVIAMFSubmix.
 * It is 释放d by av_iamf_mix_presentation_释放() alongside the rest 的 the
 * parent AVIAMFSubmix.
 *
 * @返回 a 指针 到 the 分配d submix.
 */
AVIAMFSubmixLayout *av_iamf_submix_add_layout(AVIAMFSubmix *submix);

/**
 * 释放 an AVIAMFMixPresentation 和 all its contents.
 *
 * @param mix_presentation 指针 到 指针 到 an 分配d AVIAMFMixPresentation.
 *                         upon 返回, *mix_presentation will be 设置 到 NULL.
 */
void av_iamf_mix_presentation_free(AVIAMFMixPresentation **mix_presentation);

/**
 * @}
 */

#endif /* AVUTIL_IAMF_H */
