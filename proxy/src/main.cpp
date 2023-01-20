#include "camera.hpp"
#include "video.hpp"
#include "ctrldevice.hpp"
#include "servo.hpp"
#include <asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <memory>



int main(int argc, char** argv)
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " video-port control-port control-device" << std::endl;
        return EXIT_FAILURE;
    }

    const auto listenPort = std::atoi(argv[1]);
    const auto outputAddress = std::atoi(argv[2]);
    const auto outputPort = argv[3];

}




