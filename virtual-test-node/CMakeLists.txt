cmake_minimum_required(VERSION 3.2)

project(node)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)

include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(node src/main.cpp src/application.cpp)

target_link_libraries(node PRIVATE SDL2::SDL2) 
target_link_libraries(node PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(node PRIVATE imgui::imgui)
target_link_libraries(node PRIVATE fmt::fmt)
target_link_libraries(node PRIVATE glm::glm)
target_link_libraries(node PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(node PRIVATE cereal::cereal)
