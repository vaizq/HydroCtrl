#pragma once

#include <cereal/types/string.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <string>
#include <sstream>

template <typename T>
static std::string encode(const T& msg)
{
    std::stringstream ss;

    {
        cereal::PortableBinaryOutputArchive oarchive(ss);
        oarchive(msg);
    }
    return ss.str();
}

template <typename T>
static T decode(const std::string& str)
{
    std::stringstream ss(str);
    T msg;

    {
        cereal::PortableBinaryInputArchive iarchive(ss);
        iarchive(msg);
    }

    return msg;
}


struct Header
{
    uint32_t payloadSize;

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(payloadSize);
    }

    static std::size_t encodedSize()
    {
        static std::size_t size(encode(Header()).size());
        return size;
    }
};

struct Message
{
    Message() = default;
    Message(std::string content)
        : payload(std::move(content))
    {}

    std::string serialize()
    {
        auto msg = encode(*this);
        Header h;
        h.payloadSize = msg.size();
        auto encodedHeader = encode(h);

        return encodedHeader + msg;
    }
    
    std::string payload;

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(payload);
    }
 };































