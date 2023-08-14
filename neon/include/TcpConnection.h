#pragma once
#include <Utilities.h>
#include <Socket.h>
#include <memory>

namespace Neon {
class Channel;
class TcpConnection final : public NonCopyable{
public:
    TcpConnection() = default;
    const Socket& tcp_socket() const {
        return sock_;
    }
    void start_read();
private:
    std::shared_ptr<Channel> chan_;
    Socket sock_;
};
}