cmake_minimum_required(VERSION 2.8)

project(cxx_design_pattern_singleton)

add_definitions(-std=c++11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(EagerSingleton EagerSingleton.cpp)
add_executable(LazySingleton1 LazySingleton1.cpp)
add_executable(LazySingleton2 LazySingleton2.cpp)
