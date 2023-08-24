#include <Connector.h>
#include <EventLoop.h>

int main() {
    Neon::EventLoop eventloop{};
    Neon::Connector connector{&eventloop};
    connector.connect_to({"127.0.0.1",8080});
    eventloop.loop();
    return 0;
}