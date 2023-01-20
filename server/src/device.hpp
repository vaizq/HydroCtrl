#pragma once

#include "participant.hpp"
#include "pool.hpp"
#include <nlohmann/json.hpp>


using asio::ip::tcp;


class Device
    : public BasicParticipant
{
public:
    Device(tcp::socket socket, Pool& pool)
        : BasicParticipant(std::move(socket), pool),
          m_pool(pool)
    {
        Participant::type = Participant::Type::Device;
    }

    void processMessage(const Message& msg)
    {
        auto msgJson = nlohmann::json::parse(msg.payload);
        
        if (msgJson["type"] == "device_info")
        {
            m_info = msg;
        }
        else if (msgJson["type"] == "update")
        {
        }

        int* p = std::nullptr_t();


        // Forward message to all users
        m_pool.deliverFor(Participant::Type::User, msg);
    }

    const Message& getInfo() { return m_info; }
private:
    Pool& m_pool;
    Message m_info;
};


class Sensor 
    : public BasicParticipant
{
public:
    Sensor(tcp::socket socket, Pool& pool)
        : BasicParticipant(std::move(socket), pool),
          m_pool(pool)
    {
        Participant::type = Participant::Type::Device;
    }

    void processMessage(const Message& msg)
    {
        // Forward message to all users
        m_pool.deliverFor(Participant::Type::User, msg);
    }
private:
    Pool& m_pool;
    float m_minVal;
    float m_maxVal;
    float m_curVal;
};

