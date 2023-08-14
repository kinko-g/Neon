#pragma once
#include <sys/socket.h>

#define ASSERT_IF(cond,msg)                                 \
    do {                                                    \
        if(!(cond)) {                                       \
            fprintf(stdout,(msg"\n"));                      \
            exit(0);                                        \
        }                                                   \
    } while(0)                                              \
// 

namespace Neon {
class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    virtual ~NonCopyable() = default;
};

inline int create_socket() {
    auto fd = ::socket(AF_INET,SOCK_STREAM,0);
    return fd;
}

}