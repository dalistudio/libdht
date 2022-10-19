libdht
======

[![Build Status](https://travis-ci.com/naturalpolice/libdht.svg?branch=master)](https://travis-ci.com/naturalpolice/libdht)

`libdht`是基于Kademlia的分布式哈希表（DHT）的C实现，用于BitTorrent网络（又名“mainline DHT”）。

特点
--------

* 完整的DHT节点实现。
* Search peers for an infohash. Announce as peer for an infohash.
  [BEP-5](https://www.bittorrent.org/beps/bep_0005.html)
* 保存/恢复节点状态和路由表。
* 支持DHT安全扩展（节点ID强化）。
  [BEP-42](https://www.bittorrent.org/beps/bep_0042.html)
* 不可变和可变的任意数据存储。
  [BEP-44](https://www.bittorrent.org/beps/bep_0044.html)
* 支持IPv6 [BEP-32](https://www.bittorrent.org/beps/bep_0032.html)
* 不依赖于任何外部组件/库。
* 独立于网络API，可以以事件驱动或阻塞方式使用。
* Lua 绑定库

许可
-------

libdht是根据MIT许可证分发的（参见许可证）。当前源代码还包含以下外部项目的代码：

* 基于SUPERCOP "ref10" 的 Ed25519 实现。 (https://github.com/orlp/ed25519)， zlib 许可。
* 来自 Mbed TLS (https://tls.mbed.org)的 SHA1 实现 (https://tls.mbed.org)，Apache 2.0 许可。
* 基于 Intel 的 Slicing-by-8 sourceforge project (https://sourceforge.net/projects/slicing-by-8/) CRC32C (cagtagnoli)的实现， BSD 许可。

依赖关系
------------

libdht仅依赖于以下库：

* cmocka: 构建单元测试时所需 (可选)。
  sudo apt install libcmocka-dev
  
* lua: 构建Lua绑定库时所需 (可选)。
  sudo apt install liblua5.4-dev lua5.4

它不需要任何其他外部库。

支持的平台
-------------------

到目前为止，libdht已经在以下方面成功构建和测试：

* Ubuntu Linux (16.04+)
* Windows 10 (MSVC/Visual Studio Build Tools 2017)

文档
-------------
Doxygen为库API生成的文档可在线获取，网址为 [https://naturalpolice.github.io/libdht/](https://naturalpolice.github.io/libdht/).

  sudo apt install doxygen

运行一个 DHT 节点
------------------

下面是一个开始操作基本DHT节点的示例。有关更复杂的场景，请参阅文档的其余部分。

    #include <stdlib.h>
    #include <stdio.h>
    #include <errno.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <netinet/in.h>

    #include <dht/node.h>

    // 发送 udp 数据
    static void sock_send(const unsigned char *data, size_t len,
                          const struct sockaddr *dest, socklen_t addrlen,
                          void *opaque)
    {
        int sock = *(int *)opaque;

        if (sendto(sock, data, len, 0, dest, addrlen) < 0)
            fprintf(stderr, "sendto: %s\n", strerror(errno));
    }

    // 运行节点
    int node_run(void)
    {
        int sock = socket(AF_INET, SOCK_DGRAM, 0); // 基于UDP的socket
        struct sockaddr_in sin;
        struct dht_node node; // 节点的结构

        sin.sin_family = AF_INET; // IP:PORT格式
        sin.sin_addr.s_addr = INADDR_ANY; // 任意地址 0.0.0.0
        sin.sin_port = htons(6881); // 设置端口

        // 绑定端口
        bind(sock, (struct sockaddr *)&sin, sizeof(sin));

        // 初始化节点
        if (dht_node_init(&node, NULL, sock_send, &sock))
            return -1;

        // 启动节点
        dht_node_start(&node);

        // 死循环
        while (1) {
            struct timeval tv;
            fd_set rfds;
            int rc;

            FD_ZERO(&rfds);
            FD_SET(sock, &rfds);

            // 节点超时
            dht_node_timeout(&node, &tv);
            rc = select(sock + 1, &rfds, NULL, NULL, &tv);
            if (rc < 0) {
                fprintf(stderr, "select: %s\n", strerror(errno));
                return -1;
            }
            if (rc && FD_ISSET(sock, &rfds)) {
                unsigned char buf[2048];
                struct sockaddr_storage ss;
                socklen_t sl = sizeof(ss);

                // 接收 UDP 数据
                rc = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&ss, &sl);
                if (rc < 0) {
                    fprintf(stderr, "recvfrom: %s\n", strerror(errno));
                    return -1;
                }

                // 节点输入
                dht_node_input(&node, buf, rc, (struct sockaddr *)&ss, sl);
            }

            // 节点工作
            dht_node_work(&node);
        }

        return 0;
    }
