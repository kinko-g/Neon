#include <TcpConnection.h>
#include <Channel.h>
#include <EventLoop.h>
#include <Socket.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace Neon {
TcpConnection::TcpConnection(int fd,EventLoop *eventloop)
    :sock_{fd},
    chan_{std::make_shared<Channel>(fd,eventloop)},
    eventloop_{eventloop},
    read_buf_(1024,0),
    state_{State::DISCONNECTED} {
    
    sock_.set_nonblocking();
    chan_->set_read_handler(std::bind(&TcpConnection::read_handler,this));
    chan_->set_write_handler(std::bind(&TcpConnection::write_handler,this));
}

void TcpConnection::start_read() {
    eventloop_->queue_in_loop([this]{
        if(!this->chan_->is_reading()) {
            this->chan_->enable_reading();
        }
    });
}

void TcpConnection::read_handler() {
    state_ = State::READING;
    read_buf_.clear();
    auto n = ::read(sock_.fd(),&*read_buf_.begin(),read_buf_.size());
    // fprintf(stdout,"conn read , n : %ld,fd : %d",n,chan_->fd());
    if(n > 0 && on_read_cb_) {
        on_read_cb_(read_buf_);
    }
    if(n == 0 && on_close_cb_) {
        on_close_cb_();
    }
    if(n < 0) {
        state_ = State::ERROR;
        if(on_error_cb_) {
            on_error_cb_();
        }
        return;
    }
    state_ = State::CONNECTED;
}

void TcpConnection::send(const std::string& message) {
    send_buffers_.push({message,message.size()});
    chan_->enable_wrting();
}

void TcpConnection::write_handler() {
    state_ = State::SENDING;
    if(send_buffers_.empty()) {
        chan_->disable_writing();
    }
    auto& msg_node = send_buffers_.front();
    auto n = ::write(sock_.fd(),
        msg_node.data.data() + (msg_node.data.size() - msg_node.remain),
        msg_node.remain
    );
    msg_node.remain -= n;
    if(msg_node.remain == 0) {
        send_buffers_.pop();
        if(on_write_completed_cb_) {
            on_write_completed_cb_(shared_from_this());
        }
    }
    if(n < 0) {
        if(on_error_cb_) {
            on_error_cb_();
        }
        state_ = State::ERROR;
        return;
    }
    state_ = State::CONNECTED;
}

void TcpConnection::close_handler() {
    chan_->disable();
    state_ = State::DISCONNECTED;
    if(on_close_cb_) {
        on_close_cb_();
    }
}

void TcpConnection::close() {
    eventloop_->queue_in_loop([chan = chan_]{
        chan->disable();
    });
    this->state_ = State::DISCONNECTED;
}

TcpConnection::~TcpConnection() {
    close();
}

// std::shared_ptr<TcpConnection> TcpConnection::connect_to(const Socket::Endpoint& endpoint,EventLoop* eventloop) {
//     auto fd = ::socket(AF_INET,SOCK_STREAM,0);
//     sockaddr_in addr{
//         .sin_family = AF_INET,
//         .sin_port = htons(endpoint.port)
//     };
//     auto ret = inet_pton(AF_INET,endpoint.ip.c_str(),&addr.sin_addr);
//     ret = ret && (connect(fd,(sockaddr*)&addr,sizeof(addr)) == 0);
//     auto conn = std::make_shared<TcpConnection>(fd,eventloop);
//     if(ret) {
//         conn->state_ = State::CONNECTED;
//     }
//     return conn;
// }


}