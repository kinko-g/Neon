#include <TcpConnection.h>
#include <Channel.h>
namespace Neon {
TcpConnection::TcpConnection(int fd,EventLoop *eventloop)
    :sock_{fd},
    chan_{std::make_shared<Channel>(fd,eventloop)},
    eventloop_{eventloop} {

}

}