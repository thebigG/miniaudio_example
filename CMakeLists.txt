#include_directories(miniaudio)
cmake_minimum_required(VERSION 3.5)
project(example LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(MINIAUDIO_IMPLEMENTATION)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia Charts)

add_executable(example main.cpp)


if(WIN32)
    target_link_libraries(example pthread)

else(WIN32)
    target_link_libraries(example pthread dl)

endif()

