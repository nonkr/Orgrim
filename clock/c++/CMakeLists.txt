cmake_minimum_required(VERSION 2.8)

project(clock)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-std=c++11)

add_executable(clock_t clock_t.cpp)
add_executable(chrono chrono.cpp)
