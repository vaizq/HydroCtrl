cmake_minimum_required(VERSION 3.2)

project(bball_ai)

find_package(asio CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(app src/main.cpp)

target_link_libraries(app PRIVATE asio asio::asio)
target_link_libraries(app PRIVATE cereal::cereal)
target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json)
