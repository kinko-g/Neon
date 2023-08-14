#include <Poller.h>
#include <cstdio>
#include <algorithm>
#include <unistd.h>
using namespace Neon;

Poller::Poller() 
    : events_(MAX_EVENTS_SZ),
    epfd_{::epoll_create1(EPOLL_CLOEXEC)} {
    
    if(epfd_ < 0) {
        fprintf(stdout,"epoll create error\n");
        exit(0);
    }
}

void Poller::add_event(ChannelSP& chan) {
    auto fd = chan->fd();
    fd2channel_.insert({fd,chan});
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = chan->events();
    if(epoll_ctl(epfd_,EPOLL_CTL_ADD,fd,&ev) < 0) {
        fprintf(stdout,"add_event,epoll ctl error\n");
        fd2channel_.erase(fd);
        return;
    }
}

void Poller::update_event(ChannelSP& chan) {
    fprintf(stdout,"poller::update_event\n");
    auto fd = chan->fd();
    if(fd2channel_.count(fd) <= 0) {
        fprintf(stdout,"poller::add_event\n");
        add_event(chan);
        return;
    }
    auto events = chan->events();
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    
    if(epoll_ctl(epfd_,EPOLL_CTL_MOD,fd,&ev) < 0) {
        fprintf(stdout,"update_event,epoll ctl error\n");
    }
}

void Poller::delete_event(ChannelSP& chan) {
    auto fd = chan->fd();
    auto events = chan->events();
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = chan->events();
    if(epoll_ctl(epfd_,EPOLL_CTL_DEL,fd,&ev) < 0) {
        fprintf(stdout,"delete_event,epoll ctl error\n");
    }
    fd2channel_.erase(fd);
}

std::vector<ChannelSP> Poller::poll(double timeout) {
    // fprintf(stdout,"Poller::poll\n");
    auto readyn = epoll_wait(epfd_,events_.data(),events_.size(),timeout);
    if(readyn == 0) {
        fprintf(stdout,"epoll wait timeout\n");
    }

    if(readyn < 0) {
        fprintf(stdout,"epoll wait error\n");
    }

    if(readyn > 0) {
        std::vector<ChannelSP> actives{};
        for(size_t i = 0;i < readyn;i ++) {
            auto fd = events_[i].data.fd;
            auto& chan = fd2channel_[fd];
            chan->set_revents(events_[i].events);
            actives.push_back(chan);
        }
        return actives;
    }
    return {};
}

Poller::~Poller() {
    ::close(epfd_);
}
