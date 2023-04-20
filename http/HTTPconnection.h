#ifndef __HTTP_CONNECTION_H
#define __HTTP_CONNECTION_H

#include <arpa/inet.h>      // sockaddr_in
#include <sys/uio.h>        // readv/writev
#include <iostream>
#include <sys/types.h>
#include <assert.h>

#include "../buffer/buffer.h"
#include "HTTPrequest.h"
#include "HTTPresponse.h"

class HTTPconnection
{
public:
    HTTPconnection();
    ~HTTPconnection();

    void initHTTPConn(int socketFd, const sockaddr_in& addr);

    // 每个连接中的缓冲区读写接口
    ssize_t readBuffer(int* saveErrno);
    ssize_t writeBuffer(int* saveErrno);

    // 关闭HTTP连接
    void closeHTTPConn();
    // 处理HTTP连接的接口，分为request解析和reponse生成
    bool process();

    const char* getIP() const;          // 获取IP信息
    int getPort() const;
    int getFd() const;          // 获取HTTP连接描述符
    sockaddr_in getAddr() const;

    int writeBytes()
    {
        return iov_[1].iov_len + iov_[0].iov_len;
    }

    bool isKeepAlive() const
    {
        return request_.isKeepAlive();
    }

    static bool isET;
    static const char* srcDir;          // 当前目录的路径
    static std::atomic<int> userCount;      // HTTP连接个数

private:
    int fd_;        // HTTP连接对应的连接符
    struct sockaddr_in addr_;
    bool isClose_;

    int iovCnt_;
    struct iovec iov_[2];

    Buffer readBuffer_;     // 读缓冲区
    Buffer writeBuffer_;    // 写缓冲区

    HTTPrequest request_;
    HTTPresponse response_;
};

#endif