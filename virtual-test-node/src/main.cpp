#include <SDL_events.h>
#include <SDL2/SDL.h>
#include <exception>
#include <iostream>
#include "application.hpp"
#include "inputsystem.hpp"


int main(int argc, char** argv)
{
    try
    {
        Application app;

        while (app.isRunning())
        {
            app.tick(144);
        }
    }
    catch(const std::exception& e)
    {
        std::cout << "Application crashed: " << e.what();
        return EXIT_FAILURE;
    }
    return 0;
}
