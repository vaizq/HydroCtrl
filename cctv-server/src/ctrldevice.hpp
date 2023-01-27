#ifndef CTRL_DEVICE_HPP
#define CTRL_DEVICE_HPP

#include <asio.hpp>
#include <memory>
#include <string>
#include <set>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <vector>
#include "message.hpp"


using Guard = std::lock_guard<std::mutex>;


template <typename T>
class Actuator;


template <typename Writeable>
class CtrlDevice
{
public:
    friend Actuator<Writeable>;

    CtrlDevice(Writeable&& socket)
        : m_socket(std::move(socket))
    {
        assert(m_socket.is_open());

        doRead(); // Keep io busy
    }

    ~CtrlDevice()
    {
        m_socket.close();
    }

    CtrlDevice(CtrlDevice&) = delete;
    CtrlDevice& operator=(CtrlDevice&) = delete;

private:
    void doRead()
    {
        std::vector<uint8_t> buf(1);
        asio::async_read(m_socket, asio::buffer(buf),
                [this](const std::error_code& ec, std::size_t)
                {
                    if (ec)
                    {
                        throw std::runtime_error("Failed to read " + ec.message());
                    }
                    
                    doRead();
                });
    }

    void sendMessage(const std::string& msg)
    {

        auto packet = Message::encode(msg);

        asio::async_write(m_socket, asio::buffer(packet),
                [this](const asio::error_code& ec, std::size_t bytesSent)
                {
                    if (ec)
                    {
                        throw std::runtime_error("Failed to send message: " + ec.message());
                    }
                });
    }

private:
    Writeable m_socket;
};



template <typename T>
class Actuator
{
public:
    using Ptr = std::shared_ptr<Actuator>;

    Actuator(CtrlDevice<T>& ctrlDevice)
        : m_ctrlDevice(ctrlDevice)
    {}
    ~Actuator() {}

    virtual std::string updateMsg() = 0;

    void update()
    {
        auto msg = updateMsg();
        m_ctrlDevice.sendMessage(msg);
    }

private:
    CtrlDevice<T>& m_ctrlDevice;
    std::mutex m_mtx;
    bool m_needUpdate;
};


#endif

