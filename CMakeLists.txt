cmake_minimum_required(VERSION 3.0.0)
project(sdlraycaster VERSION 0.1.0)

enable_testing()

add_executable(sdlraycaster main.cpp)

find_package(SDL2 REQUIRED COMPONENTS SDL2)

target_link_libraries(sdlraycaster SDL2::SDL2)