#pragma once
#include <Utilities.h>
#include <memory>
#include <atomic>
#include <functional>
#include <vector>
#include <mutex>
#include <thread>

namespace Neon {
class Poller;
class Channel;
using ChannelSP = std::shared_ptr<Channel>;
class EventLoop final : public NonCopyable {
public:
    using TaskType = std::function<void()>;
    EventLoop();
    ~EventLoop();
    void loop();
    void queue_in_loop(const TaskType& task);
    void run_in_loop(const TaskType& task);
    void update_channel(ChannelSP& chan);
    bool is_in_loop_thread() {
        return std::this_thread::get_id() == thread_id_;
    }
    void quit() {
        quit_ = true;
    }
private:
    void do_pending_tasks();
    void weakup();
private:
    std::unique_ptr<Poller> poller_;
    std::vector<TaskType> tasks_;
    std::mutex mtx_;
    std::atomic<bool> quit_;
    std::shared_ptr<Channel> weakup_chan_;
    bool doing_tasks_;
    std::thread::id thread_id_;
};
}