#ifndef SERVO_HPP
#define SERVO_HPP

#include "ctrldevice.hpp"
#include <memory>
#include <chrono>
#include <string>

template <typename T>
class Servo 
    : public Actuator<T>
{
public:
    Servo(CtrlDevice<T>& mcu, int pin)
        : Actuator<T>(mcu), m_pin(pin)
    {
    }

    std::string updateMsg() override
    {
        std::string msg = "{\nDevice: servo,\n";
        msg += "pin: " + std::to_string(m_pin) + "\n";
        msg += "value: " + std::to_string(m_angle) + "\n}";
        return msg;
    }

    void rotate(float angle)
    {
        m_angle += angle;
    }

private:
    int m_pin;
    float m_angle = 0.0f;
};


#endif
