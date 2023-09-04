#include <Connector.h>
#include <arpa/inet.h>
#include <cstdio>
#include <EventLoop.h>
namespace Neon {

Connector::Connector(EventLoop* eventloop) 
    : eventloop_{eventloop} {

}

void Connector::connect_to(const Socket::Endpoint& endpoint) {
    auto fd = ::socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(endpoint.port);
    auto ret = inet_pton(AF_INET,endpoint.ip.c_str(),&addr.sin_addr);
    ret = ret && (connect(fd,(sockaddr*)&addr,sizeof(addr)) == 0);
    conn_ = std::make_shared<TcpConnection>(fd,eventloop_);
    if(ret) {
        if(on_conn_cb_) {
            on_conn_cb_(conn_);
        }
        conn_->set_state(TcpConnection::State::CONNECTED);
    }
    else {
        // on error
        return;
    }
    conn_->start_read();
    conn_->on_close([]{
        printf("connection closeed\n");
    });
    conn_->on_error([]{
        printf("connection error\n");
    });
    conn_->on_message([](std::vector<char>& data) {
        printf("connection on message\n");
    });
}

void Connector::send_message(const std::string& data) {
    if(conn_) {
        conn_->send(data);
    }
}

void Connector::close() {
    conn_->close();
}

}