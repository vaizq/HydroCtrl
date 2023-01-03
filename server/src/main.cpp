#include <iostream>
#include <asio.hpp>
#include <chrono>


class Server
{
public:
    Server(asio::io_context& io) 
    {};

    void run()
    {
    }

    void handler(const asio::error_code& ec)
    {
    }
};

int main(int argc, char** argv)
{
    asio::io_context io;
    Server s(io);
    s.run();

    io.run();

    return 0;
}
