/*
 * AV选项
 * 复制right (c) 2005 Michael Niedermayer <michaelni@gmx.at>
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

#ifndef AVUTIL_OPT_H
#define AVUTIL_OPT_H

/**
 * @file
 * AV选项
 */

#include "rational.h"
#include "channel_layout.h"
#include "dict.h"
#include "log.h"
#include "pixfmt.h"
#include "samplefmt.h"

/**
 * @defgroup av选项 AV选项
 * @ingroup lavu_data
 * @{
 * AV选项 provide a generic system 到 declare 选项 上 arbitrary structs
 * ("objects"). 选项 can have a help text, a type 和 a range 的 possible
 * 值. 选项 may then be enumerated, read 和 written to.
 *
 * There are two modes 的 access 到 members 的 AV选项 和 its child structs.
 * One is called 'native access', 和 refers 到 access，来自 the code that
 * declares the AV选项 中 question.  other is 'foreign access', 和 refers
 * 到 access，来自 other code.
 *
 * Certain struct members 中 this header are documented as 'native access only'
 * 或 similar - it means that only the code that declared the AV选项 in
 * question is allowed 到 access the field. This allows us 到 extend the
 * semantics 的 those fields without breaking API compatibility.
 *
 * @section av选项_scope Scope 的 AV选项
 *
 * AV选项 is designed 到 support any 设置 的 multimedia configuration 选项
 * that can be defined at compile-time.  Although it is mainly 用于 expose
 * FFmpeg 选项, you are welcome 到 adapt it 到 your own use case.
 *
 * No single approach can ever fully solve the problem 的 configuration,
 * but please submit a patch 如果 you believe you have found a problem
 * that is best solved by extending AV选项.
 *
 * @section av选项_implement Implementing AV选项
 * This section describes how 到 add AV选项 capabilities 到 a struct.
 *
 * All AV选项-related in格式ion is stored 中 an AVClass. Therefore
 * the first member 的 the struct should be a 指针 到 an AVClass describing it.
 * 选项 field 的 the AVClass must be 设置 到 a NULL-terminated static 数组
 * 的 AV选项. Each AV选项 must have a non-empty name, a type, a 默认
 * 值 和 用于 数量-type AV选项 also a range 的 allowed 值. It must
 * also declare an off设置 中 bytes，来自 the start 的 the struct, where the field
 * associated，使用 this AV选项 is located. Other fields 中 the AV选项 struct
 * should also be 设置 当 applicable, but are not required.
 *
 * following example illustrates an AV选项-enabled struct:
 * @code
 * typedef struct test_struct {
 *     const AVClass *class;
 *     int      int_opt;
 *     char    *str_opt;
 *     uint8_t *bin_opt;
 *     int      bin_len;
 * } test_struct;
 *
 * static const AV选项 test_选项[] = {
 *   { "test_int", "This is a test 选项 的 int type.", off设置of(test_struct, int_opt),
 *     AV_OPT_TYPE_INT, { .i64 = -1 }, INT_MIN, INT_MAX },
 *   { "test_str", "This is a test 选项 的 string type.", off设置of(test_struct, str_opt),
 *     AV_OPT_TYPE_STRING },
 *   { "test_bin", "This is a test 选项 的 binary type.", off设置of(test_struct, bin_opt),
 *     AV_OPT_TYPE_BINARY },
 *   { NULL },
 * };
 *
 * static const AVClass test_class = {
 *     .class_name = "test class",
 *     .item_name  = av_默认_item_name,
 *     .选项     = test_选项,
 *     .version    = LIBAVUTIL_VERSION_INT,
 * };
 * @endcode
 *
 * Next, 当 allocating your struct, you must ensure that the AVClass 指针
 * is 设置 到 the correct 值. Then, av_opt_设置_默认s() can be called to
 * 初始化 默认s. After that the struct is ready 到 be used，使用 the
 * AV选项 API.
 *
 * 当 cleaning up, you may use the av_opt_释放() function 到 automatically
 * 释放 all the 分配d string 和 binary 选项.
 *
 * Continuing，使用 the above example:
 *
 * @code
 * test_struct *alloc_test_struct(void)
 * {
 *     test_struct *ret = av_mallocz(大小of(*ret));
 *     ret->class = &test_class;
 *     av_opt_设置_默认s(ret);
 *     返回 ret;
 * }
 * void 释放_test_struct(test_struct **foo)
 * {
 *     av_opt_释放(*foo);
 *     av_释放p(foo);
 * }
 * @endcode
 *
 * @subsection av选项_implement_nesting Nesting
 *      It may happen that an AV选项-enabled struct 包含 another
 *      AV选项-enabled struct as a member (e.g. AVCodec上下文 in
 *      libavcodec exports generic 选项, while its priv_data field exports
 *      codec-spec如果ic 选项). In such a case, it is possible 到 设置 up the
 *      parent struct 到 export a child's 选项. To do that, simply
 *      implement AVClass.child_next() 和 AVClass.child_class_iterate() 中 the
 *      parent struct's AVClass.
 *      Assuming that the test_struct，来自 above now also 包含 a
 *      child_struct field:
 *
 *      @code
 *      typedef struct child_struct {
 *          AVClass *class;
 *          int 标志_opt;
 *      } child_struct;
 *      static const AV选项 child_opts[] = {
 *          { "test_标志", "This is a test 选项 的 标志 type.",
 *            off设置of(child_struct, 标志_opt), AV_OPT_TYPE_标志, { .i64 = 0 }, INT_MIN, INT_MAX },
 *          { NULL },
 *      };
 *      static const AVClass child_class = {
 *          .class_name = "child class",
 *          .item_name  = av_默认_item_name,
 *          .选项     = child_opts,
 *          .version    = LIBAVUTIL_VERSION_INT,
 *      };
 *
 *      void *child_next(void *obj, void *prev)
 *      {
 *          test_struct *t = obj;
 *          如果 (!prev && t->child_struct)
 *              返回 t->child_struct;
 *          返回 NULL
 *      }
 *      const AVClass child_class_iterate(void **iter)
 *      {
 *          const AVClass *c = *iter ? NULL : &child_class;
 *          *iter = (void*)(uintptr_t)c;
 *          返回 c;
 *      }
 *      @endcode
 *      Putting child_next() 和 child_class_iterate() as defined above into
 *      test_class will now make child_struct's 选项 accessible through
 *      test_struct (again, proper 设置up as described above needs 到 be done on
 *      child_struct right after it is 创建d).
 *
 *      From the above example it might not be clear why both child_next()
 *      和 child_class_iterate() are needed. distinction is that child_next()
 *      iterates over actually existing objects, while child_class_iterate()
 *      iterates over all possible child classes. E.g. 如果 an AVCodec上下文
 *      was 初始化d 到 use a codec which has 私有 选项, then its
 *      child_next() will 返回 AVCodec上下文.priv_data 和 finish
 *      iterating. OTOH child_class_iterate() 上 AVCodec上下文.av_class will
 *      iterate over all available codecs，使用 私有 选项.
 *
 * @subsection av选项_implement_named_constants Named constants
 *      It is possible 到 创建 named constants 用于 选项. Simply 设置 the unit
 *      field 的 the 选项 the constants should apply 到 a string and
 *      创建 the constants themselves as 选项 的 type AV_OPT_TYPE_CONST
 *     ，使用 their unit field 设置 到 the same string.
 *      Their 默认_val field should contain the 值 的 the named
 *      constant.
 *      For example, 到 add some named constants 用于 the test_标志 选项
 *      above, put the following into the child_opts 数组:
 *      @code
 *      { "test_标志", "This is a test 选项 的 标志 type.",
 *        off设置of(child_struct, 标志_opt), AV_OPT_TYPE_标志, { .i64 = 0 }, INT_MIN, INT_MAX, "test_unit" },
 *      { "标志1", "This is a 标志，使用 值 16", 0, AV_OPT_TYPE_CONST, { .i64 = 16 }, 0, 0, "test_unit" },
 *      @endcode
 *
 * @section av选项_use Using AV选项
 * This section deals，使用 accessing 选项 中 an AV选项-enabled struct.
 * Such structs 中 FFmpeg are e.g. AVCodec上下文 中 libavcodec or
 * AV格式上下文 中 libav格式.
 *
 * @subsection av选项_use_examine Examining AV选项
 * basic functions 用于 examining 选项 are av_opt_next(), which iterates
 * over all 选项 defined 用于 one object, 和 av_opt_find(), which searches
 * 用于 an 选项，使用 the given name.
 *
 * situation is more complicated，使用 nesting. AV选项-enabled struct
 * may have AV选项-enabled children. Passing the AV_OPT_SEARCH_CHILDREN 标志
 * 到 av_opt_find() will make the function search children recursively.
 *
 * For enumerating there are basically two cases. first is 当 you want to
 * get all 选项 that may potentially exist 上 the struct 和 its children
 * (e.g.  当 constructing documentation). In that case you should call
 * av_opt_child_class_iterate() recursively 上 the parent struct's AVClass.  The
 * second case is 当 you have an already 初始化d struct，使用 all its
 * children 和 you want 到 get all 选项 that can be actually written 或 read
 *，来自 it. In that case you should call av_opt_child_next() recursively (and
 * av_opt_next() 上 each result).
 *
 * @subsection av选项_use_get_设置 Reading 和 writing AV选项
 * 当 设置ting 选项, you often have a string read directly，来自 the
 * user. In such a case, simply passing it 到 av_opt_设置() is enough. For
 * non-string type 选项, av_opt_设置() will 解析 the string according 到 the
 * 选项 type.
 *
 * Similarly av_opt_get() will read any 选项 type 和 转换 it 到 a string
 * which will be 返回ed. Do not forget that the string is 分配d, so you
 * have 到 释放 it，使用 av_释放().
 *
 * In some cases it may be more convenient 到 put all 选项 into an
 * AVDictionary 和 call av_opt_设置_dict() 上 it. spec如果ic case 的 this
 * are the 格式/codec open functions 中 lavf/lavc which take a dictionary
 * filled，使用 选项 as a parameter. This makes it possible 到 设置 some 选项
 * that cannot be 设置 otherwise, since e.g. the 输入 file 格式 is not known
 * before the file is actually opened.
 */

