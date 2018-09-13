cmake_minimum_required(VERSION 2.8)

project(cxx_oop_class_member_initialize)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(sample main.cpp A.cpp B.cpp)
