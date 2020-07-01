cmake_minimum_required(VERSION 2.8)

project(c_logical_operation)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG -Wno-unused-but-set-variable")

add_definitions(-std=c++11)

add_executable(logical_operation logical_operation.cpp)
add_executable(XOR XOR.cpp)