/**
 * 选项 type determines:
 * - 用于 native access, the underlying C type 的 the field that an AV选项
 *   refers to;
 * - 用于 foreign access, the semantics 的 accessing the 选项 through this API,
 *   e.g. which av_opt_get_*() 和 av_opt_设置_*() functions can be called, or
 *   what 格式 will av_opt_get()/av_opt_设置() expect/produce.
 */
enum AVOptionType{
    /**
     * Underlying C type is unsigned int.
     */
    AV_OPT_TYPE_FLAGS = 1,
    /**
     * Underlying C type is int.
     */
    AV_OPT_TYPE_INT,
    /**
     * Underlying C type is int64_t.
     */
    AV_OPT_TYPE_INT64,
    /**
     * Underlying C type is double.
     */
    AV_OPT_TYPE_DOUBLE,
    /**
     * Underlying C type is float.
     */
    AV_OPT_TYPE_FLOAT,
    /**
     * Underlying C type is a uint8_t* that is either NULL 或 points 到 a C
     * string 分配d，使用 the av_malloc() family 的 functions.
     */
    AV_OPT_TYPE_STRING,
    /**
     * Underlying C type is AVRational.
     */
    AV_OPT_TYPE_RATIONAL,
    /**
     * Underlying C type is a uint8_t* that is either NULL 或 points 到 an 数组
     * 分配d，使用 the av_malloc() family 的 functions. 指针 is
     * immediately followed by an int containing the 数组 length 中 bytes.
     */
    AV_OPT_TYPE_BINARY,
    /**
     * Underlying C type is AVDictionary*.
     */
    AV_OPT_TYPE_DICT,
    /**
     * Underlying C type is uint64_t.
     */
    AV_OPT_TYPE_UINT64,
    /**
     * Special 选项 type 用于 declaring named constants. Does not correspond to
     * an actual field 中 the object, off设置 must be 0.
     */
    AV_OPT_TYPE_CONST,
    /**
     * Underlying C type is two consecutive integers.
     */
    AV_OPT_TYPE_IMAGE_SIZE,
    /**
     * Underlying C type is enum AV像素格式.
     */
    AV_OPT_TYPE_PIXEL_FMT,
    /**
     * Underlying C type is enum AV采样格式.
     */
    AV_OPT_TYPE_SAMPLE_FMT,
    /**
     * Underlying C type is AVRational.
     */
    AV_OPT_TYPE_VIDEO_RATE,
    /**
     * Underlying C type is int64_t.
     */
    AV_OPT_TYPE_DURATION,
    /**
     * Underlying C type is uint8_t[4].
     */
    AV_OPT_TYPE_COLOR,
    /**
     * Underlying C type is int.
     */
    AV_OPT_TYPE_BOOL,
    /**
     * Underlying C type is AV声道Layout.
     */
    AV_OPT_TYPE_CHLAYOUT,
    /**
     * Underlying C type is unsigned int.
     */
    AV_OPT_TYPE_UINT,

