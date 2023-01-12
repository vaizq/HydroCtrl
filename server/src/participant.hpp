#pragma once

#include "../../lib/message.hpp"
#include <asio.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>
#include <set>
#include <string>
#include <memory>
#include <queue>


class Participant
{
public:
    virtual ~Participant() {};
    virtual void deliver(const Message& msg) = 0;
    enum class Type
    {
        User,
        Device,
    } type;
    std::string name;
    using Ptr = std::shared_ptr<Participant>;
};

class Pool
{
public:
    void join(Participant::Ptr participant)
    {
        if (participant->type == Participant::Type::User)
        {
            // Inform new user about devices 
            for (auto p : m_participants)
            {
                if (p->type == Participant::Type::Device)
                {
                    std::cout << "Send message to new participant\n";
                    Message msg("Sensor[0,12]: PH-meter");
                    participant->deliver(msg);
                }
                else if (p->type == Participant::Type::User)
                {
                    // Inform existing users of a new user
                    Message msg("New user connected");
                    p->deliver(msg);
                }
            }
        }
        else if (participant->type == Participant::Type::Device)
        {
            // Inform users about new device
            for (auto p : m_participants)
            {
                if (p->type == Participant::Type::User)
                {
                    Message msg("New device connected");
                    p->deliver(msg);
                }
            }
        }

        m_participants.insert(participant);
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

