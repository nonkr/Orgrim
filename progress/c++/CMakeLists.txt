cmake_minimum_required(VERSION 2.8)

project(cxx_progress)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(progress sample.cpp)

target_link_libraries(progress m)
