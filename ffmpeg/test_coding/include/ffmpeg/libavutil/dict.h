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

/**
 * @file
 * 公共字典 API。
 * @deprecated
 *  AVDictionary 为兼容 libav 而提供，其实现和 API 效率都很低，扩展性差，
 *  处理大型字典时极慢。适用时建议新代码使用 tree.c/h 中基于 AVL 树、
 *  可达到 O(log n) 性能的树容器。
 */

#ifndef AVUTIL_DICT_H
#define AVUTIL_DICT_H

#include <stdint.h>

/**
 * @addtogroup lavu_dict AVDictionary
 * @ingroup lavu_data
 *
 * @brief 简单的键值存储
 *
 * @{
 * 字典用于存储键值对。
 *
 * - 要**创建 AVDictionary**，只需将 NULL 指针的地址传给 av_dict_set()。
 *   需要 AVDictionary 指针的任何地方，都可以使用 NULL 表示空字典。
 * - 使用 av_dict_set() **插入条目**。
 * - 使用 av_dict_get() **获取条目**。
 * - 使用 av_dict_iterate() **遍历所有条目**。
 * - 使用 av_dict_free() **释放字典及其全部内容**。
 *
 @code
   AVDictionary *d = NULL;           // "create" an empty dictionary
   AVDictionaryEntry *t = NULL;

   av_dict_set(&d, "foo", "bar", 0); // add an entry

   char *k = av_strdup("key");       // if your strings are already allocated,
   char *v = av_strdup("value");     // you can avoid copying them like this
   av_dict_set(&d, k, v, AV_DICT_DONT_STRDUP_KEY | AV_DICT_DONT_STRDUP_VAL);

   while ((t = av_dict_iterate(d, t))) {
       <....>                        // iterate over all entries in d
   }
   av_dict_free(&d);
 @endcode
 */

/**
 * @name AVDictionary 标志
 * 影响键匹配或向字典插入行为的标志。
 * @{
 */
#define AV_DICT_MATCH_CASE      1   /**< 仅获取键大小写完全匹配的条目。仅与 av_dict_get() 有关。 */
#define AV_DICT_IGNORE_SUFFIX   2   /**< 返回字典中首部与搜索键对应的第一个条目，忽略找到的键字符串后缀。仅与 av_dict_get() 有关。 */
#define AV_DICT_DONT_STRDUP_KEY 4   /**< 接管由 av_malloc() 或其他内存分配函数分配的键的所有权。 */
#define AV_DICT_DONT_STRDUP_VAL 8   /**< 接管由 av_malloc() 或其他内存分配函数分配的值的所有权。 */
#define AV_DICT_DONT_OVERWRITE 16   /**< 不覆盖现有条目。 */
#define AV_DICT_APPEND         32   /**< 条目已存在时追加。注意不会添加分隔符，只会直接拼接字符串。 */
#define AV_DICT_MULTIKEY       64   /**< 允许在字典中存储多个相同的键 */
#define AV_DICT_DEDUP         128   /**< 插入键下已存在的值时不执行操作。仅与 AV_DICT_MULTIKEY 一起使用。 */
/**
 * @}
 */

typedef struct AVDictionaryEntry {
    char *key;
    char *value;
} AVDictionaryEntry;

typedef struct AVDictionary AVDictionary;

/**
 * 获取键匹配的字典条目。
 *
 * 不得更改返回条目的键或值，否则会导致未定义行为。
 *
 * @param prev  设为前一个匹配元素以查找下一个；为 NULL 时返回第一个匹配元素。
 * @param key   匹配键
 * @param flags 控制获取条目方式的 AV_DICT_* 标志集合
 *
 * @return 找到的条目；字典中没有匹配条目时返回 NULL
 */
AVDictionaryEntry *av_dict_get(const AVDictionary *m, const char *key,
                               const AVDictionaryEntry *prev, int flags);

/**
 * 遍历字典。
 *
 * 遍历字典中的所有条目。
 *
 * @warning 不得更改返回的 AVDictionaryEntry 键/值。
 *
 * @warning av_dict_set() 会使此函数之前返回的所有条目失效，因此遍历字典时
 * 不得调用它。
 *
 * 典型用法：
 * @code
 * const AVDictionaryEntry *e = NULL;
 * while ((e = av_dict_iterate(m, e))) {
 *     // ...
 * }
 * @endcode
 *
 * @param m     要遍历的字典
 * @param prev  指向前一个 AVDictionaryEntry 的指针，初始为 NULL
 *
 * @retval AVDictionaryEntry* 字典中的下一个元素
 * @retval NULL               字典中没有更多元素
 */