    /**
     * May be combined，使用 another regular 选项 type 到 declare an 数组
     * 选项.
     *
     * For 数组 选项, @ref AV选项.off设置 should refer 到 a 指针
     * corresponding 到 the 选项 type. 指针 should be immediately
     * followed by an unsigned int that will store the 数量 的 elements 中 the
     * 数组.
     */
    AV_OPT_TYPE_FLAG_ARRAY = (1 << 16),
};

/**
 * generic parameter which can be 设置 by the user 用于 muxing 或 编码.
 */
#define AV_OPT_FLAG_ENCODING_PARAM  (1 << 0)
/**
 * generic parameter which can be 设置 by the user 用于 demuxing 或 解码.
 */
#define AV_OPT_FLAG_DECODING_PARAM  (1 << 1)
#define AV_OPT_FLAG_AUDIO_PARAM     (1 << 3)
#define AV_OPT_FLAG_VIDEO_PARAM     (1 << 4)
#define AV_OPT_FLAG_SUBTITLE_PARAM  (1 << 5)
/**
 * 选项 is intended 用于 exporting 值 到 the caller.
 */
#define AV_OPT_FLAG_EXPORT          (1 << 6)
/**
 * 选项 may not be 设置 through the AV选项 API, only read.
 * This 标志 only makes sense 当 AV_OPT_标志_EXPORT is also 设置.
 */
#define AV_OPT_FLAG_READONLY        (1 << 7)
/**
 * generic parameter which can be 设置 by the user 用于 bit stream filtering.
 */
#define AV_OPT_FLAG_BSF_PARAM       (1 << 8)

/**
 * generic parameter which can be 设置 by the user at runtime.
 */
#define AV_OPT_FLAG_RUNTIME_PARAM   (1 << 15)
/**
 * generic parameter which can be 设置 by the user 用于 filtering.
 */
#define AV_OPT_FLAG_FILTERING_PARAM (1 << 16)
/**
 * 设置 如果 选项 is deprecated, users should refer 到 AV选项.help text for
 * more in格式ion.
 */
#define AV_OPT_FLAG_DEPRECATED      (1 << 17)
/**
 * 设置 如果 选项 constants can also reside 中 child objects.
 */
#define AV_OPT_FLAG_CHILD_CONSTS    (1 << 18)

/**
 * May be 设置 as 默认_val 用于 AV_OPT_TYPE_标志_数组 选项.
 */
typedef struct AVOptionArrayDef {
    /**
     * Native access only.
     *
     * 默认 值 的 the 选项, as would be serialized by av_opt_get() (i.e.
     * using the 值 的 sep as the separator).
     */
    const char         *def;

    /**
     * Minimum 数量 的 elements 中 the 数组. 当 this field is non-zero, def
     * must be non-NULL 和 contain at least this 数量 的 elements.
     */
    unsigned            size_min;
    /**
     * Maximum 数量 的 elements 中 the 数组, 0 当 unlimited.
     */
    unsigned            size_max;

    /**
     * Separator between 数组 elements 中 string representations 的 this
     * 选项, used by av_opt_设置() 和 av_opt_get(). It must be a printable
     * ASCII character, excluding alphanumeric 和 the backslash. comma is
     * used 当 sep=0.
     *
     * separator 和 the backslash must be backslash-escaped 中 order to
     * appear 中 string representations 的 the 选项 值.
     */
    char                sep;
} AVOptionArrayDef;

/**
 * AV选项
 */
typedef struct AVOption {
    const char *name;

    /**
     * short English help text
     * @todo What about other languages?
     */
    const char *help;

    /**
     * Native access only.
     *
     * off设置 relative 到 the 上下文 结构体 where the 选项
     * 值 is stored. It should be 0 用于 named constants.
     */
    int offset;
    enum AVOptionType type;

    /**
     * Native access only, except 当 documented otherwise.
     * the 默认 值 用于 scalar 选项
     */
    union {
        int64_t i64;
        double dbl;
        const char *str;
        /* TODO those are unused now */
        AVRational q;

        /**
         * 用于 AV_OPT_TYPE_标志_数组 选项. May be NULL.
         *
         * Foreign access 到 some members allowed, as noted 中 AV选项数组Def
         * documentation.
         */
        const AVOptionArrayDef *arr;
    } default_val;
    double min;                 ///< minimum valid value for the option
    double max;                 ///< maximum valid value for the option

    /**
     * combination 的 AV_OPT_标志_*.
     */
    int flags;

    /**
     * logical unit 到 which the 选项 belongs. Non-constant
     * 选项 和 corresponding named constants share the same
     * unit. May be NULL.
     */
    const char *unit;
} AVOption;

/**
 * single allowed range 的 值, 或 a single allowed 值.
 */
typedef struct AVOptionRange {
    const char *str;
    /**
     * 值 range.
     * For string ranges this represents the min/max length.
     * For dimensions this represents the min/max 像素 count 或 宽度/高度 中 multi-component case.
     */
    double value_min, value_max;
    /**
     * 值's component range.
     * For string this represents the unicode range 用于 chars, 0-127 limits 到 ASCII.
     */
    double component_min, component_max;
    /**
     * Range 标志.
     * 如果 设置 到 1 the struct encodes a range, 如果 设置 到 0 a single 值.
     */
    int is_range;
} AVOptionRange;

