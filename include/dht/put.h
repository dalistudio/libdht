/*
 * Copyright (c) 2020 naturalpolice
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License (see LICENSE).
 */

/**
 * \file put.h
 * \brief 获取或放置可变/不可变的项。
 *
 * 该文件包含使用BEP-44 DHT协议扩展从DHT存储和检索不可变和可变数据的高级定义。
 */

#ifndef DHT_PUT_H_
#define DHT_PUT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <dht/node.h>

/*!
 * Get 完成时回调函数
 *
 * 当不可变或可变的DHT执行 get 操作完成时，将调用此回调。
 *
 * \param val 从DHT检索到的值，如果未找到或搜索被取消，则返回NULL。
 * \param opaque 传递给dht_get_immutable或dht_get_ mutable的用户数据指针。
 */
typedef void (*get_callback)(const struct bvalue *val, void *opaque);

/*!
 * Put immutable callback
 *
 * This callback is called when an immutable DHT put operation completes.
 *
 * \param result 0 when the operation succeeded, -1 if no nodes were found or
                 the search was cancelled.
 * \param opaque User data pointer passed to \ref dht_put_immutable.
 */
typedef void (*put_immutable_callback)(int result, void *opaque);

/*!
 * Put 不可变的回调函数
 *
 * This callback is called when a mutable DHT put operation is about to
 * complete, just before sending put queries to the nodes selected to store
 * mutable data. A copy of the data about to be overwritten in the DHT (if any)
 * will be in the location pointed to by \a val. This gives the callback a
 * chance to choose what data to store in the DHT based on the previous value
 * stored, or to abort the operation altogether. The callback must return the
 * value to be stored in the DHT at the same location. If the content of \a val
 * is overwritten, it becomes the responsibility of the callee to free the
 * previous pointer. The value found in \a val will be freed by the library
 * after sending the put queries, unless it is NULL, in which case the put
 * operation will be aborted.
 *
 * \param val Pointer to a location containing the previous value retrieved
 *            from the DHT, should be updated to the new value to store upon
 *            return. NULL if the search was cancelled.
 * \param opaque User data pointer passed to \ref dht_put_mutable.
 */
typedef void (*put_mutable_callback)(struct bvalue **val, void *opaque);

/*!
 * 从DHT中获取一个不可变的值。
 *
 * Start a recursive search for an immutable value matching the given
 * hash on the DHT.
 * The returned handle can be used to cancel the pending search with
 * \ref dht_node_cancel. This function is a wrapper for the low-level
 * \ref dht_node_search function.
 *
 * \param node The DHT node.
 * \param hash The target hash of the search.
 * \param callback Function that will be called with the value retrieved from
                   the DHT when the search is complete.
 * \param opaque Opaque pointer that will be passed to the callback when the
 *               operation completes.
 * \param handle Pointer to a variable that will receive the search handle.
 * \returns 0 if the search sucessfully started, or -1 in case of failure.
 */
int dht_get_immutable(struct dht_node *node, const unsigned char hash[20],
                      get_callback callback, void *opaque,
                      dht_search_t *handle);

/*!
 * 将一个不可变的值放入DHT。
 *
 * Start a recursive search for nodes candidates for storing the specified
 * value on the DHT. Once the search completes, the specified completion
 * callback will be called, then put queries will be finally sent to
 * those nodes.
 * This function returns the hash that can be used to retrieve the value
 * stored on the DHT.
 * The returned handle can be used to cancel the pending search with
 * \ref dht_node_cancel. This function is a wrapper for the low-level
 * \ref dht_node_search function.
 *
 * \param node The DHT node.
 * \param v The value that will be stored.
 * \param callback Function that will be called when the operation completes.
 * \param opaque Opaque pointer that will be passed to the callback when the
 *               operation completes.
 * \param handle Pointer to a variable that will receive the get handle.
 * \param hash The hash of the stored value.
 * \returns 0 if the search sucessfully started, or -1 in case of failure.
 */
int dht_put_immutable(struct dht_node *node, const struct bvalue *v,
                      put_immutable_callback callback, void *opaque,
                      dht_search_t *handle, unsigned char hash[20]);

/*!
 * 从DHT获取可变值。
 *
 * Start a recursive search for an mutable value matching the given
 * public key and salt on the DHT.
 * The returned handle can be used to cancel the pending search with
 * \ref dht_node_cancel. This function is a wrapper for the low-level
 * \ref dht_node_search function.
 *
 * \param node The DHT node.
 * \param pubkey The target ed25519 public key of the search.
 * \param salt The target salt data of the search.
 * \param salt_len Length of the salt data (max=64).
 * \param callback Function that will be called with the value retrieved from
                   the DHT when the search is complete.
 * \param opaque Opaque pointer that will be passed to the callback when the
 *               operation completes.
 * \param handle Pointer to a variable that will receive the search handle.
 * \returns 0 if the search sucessfully started, or -1 in case of failure.
 */
int dht_get_mutable(struct dht_node *node,
                    const unsigned char pubkey[32],
                    const unsigned char *salt, size_t salt_len,
                    get_callback callback, void *opaque,
                    dht_search_t *handle);

/*!
 * 给DHT加一个可变值。
 *
 * Start a recursive search for nodes candidates for storing a mutable
 * value for the specified public key and salt on the DHT.
 * Once the search completes, the specified callback will be called with the
 * value currently found on the DHT. The DHT will then be updated with the value
 * returned by the callback.
 * The returned handle can be used to cancel the pending search with
 * \ref dht_node_cancel. This function is a wrapper for the low-level
 * \ref dht_node_search function.
 *
 * \param node The DHT node.
 * \param secret The ed25519 secret key used to authenticate put queries.
 * \param pubkey Public counterpart of \a secret.
 * \param salt Salt data to allow multiple stores per public key.
 * \param salt_len Length of the salt data (max=64).
 * \param callback Function that will be called before sending put queries.
 * \param opaque Opaque pointer that will be passed to the callback before sending.
 * \param handle Pointer to a variable that will receive the search handle.
 * \returns 0 if the search sucessfully started, or -1 in case of failure.
 */
int dht_put_mutable(struct dht_node *node,
                    const unsigned char secret[64],
                    const unsigned char pubkey[32],
                    const unsigned char *salt, size_t salt_len,
                    put_mutable_callback callback, void *opaque,
                    dht_search_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* DHT_PUT_H_ */
