cmake_minimum_required(VERSION 2.8)

project(cpp_file)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-std=c++11)

add_executable(data_size_32 data_size.cpp)
add_executable(data_size_64 data_size.cpp)

set_target_properties(data_size_32 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")