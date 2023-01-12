#pragma once


#include "participant.hpp"

using asio::ip::tcp;


class Device
    : public Participant,
      public std::enable_shared_from_this<Device>
{
public:
    Device(tcp::socket socket, Pool& pool)
        : m_socket(std::move(socket)),
          m_pool(pool)
    {
        type = Participant::Type::Device;
    }
    void deliver(const Message& msg)
    {
    }
    void start()
    {
    }
private:
    tcp::socket m_socket;
    Pool& m_pool;
};

