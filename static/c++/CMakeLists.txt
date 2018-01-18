cmake_minimum_required(VERSION 2.8)

project(cpp_static)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-std=c++11)

add_executable(local_static local_static.cpp)