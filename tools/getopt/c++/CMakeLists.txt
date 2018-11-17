cmake_minimum_required(VERSION 2.8)

project(CmdOptions)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-std=c++11)

add_executable(${PROJECT_NAME} main.cpp CmdOptions.cpp)
