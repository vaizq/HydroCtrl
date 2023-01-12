cmake_minimum_required(VERSION 3.2)

project(hydroctrl)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)

include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(hydroctrl src/main.cpp src/application.cpp)

target_link_libraries(hydroctrl PRIVATE SDL2::SDL2) 
target_link_libraries(hydroctrl PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(hydroctrl PRIVATE imgui::imgui)
target_link_libraries(hydroctrl PRIVATE fmt::fmt)
target_link_libraries(hydroctrl PRIVATE glm::glm)
target_link_libraries(hydroctrl PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(hydroctrl PRIVATE cereal::cereal)