/**
 * 列表 的 AV选项Range structs.
 */
typedef struct AVOptionRanges {
    /**
     * 数组 的 选项 ranges.
     *
     * Most 的 选项 types use just one component.
     * Following describes multi-component 选项 types:
     *
     * AV_OPT_TYPE_IMAGE_大小:
     * component index 0: range 的 像素 count (宽度 * 高度).
     * component index 1: range 的 宽度.
     * component index 2: range 的 高度.
     *
     * @note To obtain multi-component version 的 this 结构体, user must
     *       provide AV_OPT_MULTI_COMPONENT_RANGE 到 av_opt_query_ranges or
     *       av_opt_query_ranges_默认 function.
     *
     * Multi-component range can be read as 中 following example:
     *
     * @code
     * int range_index, component_index;
     * AV选项Ranges *ranges;
     * AV选项Range *range[3]; //may require more than 3 中 the future.
     * av_opt_query_ranges(&ranges, obj, key, AV_OPT_MULTI_COMPONENT_RANGE);
     * 用于 (range_index = 0; range_index < ranges->nb_ranges; range_index++) {
     *     用于 (component_index = 0; component_index < ranges->nb_components; component_index++)
     *         range[component_index] = ranges->range[ranges->nb_ranges * component_index + range_index];
     *     //do something，使用 range here.
     * }
     * av_opt_释放p_ranges(&ranges);
     * @endcode
     */
    AVOptionRange **range;
    /**
     * 数量 的 ranges per component.
     */
    int nb_ranges;
    /**
     * 数量 的 components.
     */
    int nb_components;
} AVOptionRanges;

/**
 * @defgroup opt_mng AV选项 (un)initialization 和 inspection.
 * @{
 */

/**
 * 设置 the 值 的 all AV选项 fields 到 their 默认 值.
 *
 * @param s an AV选项-enabled struct (its first member must be a 指针 到 AVClass)
 */
void av_opt_set_defaults(void *s);

/**
 * 设置 the 值 的 all AV选项 fields 到 their 默认 值. Only these
 * AV选项 fields 用于 which (opt->标志 & mask) == 标志 will have their
 * 默认 applied 到 s.
 *
 * @param s an AV选项-enabled struct (its first member must be a 指针 到 AVClass)
 * @param mask combination 的 AV_OPT_标志_*
 * @param 标志 combination 的 AV_OPT_标志_*
 */
void av_opt_set_defaults2(void *s, int mask, int flags);

/**
 * 释放 all 分配d objects 中 obj.
 */
void av_opt_free(void *obj);

/**
 * Iterate over all AV选项 belonging 到 obj.
 *
 * @param obj an AV选项-enabled struct 或 a double 指针 到 an
 *            AVClass describing it.
 * @param prev result 的 the previous call 到 av_opt_next() 上 this object
 *             或 NULL
 * @返回 next AV选项 或 NULL
 */
const AVOption *av_opt_next(const void *obj, const AVOption *prev);

/**
 * Iterate over AV选项-enabled children 的 obj.
 *
 * @param prev result 的 a previous call 到 this function 或 NULL
 * @返回 next AV选项-enabled child 或 NULL
 */
void *av_opt_child_next(void *obj, void *prev);

/**
 * Iterate over potential AV选项-enabled children 的 parent.
 *
 * @param iter a 指针 where iteration state is stored.
 * @返回 AVClass corresponding 到 next potential child 或 NULL
 */
const AVClass *av_opt_child_class_iterate(const AVClass *parent, void **iter);

#define AV_OPT_SEARCH_CHILDREN   (1 << 0) /**< Search in possible children of the
                                               given object first. */
/**
 *  obj passed 到 av_opt_find() is fake -- only a double 指针 到 AVClass
 *  instead 的 a required 指针 到 a struct containing AVClass. This is
 *  useful 用于 searching 用于 选项 without needing 到 分配 the corresponding
 *  object.
 */
#define AV_OPT_SEARCH_FAKE_OBJ   (1 << 1)

/**
 *  In av_opt_get, 返回 NULL 如果 the 选项 has a 指针 type 和 is 设置 到 NULL,
 *  rather than 返回ing an empty string.
 */
#define AV_OPT_ALLOW_NULL (1 << 2)

/**
 * May be used，使用 av_opt_设置_数组() 到 signal that new elements should
 * replace the existing ones 中 the indicated range.
 */
#define AV_OPT_ARRAY_REPLACE (1 << 3)

/**
 *  Allows av_opt_query_ranges 和 av_opt_query_ranges_默认 到 返回 more than
 *  one component 用于 certain 选项 types.
 *  @参见 AV选项Ranges 用于 details.
 */
#define AV_OPT_MULTI_COMPONENT_RANGE (1 << 12)

/**
 * Look 用于 an 选项 中 an object. Consider only 选项 which
 * have all the spec如果ied 标志 设置.
 *
 * @param[in] obj 指针 到 a struct whose first element is a
 *                指针 到 an AVClass.
 *                Alternatively a double 指针 到 an AVClass, 如果
 *                AV_OPT_SEARCH_FAKE_OBJ search 标志 is 设置.
 * @param[in] name name 的 the 选项 到 look for.
 * @param[in] unit 当 searching 用于 named constants, name 的 the unit
 *                 it belongs to.
 * @param opt_标志 Find only 选项，使用 all the spec如果ied 标志 设置 (AV_OPT_标志).
 * @param search_标志 combination 的 AV_OPT_SEARCH_*.
 *
 * @返回 指针 到 the 选项 found, 或 NULL 如果 no 选项
 *         was found.
 *
 * @note 选项 found，使用 AV_OPT_SEARCH_CHILDREN 标志 may not be 设置table
 * directly，使用 av_opt_设置(). Use special calls which take an 选项
 * AVDictionary (e.g. av格式_open_输入()) 到 设置 选项 found，使用 this
 * 标志.
 */
const AVOption *av_opt_find(void *obj, const char *name, const char *unit,
                            int opt_flags, int search_flags);

