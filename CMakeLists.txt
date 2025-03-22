cmake_minimum_required(VERSION 3.2)

project(sh3modding)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 17)

add_subdirectory(external)
add_subdirectory(sh3modlib)
add_subdirectory(tools)

