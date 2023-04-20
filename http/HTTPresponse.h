#ifndef __HTTP_RESPONSE_H
#define __HTTP_RESPONSE_H

#include <unordered_map>
#include <fcntl.h>  //open
#include <unistd.h> //close
#include <sys/stat.h> //stat
#include <sys/mman.h> //mmap,munmap
#include <assert.h>

#include "../buffer/buffer.h"

class HTTPresponse
{
public:
    HTTPresponse();
    ~HTTPresponse();

    void init(const std::string& srcDir, std::string& path, bool isKeepAliva = false, int code = -1);
    void makeResponse(Buffer& buffer);
    void unmapFile_();
    char* file();
    size_t fileLen() const;
    void errorContent(Buffer& buffer, std::string message);         // 请求文件打不开
    int code() const { return code_; }

private:
    void addStateLine_(Buffer& buffer);
    void addResponseHeader_(Buffer& buffer);
    void addResponseContent_(Buffer& buffer);

    void errorHTML_();
    std::string getFileType_();

    int code_;                  // HTTP状态
    bool isKeepAlive_;

    std::string path_;          // 相对路径
    std::string srcDir_;        // 根目录

    char* mmFile_;              // 指示共享内存
    struct stat mmFileStat_;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;      //  后缀 -> 文件类型
    static const std::unordered_map<int, std::string> CODE_STATUS;              // 状态码 -> 状态
    static const std::unordered_map<int, std::string> CODE_PATH;
};

#endif