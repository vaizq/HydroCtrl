#include "server.hpp"
#include "../../lib/message.hpp"
#include "participant.hpp"
#include "user.hpp"
#include "device.hpp"
#include <asio.hpp>
#include <iostream>
#include <chrono>
#include <memory>
#include <set>

using asio::ip::tcp;




int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage user-port device port\n";
        return EXIT_FAILURE;
    }
    asio::io_context io;
    tcp::endpoint userEndpoint(tcp::v4(), std::atoi(argv[1]));
    tcp::endpoint deviceEndpoint(tcp::v4(), std::atoi(argv[2]));

    try {
        Server s(io, userEndpoint, deviceEndpoint);
        io.run();
    } catch (const std::exception& e) {
        std::cout << "Server crashed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}







