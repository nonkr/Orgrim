cmake_minimum_required(VERSION 2.8)

project(cxx_vector)

add_definitions(-std=c++11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(list list.cpp)
add_executable(list_remove list_remove.cpp)
