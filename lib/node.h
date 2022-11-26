/*
 * Copyright (c) 2020 naturalpolice
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License (see LICENSE).
 */

#ifndef NODE_H
#define NODE_H

#define SEARCH_RESULT_MAX 8

// 搜索结构
struct search {
    unsigned char id[20]; // 编号
    uint16_t tid;
    struct timeval next_query; // 下一查询结构
    int search_type; // 搜索类型
    struct search_node *queue; // 搜索节点队列结构
    size_t node_count; // 节点总数
    search_complete_t callback; // 搜索完成的回调函数
    void *opaque;
    struct search *next; // 下一个搜索
    struct search **pprev; // 上一个搜索
};

// 桶的条目结构
struct bucket_entry {
    unsigned char id[20]; // 编号
    struct sockaddr_storage addr; // 地址
    socklen_t addrlen; // 地址长度
    struct timeval last_seen; // 最后一个
    struct timeval next_ping; // 下一个Ping
    int pinged;
};

#define BUCKET_ENTRY_MAX 8

// 桶的结构
struct bucket {
    unsigned char first[20]; // 第一个
    struct bucket_entry nodes[BUCKET_ENTRY_MAX]; // 桶条目，节点数
    size_t cnt;
    struct timeval refresh_time; // 刷新时间
    struct bucket *next; // 下一桶
    struct search *refresh; // 刷新搜索
};

// 对等端的结构
struct peer {
    struct sockaddr_storage addr; // 地址
    socklen_t addrlen; // 地址长度
    struct timeval expire_time; // 过期时间
    struct peer *next; // 下一个对等端
};

// 对等端列表的结构
struct peer_list {
    unsigned char info_hash[20]; // 种子散列值
    struct peer *peers; // 对等端
    struct peer_list *next; // 下一个对等端列表
};

// 放置项的结构
struct put_item {
    unsigned char hash[20]; // 散列值
    unsigned char k[32];
    int seq;
    unsigned char sig[64];
    struct bvalue *v;
    struct timeval expire_time; // 过期时间
    struct put_item *next; // 下一个放置项
};

// 桶节点超时
static const struct timeval bucket_node_timeout = {
    .tv_sec = 15 * 60,
    .tv_usec = 0
};

// 桶刷新超时
static const struct timeval bucket_refresh_timeout = {
    .tv_sec = 15 * 60,
    .tv_usec = 0
};

// 重复搜索超时
static const struct timeval search_iteration_timeout = {
    .tv_sec = 1,
    .tv_usec = 0
};

// 对等端超时
static const struct timeval peer_timeout = {
    .tv_sec = 2 * 60 * 60,
    .tv_usec = 0
};

// 搜索查询超时
static const struct timeval search_query_timeout = {
    .tv_sec = 10,
    .tv_usec = 0,
};

// Ping超时
static const struct timeval ping_timeout = {
    .tv_sec = 10,
    .tv_usec = 0,
};

// 放置超时
static const struct timeval put_timeout = {
    .tv_sec = 2 * 60 * 60,
    .tv_usec = 0,
};

#define SAVE_FILE_VERSION 2

#endif /* NODE_H */
