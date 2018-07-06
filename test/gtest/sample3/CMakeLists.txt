cmake_minimum_required(VERSION 2.8)

project(cxx_gtest_sample3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -std=c++11")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(sample3 test.cpp)

target_link_libraries(sample3 ${GTEST_LIBRARIES})
