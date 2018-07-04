cmake_minimum_required(VERSION 2.8)

project(cxx_callback)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG -std=c++11 -Wno-unused-function -Wno-unused-variable")

add_executable(bind_simple bind_simple.cpp)
add_executable(bind bind.cpp)
