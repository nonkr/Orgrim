cmake_minimum_required(VERSION 2.8)

project(cxx_gtest_sample1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")

add_executable(sample1 sample.cpp test.cpp)

target_link_libraries(sample1 gtest gtest_main)
