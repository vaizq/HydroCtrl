#pragma once

#include "../../lib/message.hpp"
#include <asio.hpp>
#include <stdexcept>
#include <string>
#include <queue>
#include <thread>


struct IotNode
{
    std::string name;
};


class Client
{
public:
    Client(const std::string& hostname, int port)
        : m_socket(m_io)
    {
        asio::ip::tcp::resolver resolver(m_io);
        asio::ip::tcp::resolver::results_type endpoints = 
            resolver.resolve(asio::ip::tcp::v4(), hostname, std::to_string(port));

        asio::connect(m_socket, endpoints);

        // Start receiving
        doRead();
        
        m_ioThread = std::make_unique<std::thread>([this]() {
                m_io.run();
                });

    }
    ~Client()
    {
        m_socket.close();
        m_io.stop();
        m_ioThread->join();
    }

    void send(const std::string msg)
    {
        asio::async_write(m_socket, asio::buffer(msg),
                [this](const asio::error_code& ec, std::size_t bytes_transferred)
                {
                    if (ec)
                    {
                        throw std::runtime_error("Failed to send message: " + ec.message() + '\n');
                    }
                });
    }

    void send(const Message& msg)
    {
        auto encodedPayload = encode(msg.payload);
        Header header;
        header.payloadSize = encodedPayload.size();
        auto encodedHeader = encode(header); 

        asio::async_write(m_socket, asio::buffer(encodedHeader),
                [this, encodedPayload](const asio::error_code& ec, std::size_t)
                {
                    if (ec)
                    {
                        throw std::runtime_error("Failed to send header: " + ec.message() + '\n');
                    }

                    // Send payload
                    asio::async_write(m_socket, asio::buffer(encodedPayload),
                            [this](std::error_code ec, std::size_t)
                            {
                                if (ec)
                                {
                                    throw std::runtime_error("Failed to send payload: " + ec.message() + '\n');
                                }
                            });
                });
    }

    std::size_t numReceived()
    {
        std::lock_guard<std::mutex> quard(m_mtx);
        return m_recvQueue.size();
    }

    const Message& peekReceived()
    {
        std::lock_guard<std::mutex> quard(m_mtx);
        return m_recvQueue.front();
    }

    Message popReceived()
    {
        std::lock_guard<std::mutex> quard(m_mtx);
        auto front = m_recvQueue.front();
        m_recvQueue.pop();
        return front;
    }

private:
    void doRead()
    {
        m_recvBuf.resize(Header::encodedSize());
        asio::async_read(m_socket,
                asio::buffer(m_recvBuf),
                [this](std::error_code ec, std::size_t)
                {
                    if (ec)
                    {
                        std::cerr << "Failed to read header\n";
                        return;
                    }

                    auto header = decode<Header>(m_recvBuf);
                    m_recvBuf.resize(header.payloadSize);
                    
                    asio::async_read(m_socket,
                            asio::buffer(m_recvBuf),
                            [this](std::error_code ec, std::size_t)
                            {
                                if (ec)
                                {
                                    std::cerr << "Failed to read payload\n";

                                    return;
                                }

                                auto message = decode<Message>(m_recvBuf);

                                m_mtx.lock();
                                m_recvQueue.push(message);
                                m_mtx.unlock();

                                doRead();
                            });
                });
    }

private:
    asio::io_context m_io;
    std::unique_ptr<std::thread> m_ioThread;
    asio::ip::tcp::socket m_socket;
    std::mutex m_mtx;
    std::string m_recvBuf;
    std::queue<Message> m_recvQueue; 
};










