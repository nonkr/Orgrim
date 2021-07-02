cmake_minimum_required(VERSION 2.8)

project(c_align)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG -Wno-unused-but-set-variable")

add_executable(align align.cpp)
