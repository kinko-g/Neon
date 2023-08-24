#include <iostream>
#include <Utilities.h>
#include <EventLoop.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <Socket.h>
#include <Acceptor.h>
#include <TcpConnection.h>
#include <Connector.h>
#include <vector>

void unit_test() {
    // socket test
#if 0
    auto fd = ::socket(AF_INET,SOCK_STREAM,0);
    Neon::Socket sock{fd};
    auto ret = sock.bind({"192.168.230.128",8080});
    auto endpoint = sock.address();
    std::cout << std::boolalpha << ret << "\n";
    std::cout << endpoint.ip << ":" << endpoint.port << "\n";
#endif
    std::vector<std::shared_ptr<Neon::TcpConnection>> conns{};
    Neon::EventLoop eventloop{};
    Neon::Acceptor acceptor{&eventloop};
    acceptor.listen_on({"127.0.0.1",8080});
    acceptor.set_new_conn_handler([&conns,ep = &eventloop](std::shared_ptr<Neon::TcpConnection> conn) {
        std::cout << "new connection comming\n";
        std::cout << "fd : " << conn->tcp_socket().fd() << "\n";
        ep->queue_in_loop([conn]{
            conn->start_read();
        });
        conns.push_back(conn);
    });
    // Neon::Connector connector{&eventloop};
    // connector.connect_to({"127.0.0.1",8080});
    eventloop.loop();
}

int main() {
    unit_test();
    ASSERT_IF(1==1,"1 != 1");
    // ASSERT_IF(1==2,"1 != 2");

    // std::cout << "hello Neon\n";
    // Neon::EventLoop eventloop{};
    // eventloop.loop();


    return 0;
}