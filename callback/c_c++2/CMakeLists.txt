cmake_minimum_required(VERSION 2.8)

project(c_cxx_callback2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG -std=c++11")

add_executable(example1 main.c c.c)
add_executable(example2 main.cpp c.c cxx.cpp)