const AVDictionaryEntry *av_dict_iterate(const AVDictionary *m,
                                         const AVDictionaryEntry *prev);

/**
 * 获取字典中的条目数。
 *
 * @param m 字典
 * @return 字典中的条目数
 */
int av_dict_count(const AVDictionary *m);

/**
 * 在 *pm 中设置给定条目，覆盖现有条目。
 *
 * 注意：如果设置 AV_DICT_DONT_STRDUP_KEY 或 AV_DICT_DONT_STRDUP_VAL，
 * 出错时会释放这些参数。
 *
 * @warning 向字典添加新条目会使之前由 av_dict_get() 或 av_dict_iterate()
 * 返回的所有现有条目失效。
 *
 * @param pm        指向字典结构指针的指针。*pm 为 NULL 时会分配字典结构并放入 *pm。
 * @param key       要添加到 *pm 的条目键（根据 flags 复制或作为新键添加）
 * @param value     要添加到 *pm 的条目值（根据 flags 复制或作为新值添加）。
 *                  传入 NULL 值会删除现有条目。
 *
 * @return 成功时 >= 0，否则返回 <0 的错误码
 */
int av_dict_set(AVDictionary **pm, const char *key, const char *value, int flags);

/**
 * av_dict_set() 的便捷封装，将值转换为字符串并存储。
 *
 * 注意：设置 ::AV_DICT_DONT_STRDUP_KEY 时，出错会释放 key。
 */
int av_dict_set_int(AVDictionary **pm, const char *key, int64_t value, int flags);

/**
 * 解析键/值对列表，并将解析出的条目添加到字典。
 *
 * 失败时，所有成功设置的条目都会存储在 *pm 中。可能需要手动释放创建的字典。
 *
 * @param key_val_sep  用于分隔键和值、以 0 结尾的字符列表
 * @param pairs_sep    用于分隔两个键值对、以 0 结尾的字符列表
 * @param flags        添加到字典时使用的标志。由于键/值标记始终会被复制，
 *                     因此忽略 ::AV_DICT_DONT_STRDUP_KEY 和
 *                     ::AV_DICT_DONT_STRDUP_VAL。
 *
 * @return 成功时返回 0，失败时返回负的 AVERROR 错误码
 */
int av_dict_parse_string(AVDictionary **pm, const char *str,
                         const char *key_val_sep, const char *pairs_sep,
                         int flags);

/**
 * 将一个 AVDictionary 结构中的条目复制到另一个结构。
 *
 * @note 使用 ::AV_DICT_IGNORE_SUFFIX 标志读取元数据
 *
 * @param dst   指向目标 AVDictionary 结构指针的指针。*dst 为 NULL 时，此函数会
 *              分配结构并放入 *dst
 * @param src   指向源 AVDictionary 结构的指针
 * @param flags 在 *dst 中设置条目时使用的标志
 *
 * @return 成功时返回 0，失败时返回负的 AVERROR 错误码。如果 dst 由此函数分配，
 *         调用者应释放相关内存。
 */
int av_dict_copy(AVDictionary **dst, const AVDictionary *src, int flags);

/**
 * 释放为 AVDictionary 结构及所有键和值分配的全部内存。
 */
void av_dict_free(AVDictionary **m);

/**
 * 以字符串形式获取字典条目。
 *
 * 创建包含字典条目的字符串。该字符串可传回 av_dict_parse_string()。
 * @note 字符串使用反斜杠（'\'）转义。
 *
 * @warning 分隔符不能是 '\\' 或 '\0'，也不能彼此相同。
 *
 * @param[in]  m             字典
 * @param[out] buffer        指向将被分配并保存条目字符串的缓冲区的指针。
 *                           不再需要时必须由调用者释放缓冲区。
 * @param[in]  key_val_sep   用于分隔键和值的字符
 * @param[in]  pairs_sep     用于分隔两个键值对的字符
 *
 * @return 成功时 >= 0，出错时为负值
 */
int av_dict_get_string(const AVDictionary *m, char **buffer,
                       const char key_val_sep, const char pairs_sep);

/**
 * @}
 */

#endif /* AVUTIL_DICT_H */
