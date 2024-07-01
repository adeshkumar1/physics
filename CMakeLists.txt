cmake_minimum_required(VERSION 3.10)

project(main)

set(CMAKE_CXX_STANDARD 20)

# Find SFML
find_package(
  SFML 2.5
  COMPONENTS graphics window system
  REQUIRED)

# Include headers
include_directories(${SFML_INCLUDE_DIR})

# Add source files
set(SOURCES main.cpp Vector2D.cpp Ball.h Box.h Physics.cpp)

add_executable(main ${SOURCES})

# Link SFML libraries
target_link_libraries(main sfml-graphics sfml-window sfml-system)