/**
 * Look 用于 an 选项 中 an object. Consider only 选项 which
 * have all the spec如果ied 标志 设置.
 *
 * @param[in] obj 指针 到 a struct whose first element is a
 *                指针 到 an AVClass.
 *                Alternatively a double 指针 到 an AVClass, 如果
 *                AV_OPT_SEARCH_FAKE_OBJ search 标志 is 设置.
 * @param[in] name name 的 the 选项 到 look for.
 * @param[in] unit 当 searching 用于 named constants, name 的 the unit
 *                 it belongs to.
 * @param opt_标志 Find only 选项，使用 all the spec如果ied 标志 设置 (AV_OPT_标志).
 * @param search_标志 combination 的 AV_OPT_SEARCH_*.
 * @param[out] target_obj 如果 non-NULL, an object 到 which the 选项 belongs will be
 * written here. It may be d如果ferent，来自 obj 如果 AV_OPT_SEARCH_CHILDREN is present
 * 中 search_标志. This parameter is ignored 如果 search_标志 contain
 * AV_OPT_SEARCH_FAKE_OBJ.
 *
 * @返回 指针 到 the 选项 found, 或 NULL 如果 no 选项
 *         was found.
 */
const AVOption *av_opt_find2(void *obj, const char *name, const char *unit,
                             int opt_flags, int search_flags, void **target_obj);

/**
 * Show the obj 选项.
 *
 * @param req_标志 requested 标志 用于 the 选项 到 show. Show only the
 * 选项 用于 which it is opt->标志 & req_标志.
 * @param rej_标志 rejected 标志 用于 the 选项 到 show. Show only the
 * 选项 用于 which it is !(opt->标志 & req_标志).
 * @param av_log_obj log 上下文 到 use 用于 showing the 选项
 */
int av_opt_show2(void *obj, void *av_log_obj, int req_flags, int rej_flags);

/**
 * Extract a key-值 pair，来自 the beginning 的 a string.
 *
 * @param ropts        指针 到 the 选项 string, will be updated to
 *                     point 到 the rest 的 the string (one 的 the pairs_sep
 *                     或 the final NUL)
 * @param key_val_sep  a 0-terminated 列表 的 characters 用于 separate
 *                     key，来自 值, 用于 example '='
 * @param pairs_sep    a 0-terminated 列表 的 characters 用于 separate
 *                     two pairs，来自 each other, 用于 example ':' 或 ','
 * @param 标志        标志; 参见 the AV_OPT_标志_* 值 below
 * @param rkey         解析d key; must be 释放d using av_释放()
 * @param rval         解析d 值; must be 释放d using av_释放()
 *
 * @返回  >=0 用于 success, 或 a negative 值 corresponding 到 an
 *          AVERROR code 中 case 的 error; 中 particular:
 *          AVERROR(EINVAL) 如果 no key is present
 *
 */
int av_opt_get_key_value(const char **ropts,
                         const char *key_val_sep, const char *pairs_sep,
                         unsigned flags,
                         char **rkey, char **rval);

enum {

    /**
     * Accept 到 解析 a 值 without a key; the key will then be 返回ed
     * as NULL.
     */
    AV_OPT_FLAG_IMPLICIT_KEY = 1,
};

/**
 * @}
 */

/**
 * @defgroup opt_write 设置ting 和 mod如果ying 选项 值
 * @{
 */

/**
 * 解析 the key/值 pairs 列表 中 opts. For each key/值 pair
 * found, stores the 值 中 the field 中 ctx that is named like the
 * key. ctx must be an AVClass 上下文, storing is done using
 * AV选项.
 *
 * @param opts 选项 string 到 解析, may be NULL
 * @param key_val_sep a 0-terminated 列表 的 characters 用于
 * separate key，来自 值
 * @param pairs_sep a 0-terminated 列表 的 characters 用于 separate
 * two pairs，来自 each other
 * @返回 the 数量 的 successfully 设置 key/值 pairs, 或 a negative
 * 值 corresponding 到 an AVERROR code 中 case 的 error:
 * AVERROR(EINVAL) 如果 opts cannot be 解析d,
 * the error code issued by av_opt_设置() 如果 a key/值 pair
 * cannot be 设置
 */
int av_set_options_string(void *ctx, const char *opts,
                          const char *key_val_sep, const char *pairs_sep);

/**
 * 解析 the key-值 pairs 列表 中 opts. For each key=值 pair found,
 * 设置 the 值 的 the corresponding 选项 中 ctx.
 *
 * @param ctx          the AVClass object 到 设置 选项 on
 * @param opts         the 选项 string, key-值 pairs separated by a
 *                     delimiter
 * @param shorthand    a NULL-terminated 数组 的 选项 names 用于 shorthand
 *                     notation: 如果 the first field 中 opts has no key part,
 *                     the key is taken，来自 the first element 的 shorthand;
 *                     then again 用于 the second, etc., until either opts is
 *                     finished, shorthand is finished 或 a named 选项 is
 *                     found; after that, all 选项 must be named
 * @param key_val_sep  a 0-terminated 列表 的 characters 用于 separate
 *                     key，来自 值, 用于 example '='
 * @param pairs_sep    a 0-terminated 列表 的 characters 用于 separate
 *                     two pairs，来自 each other, 用于 example ':' 或 ','
 * @返回  the 数量 的 successfully 设置 key=值 pairs, 或 a negative
 *          值 corresponding 到 an AVERROR code 中 case 的 error:
 *          AVERROR(EINVAL) 如果 opts cannot be 解析d,
 *          the error code issued by av_设置_string3() 如果 a key/值 pair
 *          cannot be 设置
 *
 * 选项 names must use only the following characters: a-z A-Z 0-9 - . / _
 * Separators must use characters distinct，来自 选项 names and，来自 each
 * other.
 */
int av_opt_set_from_string(void *ctx, const char *opts,
                           const char *const *shorthand,
                           const char *key_val_sep, const char *pairs_sep);

/**
 * 设置 all the 选项，来自 a given dictionary 上 an object.
 *
 * @param obj a struct whose first element is a 指针 到 AVClass
 * @param 选项 选项 到 process. This dictionary will be 释放d 和 replaced
 *                by a new one containing all 选项 not found 中 obj.
 *                Of course this new dictionary needs 到 be 释放d by caller
 *               ，使用 av_dict_释放().
 *
 * @返回 0 上 success, a negative AVERROR 如果 some 选项 was found 中 obj,
 *         but could not be 设置.
 *
 * @参见 av_dict_复制()
 */
