cmake_minimum_required(VERSION 3.16)

project(main)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

set(SOURCES main.cpp Vector2D.cpp Ball.h Box.h Physics.cpp)

add_executable(main ${SOURCES})

target_link_libraries(main SFML::Graphics SFML::Window SFML::System)
