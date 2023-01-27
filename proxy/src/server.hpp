#pragma once

#include <iostream>
#include <asio.hpp>
#include <asio/connect.hpp>
#include <asio/io_context.hpp>
#include <memory>
#include <stdexcept>
#include <queue>

// Listens socket or device and proxies messages to a socket or device


using asio::ip::tcp;
using asio::serial_port;


template <typename Stream>
class Proxy
{
public:
    Proxy(Stream stream)
        : m_device(std::move(stream))
    {
    }

    void start()
    {
        doRead();
       
    }

    ~Proxy()
    {
        if (m_device.is_open())
            m_device.close();
    }

    void forwardMsg(const std::string& msg)
    {
        asio::async_write(m_device, asio::buffer(msg),
                [](const std::error_code& ec, std::size_t) {
                    if (ec)
                    {
                        throw std::runtime_error("async_write failed: " + ec.message() + '\n');
                    }
                });
    }

private:
    void doRead()
    {
        m_device.async_read_some(asio::buffer(m_buf),
            [this](const std::error_code& ec, std::size_t bytes)
            {
                if (ec)
                {
                    std::cerr << "Failed to receive " << ec.message() << std::endl;
                }

                std::string msg(m_buf.cbegin(), m_buf.cbegin() + bytes);
                std::cout << msg;

                doRead();
            });
    }

private:
    std::array<char, 24> m_buf;
    Stream m_device;
};



template <typename T>
class Session
    : public std::enable_shared_from_this<Session<T>>
{
public:
    Session(tcp::socket socket, Proxy<T>& proxy)
        : m_socket(std::move(socket)), m_proxy(proxy)
    {
    }

    void start()
    {
        doRead();
    }
private:
    void doRead()
    {
        // Read header
        auto self(this->shared_from_this());


        m_socket.async_read_some(asio::buffer(m_buf),
                [this, self](const std::error_code& ec, std::size_t bytesReceived)
                {
                    if (ec)
                    {
                        std::cerr << "Failed to read\n";
                        return;
                    }

                    m_proxy.forwardMsg(std::string(m_buf.cbegin(), m_buf.cbegin() + bytesReceived));

                    doRead();
                });
    }

private:
    std::array<uint8_t, 1028> m_buf;
    tcp::socket m_socket;
    Proxy<T>& m_proxy;
};


template <typename T>
class ProxyServer 
{
public:
    ProxyServer(asio::io_context& io, int port, Proxy<T>& proxy)
        :  m_acceptor(io, tcp::endpoint(tcp::v4(), port)), m_proxy(proxy)
    {
    }

    void start()
    {
        // Start listening for incoming connections to port
        doAccept();
    }

private:
    void doAccept()
    {
        m_acceptor.async_accept(
                [this](const std::error_code& ec, tcp::socket socket)
                {
                    if (!ec)
                    {
                        std::cout << "Got new connection\n";
                        std::make_shared<Session<T>>(std::move(socket), m_proxy)->start();
                    }

                    doAccept();
                });
    }

private:
    tcp::acceptor m_acceptor;
    Proxy<T>& m_proxy;
};



class TcpProxy
{
public:
    TcpProxy(int inPort, const char* outHostname, int outPort)
    {
        asio::ip::tcp::resolver resolver(m_io);
        asio::ip::tcp::resolver::results_type endpoints = 
            resolver.resolve(asio::ip::tcp::v4(), outHostname, std::to_string(outPort));

        tcp::socket socket(m_io);
        asio::connect(socket, endpoints);

        m_proxy = std::make_unique<Proxy<tcp::socket>>(std::move(socket));

        m_server = std::make_unique<ProxyServer<tcp::socket>>(m_io, inPort, *m_proxy);
    }

    void start()
    {
        m_server->start();
        m_io.run();
    }

private:
    asio::io_context m_io;
    std::unique_ptr<Proxy<tcp::socket>> m_proxy;
    std::unique_ptr<ProxyServer<tcp::socket>> m_server;
};


class Tcp2SerialProxy
{
public:
    Tcp2SerialProxy(int inPort, const char* outDeviceName)
    {
        serial_port device(m_io, outDeviceName);
        m_proxy = std::make_unique<Proxy<serial_port>>(std::move(device));

        m_server = std::make_unique<ProxyServer<serial_port>>(m_io, inPort, *m_proxy);
    }

    void start()
    {
        m_proxy->start();
        m_server->start();
        m_io.run();
    }

private:
    asio::io_context m_io;
    std::unique_ptr<Proxy<serial_port>> m_proxy;
    std::unique_ptr<ProxyServer<serial_port>> m_server;
};

