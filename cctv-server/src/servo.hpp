#ifndef SERVO_HPP
#define SERVO_HPP

#include "ctrldevice.hpp"
#include <nlohmann/json.hpp>
#include <memory>
#include <chrono>
#include <string>


using json = nlohmann::json;


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
        // Create json representing this servo
        json j;
        j["device"] = 1;
        j["pin"] = m_pin;
        j["angle"] = m_angle;
        return j.dump();
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
