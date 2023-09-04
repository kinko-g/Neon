#pragma once
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <Utilities.h>
namespace Neon {

class EventLoop;

class EventLoopThread final : public NonCopyable {
public:

    EventLoopThread();
    
    EventLoop* eventloop() {
        return eventloop_;
    }

    EventLoop* start();
    void quit();

    ~EventLoopThread();
private:
    void thread_func();
private:
    std::unique_ptr<std::thread> thread_;
    EventLoop* eventloop_;
    std::atomic<bool> quit_;
    std::mutex mtx_;
    std::condition_variable cond_;
};

class EventLoopPool final : public NonCopyable {
public:
    EventLoopPool(size_t num = 0);
    EventLoop* next_eventloop();
    ~EventLoopPool();
private:
    std::vector<EventLoopThread*> eventloop_threads_;
    size_t current_idx_;
};
}