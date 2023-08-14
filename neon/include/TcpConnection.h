#pragma once
#include <Utilities.h>
#include <Socket.h>
#include <memory>

namespace Neon {
class Channel;
class EventLoop;
class TcpConnection final : public NonCopyable{
public:
    TcpConnection(int fd,EventLoop *eventloop);
    Socket& tcp_socket() {
        return sock_;
    }
    void start_read();
private:
    Socket sock_;
    std::shared_ptr<Channel> chan_;
    EventLoop* eventloop_;
};
}