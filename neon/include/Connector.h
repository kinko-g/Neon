#pragma once
#include <TcpConnection.h>
#include <Utilities.h>
namespace Neon {
class EventLoop;
class Connector : public NonCopyable{
public:
    Connector(EventLoop* eventloop);
    using OnConnectedType = std::function<void(std::shared_ptr<TcpConnection>)>;
    void on_connected(const OnConnectedType& cb) {
        on_conn_cb_ = std::move(cb);
    }

    void send_message(const std::string& data);
    void connect_to(const Socket::Endpoint& endpoint);
    void close();
private:
    EventLoop* eventloop_;
    std::shared_ptr<TcpConnection> conn_;
    OnConnectedType on_conn_cb_;
};
}