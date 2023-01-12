#ifndef INPUTSYSTEM_HPP
#define INPUTSYSTEM_HPP


#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include <memory>
#include <iostream>
#include <set>
#include <map>


struct Command
{
    virtual ~Command() {};
    virtual void exec() = 0;
};


class InputSystem
{
public:
    using CommandPtr = std::shared_ptr<Command>;

    enum class Event // These events get handled
    {
        Any,
        Quit,
        LeftButtonDown,
        RightButtonDown,
        LeftButtonUp,
        RightButtonUp,
        MouseMotion
    };

    InputSystem(InputSystem&) = delete;

    void operator=(const InputSystem&) = delete;

    static InputSystem& instance()
    {
        static InputSystem self;
        return self;
    }

    void handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch(event.type)
            {
                case SDL_QUIT:
                    handleEvent(Event::Quit);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                        handleEvent(Event::LeftButtonDown);
                    else if (event.button.button == SDL_BUTTON_RIGHT)
                        handleEvent(Event::RightButtonDown);
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                        handleEvent(Event::LeftButtonUp);
                    else if (event.button.button == SDL_BUTTON_RIGHT)
                        handleEvent(Event::RightButtonUp);
                    break;
                case SDL_MOUSEMOTION:
                    m_mousePos.x = event.motion.x;
                    m_mousePos.y = event.motion.y;
                    handleEvent(Event::MouseMotion);
                    break;
            }
        }
    }

    void bindCmd(Event event, CommandPtr cmd)
    {
        m_commands[event].insert(cmd);
    }

    void unbindCmd(Event event, CommandPtr cmd)
    {
        m_commands[event].erase(cmd);
    }

    glm::vec2 getMousePos()
    {
        return m_mousePos;
    }

private:
    InputSystem() {}

    void executeCommand(std::shared_ptr<Command> cmd)
    {
        cmd->exec();
    }

    void handleEvent(Event event)
    {
        auto quitCommands = m_commands[event];
        for (auto command : quitCommands)
        {
            executeCommand(command);
        }
    }

private:
    std::map<Event, std::set<CommandPtr>> m_commands;
    glm::vec2 m_mousePos;
};


#endif




