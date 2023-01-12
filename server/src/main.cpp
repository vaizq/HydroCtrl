#include "../../lib/message.hpp"
#include "participant.hpp"
#include "user.hpp"
#include "device.hpp"
#include <asio.hpp>
#include <iostream>
#include <chrono>
#include <memory>
#include <set>

using asio::ip::tcp;



class Server
{
public:
    Server(asio::io_context& io, 
            const tcp::endpoint& userEndpoint,
            const tcp::endpoint& deviceEndpoint) 
        : m_userAcceptor(io, userEndpoint), 
          m_deviceAcceptor(io, deviceEndpoint)
    {
        doAccept();
    };

private:
    void doAccept()
    {

        m_userAcceptor.async_accept(
                [this](std::error_code ec, tcp::socket socket)
                {
                    if (!ec)
                    {
                        // Start new session with user
                        std::cout << "Got new connection\n";
                        std::make_shared<User>(std::move(socket), pool)->start();
                    }

                    doAccept();
                });

        m_deviceAcceptor.async_accept(
                [this](std::error_code ec, tcp::socket socket)
                {
                    if (!ec)
                    {
                        // Start new session with device
                        std::cout << "Got new device connection\n";
                        std::make_shared<Device>(std::move(socket), pool)->start();
                    }

                    doAccept();
                });
    }

    tcp::acceptor m_userAcceptor;
    tcp::acceptor m_deviceAcceptor;
    Pool pool;
};

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage user-port device port\n";
        return EXIT_FAILURE;
    }
    asio::io_context io;
    tcp::endpoint userEndpoint(tcp::v4(), std::atoi(argv[1]));
    tcp::endpoint deviceEndpoint(tcp::v4(), std::atoi(argv[2]));

    try {
        Server s(io, userEndpoint, deviceEndpoint);
        io.run();
    } catch (const std::exception& e) {
        std::cout << "Server crashed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}







