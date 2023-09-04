#pragma once
#include <Utilities.h>
#include <Socket.h>
#include <memory>
#include <string>
#include <memory>
#include <queue>
#include <functional>

namespace Neon {
class Channel;
class EventLoop;
class TcpConnection final : public NonCopyable,public std::enable_shared_from_this<TcpConnection> {
private:
    struct MessageNode {
        std::string data;
        size_t remain;
    };
public:
    enum class State : int {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        SENDING,
        READING,
        ERROR
    };
    using OnReadType = std::function<void(std::vector<char>&)>;
    using OnCloseType = std::function<void()>;
    using OnErrorType = OnCloseType;
    using OnWriteCompletedType = std::function<void(std::shared_ptr<TcpConnection>)>;
    // using OnWriteCompletedType = OnConnectedType;

    TcpConnection(int fd,EventLoop *eventloop);
    ~TcpConnection();
    Socket& tcp_socket() {
        return sock_;
    }
    void start_read();

    void on_message(const OnReadType& cb) {
        on_read_cb_ = std::move(cb);
    }
    void on_close(const OnCloseType& cb) {
        on_close_cb_ = std::move(cb);
    }
    void on_error(const OnCloseType& cb) {
        on_error_cb_ = std::move(cb);
    }
    // void on_connected(const OnConnectedType& cb) {
    //     on_conn_cb_ = std::move(cb);
    // }
    void on_write_completed(const OnWriteCompletedType& cb) {
        on_write_completed_cb_ = std::move(cb);
    }
    void set_state(State state) {
        state_ = state;
    }
    State state() {
        return state_;
    }
    void send(const std::string& message);
    void close();
    // static std::shared_ptr<TcpConnection> connect_to(const Socket::Endpoint& endpoint,EventLoop* eventloop);
private:
    void read_handler();
    void close_handler();
    void write_handler();

private:
    Socket sock_;
    std::shared_ptr<Channel> chan_;
    EventLoop* eventloop_;
    std::vector<char> read_buf_;
    OnReadType on_read_cb_;
    OnCloseType on_close_cb_;
    OnErrorType on_error_cb_;
    // OnConnectedType on_conn_cb_;
    OnWriteCompletedType on_write_completed_cb_;
    State state_;
    std::queue<MessageNode> send_buffers_;
};
}