int av_opt_set_dict(void *obj, struct AVDictionary **options);


/**
 * 设置 all the 选项，来自 a given dictionary 上 an object.
 *
 * @param obj a struct whose first element is a 指针 到 AVClass
 * @param 选项 选项 到 process. This dictionary will be 释放d 和 replaced
 *                by a new one containing all 选项 not found 中 obj.
 *                Of course this new dictionary needs 到 be 释放d by caller
 *               ，使用 av_dict_释放().
 * @param search_标志 combination 的 AV_OPT_SEARCH_*.
 *
 * @返回 0 上 success, a negative AVERROR 如果 some 选项 was found 中 obj,
 *         but could not be 设置.
 *
 * @参见 av_dict_复制()
 */
int av_opt_set_dict2(void *obj, struct AVDictionary **options, int search_flags);

/**
 * 复制 选项，来自 src object into dest object.
 *
 * underlying AVClass 的 both src 和 dest must coincide. guarantee
 * below does not apply 如果 this is not fulfilled.
 *
 * 选项 that require 内存 allocation (e.g. string 或 binary) are malloc'ed 中 dest object.
 * Original 内存 分配d 用于 such 选项 is 释放d unless both src 和 dest 选项 points 到 the same 内存.
 *
 * Even 上 error it is guaranteed that 分配d 选项，来自 src 和 dest
 * no longer alias each other afterwards; 中 particular calling av_opt_释放()
 * 上 both src 和 dest is safe afterwards 如果 dest has been memdup'ed，来自 src.
 *
 * @param dest Object 到 复制 from
 * @param src  Object 到 复制 into
 * @返回 0 上 success, negative 上 error
 */
int av_opt_copy(void *dest, const void *src);

/**
 * @defgroup opt_设置_funcs 选项 设置ting functions
 * @{
 * Those functions 设置 the field 的 obj，使用 the given name 到 值.
 *
 * @param[in] obj struct whose first element is a 指针 到 an AVClass.
 * @param[in] name the name 的 the field 到 设置
 * @param[in] val 值 到 设置. In case 的 av_opt_设置() 如果 the field is not
 * 的 a string type, then the given string is 解析d.
 * SI postfixes 和 some named scalars are supported.
 * 如果 the field is 的 a numeric type, it has 到 be a numeric 或 named
 * scalar. Behavior，使用 more than one scalar 和 +- infix operators
 * is undefined.
 * 如果 the field is 的 a 标志 type, it has 到 be a sequence 的 numeric
 * scalars 或 named 标志 separated by '+' 或 '-'. Prefixing a 标志
 *，使用 '+' causes it 到 be 设置 without affecting the other 标志;
 * similarly, '-' un设置s a 标志.
 * 如果 the field is 的 a dictionary type, it has 到 be a ':' separated 列表 of
 * key=值 parameters. 值 containing ':' special characters must be
 * escaped.
 * @param search_标志 标志 passed 到 av_opt_find2. I.e. 如果 AV_OPT_SEARCH_CHILDREN
 * is passed here, then the 选项 may be 设置 上 a child 的 obj.
 *
 * @返回 0 如果 the 值 has been 设置, 或 an AVERROR code 中 case of
 * error:
 * AVERROR_选项_NOT_FOUND 如果 no matching 选项 exists
 * AVERROR(ERANGE) 如果 the 值 is out 的 range
 * AVERROR(EINVAL) 如果 the 值 is not valid
 */
int av_opt_set         (void *obj, const char *name, const char *val, int search_flags);
int av_opt_set_int     (void *obj, const char *name, int64_t     val, int search_flags);
int av_opt_set_double  (void *obj, const char *name, double      val, int search_flags);
int av_opt_set_q       (void *obj, const char *name, AVRational  val, int search_flags);
int av_opt_set_bin     (void *obj, const char *name, const uint8_t *val, int size, int search_flags);
int av_opt_set_image_size(void *obj, const char *name, int w, int h, int search_flags);
int av_opt_set_pixel_fmt (void *obj, const char *name, enum AVPixelFormat fmt, int search_flags);
int av_opt_set_sample_fmt(void *obj, const char *name, enum AVSampleFormat fmt, int search_flags);
int av_opt_set_video_rate(void *obj, const char *name, AVRational val, int search_flags);
/**
 * @note Any old chlayout present is discarded 和 replaced，使用 a 复制 的 the new one. The
 * caller still owns layout 和 is responsible 用于 uninitializing it.
 */
int av_opt_set_chlayout(void *obj, const char *name, const AVChannelLayout *layout, int search_flags);
/**
 * @note Any old dictionary present is discarded 和 replaced，使用 a 复制 的 the new one. The
 * caller still owns val is 和 responsible 用于 释放ing it.
 */
int av_opt_set_dict_val(void *obj, const char *name, const AVDictionary *val, int search_flags);

/**
 * Add, replace, 或 remove elements 用于 an 数组 选项. Which 的 these
 * operations is performed depends 上 the 值 的 val 和 search_标志.
 *
 * @param start_elem Index 的 the first 数组 element 到 mod如果y; must not be
 *                   larger than 数组 大小 as 返回ed by
 *                   av_opt_get_数组_大小().
 * @param nb_elems 数量 的 数组 elements 到 mod如果y; 当 val is NULL,
 *                 start_elem+nb_elems must not be larger than 数组 大小 as
 *                 返回ed by av_opt_get_数组_大小()
 *
 * @param val_type 选项 type corresponding 到 the type 的 val, ignored 当 val is
 *                 NULL.
 *
 *                 effect 的 this function will will be as 如果 av_opt_设置X()
 *                 was called 用于 each element, where X is spec如果ied by type.
 *                 E.g. AV_OPT_TYPE_STRING corresponds 到 av_opt_设置().
 *
 *                 Typically this should be the same as the scalarized type of
 *                 the AV选项 being 设置, but certain conversions are also
 *                 possible - the same as those done by the corresponding
 *                 av_opt_设置*() function. E.g. any 选项 type can be 设置 from
 *                 a string, numeric types can be 设置，来自 int64, double, or
 *                 rational, etc.
 *
 * @param val 数组，使用 nb_elems elements 或 NULL.
 *
 *            当 NULL, nb_elems 数组 elements starting at start_elem are
 *            removed，来自 the 数组. Any 数组 elements remaining at the end
 *            are sh如果ted by nb_elems towards the first element 中 order 到 keep
 *            the 数组 contiguous.
 *
 *            Otherwise (val is non-NULL), the type 的 val must match the
 *            underlying C type as documented 用于 val_type.
 *
 *            当 AV_OPT_数组_REPLACE is not 设置 中 search_标志, the 数组 is
 *            enlarged by nb_elems, 和 the contents 的 val are inserted at
 *            start_elem. Previously existing 数组 elements，来自 start_elem
 *            onwards (如果 present) are sh如果ted by nb_elems away，来自 the first
 *            element 中 order 到 make space 用于 the new elements.
 *
 *            当 AV_OPT_数组_REPLACE is 设置 中 search_标志, the contents
 *            的 val replace existing 数组 elements，来自 start_elem to
 *            start_elem+nb_elems (如果 present). New 数组 大小 is
 *            max(start_elem + nb_elems, old 数组 大小).
 */
