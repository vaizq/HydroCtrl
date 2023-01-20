#pragma once

#include "../../lib/message.hpp"
#include <string>
#include <memory>


class Participant
{
public:
    virtual ~Participant() {};
    virtual void deliver(const Message& msg) = 0;
    virtual void onJoin() = 0;
    enum class Type
    {
        User,
        Device,
    } type;
    std::string name;
    using Ptr = std::shared_ptr<Participant>;
};



