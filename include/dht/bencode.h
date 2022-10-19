/*
 * Copyright (c) 2020 naturalpolice
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License (see LICENSE).
 */

/*!
 * \file bencode.h
 * \brief Provides routines for parsing and formating bencoded data.
 *
 * All BitTorrent specifications use a special data serialization format
 * called bencoding. This API contains the tools necessary to parse and format
 * bencoded data.
 */

#ifndef DHT_BENCODE_H_
#define DHT_BENCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/*!
 * B编码的值
 *
 * 一种B编码值表示形式，能够容纳四种类型之一：
 * "integer", "string", "list", 和 "dictionary".
 */
struct bvalue {
    /*!
     * 所包含值的类型。
     */
    enum {
        BVALUE_INTEGER, // 整数
        BVALUE_STRING, // 字符串
        BVALUE_LIST, // 列表
        BVALUE_DICTIONARY, // 字典
    } type;
    /*!
     * 特定类型数据
     */
    union {
        /*!
         * 整数值
         */
        long long int i;
        /*!
         * 字符串值
         */
        struct {
            unsigned char *bytes;   /*!< string pointer (zero-terminated) */
            size_t len;             /*!< length of the string */
        } s;
        /*!
         * 列表值
         */
        struct {
            struct bvalue **array;  /*!< array of elements */
            size_t len;             /*!< number of elements */
        } l;
        /*!
         * 字典值
         */
        struct {
            char **key;             /*!< array of keys (in lexicographical order) */
            struct bvalue **val;    /*!< array of values */
            size_t len;             /*!< number of key-value pairs */
        } d;
    };
};

/*!
 * 分配字典值
 *
 * 生成新的空字典值
 *
 * \returns Pointer to newly allocated value, or NULL on allocation failure.
 */
struct bvalue *bvalue_new_dict(void);
/*!
 * 分配列表值
 *
 * 生成新的空列表值
 *
 * \returns Pointer to newly allocated value, or NULL on allocation failure.
 */
struct bvalue *bvalue_new_list(void);
/*!
 * 分配一个整数值
 *
 * 生成一个新的整数值并将其设置为i
 *
 * \param i Initial integer value.
 * \returns Pointer to newly allocated value, or NULL on allocation failure.
 */
struct bvalue *bvalue_new_integer(long long int i);
/*!
 * 分配字符串值
 *
 * Builds a new string value initially set to \a s.
 * This function allocates its own copy of \a s, so the memory can be reused
 * after the call. The internal string buffer is always null-terminated.
 *
 * \param s pointer to the initial string value.
 * \param len length (in bytes) of the intial string value.
 * \returns Pointer to newly allocated value, or NULL on allocation failure.
 */
struct bvalue *bvalue_new_string(const unsigned char *s, size_t len);
/*!
 * 深度拷贝B编码值
 *
 * Recursively copy \a val an all of it's children.
 *
 * \param val The value to copy.
 */
struct bvalue *bvalue_copy(const struct bvalue *val);
/*!
 * Free bencoding value.
 *
 * Recursively free \a val and all of its children.
 *
 * \param val The value to free.
 */
void bvalue_free(struct bvalue *val);

/*!
 * 将值附加到列表。
 *
 * Add \a val to the end of the list value \a list. \a list must be of
 * type \a BVALUE_LIST.
 *
 * \param list The list value to append to.
 * \param val The value to append.
 * \returns 0 on success, -1 on memory allocation failure.
 */
int bvalue_list_append(struct bvalue *list, struct bvalue *val);
/*!
 * 向字典中添加键值对
 *
 * Set key \a key to value \a val in dictionary value \a dict. \a dict must
 * be of type \a BVALUE_DICTIONARY. If the key is already set in \a dict, the
 * old value is simply freed and replaced.
 *
 * \param dict The dictionary value to add the key-value pair to.
 * \param key The key to set.
 * \param val The value to corresponding to the key.
 * \returns 0 on success, -1 on memory allocation failure.
 */
int bvalue_dict_set(struct bvalue *dict, const char *key, struct bvalue *val);

/*!
 * 获取字典值
 *
 * Lookup a value by key in a dictionary.
 *
 * \param dict The dictionary value to get the value from. Must be of type
               \a BVALUE_DICTIONARY.
 * \param key The key the value is set to in the dictionary.
 * \returns The value or NULL if not found.
 */
