#pragma once

#include <asio.hpp>
#include <iostream>
#include "user.hpp"
#include "device.hpp"


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
                        std::cout << "User connected\n";
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
                        std::cout << "Device connected\n";
                        std::make_shared<Device>(std::move(socket), pool)->start();
                    }

                    doAccept();
                });
    }

    tcp::acceptor m_userAcceptor;
    tcp::acceptor m_deviceAcceptor;
    Pool pool;
};

