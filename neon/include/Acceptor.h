#pragma once
#include <Utilities.h>
#include <Socket.h>
#include <memory>
#include <functional>

namespace Neon {
class Channel;
class EventLoop;
class Acceptor final : NonCopyable {
public:
    using NewConnHandlerType = std::function<void(int)>;
    Acceptor(EventLoop* eventloop);
    void listen_on(const Socket::Endpoint& endpoint,int backlogs = 10);
    const Socket& tcp_socket() const {
        return sock_;
    }

    void set_new_conn_handler(const NewConnHandlerType& cb) {
        new_conn_handler_ = std::move(cb);
    }
    
private:
    EventLoop* eventloop_;
    Socket sock_;
    std::shared_ptr<Channel> chan_;
    NewConnHandlerType new_conn_handler_;
};
}