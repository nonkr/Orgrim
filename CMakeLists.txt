cmake_minimum_required(VERSION 3.3.2)
project(Orgrim)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE SOURCE_FILES
    *.c
    ./*/*.c
    ./*/*.cpp
    )

#include_directories(/usr/local/include)

add_executable(Orgrim ${SOURCE_FILES})
