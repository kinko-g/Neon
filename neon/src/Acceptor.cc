#include <Acceptor.h>
#include <EventLoop.h>
#include <Channel.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace Neon {
Acceptor::Acceptor(EventLoop* eventloop) :
    eventloop_{eventloop},
    sock_{create_socket()},
    chan_{std::make_shared<Channel>(sock_.fd(),eventloop_)} {
    
    sock_.set_nonblocking();
    sock_.set_reuse_addr();
}

void Acceptor::listen_on(const Socket::Endpoint& endpoint,int backlogs) {
    auto ret = sock_.listen_on(endpoint,backlogs);
    ASSERT_IF(ret,"Acceptor listen error");

    chan_->set_read_handler([this]{
        // sockaddr_in addr{};
        // socklen_t addr_len = sizeof(addr);
        auto fd = ::accept(this->chan_->fd(),nullptr,nullptr);
        if(fd > 0 && this->new_conn_handler_) {
            this->new_conn_handler_(fd);
        } 
        else {
            ::close(fd);
        }
    });
    eventloop_->queue_in_loop([this]{
        fprintf(stdout,"acceptor enable reading\n");
        this->chan_->enable_reading();
    });
}

}