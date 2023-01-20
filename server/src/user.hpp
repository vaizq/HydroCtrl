#pragma once


#include "participant.hpp"
#include "pool.hpp"


using asio::ip::tcp;


class User 
    : public BasicParticipant
{
public:
    User(tcp::socket socket, Pool& pool)
        : BasicParticipant(std::move(socket), pool),
          m_pool(pool)
    {
        type = Participant::Type::User;
    }

    void processMessage(const Message& msg)
    {
        // Forward message to all devices 
        m_pool.deliverFor(Participant::Type::Device, msg);
    }
private:
    Pool& m_pool;
};

