#include "./buffer.h"

Buffer::Buffer(int initBufferSize):
    buffer_(initBufferSize), readPos_(0), writePos_(0)
    {}

inline size_t Buffer::readMaxBytes() const
{
    return writePos_ - readPos_;
}

inline size_t Buffer::writeMaxBytes() const
{
    return buffer_.size() - writePos_;
}

inline size_t Buffer::readBytes() const
{
    return readPos_;
}

inline const char* Buffer::curReadPtr() const
{
    return beginPtr_() + readPos_;
}

inline const char* Buffer::curWritePtrConst() const
{
    return beginPtr_() + writePos_;
}

inline char* Buffer::curWritePtr()
{
    return beginPtr_() + writePos_;
}

inline void Buffer::updateReadPtr(size_t len)
{
    assert(len < readMaxBytes());
    readPos_ += len;
}

inline void Buffer::updateReadPtrToEnd(const char* end)
{
    assert(end >= curReadPtr());
    updateReadPtr(end - curReadPtr());
}

inline void Buffer::updateWritePtr(size_t len)
{
    assert(len <= writeMaxBytes());
    writePos_ += len;
}

inline void Buffer::initPtr()
{
    bzero(&buffer_[0], buffer_.size());
    readPos_ = 0;
    writePos_ = 0;
}

void Buffer::reserveSpace(size_t len)
{
    if(writeMaxBytes() + readMaxBytes() <= len)
    {
        buffer_.resize(len);
    }
    else
    {
        size_t readMax = readMaxBytes();
        std::copy(beginPtr_() + readPos_, beginPtr_() + writePos_, beginPtr_());
        readPos_ = 0;
        writePos_ = readMax;
        assert(readMax == readMaxBytes());
    }
}

void Buffer::ensureWriteAll(size_t len)
{
    if(writeMaxBytes() < len)
    {
        reserveSpace(len);
    }
    assert(writeMaxBytes() >= len);
}

void Buffer::append(const char* str, size_t len)
{
    assert(str);
    ensureWriteAll(len);
    std::copy(str, str + len, curWritePtr());
    updateWritePtr(len);
}

void Buffer::append(const std::string& str)
{
    append(str.data(), str.size());
}

void Buffer::append(void* data, size_t len)
{
    assert(data);
    append(static_cast<const char*>(data), len);
}

void Buffer::append(const Buffer& buffer)
{
    append(buffer.curReadPtr(), buffer.readMaxBytes());
}

ssize_t Buffer::readFd(int fd, int* Errno)
{
    char tmpBuff[65535];
    struct iovec iov[2];        // 分散/聚集IO操作
    const size_t writeMax = writeMaxBytes();
    // 分散读，保证数据全读完
    iov[0].iov_base = beginPtr_() + writePos_;
    iov[0].iov_len = writeMax;
    iov[1].iov_base = tmpBuff;
    iov[1].iov_len = sizeof(tmpBuff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0)
    {
        *Errno = errno;
    }
    else if(static_cast<size_t>(len) <= writeMax)
    {
        writePos_ += len;
    }
    else
    {
        writePos_ = buffer_.size();             // 这里不明白
        append(tmpBuff, len - writeMax);
    }
    return len;
}

ssize_t Buffer::writeFd(int fd, int* Errno)
{
    ssize_t readSize = readMaxBytes();
    ssize_t len = write(fd, curReadPtr(), readSize);
    if(len < 0)
    {
        *Errno = errno;
        return len;
    }
    readPos_ += len;
    return len;
}

std::string Buffer::alltoString()
{
    std::string str(curReadPtr(), readMaxBytes());
    initPtr();
    return str;
}

inline char* Buffer::beginPtr_()
{
    return &*buffer_.begin();
}

inline const char* Buffer::beginPtr_() const
{
    return &*buffer_.cbegin();
}



