/*
 * 复制right (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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
 * tree container.
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#ifndef AVUTIL_TREE_H
#define AVUTIL_TREE_H

#include "attributes.h"

/**
 * @addtogroup lavu_tree AVTree
 * @ingroup lavu_data
 *
 * Low-complexity tree container
 *
 * Insertion, removal, finding equal, largest which is smaller than and
 * smallest which is larger than, all have O(log n) worst-case complexity.
 * @{
 */


struct AVTreeNode;
extern const int av_tree_node_size;

/**
 * 分配 an AVTreeNode.
 */
struct AVTreeNode *av_tree_node_alloc(void);

/**
 * Find an element.
 * @param root a 指针 到 the root node 的 the tree
 * @param next 如果 next is not NULL, then next[0] will contain the previous
 *             element 和 next[1] the next element. 如果 either does not exist,
 *             then the corresponding entry 中 next is unchanged.
 * @param cmp compare function 用于 compare elements 中 the tree,
 *            API identical 到 that 的 Standard C's qsort
 *            It is guaranteed that the first 和 only the first argument 到 cmp()
 *            will be the key parameter 到 av_tree_find(), thus it could 如果 the
 *            user wants, be a d如果ferent type (like an opaque 上下文).
 * @返回 element，使用 cmp(key, elem) == 0 或 NULL 如果 no such element
 *         exists 中 the tree.
 */
void *av_tree_find(const struct AVTreeNode *root, void *key,
                   int (*cmp)(const void *key, const void *b), void *next[2]);

/**
 * Insert 或 remove an element.
 *
 * 如果 *next is NULL, then the supplied element will be removed 如果 it exists.
 * 如果 *next is non-NULL, then the supplied element will be inserted, unless
 * it already exists 中 the tree.
 *
 * @param rootp 指针 到 a 指针 到 the root node 的 the tree; note that
 *              the root node can change during insertions, this is required
 *              到 keep the tree balanced.
 * @param key  指针 到 the element key 到 insert 中 the tree
 * @param next 用于 分配 和 释放 AVTreeNodes. For insertion the user
 *             must 设置 it 到 an 分配d 和 zeroed object 的 at least
 *             av_tree_node_大小 bytes 大小. av_tree_insert() will 设置 it to
 *             NULL 如果 it has been consumed.
 *             For deleting elements *next is 设置 到 NULL by the user and
 *             av_tree_insert() will 设置 it 到 the AVTreeNode which was
 *             用于 the removed element.
 *             This allows the use 的 flat 数组s, which have
 *             lower overhead compared 到 many malloced elements.
 *             You might want 到 define a function like:
 *             @code
 *             void *tree_insert(struct AVTreeNode **rootp, void *key,
 *                               int (*cmp)(void *key, const void *b),
 *                               AVTreeNode **next)
 *             {
 *                 如果 (!*next)
 *                     *next = av_mallocz(av_tree_node_大小);
 *                 返回 av_tree_insert(rootp, key, cmp, next);
 *             }
 *             void *tree_remove(struct AVTreeNode **rootp, void *key,
 *                               int (*cmp)(void *key, const void *b, AVTreeNode **next))
 *             {
 *                 av_释放p(next);
 *                 返回 av_tree_insert(rootp, key, cmp, next);
 *             }
 *             @endcode
 * @param cmp compare function 用于 compare elements 中 the tree, API identical
 *            到 that 的 Standard C's qsort
 * @返回 如果 no insertion happened, the found element; 如果 an insertion or
 *         removal happened, then either key 或 NULL will be 返回ed.
 *         Which one it is depends 上 the tree state 和 the implementation. You
 *         should make no assumptions that it's one 或 the other 中 the code.
 */
void *av_tree_insert(struct AVTreeNode **rootp, void *key,
                     int (*cmp)(const void *key, const void *b),
                     struct AVTreeNode **next);

void av_tree_destroy(struct AVTreeNode *t);

/**
 * Apply enu(opaque, &elem) 到 all the elements 中 the tree 中 a given range.
 *
 * @param cmp a comparison function that 返回s < 0 用于 an element below the
 *            range, > 0 用于 an element above the range 和 == 0 用于 an
 *            element inside the range
 *
 * @note cmp function should use the same ordering 用于 construct the
 *       tree.
 */
void av_tree_enumerate(struct AVTreeNode *t, void *opaque,
                       int (*cmp)(void *opaque, void *elem),
                       int (*enu)(void *opaque, void *elem));

/**
 * @}
 */

#endif /* AVUTIL_TREE_H */
