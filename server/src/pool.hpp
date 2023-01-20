#pragma once

#include "../../lib/message.hpp"
#include "participant.hpp"
#include <asio.hpp>
#include <set>
#include <string>
#include <memory>
#include <queue>


using asio::ip::tcp;


class Pool
{
public:
    void join(Participant::Ptr participant)
    {
        m_participants.insert(participant);

        participant->onJoin();
    }
    void leave(Participant::Ptr participant)
    {
        m_participants.erase(participant);
    }
    void deliverFor(Participant::Type type, const Message& msg)
    {
        for (auto p : m_participants)
        {
            if (p->type == type) 
                p->deliver(msg);
        }
    }

private:
    std::set<Participant::Ptr> m_participants;
};


class BasicParticipant
    : public Participant,
      public std::enable_shared_from_this<BasicParticipant>
{
public:
    BasicParticipant(tcp::socket socket, Pool& pool)
        : m_socket(std::move(socket)),
          m_pool(pool)
    {}

    virtual void processMessage(const Message& msg) = 0;

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
        m_buffer.resize(Header::encodedSize());
        asio::async_read(m_socket,
                asio::buffer(m_buffer),
                [this, self](std::error_code ec, std::size_t bytesReceived)
                {
                    if (!ec)
                    {
                        Header header = decode<Header>(m_buffer);
                        doReadPayload(header.payloadSize);
                    }
                    else
                    {
                        m_pool.leave(shared_from_this());
                    }
                });
    }

    void doReadPayload(std::size_t payloadSize)
    {
        auto self(shared_from_this());
        m_buffer.resize(payloadSize);
        asio::async_read(m_socket,
                asio::buffer(m_buffer),
                [this, self](std::error_code ec, std::size_t bytesReceived)
                {
                    if(!ec)
                    {
                        Message msg = decode<Message>(m_buffer);
                        processMessage(msg);
                        doReadHeader();
                    }
                    else
                    {
                        m_pool.leave(shared_from_this());
                    }
                });
    }

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
private:
    tcp::socket m_socket;
    Pool& m_pool;
    std::string m_buffer;
    std::queue<Message> m_writeQueue;
};

