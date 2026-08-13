/*
 * Copyright (c) 2002 Michael Niedermayer <michaelni@gmx.at>
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

/**
 * @file
 * 简单算术表达式求值器
 */

#ifndef AVUTIL_EVAL_H
#define AVUTIL_EVAL_H

typedef struct AVExpr AVExpr;

/**
 * 解析并计算表达式。注意，这比 av_expr_eval() 慢得多。
 *
 * @param res 指向 double 的指针，用于存放表达式结果；出错时存放 NAN
 * @param s 以零结尾的表达式字符串，例如 "1+2^3+5*5+sin(2/3)"
 * @param const_names 以 NULL 结尾的常量标识符字符串数组，例如 {"PI", "E", 0}
 * @param const_values const_names 中标识符对应的值数组
 * @param func1_names 以 NULL 结尾的 funcs1 标识符字符串数组
 * @param funcs1 以 NULL 结尾、指向单参数函数的函数指针数组
 * @param func2_names 以 NULL 结尾的 funcs2 标识符字符串数组
 * @param funcs2 以 NULL 结尾、指向双参数函数的函数指针数组
 * @param opaque 将传给 funcs1 和 funcs2 中所有函数的指针
 * @param log_offset 日志级别偏移量，可用于屏蔽错误消息
 * @param log_ctx 父日志上下文
 * @return 成功时 >= 0，否则返回与 AVERROR 错误码对应的负值
 */
int av_expr_parse_and_eval(double *res, const char *s,
                           const char * const *const_names, const double *const_values,
                           const char * const *func1_names, double (* const *funcs1)(void *, double),
                           const char * const *func2_names, double (* const *funcs2)(void *, double, double),
                           void *opaque, int log_offset, void *log_ctx);

/**
 * 解析表达式。
 *
 * @param expr 成功解析时用于存放含解析值的 AVExpr 的指针，否则存放 NULL。
 *             不再需要时，用户必须使用 av_expr_free() 释放所指 AVExpr。
 * @param s 以零结尾的表达式字符串，例如 "1+2^3+5*5+sin(2/3)"
 * @param const_names 以 NULL 结尾的常量标识符字符串数组，例如 {"PI", "E", 0}
 * @param func1_names 以 NULL 结尾的 funcs1 标识符字符串数组
 * @param funcs1 以 NULL 结尾、指向单参数函数的函数指针数组
 * @param func2_names 以 NULL 结尾的 funcs2 标识符字符串数组
 * @param funcs2 以 NULL 结尾、指向双参数函数的函数指针数组
 * @param log_offset 日志级别偏移量，可用于屏蔽错误消息
 * @param log_ctx 父日志上下文
 * @return 成功时 >= 0，否则返回与 AVERROR 错误码对应的负值
 */
int av_expr_parse(AVExpr **expr, const char *s,
                  const char * const *const_names,
                  const char * const *func1_names, double (* const *funcs1)(void *, double),
                  const char * const *func2_names, double (* const *funcs2)(void *, double, double),
                  int log_offset, void *log_ctx);

/**
 * 计算之前解析的表达式。
 *
 * @param e 要计算的 AVExpr
 * @param const_values av_expr_parse() 的 const_names 中标识符对应的值数组
 * @param opaque 将传给 funcs1 和 funcs2 中所有函数的指针
 * @return 表达式的值
 */
double av_expr_eval(AVExpr *e, const double *const_values, void *opaque);

/**
 * 跟踪已解析表达式中变量是否存在及其出现次数。
 *
 * @param e 要跟踪变量的 AVExpr
 * @param counter 已清零的数组，用于存储每个变量的计数
 * @param size 数组大小
 * @return 成功时返回 0；负值表示未传入表达式或数组，或者 size 为零
 */
int av_expr_count_vars(AVExpr *e, unsigned *counter, int size);

/**
 * 跟踪已解析表达式中用户提供的函数是否存在及其出现次数。
 *
 * @param e 要跟踪用户提供函数的 AVExpr
 * @param counter 已清零的数组，用于存储每个函数的计数。如果向 av_expr_parse()
 *                传入 5 个双参数函数，则 arg=2 时最多使用 5 个条目。
 * @param size 数组大小
 * @param arg 被计数函数的参数数量
 * @return 成功时返回 0；负值表示未传入表达式或数组，或者 size 为零
 */
int av_expr_count_func(AVExpr *e, unsigned *counter, int size, int arg);

/**
 * 释放之前使用 av_expr_parse() 创建的已解析表达式。
 */
void av_expr_free(AVExpr *e);

/**
 * 解析 numstr 中的字符串并以 double 返回其值。如果字符串为空、仅包含空白，
 * 或开头不含符合浮点数预期语法的子串，则不执行转换。此时返回零，并在 tail
 * 中返回 numstr 的值。
 *
 * @param numstr 表示数值的字符串，可包含国际单位制数值后缀，例如 'K'、'M'、
 *               'G'。后缀后加 'i' 时使用 2 的幂而非 10 的幂。'B' 后缀会将值
 *               乘以 8，可附加在其他后缀后，也可单独使用。例如可使用 'KB'、
 *               'MiB'、'G' 和 'B' 作为后缀。
 * @param tail 非 NULL 时，在此放置指向最后一个已解析字符之后字符的指针
 */
double av_strtod(const char *numstr, char **tail);

#endif /* AVUTIL_EVAL_H */
