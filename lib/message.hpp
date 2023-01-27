#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <iterator>
#include <vector>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cassert>

#ifdef _WIN32
#include <winsock.h>
#elif __linux__
#include <arpa/inet.h>
#endif


class Message
{
public:
    using Header = uint32_t;

    static constexpr std::size_t headerSize()
    {
        return sizeof (Header);
    }

    static std::string encode(const std::string& msg)
    {

        // Serialize payload
        /*
        std::stringstream ss;
        {
            cereal::PortableBinaryOutputArchive oarchive(ss);
            oarchive(msg);
        }
        */

        auto encodedPayload = msg; // ss.str();

        // Create header
        uint32_t psize = htonl(static_cast<uint32_t>(encodedPayload.size()));
        std::string header; header.resize(sizeof psize);
        std::memcpy(&header[0], &psize, sizeof psize);

        auto encodedMsg = header + encodedPayload;

        return encodedMsg;
    }

    static uint32_t decodeHeader(const std::string& s)
    {
        uint32_t header;
        std::memcpy(&header, s.data(), sizeof header);
        return ntohl(header);
    }

    static std::string decodePayload(const std::string& s)
    {
        std::stringstream ss(s);
        std::string result;
        {
            cereal::PortableBinaryInputArchive iarchive(ss);
            iarchive(result);
        }

        return result;
    }
};



#endif

