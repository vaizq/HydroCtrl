#ifndef APPLICATION_HPP
#define APPLICATION_HPP


#include "inputsystem.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>


class Application
{
public:
    Application();
    Application(Application&) = delete;
    ~Application();
    void tick(float targetFps);
    void stop() { running = false; }
    bool isRunning() { return running; }
private:
    void updateNodes();
    void handleGui();
    void renderAll();
private:
    bool running = true;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
};


#endif

