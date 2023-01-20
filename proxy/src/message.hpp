#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iterator>
#include <vector>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>

#ifdef _WIN32
#include <winsock.h>
#elif __linux__
#include <arpa/inet.h>
#endif


class Message
{
public:
    static std::string encode(const std::string& msg)
    {
        // Create header
        uint32_t psize = htonl(msg.size());
        std::string header(&psize, &psize + sizeof psize);

        // Serialize payload
        std::stringstream ss;
        {
            cereal::PortableBinaryOutputArchive oarchive(ss);
            oarchive(msg);
        }

        return header + ss.str();
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






















