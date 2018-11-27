cmake_minimum_required(VERSION 2.8)

project(serial)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -DORGRIM_DEBUG")

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_executable(simple_read simple_read.cpp)
add_executable(ttytest ttytest.c)
