#ifndef _EPOLLER_H
#define _EPOLLER_H

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <errno.h>


class Epoller
{
public:
    explicit Epoller(int maxEvent = 1024);
    ~Epoller();


    // 将fd加入监测
    bool addFd(int fd, uint32_t events);
    // 修改fd对应时间
    bool modFd(int fd, uint32_t events);
    // 移除监测fd
    bool delFd(int fd);
    // 返回监测结果
    int wait(int timewait = -1);        // 调用成功返回fd个数


    // 获取fd函数
    int getEventFd(size_t i) const;
    // 获取events函数
    uint32_t getEvents(size_t i) const;

private:
    int epollerFd_;      // 标志epoll的描述符
    std::vector<struct epoll_event> events_;        // 就绪的事件
};

#endif