const struct bvalue *bvalue_dict_get(const struct bvalue *dict, const char *key);
/*!
 * 获取列表值
 *
 * Get a value by position in a list.
 *
 * \param list The list value to get the value from. Must be of type
               \a BVALUE_LIST.
 * \param pos The position of the value in the list.
 * \returns The value or NULL if \a pos is invalid.
 */
const struct bvalue *bvalue_list_get(const struct bvalue *list, size_t pos);
/*!
 * 获取字符串值
 *
 * Get C string pointer from a value of type \a BVALUE_STRING. The return value
 * is a pointer to the value's own internal storage. The returned string is
 * always null-terminated so it can be used in printf-like functions. However,
 * since it is designed to also store binary (non-ASCII) data, it can contain
 * embedded zeroes. This function will optionnaly return the length of the
 * string in \a len if not NULL.
 *
 * \param val The string value. Must be of type \a BVALUE_STRING.
 * \param len Optional pointer to an integer that will receive the length of
 *            the string.
 * \returns C string or NULL if \a val is invalid.
 */
const unsigned char *bvalue_string(const struct bvalue *val, size_t *len);
/*!
 * 获取整数(int)的值
 *
 * Returns the integer value of a value of type \a BVALUE_INTEGER.
 *
 * \param val The integer value. Must be of type \a BVALUE_INTEGER.
 * \param intval Pointer to an int that will receive the integer value.
 * \returns 0 on success, or -1 if \a val has the wrong type or in case of
 *            overflow.
 */
int bvalue_integer(const struct bvalue *val, int *intval);
/*!
 * 获取整数(long int)的值
 *
 * Returns the integer value of a value of type \a BVALUE_INTEGER.
 *
 * \param val The integer value. Must be of type \a BVALUE_INTEGER.
 * \param intval Pointer to an int that will receive the integer value.
 * \returns 0 on success, or -1 if \a val has the wrong type or in case of
 *            overflow.
 */
int bvalue_integer_l(const struct bvalue *val, long int *intval);
/*!
 * 获取整数(long long int)的值
 *
 * Returns the integer value of a value of type \a BVALUE_INTEGER.
 *
 * \param val The integer value. Must be of type \a BVALUE_INTEGER.
 * \param intval Pointer to an int that will receive the integer value.
 * \returns 0 on success, or -1 if \a val has the wrong type or in case of
 *            overflow.
 */
int bvalue_integer_ll(const struct bvalue *val, long long int *intval);

/*!
 * 分析B编码文件
 *
 * \param stream Stream to parse (FILE pointer).
 * \returns Parsed value or NULL if parsing failed.
 */
struct bvalue *bdecode_file(FILE *stream);
/*!
 * B编码值到文件
 *
 * \param val Value to encode.
 * \param stream Stream to write encoded data to.
 * \returns number of characters written to stream or -1 if write failed.
 */
int bencode_file(const struct bvalue *val, FILE *stream);

/*!
 * 分析B编码字符串缓冲
 *
 * \param buf string buffer to parse.
 * \param len Length to parse.
 * \returns Parsed value or NULL if parsing failed.
 */
struct bvalue *bdecode_buf(const unsigned char *buf, size_t len);
/*!
 * B编码值到一个字符串缓冲
 *
 * \param val Value to encode.
 * \param buf Memory buffer to write encoded data to.
 * \param len Length of the memory buffer.
 * \returns number of characters written to stream or -1 if \a buf is not large
 *          enough.
 */
int bencode_buf(const struct bvalue *val, unsigned char *buf, size_t len);
/*!
 * 分配和B编码值到一个字符串缓冲
 *
 * This function is analog to \ref bencode_buf, except it allocates a buffer
 * large enough to hold the output. The buffered returned in \a bufp should
 * be freed by the application when it is no longer needed.
 *
 * \param val Value to encode.
 * \param bufp Pointer to a location that will receive the address of the
 *             encoded data.
 * \returns number of characters written to stream or -1 if an error occured.
 */
int bencode_buf_alloc(const struct bvalue *val, unsigned char **bufp);

#ifdef __cplusplus
}
#endif

#endif /* BENCODE_H_ */
