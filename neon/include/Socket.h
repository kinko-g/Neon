#pragma once
#include <string>
#include <Utilities.h>
namespace Neon {
class Socket final : public NonCopyable {
public:
    struct Endpoint {
        std::string ip;
        unsigned short port;
    };
    Socket(int fd);
    ~Socket();
    int fd() {
        return fd_;
    }
    bool listen_on(const Endpoint& endpoint,int backlogs);
    bool set_nonblocking(bool flag = true);
    bool set_reuse_addr(bool flag = true);
    bool is_blocking();
    bool is_reused();
    Endpoint address();
private:
    int fd_;
};
}