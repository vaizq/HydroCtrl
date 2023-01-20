#include "application.hpp"
#include "commands.hpp"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include <SDL_render.h>
#include <SDL2/SDL_image.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <functional>
#include <chrono>
#include <thread>


#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720


constexpr int PORT = 666666;


// This is a test PH-sensor node




Application::Application()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(fmt::format("Error: Unable to initialize SDL {}\n", SDL_GetError()));
    }

    // Create the window
    m_window = SDL_CreateWindow("HydroCtrl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (m_window == nullptr) 
        throw std::runtime_error(fmt::format("Error: Unable to create window: {}\n", SDL_GetError()));

    // Create renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (m_renderer == nullptr)
        throw std::runtime_error(fmt::format("Error: Unable to create renderer: {}\n", SDL_GetError()));

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer_Init(m_renderer);

    auto quitCommand = makeCallerCmd(std::bind(&Application::stop, this));

    // Wire events
    InputSystem::instance().bindCmd(InputSystem::Event::Quit, quitCommand);

    // Connect to server
    std::cout << "Connect to server\n";
    m_client = std::make_unique<Client>(std::string("localhost"), PORT);

    sendDeviceInfo("res/deviceinfo.json");
}

void Application::tick(float targetFps)
{
    using namespace std::chrono_literals;

    auto t0 = std::chrono::steady_clock::now();
    // Start of main loop

    InputSystem::instance().handleEvents();
    updateNodes();
    handleGui();
    renderAll();

    // Sleep for remaining time
    auto t1 = std::chrono::steady_clock::now();
    auto frameDuration = (t1 - t0);

    auto targetDuration = std::chrono::milliseconds(static_cast<int>(1000 / targetFps));
    auto sleepDuration = targetDuration - frameDuration;
    if (sleepDuration > 0ms)
    {
        std::this_thread::sleep_for(sleepDuration);
    }
}

void Application::updateNodes()
{
    while (m_client->numReceived() > 0)
    {
        auto msg = m_client->popReceived();
    }
}


void Application::handleGui()
{
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    static float ph = 7.0f;

    // Build imgui dashboard
    ImGui::Begin("Dashboard");

    if (ImGui::SliderFloat("PH", &ph, 0, 14))
    {
        Message msg(fmt::format("PH {}", ph));
        m_client->send(msg);
    }

    ImGui::End();
}

void Application::renderAll()
{
    SDL_RenderClear(m_renderer); 

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_renderer);
}

Application::~Application()
{
    // Clean up and quit
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Application::sendDeviceInfo(const std::string& filename)
{
    std::ifstream ifs(filename);
    auto jsonData = nlohmann::json::parse(ifs);

    Message msg(jsonData.dump());
    m_client->send(msg);
}






