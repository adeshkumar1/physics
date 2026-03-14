cmake_minimum_required(VERSION 3.16)

project(physics_sim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

set(SOURCES
    src/main.cpp
    src/math/Vector2D.cpp
    src/physics/Physics.cpp
)

add_executable(physics_sim ${SOURCES})

target_include_directories(physics_sim PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(physics_sim SFML::Graphics SFML::Window SFML::System)
