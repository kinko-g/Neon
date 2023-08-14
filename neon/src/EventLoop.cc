#include <EventLoop.h>
#include <Poller.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace Neon;

static int create_event_fd() {
    int event_fd = eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
    ASSERT_IF(event_fd >= 0,"create event fd error");
    return event_fd;
} 

thread_local EventLoop* local_eventloop = nullptr;

EventLoop::EventLoop()
    :poller_{new Poller{}},
    tasks_{},
    quit_{false},
    weakup_chan_{std::make_shared<Channel>(create_event_fd(),this)},
    doing_tasks_{false},
    thread_id_{std::this_thread::get_id()} {

    if(local_eventloop) {
        fprintf(stdout,"has exist other eventloop run in this thread");
    }
    else {
        local_eventloop = this;
    }
    weakup_chan_->set_read_handler([weakup_fd = weakup_chan_->fd()]{
        int n = 1;
        ::read(weakup_fd,&n,sizeof(n));
    });
    weakup_chan_->enable_reading();
}

EventLoop::~EventLoop() {
    ::close(weakup_chan_->fd());
    local_eventloop = nullptr;
}

void EventLoop::loop() {
    while(!quit_) {
        auto active_channels = poller_->poll(2000);
        for(auto& chan : active_channels) {
            chan->handle_events();
        }
        doing_tasks_ = true;
        do_pending_tasks();
        doing_tasks_ = false;
    }
}

void EventLoop::update_channel(ChannelSP& chan) {
    poller_->update_event(chan);
}

void EventLoop::queue_in_loop(const TaskType& task) {
    std::lock_guard<std::mutex> guard{mtx_};
    tasks_.push_back(std::move(task));
    if(!is_in_loop_thread() || doing_tasks_) {
        weakup();
    }
}

void EventLoop::run_in_loop(const TaskType& task) {
    if(is_in_loop_thread()) {
        task();
    }
    else {
        queue_in_loop(std::move(task));
    }
}

void EventLoop::do_pending_tasks() {
    std::vector<TaskType> tasks;
    {
        std::lock_guard<std::mutex> guard{mtx_};
        tasks.swap(tasks_);
    }
    // do tasks
    for(auto& task : tasks) {
        task();
    }
}

void EventLoop::weakup() {
    int n = 1;
    auto writen = ::write(weakup_chan_->fd(),&n,sizeof(n));
    if(writen != sizeof(n)) {
        fprintf(stdout,"weakup channel fault\n");;
        return;
    }
}
