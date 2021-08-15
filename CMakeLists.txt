#include_directories(miniaudio)
project(miniaudio_example)

add_compile_definitions(MINIAUDIO_IMPLEMENTATION)

find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia Charts)
add_executable(example main.cpp)

target_link_libraries(example Qt5::Core)
target_link_libraries(example Qt5::Multimedia)
target_link_libraries(example Qt5::Charts)

target_link_libraries(example pthread dl)
