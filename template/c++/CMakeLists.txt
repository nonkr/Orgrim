cmake_minimum_required(VERSION 2.8)

project(cxx_template)

add_definitions(-std=c++11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(class_template class_template.cpp)
add_executable(function_template function_template.cpp)
