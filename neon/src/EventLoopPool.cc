#include <EventLoopPool.h>
#include <EventLoop.h>
#include <cstdio>
namespace Neon {

EventLoopThread::EventLoopThread()
    : eventloop_{nullptr} {

}

EventLoop* EventLoopThread::start() {
    printf("eventloop thread start\n");
    // thread_ = std::make_unique<std::thread>(std::bind(&EventLoopThread::thread_func,this));
    thread_ = std::move(std::unique_ptr<std::thread>(new std::thread{&EventLoopThread::thread_func,this}));
    std::unique_lock<std::mutex> lock{mtx_};
    cond_.wait(lock,[this]{
        return this->eventloop_;
    });
    return eventloop_;
}

void EventLoopThread::quit() {
    if(!quit_) {
        eventloop_->quit();
    }
}

void EventLoopThread::thread_func() {
    EventLoop ep{};
    {
        std::lock_guard<std::mutex> guard{mtx_};
        eventloop_ = &ep;
        printf("eventloop thread_func get lock \n");
        cond_.notify_one();
    }
    ep.loop();
}

EventLoopThread::~EventLoopThread() {
    if(!quit_) {
        eventloop_->quit();
        eventloop_ = nullptr;
        thread_->join();
    }
}

EventLoopPool::EventLoopPool(size_t num)
    : current_idx_ {0} {
    if(num <= 0) {
        num = std::thread::hardware_concurrency();
    }
    for(size_t i = 0;i < num;i ++) {
        auto eventloop_th = new EventLoopThread{};
        eventloop_th->start();
        eventloop_threads_.push_back(eventloop_th);
    }
    printf("eventloop_threads_.size : %d\n",(int)eventloop_threads_.size());
}

EventLoop* EventLoopPool::next_eventloop() {
    return eventloop_threads_[(current_idx_ ++) % eventloop_threads_.size()]->eventloop();
}

EventLoopPool::~EventLoopPool() {
    for(auto& ptr : eventloop_threads_) {
        ptr->quit();
    }
}

}