int av_opt_set_array(void *obj, const char *name, int search_flags,
                     unsigned int start_elem, unsigned int nb_elems,
                     enum AVOptionType val_type, const void *val);

/**
 * @}
 * @}
 */

/**
 * @defgroup opt_read Reading 选项 值
 * @{
 */

/**
 * @defgroup opt_get_funcs 选项 getting functions
 * @{
 * Those functions get a 值 的 the 选项，使用 the given name，来自 an object.
 *
 * @param[in] obj a struct whose first element is a 指针 到 an AVClass.
 * @param[in] name name 的 the 选项 到 get.
 * @param[in] search_标志 标志 passed 到 av_opt_find2. I.e. 如果 AV_OPT_SEARCH_CHILDREN
 * is passed here, then the 选项 may be found 中 a child 的 obj.
 * @param[out] out_val 值 的 the 选项 will be written here
 * @返回 >=0 上 success, a negative error code otherwise
 */
/**
 * @note the 返回ed string will be av_malloc()ed 和 must be av_释放()ed by the caller
 *
 * @note 如果 AV_OPT_ALLOW_NULL is 设置 中 search_标志 中 av_opt_get, 和 the
 * 选项 is 的 type AV_OPT_TYPE_STRING, AV_OPT_TYPE_BINARY 或 AV_OPT_TYPE_DICT
 * 和 is 设置 到 NULL, *out_val will be 设置 到 NULL instead 的 an 分配d
 * empty string.
 */
int av_opt_get         (void *obj, const char *name, int search_flags, uint8_t   **out_val);
int av_opt_get_int     (void *obj, const char *name, int search_flags, int64_t    *out_val);
int av_opt_get_double  (void *obj, const char *name, int search_flags, double     *out_val);
int av_opt_get_q       (void *obj, const char *name, int search_flags, AVRational *out_val);
int av_opt_get_image_size(void *obj, const char *name, int search_flags, int *w_out, int *h_out);
int av_opt_get_pixel_fmt (void *obj, const char *name, int search_flags, enum AVPixelFormat *out_fmt);
int av_opt_get_sample_fmt(void *obj, const char *name, int search_flags, enum AVSampleFormat *out_fmt);
int av_opt_get_video_rate(void *obj, const char *name, int search_flags, AVRational *out_val);
/**
 * @param[out] layout 返回ed layout is a 复制 的 the actual 值 和 must
 * be 释放d，使用 av_声道_layout_uninit() by the caller
 */
int av_opt_get_chlayout(void *obj, const char *name, int search_flags, AVChannelLayout *layout);
/**
 * @param[out] out_val 返回ed dictionary is a 复制 的 the actual 值 和 must
 * be 释放d，使用 av_dict_释放() by the caller
 */
int av_opt_get_dict_val(void *obj, const char *name, int search_flags, AVDictionary **out_val);

/**
 * For an 数组-type 选项, get the 数量 的 elements 中 the 数组.
 */
int av_opt_get_array_size(void *obj, const char *name, int search_flags,
                          unsigned int *out_val);

/**
 * For an 数组-type 选项, retrieve the 值 的 one 或 more 数组 elements.
 *
 * @param start_elem index 的 the first 数组 element 到 retrieve
 * @param nb_elems 数量 的 数组 elements 到 retrieve; start_elem+nb_elems
 *                 must not be larger than 数组 大小 as 返回ed by
 *                 av_opt_get_数组_大小()
 *
 * @param out_type 选项 type corresponding 到 the desired 输出.
 *
 *                 数组 elements produced by this function will
 *                 will be as 如果 av_opt_getX() was called 用于 each element,
 *                 where X is spec如果ied by out_type. E.g. AV_OPT_TYPE_STRING
 *                 corresponds 到 av_opt_get().
 *
 *                 Typically this should be the same as the scalarized type of
 *                 the AV选项 being retrieved, but certain conversions are
 *                 also possible - the same as those done by the corresponding
 *                 av_opt_get*() function. E.g. any 选项 type can be retrieved
 *                 as a string, numeric types can be retrieved as int64, double,
 *                 或 rational, etc.
 *
 * @param out_val  数组，使用 nb_elems members into which the 输出 will be
 *                 written. 数组 type must match the underlying C type as
 *                 documented 用于 out_type, 和 be zeroed 上 entry 到 this
 *                 function.
 *
 *                 For dynamically 分配d types (strings, binary, dicts,
 *                 etc.), the result is owned 和 释放d by the caller.
 */
int av_opt_get_array(void *obj, const char *name, int search_flags,
                     unsigned int start_elem, unsigned int nb_elems,
                     enum AVOptionType out_type, void *out_val);
/**
 * @}
 */

/**
 * @defgroup opt_eval_funcs Evaluating 选项 strings
 * @{
 * This group 的 functions 可用于 到 evaluate 选项 strings
 * 和 get 数量s out 的 them. They do the same thing as av_opt_设置(),
 * except the result is written into the caller-supplied 指针.
 *
 * @param obj a struct whose first element is a 指针 到 AVClass.
 * @param o an 选项 用于 which the string is 到 be evaluated.
 * @param val string 到 be evaluated.
 * @param *_out 值 的 the string will be written here.
 *
 * @返回 0 上 success, a negative 数量 上 failure.
 */
