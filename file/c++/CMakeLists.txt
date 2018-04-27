cmake_minimum_required(VERSION 2.8)

project(cpp_file)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-std=c++11)

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_executable(read_file_line_by_line read_file_line_by_line.cpp)
add_executable(read_file_by_block read_file_by_block.cpp)
add_executable(read_file_by_block2 read_file_by_block2.cpp)
add_executable(get_file_size get_file_size.cpp)
