#include <Channel.h>
#include <EventLoop.h>

namespace Neon {
Channel::Channel(int fd,EventLoop* eventloop) : 
    fd_{fd},
    eventloop_{eventloop} {

}

void Channel::handle_events() {
    // do epoll events with revents
    // 服务端出错
    // fprintf(stdout,"handle events ,fd : %d,revents : %d",fd_,revents_);
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        handle_close();
        return;
    }
    if(revents_ & EPOLLERR) {
        handle_errro();
        return;
    }
    // 有数据或对端关闭
    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        handle_read();
    }
    if(revents_ & EPOLLOUT) {
        handle_write();
    }
    handle_conn();
}

void Channel::handle_read() {
    if(read_handler_) {
        read_handler_();
    }
}

void Channel::handle_write() {
    if(write_handler_) {
        write_handler_();
    }
}

void Channel::handle_errro() {
    if(error_handler_) {
        error_handler_();
    }
}

void Channel::handle_close() {
    if(close_handler_) {
        close_handler_();
    }
}

void Channel::handle_conn() {

}

void Channel::update() {
    // update poller events
    auto sp = shared_from_this();
    eventloop_->update_channel(sp);
}

}
