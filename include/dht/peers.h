/*
 * Copyright (c) 2020 naturalpolice
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License (see LICENSE).
 */

/**
 * \file peers.h
 * \brief 发布(Announce)或者搜索对等端的种子特征码(infohash)
 *
 * 它包含用于搜索或发布对等端的高级定义。
 */

#ifndef DHT_PEERS_H_
#define DHT_PEERS_H_

#include <stdlib.h>

/*!
 * 对等端搜索完成时回调函数
 *
 * 当对等端搜索完成时，将调用此回调函数。
 *
 * \param info_hash 搜索的目标种子特征码(infohash)。
 * \param peers 对等端的地址数组，如果找不到对等端地址或搜索被取消，则为 NULL。
 * \param count Length of the \a peers array.
 * \param opaque User data pointer passed to \ref dht_get_peers or
 *               \ref dht_announce_peer.
 */
typedef void (*get_peers_callback)(const unsigned char info_hash[20],
                                   const struct sockaddr_storage *peers,
                                   size_t count,
                                   void *opaque);

/*!
 * 在DHT中搜索对等端的种子特征码(infohash)
 *
 * Start a recursive search for peers for the given infohash on the DHT.
 * The returned handle can be used to cancel the pending search with
 * \ref dht_node_cancel. This function is a wrapper for the low-level
 * \ref dht_node_search function.
 *
 * \param node The DHT node.
 * \param info_hash The target infohash of the search.
 * \param callback Function that will be called when the search completes.
 * \param opaque Opaque pointer that will be passed to the callback when the
 *               search completes.
 * \param handle Pointer to a variable that will receive the search handle.
 * \returns 0 if the search sucessfully started, or -1 in case of failure.
 */
int dht_get_peers(struct dht_node *node, const unsigned char info_hash[20],
                  get_peers_callback callback, void *opaque,
                  dht_search_t *handle);

/*!
 * 发布一个种子特征码到对等端
 *
 * Add the IP address of the current node and the given port number to the list
 * of peers for the given infohash. This is traditionally used to announce that
 * the "peer" controlling the current node is downloading a torrent, but other
 * use are possible. If the \a port parameter is -1, the node's current UDP port
 * is used.
 * Since it is necessary to perform a search on the DHT before sending announce
 * queries, the completion callback will be called with the list of already
 * existing peers found for the given infohash.
 * The returned handle can be used to cancel the pending announce with
 * \ref dht_node_cancel. This function is a wrapper for the low-level
 * \ref dht_node_announce function.
 *
 * \param node The DHT node.
 * \param info_hash The target infohash of the announce.
 * \param port Peer port number to announce, or -1 to use the nodes's current
               UDP port number.
 * \param callback Function that will be called when the announce is complete.
 * \param opaque Opaque pointer that will be passed to the callback when the
                 announce is complete.
 * \param handle Pointer to a variable that will receive the search handle.
 * \returns 0 if the announce sucessfully started, or -1 in case of failure.
 */
int dht_announce_peer(struct dht_node *node, const unsigned char info_hash[20],
                      int port, get_peers_callback callback, void *opaque,
                      dht_search_t *handle);

#endif /* DHT_PEERS_H_ */