int av_opt_eval_flags (void *obj, const AVOption *o, const char *val, int        *flags_out);
int av_opt_eval_int   (void *obj, const AVOption *o, const char *val, int        *int_out);
int av_opt_eval_uint  (void *obj, const AVOption *o, const char *val, unsigned   *uint_out);
int av_opt_eval_int64 (void *obj, const AVOption *o, const char *val, int64_t    *int64_out);
int av_opt_eval_float (void *obj, const AVOption *o, const char *val, float      *float_out);
int av_opt_eval_double(void *obj, const AVOption *o, const char *val, double     *double_out);
int av_opt_eval_q     (void *obj, const AVOption *o, const char *val, AVRational *q_out);
/**
 * @}
 */

/**
 * 检查 如果 given 选项 is 设置 到 its 默认 值.
 *
 * 选项 o must belong 到 the obj. 此函数 must not be called 到 检查 child's 选项 state.
 * @参见 av_opt_is_设置_to_默认_by_name().
 *
 * @param obj  AVClass object 到 检查 选项 on
 * @param o    选项 到 be 检查ed
 * @返回     >0 当 选项 is 设置 到 its 默认,
 *              0 当 选项 is not 设置 its 默认,
 *             <0 上 error
 */
int av_opt_is_set_to_default(void *obj, const AVOption *o);

/**
 * 检查 如果 given 选项 is 设置 到 its 默认 值.
 *
 * @param obj          AVClass object 到 检查 选项 on
 * @param name         选项 name
 * @param search_标志 combination 的 AV_OPT_SEARCH_*
 * @返回             >0 当 选项 is 设置 到 its 默认,
 *                     0 当 选项 is not 设置 its 默认,
 *                     <0 上 error
 */
int av_opt_is_set_to_default_by_name(void *obj, const char *name, int search_flags);

/**
 * 检查 是否 a particular 标志 is 设置 中 a 标志 field.
 *
 * @param field_name the name 的 the 标志 field 选项
 * @param 标志_name the name 的 the 标志 到 检查
 * @返回 non-zero 如果 the 标志 is 设置, zero 如果 the 标志 isn't 设置,
 *         isn't 的 the right type, 或 the 标志 field doesn't exist.
 */
int av_opt_flag_is_set(void *obj, const char *field_name, const char *flag_name);

#define AV_OPT_SERIALIZE_SKIP_DEFAULTS              0x00000001  ///< Serialize options that are not set to default values only.
#define AV_OPT_SERIALIZE_OPT_FLAGS_EXACT            0x00000002  ///< Serialize options that exactly match opt_flags only.
#define AV_OPT_SERIALIZE_SEARCH_CHILDREN            0x00000004  ///< Serialize options in possible children of the given object.

/**
 * Serialize object's 选项.
 *
 * 创建 a string containing object's serialized 选项.
 * Such string may be passed back 到 av_opt_设置_from_string() 中 order 到 restore 选项 值.
 * key/值 或 pairs separator occurring 中 the serialized 值 or
 * name string are escaped through the av_escape() function.
 *
 * @param[in]  obj           AVClass object 到 serialize
 * @param[in]  opt_标志     serialize 选项，使用 all the spec如果ied 标志 设置 (AV_OPT_标志)
 * @param[in]  标志         combination 的 AV_OPT_SERIALIZE_* 标志
 * @param[out] 缓冲区        指针 到 缓冲区 that will be 分配d，使用 string containing serialized 选项.
 *                           缓冲区 must be 释放d by the caller 当 is no longer needed.
 * @param[in]  key_val_sep   character 用于 separate key，来自 值
 * @param[in]  pairs_sep     character 用于 separate two pairs，来自 each other
 * @返回                   >= 0 上 success, negative 上 error
 * @warning Separators cannot be neither '\\' nor '\0'. They also cannot be the same.
 */
int av_opt_serialize(void *obj, int opt_flags, int flags, char **buffer,
                     const char key_val_sep, const char pairs_sep);

/**
 * @}
 */

/**
 * 释放 an AV选项Ranges struct 和 设置 it 到 NULL.
 */
void av_opt_freep_ranges(AVOptionRanges **ranges);

/**
 * 获取 a 列表 的 allowed ranges 用于 the given 选项.
 *
 * 返回ed 列表 may depend 上 other fields 中 obj like 用于 example profile.
 *
 * @param 标志 is a bitmask 的 标志, undefined 标志 should not be 设置 和 should be ignored
 *              AV_OPT_SEARCH_FAKE_OBJ indicates that the obj is a double 指针 到 a AVClass instead 的 a full instance
 *              AV_OPT_MULTI_COMPONENT_RANGE indicates that function may 返回 more than one component, @参见 AV选项Ranges
 *
 * result must be 释放d，使用 av_opt_释放p_ranges.
 *
 * @返回 数量 的 components 返回ed 上 success, a negative error code otherwise
 */
int av_opt_query_ranges(AVOptionRanges **, void *obj, const char *key, int flags);

/**
 * 获取 a 默认 列表 的 allowed ranges 用于 the given 选项.
 *
 * This 列表 is constructed without using the AVClass.query_ranges() callback
 * 和 可用于 as fallback，来自 within the callback.
 *
 * @param 标志 is a bitmask 的 标志, undefined 标志 should not be 设置 和 should be ignored
 *              AV_OPT_SEARCH_FAKE_OBJ indicates that the obj is a double 指针 到 a AVClass instead 的 a full instance
 *              AV_OPT_MULTI_COMPONENT_RANGE indicates that function may 返回 more than one component, @参见 AV选项Ranges
 *
 * result must be 释放d，使用 av_opt_释放_ranges.
 *
 * @返回 数量 的 components 返回ed 上 success, a negative error code otherwise
 */
int av_opt_query_ranges_default(AVOptionRanges **, void *obj, const char *key, int flags);

/**
 * @}
 */

#endif /* AVUTIL_OPT_H */
