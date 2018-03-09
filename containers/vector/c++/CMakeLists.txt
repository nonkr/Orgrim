cmake_minimum_required(VERSION 2.8)

project(cxx_vector)

add_definitions(-std=c++11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(vector1 vector1.cpp)
add_executable(vector2 vector2.cpp)
add_executable(vector3 vector3.cpp)
