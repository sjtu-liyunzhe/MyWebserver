#ifndef _BUFFER_H
#define _BUFFER_H

#include <vector>
#include <iostream>
#include <cstring>
#include <unistd.h>     // read() write()
#include <sys/uio.h>    // readv()  write()
#include <memory>
#include <atomic>
#include <assert.h>

class Buffer
{
public:
    Buffer(int initBufferSize = 1024);
    ~Buffer() = default;


    inline size_t writeMaxBytes() const;       // 缓冲区可以写入的字节数
    inline size_t readMaxBytes() const;        // 缓冲区可以读取的字节数
    inline size_t readBytes() const;           // 已经读取的字节数

    inline const char* curReadPtr() const;     // 获取当前读指针
    inline const char* curWritePtrConst() const;       // 写指针
    inline char* curWritePtr(); 

    // 更新指针
    inline void updateReadPtr(size_t len);
    inline void updateReadPtrToEnd(const char* end);
    inline void updateWritePtr(size_t len);
    inline void initPtr();     // 初始化智能指针

    // 处理http所需接口
    void ensureWriteAll(size_t len);      // 保证数据写入缓冲区
    void append(const char* str, size_t len);
    void append(const std::string& str);
    void append(void* data, size_t len);
    void append(const Buffer& buffer);



    // IO操作接口
    ssize_t readFd(int fd, int* Errno);
    ssize_t writeFd(int fd, int* Errno);

    // 将缓冲区数据转换为字符串
    std::string alltoString();

    void printContent()
    {
        std::cout << "pointer info: " << readPos_ << " " << writePos_ << std::endl;
        for(int i = readPos_; i < writePos_; ++i)
        {
            std::cout << buffer_[i] << " ";
        }
        std::cout << std::endl;
    }

private:

    inline char* beginPtr_();
    inline const char* beginPtr_() const;
    // 扩容
    void reserveSpace(size_t len);

    std::vector<char> buffer_;
    std::atomic<std::size_t> readPos_;      // 指示读指针
    std::atomic<std::size_t> writePos_;     // 指示写指针

};


#endif