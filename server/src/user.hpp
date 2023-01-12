#pragma once


#include "participant.hpp"


using asio::ip::tcp;


class User 
    : public Participant,
      public std::enable_shared_from_this<User>
{
public:
    User(tcp::socket socket, Pool& pool)
        : m_socket(std::move(socket)),
          m_pool(pool)
    {
        type = Participant::Type::User;
    }

    void start()
    {
        m_pool.join(shared_from_this());
        doReadHeader();
    }

    void deliver(const Message& msg)
    {
        bool writeInProgress = !m_writeQueue.empty();

        m_writeQueue.push(msg);

        if (!writeInProgress)
        {
            doWrite();
        }
    }

private:
    void doReadHeader()
    {
        auto self(shared_from_this());
        m_recvBuf.resize(Header::encodedSize());
        asio::async_read(m_socket,
                asio::buffer(m_recvBuf),
                [this, self](std::error_code ec, std::size_t bytesReceived)
                {
                    if (!ec)
                    {
                        m_header = decode<Header>(m_recvBuf);
                        std::cout << "Header received. Payload: " << m_header.payloadSize << '\n';
                        doReadPayload();
                    }
                    else
                    {
                        m_pool.leave(shared_from_this());
                    }
                });
    }

    void doReadPayload()
    {
        auto self(shared_from_this());
        m_recvBuf.resize(m_header.payloadSize);
        asio::async_read(m_socket,
                asio::buffer(m_recvBuf),
                [this, self](std::error_code ec, std::size_t bytesReceived)
                {
                    if(!ec)
                    {
                        m_msg = decode<Message>(m_recvBuf);
                        std::cout << "Received: " << m_msg.payload << std::endl;
                        doReadHeader();
                    }
                    else
                    {
                        m_pool.leave(shared_from_this());
                    }
                });
    }

    // Write in a chain until writeQueue is empty
    void doWrite()
    {
        auto self(shared_from_this());

        auto msg = m_writeQueue.front();
        auto serialized = msg.serialize();

        asio::async_write(m_socket, 
            asio::buffer(serialized),
            [this, self](std::error_code ec, std::size_t bytes)
            {
                if (!ec)
                {
                    m_writeQueue.pop();
                    if (!m_writeQueue.empty())
                    {
                        doWrite();
                    }
                }
                else
                {
                    m_pool.leave(shared_from_this());
                }
            });
    }

    tcp::socket m_socket;
    Pool& m_pool;
    std::string m_recvBuf;
    Header m_header;
    Message m_msg;
    std::queue<Message> m_writeQueue;
};



