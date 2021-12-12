cmake_minimum_required(VERSION 2.8)

project(cxx_callback)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -DORGRIM_DEBUG -std=c++11 -Wno-unused-function -Wno-unused-variable")

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_executable(bind_simple bind_simple.cpp)
add_executable(bind bind.cpp)
add_executable(bind_class bind_class.cpp)
