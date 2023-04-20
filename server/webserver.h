#ifndef __WEBSERVER_H
#define __WEBSERVER_H

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../epoller/epoller.h"
#include "../timer/timer.h"
#include "../threadpool/threadpool.h"
#include "../http/HTTPconnection.h"


class WebServer
{
public:
    WebServer(int port, int trigMode, int timeoutMs, bool optLinger, int threadNum);
    ~WebServer();

    void start();

private:
    bool initSocket_();         // 初始化socket，得到listenFd
    void initEventMode_(int trigMode);          // 初始化事件模式

    void addClientConnection(int fd, sockaddr_in addr);     // 添加HTTP连接
    void closeConn_(HTTPconnection* clinet);                // 关闭HTTP连接

    void handleListen_();       // 收到新的HTTP请求

    // 已经建立连接的HTTP发来IO请求，将读写加入线程池
    void handleWrite_(HTTPconnection* client);
    void handleRead_(HTTPconnection* client);

    // 将读写底层函数
    void onRead_(HTTPconnection* client);
    void onWrite_(HTTPconnection* client);
    void onProcess_(HTTPconnection* client);

    void sendError_(int fd, const char* info);
    void extentTime_(HTTPconnection* client);

    static const int MAX_FD = 65536;
    static int setFdNonblock(int fd);

    int port_;
    int timeoutMs_;     // 定时器默认过期时间
    bool isClose_;
    int listenFd_;
    int openLinger_;
    char* srcDir_;

    uint32_t listenEvent_;
    uint32_t connectionEvent_;

    std::unique_ptr<TimerManager> timer_;
    std::unique_ptr<Threadpool> threadpool_;
    std::unique_ptr<Epoller> epoller_;
    std::unordered_map<int, HTTPconnection> users_;

};
#endif