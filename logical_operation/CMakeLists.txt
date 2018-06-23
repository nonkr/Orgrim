cmake_minimum_required(VERSION 2.8)

project(c_logical_operation)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -DORGRIM_DEBUG")

add_executable(logical_operation logical_operation.cpp)
