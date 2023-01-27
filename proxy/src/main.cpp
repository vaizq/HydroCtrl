#include "server.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <memory>



int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " listen-port device-name" << std::endl;
        return EXIT_FAILURE;
    }

    const auto listenPort = std::atoi(argv[1]);
    const auto deviceName = argv[2];


    Tcp2SerialProxy(listenPort, deviceName).start();
}




