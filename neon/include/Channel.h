#pragma once
#include <Utilities.h>
#include <memory>
#include <sys/epoll.h>
#include <functional>

namespace Neon {
class EventLoop;
class Channel final : public NonCopyable,public std::enable_shared_from_this<Neon::Channel>{
public:
    using HandlerType = std::function<void()>;
    Channel(int fd,EventLoop* eventloop);

    void set_revents(int events) {
        revents_ = events;
    }
    void enable_reading() {
        events_ |= EPOLLIN;
        update();
    }
    void enable_wrting() {
        events_ |= EPOLLOUT;
        update();
    }
    void disable_reading() {
        events_ &= ~EPOLLIN;
        update();
    }
    void disable_writing() {
        events_ &= ~EPOLLOUT;
        update();
    }
    void disable() {
        events_ = 0;
        update();
    }
    bool is_reading() {
        return events_ & EPOLLIN;
    }
    bool is_writeing() {
        return events_ & EPOLLOUT;
    }

    int events() {
        return events_;
    }

    void set_read_handler(const HandlerType& cb) {
        read_handler_ = std::move(cb);
    }
    void set_write_handler(const HandlerType& cb) {
        write_handler_ = std::move(cb);
    }
    void set_error_handler(const HandlerType& cb) {
        error_handler_ = std::move(cb);
    }
    void set_close_handler(const HandlerType& cb) {
        close_handler_ = std::move(cb);
    }
    int fd() {
        return fd_;
    }
    void set_fd(int fd) {
        fd_ = fd;
    }
    void handle_events();
private:
    void handle_read();
    void handle_write();
    void handle_errro();
    void handle_close();
    void handle_conn();
    void update();
private:
    int fd_;
    int events_;
    int revents_;
    EventLoop *eventloop_;
    HandlerType write_handler_;
    HandlerType read_handler_;
    HandlerType close_handler_;
    HandlerType error_handler_;
};

using ChannelSP = std::shared_ptr<Channel>;
}