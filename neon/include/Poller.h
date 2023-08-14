#pragma once
#include <Channel.h>
#include <sys/epoll.h>
#include <unordered_map>
#include <vector>
namespace Neon {
static constexpr size_t MAX_EVENTS_SZ = 1024;
class Poller final : public NonCopyable{
public:
    Poller();
    void add_event(ChannelSP& chan);
    void update_event(ChannelSP& chan);
    void delete_event(ChannelSP& chan);
    ~Poller();
    std::vector<ChannelSP> poll(double timeout);
private:
    std::vector<epoll_event> events_;
    int epfd_;
    std::unordered_map<int,ChannelSP> fd2channel_;
};
}

