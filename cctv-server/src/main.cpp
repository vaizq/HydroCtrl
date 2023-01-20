#include "camera.hpp"
#include "video.hpp"
#include "ctrldevice.hpp"
#include "servo.hpp"
#include <asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <memory>


constexpr int SERVO_PIN = 2;


asio::ip::tcp::socket createSocket(asio::io_context& io, const char* address, int port) 
{
    asio::ip::tcp::socket socket(io);

    asio::ip::tcp::resolver resolver(io);
        asio::ip::tcp::resolver::results_type endpoints = 
            resolver.resolve(asio::ip::tcp::v4(), address, "6969");

    asio::connect(socket, endpoints);

    return socket;
}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " video-port control-port control-device" << std::endl;
        return EXIT_FAILURE;
    }

    const auto videoPort = std::atoi(argv[1]);
    const auto ctrlPort = std::atoi(argv[2]);
    const auto ctrlDevice = argv[3];
    constexpr auto address = "127.0.0.1";


    // Create video server
    VideoServer video(address, videoPort);

    std::thread videoThread([&]() {
            video.start();
        });

    // Create socket and connect
    asio::io_context io;

    auto socket = createSocket(io, address, 6969);
   
    CtrlDevice<asio::ip::tcp::socket> arduino(std::move(socket));

    Servo<asio::ip::tcp::socket> servo(arduino, SERVO_PIN);

    // Run io in another thread
    std::thread ioThread([&io]() {
            io.run();
            });


    bool run = true;

    while (run)
    {
        float angle;
        std::cout << "Enter angle" << std::endl;
        std::cin >> angle;

        servo.rotate(angle);
        servo.update();
    }

    io.stop();
    video.stop();

    ioThread.join();
    videoThread.join();
}




