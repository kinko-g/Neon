#include <Socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdio>
namespace Neon {
Socket::Socket(int fd) : fd_{fd} {

}

bool Socket::set_nonblocking(bool flag) {
    int fd_flag = fcntl(fd_,F_GETFL,0);
    if(fd_flag < 0) {
        return fd_flag;
    }
    if(flag) {
        fd_flag |= O_NONBLOCK;
    }
    else {
        fd_flag &= ~O_NONBLOCK;
    }
    return fcntl(fd_,F_SETFL,fd_flag) != -1;
}

bool Socket::set_reuse_addr(bool flag) {
    int val = flag ? 1 : 0;
    return ::setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val)) == 0;
}

bool Socket::is_blocking() {
    auto flag = fcntl(fd_,F_GETFL,0);
    return flag & O_NONBLOCK;
}

bool Socket::is_reused() {
    int val = 0;
    socklen_t len = sizeof(val);
    ::getsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&val,&len);
    return val;
}

bool Socket::listen_on(const Endpoint& endpoint,int backlogs) {
    sockaddr_in addr{
        .sin_family = AF_INET,
        .sin_port = htons(endpoint.port)
    };
    inet_pton(AF_INET,endpoint.ip.c_str(),&addr.sin_addr);
    if(::bind(fd_,(sockaddr*)&addr,sizeof(addr)) != 0) {
        return false;
    }
    return ::listen(fd_,backlogs) == 0;
}

Socket::Endpoint Socket::address() {
    sockaddr_in addr{};
    socklen_t addr_len = sizeof(addr);
    if(getsockname(fd_,(sockaddr*)&addr,&addr_len) == 0) {
        std::cout << "getsockname\n";
        return {inet_ntoa(addr.sin_addr),ntohs(addr.sin_port)};
    }
    return {};
}

int Socket::accepet() {
    return ::accept(fd_,nullptr,nullptr);
}


Socket::~Socket() {
    fprintf(stdout,"~Socket\n");
    ::close(fd_);
}

}