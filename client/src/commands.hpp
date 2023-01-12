#ifndef COMMANDS_HPP
#define COMMANDS_HPP


#include "inputsystem.hpp"
#include "SDL2/SDL.h"


template <typename T>
class CallerCommand : public Command
{
public:
    CallerCommand(T callable) : m_callable(callable) {};
    virtual void exec()
    {
        m_callable();
    }
private:
    T m_callable;
};

template <typename T>
std::shared_ptr<CallerCommand<T>> makeCallerCmd(T f)
{
    return std::make_shared<CallerCommand<T>>(f);
}

class MousePosCommand : public Command
{
public:
    virtual void exec()
    {
        float x = event.x;
        float y = event.y;
        std::cout << "Mouse is at " << x << " " << y << '\n';
    }

    SDL_MouseMotionEvent event;
};


#endif
